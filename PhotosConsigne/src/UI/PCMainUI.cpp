

/**
 * \file PCMainUI.cpp
 * \brief defines PCMainUI
 * \author Florian Lance
 * \date 04/04/2017
 */

// std
//#include <chrono>

// Qt
#include <QCollator>
#include <QMessageBox>
#include <QDesktopServices>

// local
#include "PCMainUI.hpp"


using namespace pc;

PCMainUI::PCMainUI(QApplication *parent) : m_version("4.0"), m_ui(this){

    Q_UNUSED(parent)

    // set icon/title
    setWindowTitle("PhotosConsigne " + m_version + " (générateur de PDF à partir de photos)");
    setWindowIcon(QIcon(":/images/icon"));

    // init workers
    m_loadPhotoWorker       = std::make_unique<PhotoLoaderWorker>();
    m_pdfGeneratorWorker    = std::make_unique<PDFGeneratorWorker>();

    // connections
    from_main_UI_connections();
    from_UI_elements_connections();
    from_main_module_connections();
    from_pdf_generator_worker_connections();
    from_photos_loader_worker_connections();

    // init threads
    m_loadPhotoWorker->moveToThread(&m_displayPhotoWorkerThread);
    m_displayPhotoWorkerThread.start();

    m_pdfGeneratorWorker->moveToThread(&m_pdfGeneratorWorkerThread);
    m_pdfGeneratorWorkerThread.start();

    // update settings with current UI
    emit init_document_signal();
    update_settings();
}

PCMainUI::~PCMainUI(){

    DebugMessage("~PCMainUI");
    emit kill_signal();

    // kill threads
    m_displayPhotoWorkerThread.quit();
    m_displayPhotoWorkerThread.wait();

    m_pdfGeneratorWorkerThread.quit();
    m_pdfGeneratorWorkerThread.wait();
}

void PCMainUI::closeEvent(QCloseEvent *event){

    Q_UNUSED(event);
    emit kill_signal();
}

void PCMainUI::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Left){
        if(m_ui.mainUI.twMiddle->currentIndex() == 0){
            m_ui.mainUI.pbLeft->click();
        }
    } else if(event->key() == Qt::Key_Right){
        if(m_ui.mainUI.twMiddle->currentIndex() == 0){
            m_ui.mainUI.pbRight->click();
        }
    }
}

void PCMainUI::add_photos_directory(){

    if(m_isLoadingPhotos){
        emit stop_loading_photos_signal();
        return;
    }

    QString previousDirectory = m_settings.lastPhotosDirectoryPath;
    m_settings.lastPhotosDirectoryPath = QFileDialog::getExistingDirectory(this, "Sélection du répertoire d'images",
                            (m_settings.lastPhotosDirectoryPath.size() > 0) ? m_settings.lastPhotosDirectoryPath : QDir::homePath());

    // no directory selected
    if(m_settings.lastPhotosDirectoryPath.size() == 0 ){
        if(previousDirectory.size() != 0){
            m_settings.lastPhotosDirectoryPath = previousDirectory; // retrieve precedent path
        }
        return;
    }

    // retrieve dir
    QDir dir(m_settings.lastPhotosDirectoryPath);
    dir.setSorting(QDir::NoSort); // will be sorted manually with std::sort
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.jfif" << "*.jpe" << "*.tif" << "*.gif" << "*.bmp" << "*.pdm" << "*.ppm" << "*.xdm" << "*.xpm");

    // retrieve photos name list
    QStringList fileList = dir.entryList();

    // sort the list
    QCollator collator;
    collator.setNumericMode(true);
    std::sort( fileList.begin(), fileList.end(), [&collator](const QString &file1, const QString &file2){
        return collator.compare(file1, file2) < 0;
    });

    // no photo in the directory
    if(fileList.size() == 0){
        if(previousDirectory.size() != 0){
            m_settings.lastPhotosDirectoryPath = previousDirectory; // retrieve precedent path
        }

        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune image (jpg,png) n'a pu être trouvée dans ce répertoire, veuillez en selectionner un autre.\n"),QMessageBox::Ok);
        return;
    }

    // block ui
    m_isLoadingPhotos = true;
    m_ui.set_ui_state_for_adding_photos(false);

    // update photos paths
    for(auto &&photoPath : fileList){
        photoPath = m_settings.lastPhotosDirectoryPath + "/" + photoPath;
    }

    // send photos path to the worker to be loaded
    emit start_loading_photos_signal(fileList, m_settings.photosLoaded->size());
}


void PCMainUI::insert_transparent_space(){

    int indexPhoto = m_settings.currentPhotoId;

    // insert it
    m_settings.photosLoaded->insert(indexPhoto, std::make_shared<Photo>(Photo("", true)));

    // insert new consign in UI
    m_ui.settingsW.insert_individual_set(indexPhoto);

    select_photo(indexPhoto, m_ui.mainUI.twMiddle->currentIndex()==0);
    update_settings();

    m_ui.mainUI.twMiddle->setTabEnabled(0, true);
}

void PCMainUI::load_new_photos(){

    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Choisissez les images à insérer dans la liste"),
                                                    QDir::homePath(), "Images (*.png *.jpg *.jpeg *.jfif, *.jpe *.tif *.bmp *.pdm *.ppm *.xdm *.xpm)");
    if(filePaths.size() == 0){
        return;
    }

    m_isLoadingPhotos = true;
    m_ui.set_ui_state_for_adding_photos(false);
    emit start_loading_photos_signal(filePaths,  m_settings.currentPhotoId);
}

void PCMainUI::remove_all_photos(){

    m_settings.previousPageId      = 0;
    m_settings.currentPageId       = 0;
    m_settings.currentPhotoId      = 0;
    m_settings.currentSetId  = 0;

    m_settings.photosLoaded->clear();
    m_ui.settingsW.reset_individual_sets(0);

    update_settings();
}


void PCMainUI::update_settings_with_no_preview(){

    m_settings.noPreviewGeneration = true;
    update_settings();
    m_settings.noPreviewGeneration = false;
}

void PCMainUI::build_valid_sets(){

    // build valid arrays
    auto nbPhotosLoaded = m_settings.photosLoaded->size();
    m_settings.photosValided->clear();
    m_settings.photosValided->reserve(nbPhotosLoaded);
    m_ui.settingsW.setsValidedW.clear();
    m_ui.settingsW.setsValidedW.reserve(nbPhotosLoaded);

    // resets id and valid set list
    for(int ii = 0; ii < nbPhotosLoaded; ++ii){

        m_settings.photosLoaded->at(ii)->loadedId = ii;

        if(m_settings.photosLoaded->at(ii)->isRemoved){
            continue;
        }

        m_settings.photosValided->push_back(m_settings.photosLoaded->at(ii));
        m_settings.photosValided->last()->id = m_settings.photosValided->size();
        m_ui.settingsW.setsValidedW.push_back(m_ui.settingsW.setsLoadedW.at(ii));
    }

    // update current PD displayed if necessary
    auto nbPhotosValid = m_settings.photosValided->size();
    if(m_settings.currentSetId >= nbPhotosValid && nbPhotosValid > 0){
        m_settings.currentSetId = nbPhotosValid-1;
    }
}


void PCMainUI::build_pages(){

    // update pages ui
    m_ui.settingsW.update_individual_pages(m_settings);

    // block signals
    m_ui.mainUI.twMiddle->blockSignals(true);

    // reset pages
    m_pcPages.pages.clear();
    m_pcPages.pages.reserve(m_settings.nbPages);

    // set all photos not in the document
    for(auto &&loadedPhoto : (*m_settings.photosLoaded)){
        loadedPhoto->isOnDocument = false;
        loadedPhoto->pageId = -1;
    }

    // general pages parameters
    m_pcPages.grayScale = m_settings.grayScale;
    m_pcPages.paperFormat = m_settings.paperFormat;

    // create new pages
    int currentPhotoId = 0;
    for(int ii = 0; ii < m_settings.nbPages; ++ii){

        // build page
        SPCPage pcPage = std::make_shared<PCPage>(PCPage());

        // define current nb of photos for this page
        auto pageUI = m_ui.settingsW.pagesW[ii].get();

        // only global
        // # misc
        pcPage->id                      = ii;
        pcPage->drawThisPage            = m_settings.saveOnlyCurrentPage ? (ii == m_settings.currentPageId) : true;

        // # header
        pcPage->header->settings        = m_settings.headerSettings;

        // # footer
        pcPage->footer->settings        = m_settings.footerSettings;

        // global or individual
        bool individualPageSettings = pageUI->individual();
        // # borders
        pcPage->bordersSettings         = individualPageSettings ? pageUI->bordersW.settings()      : m_settings.bordersSettings;
        // # background
        pcPage->backgroundSettings      = individualPageSettings ? pageUI->backgroundW.settings()   : m_settings.backgroundSettings;
        // # margins
        pcPage->margins                 = individualPageSettings ? pageUI->marginsW.settings()      : m_settings.marginsSettings;
        // # sets
        pcPage->pageSetsSettings        = individualPageSettings ? pageUI->setsW.settings()         : m_settings.setsSettings;
        // # misc
        pcPage->miscSettings            = individualPageSettings ? pageUI->miscW.settings()         : m_settings.miscSettings;

        // sets
        int nbSets = pcPage->pageSetsSettings.customMode ? pcPage->pageSetsSettings.relativePosCustom.size() : pcPage->pageSetsSettings.nbPhotos;

        pcPage->sets.reserve(nbSets);
        for(int jj = 0; jj < nbSets; ++jj){

            // build set
            SPCSet set = std::make_shared<PCSet>(PCSet());

            // only global
            set->id      = jj;
            set->totalId = currentPhotoId;
            if(currentPhotoId < m_settings.photosValided->size()){
                set->photo = m_settings.photosValided->at(currentPhotoId);
                set->photo->isOnDocument = true; // photo will be on the generated document
                set->photo->pageId = ii;
                set->consign = std::make_shared<Consign>(Consign());
            }else{
                break;
            }

            // global or individual
            auto setW = m_ui.settingsW.setsValidedW[currentPhotoId].get();
            bool individualConsignSettings = setW->ui.cbEnableIndividualConsign->isChecked();

            // # text
            set->consign->html            = individualConsignSettings ? setW->setTextW.html()                                                       : m_settings.consignText;
            // # style
            set->consignPositionFromPhoto = individualConsignSettings ? static_cast<Position>(setW->setStyleW.ui.cbPositionConsign->currentIndex()) : m_settings.consignPositionFromPhotos;
            set->ratio                    = individualConsignSettings ? setW->setStyleW.ui.dsbRatioPC->value()                                      : m_settings.PCRatio;
            set->photo->adjust            = individualConsignSettings ? Utility::photo_adjust_from_comboBox(setW->setStyleW.ui.cbAdjustPhoto)       : m_settings.photoAdust;
            set->photo->alignment         = individualConsignSettings ? Utility::photo_alignment_from_comboBox(setW->setStyleW.ui.cbPhotoAlignment) : m_settings.photoAlignment;

            pcPage->sets.push_back(set);
            currentPhotoId++;
        }

        // add page to pages list widget and update the ui
        m_pcPages.pages.push_back(pcPage);
    }

    // check row
    if(m_settings.currentPageId >= m_settings.nbPages){
        m_settings.currentPageId = m_settings.nbPages-1;
    }

    // unlock signals
    m_ui.mainUI.twMiddle->blockSignals(false);
}



void PCMainUI::update_photo_to_display(SPhoto photo)
{
    if(!photo->isWhiteSpace){
        m_ui.photoW.set_image(QImage(photo->pathPhoto).transformed(QTransform().rotate(photo->rotation)));
    }else{

        QImage whiteImg(100, 100, QImage::Format_RGB32);
        whiteImg.fill(Qt::white);
        m_ui.photoW.set_image(std::move(whiteImg));
    }

    m_ui.photoW.update();
}


void PCMainUI::define_selected_set_from_current_photo()
{
    for(int ii = 0; ii < m_settings.photosValided->size(); ++ii){
        if(m_settings.photosValided->at(ii)->loadedId ==  m_settings.currentPhotoId){
            m_settings.currentSetId = m_settings.photosValided->at(ii)->id;
            break;
        }
    }
}

void PCMainUI::select_photo(int index, bool displayPhotoPanel){

    if(index >= m_settings.photosLoaded->size()){
        qWarning() << "-Error: select_photo : " << index;
        return;
    }

    m_settings.previousPhotoId = m_settings.currentPhotoId;

    auto nbPhotosLoaded = m_settings.photosLoaded->size();
    if(index == -1 || nbPhotosLoaded == 0){
        qWarning() << "-WARNING: define_selected_pc -> bad index: " << index << " " << nbPhotosLoaded;
        return;
    }

    if(index >= nbPhotosLoaded){
        m_settings.currentPhotoId = 0;
    }

    // update index
    m_settings.currentPhotoId = index;

    // set order buttons states
    m_ui.mainUI.pbOrderMinus->setEnabled(m_settings.currentPhotoId != 0);
    m_ui.mainUI.pbOrderPlus->setEnabled(m_settings.currentPhotoId != nbPhotosLoaded-1);

    // display selected image
    if(displayPhotoPanel){
        update_photo_to_display(m_settings.photosLoaded.get()->at(m_settings.currentPhotoId));
    }

    // reconstruct sets
    build_valid_sets();
    define_selected_page_from_current_photo();
    define_selected_set_from_current_photo();
    update_settings_with_no_preview();

    if(displayPhotoPanel){
        m_ui.display_photo_panel();
    }
}

void PCMainUI::define_selected_page(int index){

    if(index > m_ui.settingsW.pagesW.size()-1){
        return;
    }
    m_settings.currentPageId = index;
}

void PCMainUI::define_selected_page_from_current_photo(){

    int idPage = -1;
    for(const auto &page : m_pcPages.pages){

        for(const auto &set : page->sets){

            if(set->photo->loadedId == m_settings.photosLoaded->at(m_settings.currentPhotoId)->loadedId){
                idPage = page->id;
                break;
            }
        }
        if(idPage != -1){
            break;
        }
    }

    if(idPage != -1){
        m_settings.currentPageId = idPage;
    }
}

void PCMainUI::from_main_UI_connections()
{
    // dyn ui elements
    // # update settings
    connect(&m_ui, &UIElements::settings_updated_signal, this, &PCMainUI::update_settings);
    // # timer
    connect(&m_ui.zonesTimer, &QTimer::timeout, this, [=]{
        m_ui.zonesTimer.stop();

        // check if the preview is already computing and will ask later to a new update if it's the case
        m_previewLocker.lockForWrite();
        bool isPreviewComputing = m_isPreviewComputing;
        m_generatePreviewAgain = isPreviewComputing;
        m_previewLocker.unlock();

        // ask for a preview generation
        if(!isPreviewComputing){
            m_previewLocker.lockForWrite();
            m_isPreviewComputing = true;
            m_previewLocker.unlock();
            emit start_preview_generation_signal(m_pcPages, m_settings.currentPageId, false);
        }
    });

    // main ui
    // # remove all photos
    connect(m_ui.mainUI.pbRemoveAllPhoto, &QPushButton::clicked, this, &PCMainUI::remove_all_photos);
    // # insert new image/blank
    connect(m_ui.mainUI.pbInsertNewWhiteSpace, &QPushButton::clicked, this, &PCMainUI::insert_transparent_space);
    connect(m_ui.mainUI.pbInsertNewImage, &QPushButton::clicked, this, &PCMainUI::load_new_photos);

    // # checkboxes
    // ## grayscale
    connect(m_ui.mainUI.cbBAndW, &QCheckBox::clicked, this, [=](bool checked){
        m_settings.grayScale = checked;
        update_settings();
    });

    // ## open PDF
    connect(m_ui.mainUI.pbOpenPDF, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(m_pcPages.pdfFileName))){
            QMessageBox::warning(this, tr("Avertissement"), tr("Le PDF n'a pu être lancé.\nVeuillez vous assurez que vous disposez d'un logiciel de lecture de PDF (ex : AdobeReader, SumatraPDF, FoxitReader...) .\n"),QMessageBox::Ok);
        }
    });
    // ## add photos directory
    connect(m_ui.mainUI.pbAddPhotosDIrectory, &QPushButton::clicked, this, &PCMainUI::add_photos_directory);
    // ## save PDF
    connect(m_ui.mainUI.pbSavePDF, &QPushButton::clicked, this, [&]{
        QString filePath = QFileDialog::getSaveFileName(this, "Entrez le nom du fichier PDF", QDir::homePath(), "PDF (*.pdf)");
        if(filePath.size() > 0){
            m_pcPages.pdfFileName = filePath;
            m_ui.set_ui_state_for_generating_pdf(false);
            emit start_PDF_generation_signal(m_pcPages);
        }
    });
    // ## left photo
    connect(m_ui.mainUI.pbLeft, &QPushButton::clicked, this, [&]{ // previous image

        int currRow = m_settings.currentPhotoId;
        if(currRow == -1){
            return;
        }

        if(currRow == 0){
            currRow = m_settings.photosLoaded->size()-1;
        } else{
            currRow--;
        }

        select_photo(currRow, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
        m_ui.previewW.set_current_pc(m_settings.currentSetId);

    });
    // ## right photo
    connect(m_ui.mainUI.pbRight, &QPushButton::clicked, this, [&]{ // next image

        int currRow = m_settings.currentPhotoId;
        if(currRow == -1){
            return;
        }

        if(currRow == m_settings.photosLoaded->size()-1){
            currRow = 0;
        }else{
            currRow++;
        }

        select_photo(currRow, m_ui.mainUI.twMiddle->currentIndex() == 0);        
        update_settings();
        qDebug() << "update pc: " << m_settings.currentSetId;
        m_ui.previewW.set_current_pc(m_settings.currentSetId);
    });
    // ## rotate photot to the left
    connect(m_ui.mainUI.pbRotateLeft, &QPushButton::clicked, this, [&] {// rotate left photo

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        SPhoto photo       = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation    =(photo->rotation - 90)%360;
        photo->scaledPhoto = photo->scaledPhoto.transformed(QTransform().rotate(-90));
        update_photo_to_display(photo);
        update_settings();
    });
    // ## rotate photot to the right
    connect(m_ui.mainUI.pbRotateRight, &QPushButton::clicked, this, [&]{ // rotate right photo

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        SPhoto photo        = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation     = (photo->rotation + 90)%360;
        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(90));
        update_photo_to_display(photo);
        update_settings();
    });
    // ## add again the photo in the list
    connect(m_ui.mainUI.pbAdd, &QPushButton::clicked, this, [&]{// add/remove photo from list

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        m_settings.photosLoaded.get()->at(m_settings.currentPhotoId)->isRemoved = false;

        select_photo(m_settings.currentPhotoId, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
    });
    // ## remove the photo from the list
    connect(m_ui.mainUI.pbRemove, &QPushButton::clicked, this, [&] {// add/remove photo from list

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        SPhoto currPhoto = m_settings.photosLoaded->at(m_settings.currentPhotoId);
        currPhoto->isRemoved = true;

        int idPhotoSelected = 0;
        if(currPhoto->isADuplicate || currPhoto->isWhiteSpace){ // delete it


            // delete photo
            m_settings.photosLoaded->takeAt(m_settings.currentPhotoId);

            // insert new consign in UI
            m_ui.settingsW.remove_individual_set(m_settings.currentPhotoId);

            // define selected id photo
            idPhotoSelected = m_settings.currentPhotoId;

            int count = m_settings.photosLoaded->size();
            if(idPhotoSelected >= count){
                idPhotoSelected = count-1;
            }

        }else{

            // define selected id photo
            int currRow = m_settings.currentPhotoId;
            if(currRow < m_settings.photosLoaded->size()-1){
                idPhotoSelected = currRow + 1;
            }else if(currRow > 0){
                idPhotoSelected = currRow - 1;
            }
        }

        select_photo(idPhotoSelected, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
    });
    // ## duplicate the photo in the list
    connect(m_ui.mainUI.pbDuplicate, &QPushButton::clicked, this, [&]{

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        // copy current photo
        SPhoto duplicatedPhoto = std::make_shared<Photo>(Photo(*(m_settings.photosLoaded.get()->at(m_settings.currentPhotoId))));
        duplicatedPhoto->isRemoved    = false;
        duplicatedPhoto->isADuplicate = true;

        // insert it
        m_settings.photosLoaded->insert(m_settings.currentPhotoId+1, duplicatedPhoto);

        // insert new consign in UI
        m_ui.settingsW.insert_individual_set(m_settings.currentPhotoId+1);
        update_settings();
    });
    // ## change photo order (minus)
    m_ui.mainUI.pbOrderMinus->hide();
    // ## change photo order (plus)
    m_ui.mainUI.pbOrderPlus->hide();

    // # spinbox
    // ## change photo order
    connect(m_ui.mainUI.sbOrder, QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int index){

        if(m_settings.currentPhotoId == -1 || m_settings.photosLoaded->size() == 0){
            return;
        }

        int row = m_settings.currentPhotoId;
        if(row < 0){
            qWarning() << "-Warning: order photo -> bad index";
            return;
        }

        m_settings.photosLoaded->swap(row,index);
        m_ui.settingsW.setsLoadedW.insert(index, m_ui.settingsW.setsLoadedW.takeAt(row));

        select_photo(index, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
    });

    // # tab widgets
     connect(m_ui.mainUI.twMiddle, &QTabWidget::currentChanged, this, [&](int index){

        if(index == 0){
            select_photo(m_settings.currentPhotoId);
        }else{
            update_settings();
        }
     });
    // # list widget
    // ## photos list
    connect(m_ui.mainUI.lwPhotosList, &QListWidget::clicked, this, [&](QModelIndex index){
        select_photo(index.row());
    });
    connect(m_ui.mainUI.lwPhotosList, &QListWidget::currentRowChanged, this, [&](int row){

        select_photo(row);
    });
    // ## pages list
    connect(m_ui.mainUI.lwPagesList, &QListWidget::currentRowChanged, this, [&](int row){

        m_settings.previousPageId = m_settings.currentPageId;
        if(row != -1){
            define_selected_page(row);
            m_ui.display_preview_panel();
            update_settings();            
        }
    });
    connect(m_ui.mainUI.lwPagesList, &QListWidget::clicked, this, [&](QModelIndex index){

        m_settings.previousPageId = m_settings.currentPageId;
        if(index.row() != -1){
            define_selected_page(index.row());
            m_ui.display_preview_panel();
            update_settings();            
        }
    });

    // # spinboxes
    connect(m_ui.mainUI.sbNbPages, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        update_settings();
    });


    // # comboBoxes
    connect(m_ui.mainUI.cbDPI, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        update_settings();
    });
    connect(m_ui.mainUI.cbFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        update_settings();
    });
}


void PCMainUI::from_main_module_connections(){

    // to pdf generator worker
    connect(this, &PCMainUI::kill_signal,                       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::kill);
    connect(this, &PCMainUI::start_preview_generation_signal,   m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_preview);
    connect(this, &PCMainUI::start_PDF_generation_signal,       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_PDF);
    connect(this, &PCMainUI::init_document_signal,              m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::init_document);
    connect(this, &PCMainUI::select_pc_signal,                  m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::update_PC_selection_with_id);

    // to photo display worker
    connect(this, &PCMainUI::kill_signal,            m_loadPhotoWorker.get(), &PhotoLoaderWorker::kill);
    connect(this, &PCMainUI::start_loading_photos_signal, m_loadPhotoWorker.get(), &PhotoLoaderWorker::load_photos_directory);
    connect(this, &PCMainUI::stop_loading_photos_signal, m_loadPhotoWorker.get(), &PhotoLoaderWorker::kill);

}


void PCMainUI::from_pdf_generator_worker_connections(){

    PDFGeneratorWorker *worker = m_pdfGeneratorWorker.get();

    // to this
    connect(worker, &PDFGeneratorWorker::end_preview_signal, this, [=](QImage previewImage, SPCPage previewPage){

        m_ui.previewW.set_image(std::move(previewImage));
        m_ui.previewW.set_page(previewPage);
        m_ui.previewW.set_current_pc(m_settings.currentSetId);
        m_ui.previewW.update();

        m_ui.mainUI.pbSavePDF->setEnabled(true);

        m_previewLocker.lockForWrite();
        m_isPreviewComputing = false;
        bool generatePreviewAgain = m_generatePreviewAgain;
        m_generatePreviewAgain = false;
        m_previewLocker.unlock();

        if(generatePreviewAgain){
            update_settings();
        }

    });
    connect(worker, &PDFGeneratorWorker::end_generation_signal, this, [=](bool success){

        if(success){
            m_ui.mainUI.pbOpenPDF->setEnabled(true);
            m_ui.mainUI.laLoadingText->setText("PDF généré avec succès\n" + m_pcPages.pdfFileName);
        }else{
            m_ui.mainUI.laLoadingText->setText("Echec génération du PDF");
        }

        m_ui.set_ui_state_for_generating_pdf(true);
    });
    connect(worker, &PDFGeneratorWorker::set_progress_bar_state_signal, m_ui.mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PDFGeneratorWorker::set_progress_bar_text_signal,  m_ui.mainUI.laLoadingText, &QLabel::setText);
    connect(worker, &PDFGeneratorWorker::abort_pdf_signal,              this, [=](QString pathFile){

        QMessageBox::warning(this, tr("Avertissement"), tr("Le fichier PDF ") + pathFile + tr(" n'a pu être écrit, il se peut que celui-ci soit en cours d'utilisation par un autre logiciel."),QMessageBox::Ok);
        m_ui.mainUI.laLoadingText->setText("Echec génération du PDF");
        m_ui.set_ui_state_for_generating_pdf(true);
    });
//    connect(worker, &PDFGeneratorWorker::current_pc_selected_signal, this, [=](QRectF pcRectRelative, int totalIdPC){

//        if(totalIdPC == -1){
//            m_ui.settingsW.display_header_panel();
//            m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
//            m_ui.previewW.update();
//            return;
//        }else if(totalIdPC == -2){
//            m_ui.settingsW.display_footer_panel();
//            m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
//            m_ui.previewW.update();
//            return;
//        }

//        m_settings.currentSetId = totalIdPC;
//        m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
//        m_ui.previewW.update();

//        // update photo list selection
//        select_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId, false);
//    });
}

void PCMainUI::from_photos_loader_worker_connections(){

    PhotoLoaderWorker *worker = m_loadPhotoWorker.get();

    // # progress bar
    connect(worker, &PhotoLoaderWorker::set_progress_bar_state_signal, m_ui.mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PhotoLoaderWorker::set_progress_bar_text_signal,  m_ui.mainUI.laLoadingText, &QLabel::setText);
    // # end loading photo
    connect(worker, &PhotoLoaderWorker::photo_loaded_signal,this, [&](SPhoto photo, int indexToInsert){

        // add loaded photo
        m_settings.photosLoaded->insert(indexToInsert, photo);
        m_ui.settingsW.insert_individual_set(indexToInsert);

        // update current photo row
        m_settings.currentPhotoId = indexToInsert;

        update_settings();
    });
    // # end loading all photos
    connect(worker, &PhotoLoaderWorker::end_loading_photos_signal,this, [&]{

        // update progress bar
        m_ui.mainUI.laLoadingText->setText("Photos chargées.");
        m_ui.mainUI.progressBarLoading->setValue(1000);

        // unlock ui
        m_isLoadingPhotos = false;
        m_ui.set_ui_state_for_adding_photos(true);

        select_photo(m_settings.currentPhotoId, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();        
    });
}

void PCMainUI::from_UI_elements_connections(){

    PreviewW &previewW    = m_ui.previewW;
    PhotoW &displayPhotoW = m_ui.photoW;

    // # UI elements
    // ### resource added
    connect(&m_ui, &UIElements::resource_added_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::add_resource);

    // # preview label
    // ### double click
    connect(&previewW, &PreviewW::double_click_on_photo_signal, this, [&]{
        if(m_settings.currentSetId < m_settings.photosValided->size()){
            select_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId);
        }
    });
    connect(&previewW, &PreviewW::current_pc_selected_signal, this, [=](int totalIdPC){

        if(totalIdPC == -1){
            m_ui.settingsW.display_header_panel();
            return;
        }else if(totalIdPC == -2){
            m_ui.settingsW.display_footer_panel();
            return;
        }

        m_settings.currentSetId = totalIdPC;

        // update photo list selection
        select_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId, false);
        m_ui.settingsW.display_current_set_panel();
    });


    // # image label
    // ### double click
    connect(&displayPhotoW, &PhotoW::double_click_signal, this, [&]{
        update_settings();
        m_ui.display_preview_panel();
    });
}

void PCMainUI::update_settings(){

    DebugMessage dbgMess("update_settings");

    qDebug() << "1";
    // update global settings
    m_ui.update_global_settings(m_settings);

    qDebug() << "2";
    // reconstruct sets
    build_valid_sets();    

    qDebug() << "3";
    // reconstruct the pages from global and individual settings
    build_pages();

    qDebug() << "4";
    m_ui.update_photos_list(m_settings);
    m_ui.update_pages_list(m_pcPages);

    qDebug() << "5";
    // update ui
    m_ui.update_UI(m_settings);

    qDebug() << "55";

    if(m_ui.settingsW.setsValidedW.size() >0){
        m_settings.currentSetIdDisplayed = m_ui.settingsW.setsValidedW[m_settings.currentSetId]->id;
    }
    if(m_ui.settingsW.pagesW.size() > 0){
        m_settings.currentPageIdDisplayed= m_ui.settingsW.pagesW[m_settings.currentPageId]->id;        
    }
    qDebug() << "6";

    // check if the preview is already computing and will ask later to a new update if it's the case
    m_previewLocker.lockForWrite();
    bool isPreviewComputing = m_isPreviewComputing;
    m_generatePreviewAgain = isPreviewComputing;
    m_previewLocker.unlock();

    // ask for a preview generation
    if(!m_settings.noPreviewGeneration && !isPreviewComputing){
        m_previewLocker.lockForWrite();
        m_isPreviewComputing = true;
        m_previewLocker.unlock();
        emit start_preview_generation_signal(m_pcPages, m_settings.currentPageId, m_ui.zonesTimer.isActive());
    }
    qDebug() << "7";
}




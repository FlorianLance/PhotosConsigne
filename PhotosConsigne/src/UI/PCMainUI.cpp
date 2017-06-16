

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
#include "Work.hpp"

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

    QString previousDirectory = m_settings.photos.lastDirectoryPath;
    m_settings.photos.lastDirectoryPath = QFileDialog::getExistingDirectory(this, "Sélection du répertoire d'images",
                            (m_settings.photos.lastDirectoryPath.size() > 0) ? m_settings.photos.lastDirectoryPath : QDir::homePath());

    // no directory selected
    if(m_settings.photos.lastDirectoryPath.size() == 0 ){
        if(previousDirectory.size() != 0){
            m_settings.photos.lastDirectoryPath = previousDirectory; // retrieve precedent path
        }
        return;
    }

    // retrieve dir
    QDir dir(m_settings.photos.lastDirectoryPath);
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
            m_settings.photos.lastDirectoryPath = previousDirectory; // retrieve precedent path
        }

        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune image (jpg,png) n'a pu être trouvée dans ce répertoire, veuillez en selectionner un autre.\n"),QMessageBox::Ok);
        return;
    }

    // block ui
    m_isLoadingPhotos = true;
    m_ui.set_ui_state_for_adding_photos(false);

    // update photos paths
    for(auto &&photoPath : fileList){
        photoPath = m_settings.photos.lastDirectoryPath + "/" + photoPath;
    }

    // send photos path to the worker to be loaded
    emit start_loading_photos_signal(fileList, m_settings.photos.loaded->size());
}


void PCMainUI::insert_transparent_space(){

    int indexPhoto = m_settings.photos.currentId;

    // insert it
    m_settings.photos.loaded->insert(indexPhoto, std::make_shared<Photo>(Photo("", true)));

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
    emit start_loading_photos_signal(filePaths,  m_settings.photos.currentId);
}

void PCMainUI::remove_all_photos(){

//    m_settings.pages.previousId = 0;
    m_settings.pages.currentId  = 0;
    m_settings.photos.currentId = 0;
    m_settings.photos.loaded->clear();
    m_settings.sets.currentId   = 0;


    m_ui.settingsW.reset_individual_sets(0);

    update_settings();
}


void PCMainUI::update_settings_with_no_preview(){

    m_settings.document.noPreviewGeneration = true;
    update_settings();
    m_settings.document.noPreviewGeneration = false;
}

void PCMainUI::build_valid_sets(){

    // build valid arrays
    auto nbloaded = m_settings.photos.loaded->size();
    m_settings.photos.valided->clear();
    m_settings.photos.valided->reserve(nbloaded);
    m_ui.settingsW.setsValidedW.clear();
    m_ui.settingsW.setsValidedW.reserve(nbloaded);

    // resets id and valid set list
    for(int ii = 0; ii < nbloaded; ++ii){

        m_settings.photos.loaded->at(ii)->loadedId = ii;

        if(m_settings.photos.loaded->at(ii)->isRemoved){
            continue;
        }

        m_settings.photos.valided->push_back(m_settings.photos.loaded->at(ii));
        m_settings.photos.valided->last()->id = m_settings.photos.valided->size()-1;
        m_ui.settingsW.setsValidedW.push_back(m_ui.settingsW.setsLoadedW.at(ii));
    }

    // update current PD displayed if necessary
    auto nbPhotosValid = m_settings.photos.valided->size();
    if(m_settings.sets.currentId >= nbPhotosValid && nbPhotosValid > 0){
        m_settings.sets.currentId = nbPhotosValid-1;
    }
}


void PCMainUI::build_pages(){

    // update pages ui
    m_ui.settingsW.update_individual_pages(m_settings.pages.nb);

    // block signals
    m_ui.mainUI.twMiddle->blockSignals(true);

    // reset pages
    m_pcPages.pages.clear();
    m_pcPages.pages.reserve(m_settings.pages.nb);

    // set all photos not in the document
    for(auto &&loadedPhoto : (*m_settings.photos.loaded)){
        loadedPhoto->isOnDocument = false;
        loadedPhoto->pageId = -1;
    }

    // general pages parameters
    m_pcPages.settings.grayScale = m_settings.document.grayScale;
    m_pcPages.settings.paperFormat = m_settings.document.paperFormat;

    // create new pages
    int currentId = 0;
    for(int ii = 0; ii < m_settings.pages.nb; ++ii){

        // build page
        SPCPage pcPage = std::make_shared<PCPage>(PCPage());

        // define current nb of photos for this page
        auto pageUI = m_ui.settingsW.pagesW[ii].get();

        // only global
        // # misc
        pcPage->id                      = ii;
        pcPage->drawThisPage            = m_settings.document.saveOnlyCurrentPage ? (ii == m_settings.pages.currentId) : true;

        // # header
        pcPage->header->settings        = m_settings.header;

        // # footer
        pcPage->footer->settings        = m_settings.footer;

        // global or individual
        bool individualPageSettings = pageUI->individual();

        // # background
        if(individualPageSettings){
            pageUI->update_settings(pcPage->settings);
        }else{
            pcPage->settings = m_settings.pages;
        }

        // sets
        int nbSets = pcPage->settings.positions.customMode ? pcPage->settings.positions.relativePosCustom.size() : pcPage->settings.positions.nbPhotos;

        pcPage->sets.reserve(nbSets);
        for(int jj = 0; jj < nbSets; ++jj){

            // build set
            SPCSet set = std::make_shared<PCSet>(PCSet());

            // only global
            set->id      = jj;
            set->totalId = currentId;

            if(currentId < m_settings.photos.valided->size()){

                auto setW = m_ui.settingsW.setsValidedW[currentId].get();
                if(setW->ui.cbEnableIndividualConsign->isChecked()){
                    setW->update_settings(set->settings);
                }else{
                    set->settings       = m_settings.sets;
                }

                set->photo = m_settings.photos.valided->at(currentId);
                set->photo->isOnDocument = true; // photo will be on the generated document
                set->photo->pageId = ii;                
                set->text = std::make_shared<Consign>(Consign());                

            }else{
                break;
            }                       

            pcPage->sets.push_back(set);
            currentId++;
        }

        // add page to pages list widget and update the ui
        m_pcPages.pages.push_back(pcPage);
    }

    // check row
    if(m_settings.pages.currentId >= m_settings.pages.nb){
        m_settings.pages.currentId = m_settings.pages.nb-1;
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
    for(int ii = 0; ii < m_settings.photos.valided->size(); ++ii){
        if(m_settings.photos.valided->at(ii)->loadedId ==  m_settings.photos.currentId){
            m_settings.sets.currentId = m_settings.photos.valided->at(ii)->id;
            break;
        }
    }
}

void PCMainUI::select_photo(int index, bool displayPhotoPanel){

    if(index >= m_settings.photos.loaded->size()){
        qWarning() << "-Error: select_photo : " << index;
        return;
    }

    m_settings.photos.previousId = m_settings.photos.currentId;

    auto nbloaded = m_settings.photos.loaded->size();
    if(index == -1 || nbloaded == 0){
        qWarning() << "-WARNING: define_selected_pc -> bad index: " << index << " " << nbloaded;
        return;
    }

    if(index >= nbloaded){
        m_settings.photos.currentId = 0;
    }

    // update index
    m_settings.photos.currentId = index;

    // set order buttons states
    m_ui.mainUI.pbOrderMinus->setEnabled(m_settings.photos.currentId != 0);
    m_ui.mainUI.pbOrderPlus->setEnabled(m_settings.photos.currentId != nbloaded-1);

    // display selected image
    if(displayPhotoPanel){
        update_photo_to_display(m_settings.photos.loaded.get()->at(m_settings.photos.currentId));
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
    m_settings.pages.currentId = index;
}

void PCMainUI::define_selected_page_from_current_photo(){

    int idPage = -1;
    for(const auto &page : m_pcPages.pages){

        for(const auto &set : page->sets){

            if(set->photo->loadedId == m_settings.photos.loaded->at(m_settings.photos.currentId)->loadedId){
                idPage = page->id;
                break;
            }
        }
        if(idPage != -1){
            break;
        }
    }

    if(idPage != -1){
        m_settings.pages.currentId = idPage;
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
            emit start_preview_generation_signal(m_pcPages, m_settings.pages.currentId, false);
        }
    });

    // main ui
    // # remove all photos
    connect(m_ui.mainUI.pbRemoveAllPhoto, &QPushButton::clicked, this, &PCMainUI::remove_all_photos);
    // # insert new image/blank
    connect(m_ui.mainUI.pbInsertNewWhiteSpace, &QPushButton::clicked, this, &PCMainUI::insert_transparent_space);
    connect(m_ui.mainUI.pbInsertNewImage, &QPushButton::clicked, this, &PCMainUI::load_new_photos);

    connect(m_ui.mainUI.pbSaveWork, &QPushButton::clicked, this, [&]{ // save work

        QString filePath = QFileDialog::getSaveFileName(this, "Entrez le nom du document de travail", QDir::homePath(), "Work (*.work)");
        if(filePath.size() > 0){
            pc::Work::save(filePath, m_settings.photos.loaded, m_ui);
        }

    });
    connect(m_ui.mainUI.pbLoadWork, &QPushButton::clicked, this, [&]{ // load work

        QString filePath = QFileDialog::getOpenFileName(this, "Choisissez un document de travail à charger", QDir::homePath(), "Work (*.work)");
        if(filePath.size() > 0){

            QFile file(filePath);
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
                return;
            }

            QXmlStreamReader xml;
            xml.setDevice(&file);

            m_settings.photos.loaded->clear();
            m_ui.settingsW.reset_individual_sets(0);

            // update current photo row


            QXmlStreamReader::TokenType token;
            while(!xml.atEnd() && !xml.hasError()) {

                // Read next element
                token = xml.readNext();
                qDebug() << "Elem" << xml.name() << token << xml.atEnd() << xml.hasError();
                if(token == QXmlStreamReader::StartDocument){
                    continue;
                }
                else if(token == QXmlStreamReader::StartElement){
                    if(xml.name() == "Photos"){

                        m_settings.photos.loaded->reserve(xml.attributes().value("number").toInt());

                    }else if(xml.name() == "Photo"){

                        SPhoto photo = std::make_shared<Photo>(xml.attributes().value("path").toString(), xml.attributes().value("white").toInt());
                        photo->id           = xml.attributes().value("id").toInt();
                        photo->pageId       = xml.attributes().value("pageId").toInt();
                        photo->loadedId     = xml.attributes().value("loadedId").toInt();
                        photo->rotation     = xml.attributes().value("rotation").toInt();
                        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(photo->rotation));
                        photo->isADuplicate = xml.attributes().value("duplicate").toInt();
                        photo->isOnDocument = xml.attributes().value("onDoc").toInt();
                        photo->isRemoved    = xml.attributes().value("removed").toInt();
                        m_settings.photos.loaded->push_back(photo);

                        m_ui.settingsW.insert_individual_set(m_settings.photos.loaded->size()-1);
                        m_settings.photos.currentId = m_settings.photos.loaded->size()-1;
                    }else if(xml.name() == "Document"){

                        m_ui.load_from_xml(xml,true);
                        update_settings_with_no_preview();
                        m_ui.load_from_xml(xml,false);
                    }
                }
            }

            update_settings();
        }
    });

    // # checkboxes
    // ## grayscale
    connect(m_ui.mainUI.cbBAndW, &QCheckBox::clicked, this, [=](bool checked){
        m_settings.document.grayScale = checked;
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

        int currRow = m_settings.photos.currentId;
        if(currRow == -1){
            return;
        }

        if(currRow == 0){
            currRow = m_settings.photos.loaded->size()-1;
        } else{
            currRow--;
        }

        select_photo(currRow, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
        m_ui.previewW.set_current_pc(m_settings.sets.currentId);

    });
    // ## right photo
    connect(m_ui.mainUI.pbRight, &QPushButton::clicked, this, [&]{ // next image

        int currRow = m_settings.photos.currentId;
        if(currRow == -1){
            return;
        }

        if(currRow == m_settings.photos.loaded->size()-1){
            currRow = 0;
        }else{
            currRow++;
        }

        select_photo(currRow, m_ui.mainUI.twMiddle->currentIndex() == 0);        
        update_settings();
        m_ui.previewW.set_current_pc(m_settings.sets.currentId);
    });
    // ## rotate photot to the left
    connect(m_ui.mainUI.pbRotateLeft, &QPushButton::clicked, this, [&] {// rotate left photo

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        SPhoto photo       = m_settings.photos.loaded.get()->at(m_settings.photos.currentId);
        photo->rotation    =(photo->rotation - 90)%360;
        photo->scaledPhoto = photo->scaledPhoto.transformed(QTransform().rotate(-90));
        update_photo_to_display(photo);
        update_settings();
    });
    // ## rotate photot to the right
    connect(m_ui.mainUI.pbRotateRight, &QPushButton::clicked, this, [&]{ // rotate right photo

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        SPhoto photo        = m_settings.photos.loaded.get()->at(m_settings.photos.currentId);
        photo->rotation     = (photo->rotation + 90)%360;
        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(90));
        update_photo_to_display(photo);
        update_settings();
    });
    // ## add again the photo in the list
    connect(m_ui.mainUI.pbAdd, &QPushButton::clicked, this, [&]{// add/remove photo from list

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        m_settings.photos.loaded.get()->at(m_settings.photos.currentId)->isRemoved = false;

        select_photo(m_settings.photos.currentId, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
    });
    // ## remove the photo from the list
    connect(m_ui.mainUI.pbRemove, &QPushButton::clicked, this, [&] {// add/remove photo from list

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        SPhoto currPhoto = m_settings.photos.loaded->at(m_settings.photos.currentId);
        currPhoto->isRemoved = true;

        int idPhotoSelected = 0;
        if(currPhoto->isADuplicate || currPhoto->isWhiteSpace){ // delete it


            // delete photo
            m_settings.photos.loaded->takeAt(m_settings.photos.currentId);

            // insert new consign in UI
            m_ui.settingsW.remove_individual_set(m_settings.photos.currentId);

            // define selected id photo
            idPhotoSelected = m_settings.photos.currentId;

            int count = m_settings.photos.loaded->size();
            if(idPhotoSelected >= count){
                idPhotoSelected = count-1;
            }

        }else{

            // define selected id photo
            int currRow = m_settings.photos.currentId;
            if(currRow < m_settings.photos.loaded->size()-1){
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

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        // copy current photo
        SPhoto duplicatedPhoto = std::make_shared<Photo>(Photo(*(m_settings.photos.loaded.get()->at(m_settings.photos.currentId))));
        duplicatedPhoto->isRemoved    = false;
        duplicatedPhoto->isADuplicate = true;

        // insert it
        m_settings.photos.loaded->insert(m_settings.photos.currentId+1, duplicatedPhoto);

        // insert new consign in UI
        m_ui.settingsW.insert_individual_set(m_settings.photos.currentId+1);
        update_settings();
    });
    // ## change photo order (minus)
    m_ui.mainUI.pbOrderMinus->hide();
    // ## change photo order (plus)
    m_ui.mainUI.pbOrderPlus->hide();

    // # spinbox
    // ## change photo order
    connect(m_ui.mainUI.sbOrder, QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int index){

        if(m_settings.photos.currentId == -1 || m_settings.photos.loaded->size() == 0){
            return;
        }

        int row = m_settings.photos.currentId;
        if(row < 0){
            qWarning() << "-Warning: order photo -> bad index";
            return;
        }

        m_settings.photos.loaded->swap(row,index);
        m_ui.settingsW.setsLoadedW.insert(index, m_ui.settingsW.setsLoadedW.takeAt(row));

        select_photo(index, m_ui.mainUI.twMiddle->currentIndex() == 0);
        update_settings();
    });

    // # tab widgets
     connect(m_ui.mainUI.twMiddle, &QTabWidget::currentChanged, this, [&](int index){

        if(index == 0){
            select_photo(m_settings.photos.currentId);
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

//        m_settings.pages.previousId = m_settings.pages.currentId;
        if(row != -1){
            define_selected_page(row);
            m_ui.display_preview_panel();
            update_settings();            
        }
    });
    connect(m_ui.mainUI.lwPagesList, &QListWidget::clicked, this, [&](QModelIndex index){

//        m_settings.pages.previousId = m_settings.pages.currentId;
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
        m_ui.previewW.set_current_pc(m_settings.sets.currentId);
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
}

void PCMainUI::from_photos_loader_worker_connections(){

    PhotoLoaderWorker *worker = m_loadPhotoWorker.get();

    // # progress bar
    connect(worker, &PhotoLoaderWorker::set_progress_bar_state_signal, m_ui.mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PhotoLoaderWorker::set_progress_bar_text_signal,  m_ui.mainUI.laLoadingText, &QLabel::setText);
    // # end loading photo
    connect(worker, &PhotoLoaderWorker::photo_loaded_signal,this, [&](SPhoto photo, int indexToInsert){

        // add loaded photo
        m_settings.photos.loaded->insert(indexToInsert, photo);
        m_ui.settingsW.insert_individual_set(indexToInsert);

        // update current photo row
        m_settings.photos.currentId = indexToInsert;

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

        select_photo(m_settings.photos.currentId, m_ui.mainUI.twMiddle->currentIndex() == 0);
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
        if(m_settings.sets.currentId < m_settings.photos.valided->size()){
            select_photo(m_settings.photos.valided->at(m_settings.sets.currentId)->loadedId);
        }
    });
    connect(&previewW, &PreviewW::current_pc_selected_signal, this, [=](int totalIdPC){

        if(totalIdPC == -2){
            m_ui.settingsW.display_header_panel();
            return;
        }else if(totalIdPC == -3){
            m_ui.settingsW.display_footer_panel();
            return;
        }

        m_settings.sets.currentId = totalIdPC;

        // update photo list selection
        select_photo(m_settings.photos.valided->at(m_settings.sets.currentId)->loadedId, false);
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

    // update global settings
    m_ui.update_global_settings(m_settings);

    // reconstruct sets
    build_valid_sets();    

    // reconstruct the pages from global and individual settings
    build_pages();

    m_ui.update_photos_list(m_settings);
    m_ui.update_pages_list(m_pcPages);

    // update ui
    m_ui.update_UI(m_settings);


    if(m_ui.settingsW.setsValidedW.size() >0){
        m_settings.sets.currentIdDisplayed = m_ui.settingsW.setsValidedW[m_settings.sets.currentId]->id;
    }

    // check if the preview is already computing and will ask later to a new update if it's the case
    m_previewLocker.lockForWrite();
    bool isPreviewComputing = m_isPreviewComputing;
    m_generatePreviewAgain = isPreviewComputing;
    m_previewLocker.unlock();

    // ask for a preview generation
    if(!m_settings.document.noPreviewGeneration && !isPreviewComputing){
        m_previewLocker.lockForWrite();
        m_isPreviewComputing = true;
        m_previewLocker.unlock();
        emit start_preview_generation_signal(m_pcPages, m_settings.pages.currentId, m_ui.zonesTimer.isActive());
    }
}






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

PCMainUI::PCMainUI(QApplication *parent) : m_version("3.0"), m_ui(this){

    Q_UNUSED(parent)

    // set icon/title
    setWindowTitle("PhotosConsigne " + m_version + " (générateur de PDF à partir de photos)");
    setWindowIcon(QIcon(":/images/icon"));

    // init workers
    m_loadPhotoWorker = std::make_unique<PhotoLoaderWorker>();
    m_pdfGeneratorWorker = std::make_unique<PDFGeneratorWorker>();

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
    m_ui.settingsW.display_global_set_panel();
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
    set_ui_state_for_adding_photos(false);

    // update photos paths
    for(auto &&photoPath : fileList){
        photoPath = m_settings.lastPhotosDirectoryPath + "/" + photoPath;
    }

    // send photos path to the worker to be loaded
    emit start_loading_photos_signal(fileList, m_settings.photosLoaded->size());
}


void PCMainUI::insert_transparent_space(){

    int indexPhoto = m_settings.currentPhotoId;
    if(indexPhoto == -1){
        indexPhoto = 0;
    }

    // create white photo
    SPhoto whiteSpacePhoto = std::make_shared<Photo>(Photo("", true));

    // insert it
    m_settings.photosLoaded->insert(indexPhoto, whiteSpacePhoto);

    // insert new consign in UI
    m_ui.settingsW.insert_individual_set(indexPhoto);

    update_settings_with_no_preview();
    select_photo(indexPhoto);

    m_ui.mainUI.twMiddle->setTabEnabled(0, true);
}

void PCMainUI::load_new_photos(){

    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Choisissez les images à insérer dans la liste"),
                                                    QDir::homePath(), "Images (*.png *.jpg *.jpeg *.jfif, *.jpe *.tif *.bmp *.pdm *.ppm *.xdm *.xpm)");
    if(filePaths.size() == 0){
        return;
    }

    m_isLoadingPhotos = true;
    set_ui_state_for_adding_photos(false);
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

    // build photo list


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
    if(m_settings.currentSetId > nbPhotosValid){
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
    for(auto &&validPhoto : (*m_settings.photosValided)){
        validPhoto->isOnDocument = false;
    }

    // general pages parameters
    m_pcPages.grayScale = m_settings.grayScale;
    m_pcPages.paperFormat   = PaperFormat(m_ui.mainUI.cbDPI->currentText(), m_ui.mainUI.cbFormat->currentText());

    // create new pages
    int currentPhotoId = 0;
    for(int ii = 0; ii < m_settings.nbPages; ++ii){

        // build page
        SPCPage pcPage = std::make_shared<PCPage>(PCPage());

        // define current nb of photos for this page
        auto &pageUI = m_ui.settingsW.pagesW[ii];

        // only global
        // # misc
        pcPage->id                      = ii;
        pcPage->drawThisPage            = m_settings.saveOnlyCurrentPage ? (ii == m_settings.currentPageId) : true;
        pcPage->orientation             = m_settings.orientation;

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
        // # page sets
        pcPage->pageSetsSettings        = individualPageSettings ? pageUI->setsW.settings()         : m_settings.setsSettings;

        // sets
        pcPage->sets.reserve(pcPage->pageSetsSettings.nbPhotos);
        for(int jj = 0; jj < pcPage->pageSetsSettings.nbPhotos; ++jj){

            // build set
            SPCSet set = std::make_shared<PCSet>(PCSet());

            // only global
            set->id      = jj;
            set->totalId = currentPhotoId;
            if(currentPhotoId < m_settings.photosValided->size()){
                set->photo = m_settings.photosValided->at(currentPhotoId);
                set->photo->isOnDocument = true; // photo will be on the generated document
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

void PCMainUI::display_current_individual_set_ui(){

    DebugMessage dbgMess("display_current_individual_set_ui: " + QString::number(m_settings.currentSetId));

    if(m_ui.settingsW.setsValidedW.size() == 0){ // empty
        return;
    }

    if(m_settings.currentSetId >= m_ui.settingsW.setsValidedW.size()){ // id too big
        m_settings.currentSetId = m_ui.settingsW.setsValidedW.size() -1;
    }

    if(m_ui.settingsW.setsValidedW[m_settings.currentSetId]->id == m_settings.currentSetIdDisplayed){ // already displayed
        return;
    }
    m_settings.currentSetIdDisplayed = m_ui.settingsW.setsValidedW[m_settings.currentSetId]->id;

    int currentIndex = m_ui.settingsW.ui.tbRight->currentIndex();
    m_ui.settingsW.ui.tbRight->blockSignals(true);
    m_ui.settingsW.ui.tbRight->removeItem(4);
    m_ui.settingsW.ui.tbRight->insertItem(4, m_ui.settingsW.setsValidedW[m_settings.currentSetId].get(),"ENSEMBLE (Photo+Texte) N°" + QString::number(m_settings.currentSetId+1));
    m_ui.settingsW.ui.tbRight->setItemIcon(4, QIcon(":/images/sets/set"));
    m_ui.settingsW.ui.tbRight->setCurrentIndex(currentIndex);
    m_ui.settingsW.ui.tbRight->blockSignals(false);
}

void PCMainUI::display_current_individual_page_ui(){

    DebugMessage dbgMess("display_current_individual_page_ui: " + QString::number(m_settings.currentPageId));
    if(m_ui.settingsW.pagesW.size() == 0){ // empty
        return;
    }

    if(m_settings.currentPageId >= m_ui.settingsW.pagesW.size()){ // id too big
        m_settings.currentPageId = m_ui.settingsW.pagesW.size() -1;
    }

    if(m_ui.settingsW.pagesW[m_settings.currentPageId]->id == m_settings.currentPageIdDisplayed){ // already displayed
        return;
    }
    m_settings.currentPageIdDisplayed = m_ui.settingsW.pagesW[m_settings.currentPageId]->id;

    m_ui.mainUI.lwPagesList->blockSignals(true);
    m_ui.mainUI.lwPagesList->setCurrentRow(m_settings.currentPageId);
    m_ui.mainUI.lwPagesList->blockSignals(false);

    int currentIndex = m_ui.settingsW.ui.tbRight->currentIndex();
    m_ui.settingsW.ui.tbRight->blockSignals(true);
    m_ui.settingsW.ui.tbRight->removeItem(3);
    m_ui.settingsW.ui.tbRight->insertItem(3, m_ui.settingsW.pagesW[m_settings.currentPageId].get(), "PAGE N°" + QString::number(m_settings.currentPageId+1));
    m_ui.settingsW.ui.tbRight->setItemIcon(3, QIcon(":/images/pages/page"));
    m_ui.settingsW.ui.tbRight->setCurrentIndex(currentIndex);
    m_ui.settingsW.ui.tbRight->blockSignals(false);

}

void PCMainUI::update_photo_to_display(SPhoto photo)
{
    DebugMessage dbgMess("update_photo_to_display");
    if(!photo->isWhiteSpace){
        m_ui.photoW.set_image(QImage(photo->pathPhoto).transformed(QTransform().rotate(photo->rotation)));
    }else{

        QImage whiteImg(100, 100, QImage::Format_RGB32);
        whiteImg.fill(Qt::white);
        m_ui.photoW.set_image(whiteImg);
    }

    m_ui.photoW.update();
}


void PCMainUI::define_selected_set_from_current_photo()
{
    DebugMessage dbgMess("define_selected_pc_from_current_photo");
    for(int ii = 0; ii < m_settings.photosValided->size(); ++ii){
        if(m_settings.photosValided->at(ii)->loadedId ==  m_settings.currentPhotoId){
            m_settings.currentSetId = ii;
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

    DebugMessage dbgMess("define_selected_photo");
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
//        if(m_settings.previousPhotoId != m_settings.currentPhotoId){
            SPhoto photo = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
            update_photo_to_display(m_settings.photosLoaded.get()->at(m_settings.currentPhotoId));

            QString textInfo = "<p><b>Chemin:</b> "  + photo->pathPhoto + "<br /><b>Taille photo:</b> "
                    + QString::number(photo->info.size()*0.000001, 'f', 2 ) + "Mo - <b>Dimensions:</b> " + QString::number(photo->size().width()) + "x" + QString::number(photo->size().height())  + "</p>";
            m_ui.mainUI.laPhotoInfo->setTextFormat(Qt::RichText);
            m_ui.mainUI.laPhotoInfo->setText(textInfo);
//        }
    }

    define_selected_set_from_current_photo();
    define_selected_page_from_current_photo();
    update_settings_with_no_preview();

    if(displayPhotoPanel){
        display_photo_panel();
    }
}

void PCMainUI::define_selected_page(int index){

    DebugMessage dbgMess("define_selected_page");
    if(index > m_ui.settingsW.pagesW.size()-1){
        return;
    }
    m_settings.currentPageId = index;
}

void PCMainUI::define_selected_page_from_current_photo(){

    DebugMessage dbgMess("define_selected_page_from_current_photo");
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


void PCMainUI::set_ui_state_for_adding_photos(bool state){

    if(!state){
        m_ui.mainUI.pbAddPhotosDIrectory->setIcon(QIcon(":/images/photos/cancel"));
        m_ui.mainUI.pbAddPhotosDIrectory->setText("Stop");
    }else{
        m_ui.mainUI.pbAddPhotosDIrectory->setIcon(QIcon(":/images/dirImages"));
        m_ui.mainUI.pbAddPhotosDIrectory->setText(tr("Ajouter un dossier"));
    }

    // tabs
    // # middle
    m_ui.mainUI.twMiddle->setEnabled(state);
    // # right
    m_ui.settingsW.ui.tbRight->setEnabled(state);
    // # action
    // ## photos
    m_ui.mainUI.pbInsertNewImage->setEnabled(state);
    m_ui.mainUI.pbInsertNewWhiteSpace->setEnabled(state);
    m_ui.mainUI.pbRemoveAllPhoto->setEnabled(state);
    // ## pdf
    m_ui.mainUI.pbSavePDF->setEnabled(state);
    // # photos list
    m_ui.mainUI.lwPhotosList->setEnabled(state);
    // # pages list
    m_ui.mainUI.lwPagesList->setEnabled(state);
    // # document
    m_ui.mainUI.tabWDocument->setEnabled(state);

    if( m_settings.photosLoaded->size() > 0){
        m_ui.mainUI.twMiddle->setTabEnabled(0, true);
    }
}

void PCMainUI::set_ui_state_for_generating_pdf(bool state){
    m_ui.mainUI.tabActions->setEnabled(state);
}

void PCMainUI::update_photos_list(){

    m_ui.mainUI.lwPhotosList->blockSignals(true);

    m_ui.mainUI.lwPhotosList->clear();
    m_ui.mainUI.twPhotosList->setTabText(1, QString::number(m_settings.photosLoaded->size()));

    for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){

        QString ext =   m_settings.photosLoaded->at(ii)->isADuplicate ? " (copie)" : "";
        m_ui.mainUI.lwPhotosList->addItem(QString::number(ii+1) + ". " + m_settings.photosLoaded->at(ii)->namePhoto + ext);

        bool individual = m_ui.settingsW.setsLoadedW[ii]->findChild<QCheckBox*>("cbEnableIndividualConsign")->isChecked();
        QBrush brush = m_ui.mainUI.lwPhotosList->item(ii)->foreground();
        SPhoto photo = m_settings.photosLoaded->at(ii);

        if(!photo->isOnDocument && !photo->isRemoved){
            brush.setColor(QRgb(qRgb(255,0,0)));
        }else if(!photo->isOnDocument){
            brush.setColor(QRgb(qRgb(255,120,120)));            
        }else if(photo->isRemoved){
            brush.setColor(QRgb(qRgb(127,180,255)));
        }else if(individual){
            brush.setColor(QRgb(qRgb(0,0,255)));
        }else{
            brush.setColor(QRgb(qRgb(0,106,255)));
        }

        if(ii == m_settings.currentPhotoId){
            if(photo->isRemoved){
                m_ui.mainUI.pbRemove->hide();
                m_ui.mainUI.pbAdd->show();
            }else{
                m_ui.mainUI.pbRemove->show();
                m_ui.mainUI.pbAdd->hide();
            }
        }

        m_ui.mainUI.lwPhotosList->item(ii)->setForeground(brush);
    }

    m_ui.mainUI.lwPhotosList->setCurrentRow(m_settings.currentPhotoId);
    m_ui.mainUI.lwPhotosList->blockSignals(false);
}

void PCMainUI::update_pages_list(){

    m_ui.mainUI.lwPagesList->blockSignals(true);

    int currentIndex = m_ui.mainUI.lwPagesList->currentRow();
    m_ui.mainUI.lwPagesList->clear();
    m_ui.mainUI.twPagesList->setTabText(1, QString::number(m_pcPages.pages.size()));

    for(int ii = 0; ii < m_pcPages.pages.size(); ++ii){

        m_ui.mainUI.lwPagesList->addItem(QString::number(ii+1) + ". " + QString::number(m_pcPages.pages[ii]->sets.size()) + ((m_pcPages.pages[ii]->sets.size() > 1 ) ? " ensembles." : " ensemble.") );

        bool individual = m_ui.settingsW.pagesW[ii]->findChild<QCheckBox*>("cbEnableIndividualPage")->isChecked();
        QBrush brush =  m_ui.mainUI.lwPagesList->item(ii)->foreground();
        if(m_pcPages.pages[ii]->sets.size() == 0){ // invalid
            brush.setColor(QRgb(qRgb(255,106,0)));
        }else if(!individual){ // global
            brush.setColor(QRgb(qRgb(0,106,255)));
        }else{ // individual
            brush.setColor(QRgb(qRgb(0,0,255)));
        }

        m_ui.mainUI.lwPagesList->item(ii)->setForeground(brush);
    }

    if(currentIndex >= m_pcPages.pages.size() && currentIndex != 0){
        currentIndex = m_pcPages.pages.size()-1;
    }
    m_ui.mainUI.lwPagesList->setCurrentRow(currentIndex);
    m_ui.mainUI.lwPagesList->blockSignals(false);
}




void PCMainUI::from_main_UI_connections()
{
    // dyn ui elements
    // # update settings
    connect(&m_ui, &UIElements::settings_updated_signal, this, &PCMainUI::update_settings);
    // # progress
    connect(&m_ui, &UIElements::set_progress_bar_state_signal, m_ui.mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(&m_ui, &UIElements::set_progress_bar_text_signal, m_ui.mainUI.laLoadingText, &QLabel::setText);
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
            display_preview_panel();
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

    // # buttons
    // ## donate
    connect(m_ui.mainUI.pbDonate, &QPushButton::clicked, &m_ui, &UIElements::display_donate_window);
    // ## help
    connect(m_ui.mainUI.pbHelp, &QPushButton::clicked, &m_ui, &UIElements::display_help_window);
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
            set_ui_state_for_generating_pdf(false);
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

        select_photo(currRow);

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

        select_photo(currRow);
    });
    // ## rotate photot to the left
    connect(m_ui.mainUI.pbRotateLeft, &QPushButton::clicked, this, [&] {// rotate left photo

        if(m_settings.currentPhotoId == -1){
            return;
        }

        SPhoto photo       = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation    =(photo->rotation - 90)%360;
        photo->scaledPhoto = photo->scaledPhoto.transformed(QTransform().rotate(-90));
        update_photo_to_display(photo);
    });
    // ## rotate photot to the right
    connect(m_ui.mainUI.pbRotateRight, &QPushButton::clicked, this, [&]{ // rotate right photo

        if(m_settings.currentPhotoId == -1){
            return;
        }

        SPhoto photo        = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation     = (photo->rotation + 90)%360;
        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(90));
        update_photo_to_display(photo);
    });
    // ## add again the photo in the list
    connect(m_ui.mainUI.pbAdd, &QPushButton::clicked, this, [&]{// add/remove photo from list

        if(m_settings.currentPhotoId == -1){
            return;
        }

        m_settings.photosLoaded.get()->at(m_settings.currentPhotoId)->isRemoved = false;

        select_photo(m_settings.currentPhotoId);
    });
    // ## remove the photo from the list
    connect(m_ui.mainUI.pbRemove, &QPushButton::clicked, this, [&] {// add/remove photo from list

        if(m_settings.currentPhotoId == -1){
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

        select_photo(idPhotoSelected);
    });
    // ## duplicate the photo in the list
    connect(m_ui.mainUI.pbDuplicate, &QPushButton::clicked, this, [&]{

        if(m_settings.currentPhotoId == -1){
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

        update_settings_with_no_preview();
    });
    // ## change photo order (minus)
    connect(m_ui.mainUI.pbOrderMinus, &QPushButton::clicked, this, [&]{

        int row = m_settings.currentPhotoId;
        if(row < 0){
            qWarning() << "-Warning: minus order photo -> bad index";
            return;
        }

        m_settings.photosLoaded->swap(row,row-1);
        m_ui.settingsW.setsLoadedW.insert(row-1, m_ui.settingsW.setsLoadedW.takeAt(row));

        select_photo(row-1);
    });
    // ## change photo order (plus)
    connect(m_ui.mainUI.pbOrderPlus, &QPushButton::clicked, this, [&]{

        int row = m_settings.currentPhotoId;
        if(row < 0){
            qWarning() << "-Warning: plus order photo -> bad index";
            return;
        }

        m_settings.photosLoaded->swap(row,row+1);
        m_ui.settingsW.setsLoadedW.insert(row+1, m_ui.settingsW.setsLoadedW.takeAt(row));

        select_photo(row+1);
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
        m_ui.previewW.draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(row != -1){
            define_selected_page(row);
            display_preview_panel();
            update_settings();
        }
    });
    connect(m_ui.mainUI.lwPagesList, &QListWidget::clicked, this, [&](QModelIndex index){

        m_settings.previousPageId = m_settings.currentPageId;
        m_ui.previewW.draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(index.row() != -1){
            define_selected_page(index.row());
            display_preview_panel();
            update_settings();
        }
    });

    // # spinboxes
    connect(m_ui.mainUI.sbNbPages, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        m_ui.zonesTimer.start(1000);
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
    connect(worker, &PDFGeneratorWorker::end_preview_signal, this, [=](QImage previewImage){
        m_ui.previewW.set_image(previewImage);
        m_ui.previewW.update();

        display_preview_panel();

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

        set_ui_state_for_generating_pdf(true);
    });
    connect(worker, &PDFGeneratorWorker::set_progress_bar_state_signal, m_ui.mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PDFGeneratorWorker::set_progress_bar_text_signal,  m_ui.mainUI.laLoadingText, &QLabel::setText);
    connect(worker, &PDFGeneratorWorker::abort_pdf_signal,              this, [=](QString pathFile){

        QMessageBox::warning(this, tr("Avertissement"), tr("Le fichier PDF ") + pathFile + tr(" n'a pu être écrit, il se peut que celui-ci soit en cours d'utilisation par un autre logiciel."),QMessageBox::Ok);
        m_ui.mainUI.laLoadingText->setText("Echec génération du PDF");
        set_ui_state_for_generating_pdf(true);
    });
    connect(worker, &PDFGeneratorWorker::current_pc_selected_signal, this, [=](QRectF pcRectRelative, int totalIdPC){

        if(totalIdPC == -1){
            m_ui.settingsW.display_header_panel();
            m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
            m_ui.previewW.update();
            return;
        }else if(totalIdPC == -2){
            m_ui.settingsW.display_footer_panel();
            m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
            m_ui.previewW.update();
            return;
        }

        m_settings.currentSetId = totalIdPC;
        m_ui.previewW.draw_current_pc_rect(totalIdPC, pcRectRelative);
        m_ui.previewW.update();

        // update photo list selection
        select_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId, false);
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
        set_ui_state_for_adding_photos(true);

        update_settings();
        select_photo(m_settings.currentPhotoId);
    });
}

void PCMainUI::from_UI_elements_connections(){

    PreviewW &previewW    = m_ui.previewW;
    PhotoW &displayPhotoW = m_ui.photoW;

    // # UI elements
    // ### update settings request
//    connect(m_ui.righSettingsW.globalSetW.setTextW.textEdit(), &TextEdit::textChanged, this, &PCMainUI::update_settings);
    // ### resource added
    connect(&m_ui, &UIElements::resource_added_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::add_resource);

    // # preview label
    // ### single click
    connect(&previewW, &PreviewW::click_on_page_signal, this, [&]{
        m_ui.settingsW.display_current_set_panel();
    });
    connect(&previewW, &PreviewW::click_on_page_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::update_PC_selection);
    // ### double click
    connect(&previewW, &PreviewW::double_click_on_photo_signal, this, [&]{
        if(m_settings.currentSetId < m_settings.photosValided->size()){
            select_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId);
        }
    });

    // # image label
    // ### double click
    connect(&displayPhotoW, &PhotoW::double_click_signal, this, [&]{
        update_settings();
        display_preview_panel();
    });
}

void PCMainUI::update_settings(){

    DebugMessage dbgMess("update_settings");

    // reconstruct sets
    build_valid_sets();
    update_photos_list();

    // update global settings
    m_ui.update_global_settings(m_settings);

    // reconstruct the pages from global and individual settings
    build_pages();
    update_pages_list();

    // update defintion txt
    bool landScape = m_settings.orientation == PageOrientation::landScape;
    int currentDPI = m_ui.mainUI.cbDPI->currentText().toInt();
    m_ui.mainUI.laDefinition->setText("<p><b>Définition: </b> " + QString::number(m_pcPages.paperFormat .width_pixels(currentDPI))+ "x" +
                                    QString::number(m_pcPages.paperFormat .height_pixels(currentDPI)) +
                                    " pixels <br /><b>Dimensions: </b> " +
                                    QString::number(landScape ? m_pcPages.paperFormat.sizeMM.height() : m_pcPages.paperFormat.sizeMM.width()) + "x" +
                                    QString::number(landScape ? m_pcPages.paperFormat.sizeMM.width() : m_pcPages.paperFormat.sizeMM.height()) + " mm</p>");

    // update size margins mm
    m_ui.settingsW.globalPageW.marginsW.update_mm_values((m_settings.orientation == PageOrientation::landScape), m_pcPages.paperFormat);
    for(auto &&pageW : m_ui.settingsW.pagesW){
        pageW->marginsW.update_mm_values((m_settings.orientation == PageOrientation::landScape), m_pcPages.paperFormat);
    }

    // display current dynamic ui
    if(m_settings.photosLoaded->size() > 0){
        display_current_individual_page_ui();
        display_current_individual_set_ui();
    }

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
        display_preview_panel();
    }
}

void PCMainUI::display_photo_panel(){
    Utility::safe_init_tab_widget_index(m_ui.mainUI.twMiddle, 0);
}

void PCMainUI::display_preview_panel(){
    Utility::safe_init_tab_widget_index(m_ui.mainUI.twMiddle, 1);
}






/**
 * \file PCMainUI.cpp
 * \brief defines PCMainUI
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "PCMainUI.hpp"

// Qt
#include <QCollator>
#include <QMessageBox>
#include <QtGlobal>
#include <QDesktopServices>
#include <QColorDialog>

// std
#include <chrono>

// test
#include "ImageReader.hpp"


using namespace pc;

PCMainUI::PCMainUI(QApplication *parent) : m_version("3.0"), m_mainUI(std::make_shared<Ui::PhotosConsigneMainW>()){

    Q_UNUSED(parent)

    // use designer ui
    m_mainUI->setupUi(this);

    // set icon/title
    setWindowTitle("PhotosConsigne " + m_version + " (générateur de PDF à partir de photos)");
    setWindowIcon(QIcon(":/images/icon"));

    // init ui
    m_dynUI = std::make_unique<UIElements>(this, m_mainUI);

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
    update_settings();//_with_no_preview();

    display_global_consign_panel();

    emit init_document_signal();
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
        if(m_mainUI->twMiddle->currentIndex() == 0){
            m_mainUI->pbLeft->click();
        }
    } else if(event->key() == Qt::Key_Right){
        if(m_mainUI->twMiddle->currentIndex() == 0){
            m_mainUI->pbRight->click();
        }
    }
}

void PCMainUI::swap_loaded_set(int currentIndex, int newIndex){

    m_settings.photosLoaded->swap(currentIndex,newIndex);
    m_dynUI->setsLoadedW.insert(newIndex, m_dynUI->setsLoadedW.takeAt(currentIndex));

    m_mainUI->lwPhotosList->blockSignals(true);{
        m_mainUI->lwPhotosList->insertItem(newIndex,m_mainUI->lwPhotosList->takeItem(currentIndex));
        m_mainUI->lwPhotosList->setCurrentRow(newIndex);
    }m_mainUI->lwPhotosList->blockSignals(false);
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

    // insert new element in photo list widget
    m_mainUI->lwPhotosList->blockSignals(true);
    m_mainUI->lwPhotosList->insertItem(indexPhoto,"(" + whiteSpacePhoto->namePhoto + ")");
    m_mainUI->lwPhotosList->blockSignals(false);

    // insert new consign in UI
    m_dynUI->insert_individual_set(indexPhoto);

    update_settings();
    define_selected_photo(indexPhoto);
    define_selected_set_from_current_photo();

    m_mainUI->twMiddle->setTabEnabled(0, true);
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

    m_mainUI->lwPhotosList->clear();
    m_mainUI->lwPagesList->clear();

    m_settings.previousPageId      = 0;
    m_settings.currentPageId       = 0;
    m_settings.currentPhotoId      = 0;
    m_settings.currentSetId  = 0;

    m_settings.photosLoaded->clear();
    m_dynUI->reset_individual_sets(0);

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
    m_dynUI->setsValidedW.clear();
    m_dynUI->setsValidedW.reserve(nbPhotosLoaded);

    // resets id and valid set list
    for(int ii = 0; ii < nbPhotosLoaded; ++ii){

        m_settings.photosLoaded->at(ii)->loadedId = ii;

        if(m_settings.photosLoaded->at(ii)->isRemoved){
            continue;
        }

        m_settings.photosValided->push_back(m_settings.photosLoaded->at(ii));
        m_settings.photosValided->last()->id = m_settings.photosValided->size();
        m_dynUI->setsValidedW.push_back(m_dynUI->setsLoadedW.at(ii));
    }

    // update current PD displayed if necessary
    auto nbPhotosValid = m_settings.photosValided->size();
    if(m_settings.currentSetId > nbPhotosValid){
        m_settings.currentSetId = nbPhotosValid-1;
    }
}


void PCMainUI::build_pages(){

    // update pages ui
    m_dynUI->update_individual_pages(m_settings);

    // block signals
    m_mainUI->lwPagesList->blockSignals(true);
    m_mainUI->twMiddle->blockSignals(true);

    // reset pages
    m_mainUI->lwPagesList->clear();
    m_pcPages.pages.clear();
    m_pcPages.pages.reserve(m_settings.nbPages);

    // set all photos not in the document
    for(auto &&validPhoto : (*m_settings.photosValided)){
        validPhoto->isOnDocument = false;
    }

    // general pages parameters
    m_pcPages.grayScale = m_settings.grayScale;
    m_pcPages.paperFormat   = PaperFormat(m_mainUI->cbDPI->currentText(), m_mainUI->cbFormat->currentText());

    // create new pages
    int currentPhotoId = 0;
    for(int ii = 0; ii < m_settings.nbPages; ++ii){

        // build page
        SPCPage pcPage = std::make_shared<PCPage>(PCPage());

        // define current nb of photos for this page
        auto &pageUI = m_dynUI->pageW[ii];

        // only global
        // # misc
        pcPage->id                      = ii;
        pcPage->drawThisPage            = m_settings.saveOnlyCurrentPage ? (ii == m_settings.currentPageId) : true;
        pcPage->orientation             = m_settings.orientation;

        // # header
        pcPage->header->settings        = m_settings.headerSettings;

        // # footer
        pcPage->footer->settings        = m_settings.footerSettings;
        qDebug() << "pcPage->footer->settings " << pcPage->footer->settings.ratio << " " << pcPage->footer->settings.enabled << " " << pcPage->footer->settings.text->size();

        // global or individual
        bool individualPageSettings = pageUI->individual();
        // # borders
        pcPage->bordersSettings         = individualPageSettings ? pageUI->bordersW.settings()      : m_settings.bordersSettings;
        // # background
        pcPage->backgroundSettings      = individualPageSettings ? pageUI->backgroundW.settings()   : m_settings.backgroundSettings;
        // # margins
        pcPage->margins                 = individualPageSettings ? pageUI->marginsW.settings()      : m_settings.marginsSettings;
        // # photos
        pcPage->nbPhotosH               = individualPageSettings ? pageUI->findChild<QSpinBox*>("sbPhotosNbWidth")->value()  : m_settings.nbPhotosPageWidth;
        pcPage->nbPhotosV               = individualPageSettings ? pageUI->findChild<QSpinBox*>("sbPhotosNbHeight")->value() : m_settings.nbPhotosPageHeight;
        pcPage->nbPhotosPage            = individualPageSettings ? pageUI->findChild<QSpinBox*>("sbPhotosNbPerPage")->value(): m_settings.nbPhotosPerPage;



        // sets
        pcPage->sets.reserve(pcPage->nbPhotosPage);
        for(int jj = 0; jj < pcPage->nbPhotosPage; ++jj){

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
            auto setW = m_dynUI->setsValidedW[currentPhotoId].get();
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
        m_mainUI->lwPagesList->addItem(QString::number(ii+1) + ". " + QString::number(pcPage->sets.size()) + ((pcPage->sets.size() > 1 ) ? " ensembles." : " ensemble."));
    }

    // check row
    if(m_settings.currentPageId >= m_settings.nbPages){
        m_settings.currentPageId = m_settings.nbPages-1;
    }

    // unlock signals
    m_mainUI->lwPagesList->blockSignals(false);
    m_mainUI->twMiddle->blockSignals(false);
}

void PCMainUI::display_current_individual_set_ui(){

    DebugMessage dbgMess("display_current_individual_set_ui: " + QString::number(m_settings.currentSetId));

    if(m_dynUI->setsValidedW.size() == 0){ // empty
        return;
    }

    if(m_settings.currentSetId >= m_dynUI->setsValidedW.size()){ // id too big
        m_settings.currentSetId = m_dynUI->setsValidedW.size() -1;
    }

    if(m_dynUI->setsValidedW[m_settings.currentSetId]->id == m_settings.currentSetIdDisplayed){ // already displayed
        return;
    }
    m_settings.currentSetIdDisplayed = m_dynUI->setsValidedW[m_settings.currentSetId]->id;

    int currentIndex = m_mainUI->tbRight->currentIndex();
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->removeItem(4);
    m_mainUI->tbRight->insertItem(4, m_dynUI->setsValidedW[m_settings.currentSetId].get(),"ENSEMBLE N°" + QString::number(m_settings.currentSetId));
    m_mainUI->tbRight->setCurrentIndex(currentIndex);
    m_mainUI->tbRight->blockSignals(false);
}

void PCMainUI::display_current_individual_page_ui(){

    DebugMessage dbgMess("display_current_individual_page_ui: " + QString::number(m_settings.currentPageId));
    if(m_dynUI->pageW.size() == 0){ // empty
        return;
    }

    if(m_settings.currentPageId >= m_dynUI->pageW.size()){ // id too big
        m_settings.currentPageId = m_dynUI->pageW.size() -1;
    }

    if(m_dynUI->pageW[m_settings.currentPageId]->id == m_settings.currentPageIdDisplayed){ // already displayed
        return;
    }
    m_settings.currentPageIdDisplayed = m_dynUI->pageW[m_settings.currentPageId]->id;

    m_mainUI->lwPagesList->blockSignals(true);
    m_mainUI->lwPagesList->setCurrentRow(m_settings.currentPageId);
    m_mainUI->lwPagesList->blockSignals(false);

    int currentIndex = m_mainUI->tbRight->currentIndex();
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->removeItem(3);
    m_mainUI->tbRight->insertItem(3, m_dynUI->pageW[m_settings.currentPageId].get(), "PAGE N°" + QString::number(m_settings.currentPageId));
    m_mainUI->tbRight->setCurrentIndex(currentIndex);
    m_mainUI->tbRight->blockSignals(false);

}

void PCMainUI::update_photo_to_display(SPhoto photo)
{
    DebugMessage dbgMess("update_photo_to_display");
    if(!photo->isWhiteSpace){
        m_dynUI->selectedPhotoW->set_image(QImage(photo->pathPhoto).transformed(QTransform().rotate(photo->rotation)));
    }else{

        QImage whiteImg(100, 100, QImage::Format_RGB32);
        whiteImg.fill(Qt::white);
        m_dynUI->selectedPhotoW->set_image(whiteImg);
    }

    m_dynUI->selectedPhotoW->update();
}

void PCMainUI::define_selected_photo(int index){

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
    m_mainUI->pbOrderMinus->setEnabled(m_settings.currentPhotoId != 0);
    m_mainUI->pbOrderPlus->setEnabled(m_settings.currentPhotoId != nbPhotosLoaded-1);

    // display selected image
    if(m_settings.previousPhotoId != m_settings.currentPhotoId){
        SPhoto photo = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        update_photo_to_display(m_settings.photosLoaded.get()->at(m_settings.currentPhotoId));

        QString textInfo = "<p><b>Chemin:</b> "  + photo->pathPhoto + "<br /><b>Taille photo:</b> "
                + QString::number(photo->info.size()*0.000001, 'f', 2 ) + "Mo - <b>Dimensions:</b> " + QString::number(photo->size().width()) + "x" + QString::number(photo->size().height())  + "</p>";
        m_mainUI->laPhotoInfo->setTextFormat(Qt::RichText);
        m_mainUI->laPhotoInfo->setText(textInfo);
    }

    m_mainUI->lwPhotosList->blockSignals(true);
    m_mainUI->lwPhotosList->setCurrentRow(index);
    m_mainUI->lwPhotosList->blockSignals(false);

    update_photos_list_style();
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

void PCMainUI::define_selected_page(int index){

    DebugMessage dbgMess("define_selected_page");
    if(index > m_dynUI->pageW.size()-1){
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
        m_mainUI->pbAddPhotosDIrectory->setIcon(QIcon(":/images/photos/cancel"));
        m_mainUI->pbAddPhotosDIrectory->setText("Stop");
    }else{
        m_mainUI->pbAddPhotosDIrectory->setIcon(QIcon(":/images/dirImages"));
        m_mainUI->pbAddPhotosDIrectory->setText(tr("Ajouter un dossier"));
    }

    // tabs
    // # middle
    m_mainUI->twMiddle->setEnabled(state);
    // # right
    m_mainUI->tbRight->setEnabled(state);    
    // # action
    // ## photos
    m_mainUI->pbInsertNewImage->setEnabled(state);
    m_mainUI->pbInsertNewWhiteSpace->setEnabled(state);
    m_mainUI->pbRemoveAllPhoto->setEnabled(state);
    // ## pdf
    m_mainUI->pbSavePDF->setEnabled(state);
    // # photos list
    m_mainUI->lwPhotosList->setEnabled(state);
    // # pages list
    m_mainUI->lwPagesList->setEnabled(state);
    // # document
    m_mainUI->tabWDocument->setEnabled(state);
}

void PCMainUI::set_ui_state_for_generating_pdf(bool state){
    m_mainUI->tabActions->setEnabled(state);
}

void PCMainUI::update_photos_list_style(){

    for(int ii = 0; ii < m_mainUI->lwPhotosList->count(); ++ii){

        bool individual = m_dynUI->setsLoadedW[ii]->findChild<QCheckBox*>("cbEnableIndividualConsign")->isChecked();
        QBrush brush = m_mainUI->lwPhotosList->item(ii)->foreground();
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
                m_mainUI->pbRemove->hide();
                m_mainUI->pbAdd->show();
            }else{
                m_mainUI->pbRemove->show();
                m_mainUI->pbAdd->hide();
            }
        }

        m_mainUI->lwPhotosList->item(ii)->setForeground(brush);
    }
    m_mainUI->twPhotosList->setTabText(1, QString::number(m_settings.photosValided->size()));
}

void PCMainUI::update_pages_list_style(){

    m_mainUI->twPagesList->setTabText(1, QString::number(m_pcPages.pages.size()));

    for(int ii = 0; ii < m_mainUI->lwPagesList->count(); ++ii){

        bool individual = m_dynUI->pageW[ii]->findChild<QCheckBox*>("cbEnableIndividualPage")->isChecked();

        QBrush brush =  m_mainUI->lwPagesList->item(ii)->foreground();
        if(m_pcPages.pages[ii]->sets.size() == 0){ // invalid
            brush.setColor(QRgb(qRgb(255,106,0)));
        }else if(!individual){ // global
            brush.setColor(QRgb(qRgb(0,106,255)));
        }else{ // individual
            brush.setColor(QRgb(qRgb(0,0,255)));
        }

        m_mainUI->lwPagesList->item(ii)->setForeground(brush);
    }
}


void PCMainUI::display_donate_window(){

    m_dynUI->wSupport = std::make_unique<QWidget>();
    m_dynUI->wSupport->setWindowIcon(QIcon(":/images/icon"));
    m_dynUI->wSupport->setWindowModality(Qt::ApplicationModal);
    m_dynUI->wSupport->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    Ui::SupportW support;
    support.setupUi(m_dynUI->wSupport.get());

    delete support.laPatreon;
    delete support.pbPatreon;
    delete support.hlPatreon;
    connect(support.pbPaypal, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("https://www.paypal.me/PhotosConsigne", QUrl::TolerantMode))){
            QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
        }
    });
    connect(support.pbMail, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("mailto:?to=lance.florian@protonmail.com&subject=PhotosConsigne&body=", QUrl::TolerantMode))){}
    });
    connect(support.pbReturn, &QPushButton::clicked, this, [=]{
        m_dynUI->wSupport->hide();
    });

    m_dynUI->wSupport->show();
}

void PCMainUI::from_main_UI_connections()
{
    // dyn ui elements
    // # update settings
    connect(m_dynUI.get(), &UIElements::update_settings_signal, this, &PCMainUI::update_settings);
    // # progress
    connect(m_dynUI.get(), &UIElements::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(m_dynUI.get(), &UIElements::set_progress_bar_text_signal, m_mainUI->laLoadingText, &QLabel::setText);
    // # timer
    connect(&m_dynUI->zonesTimer, &QTimer::timeout, this, [=]{
        m_dynUI->zonesTimer.stop();

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
//        update_settings();
    });

    // main ui
    // # remove all photos
    connect(m_mainUI->pbRemoveAllPhoto, &QPushButton::clicked, this, &PCMainUI::remove_all_photos);
    // # insert new image/blank
    connect(m_mainUI->pbInsertNewWhiteSpace, &QPushButton::clicked, this, &PCMainUI::insert_transparent_space);
    connect(m_mainUI->pbInsertNewImage, &QPushButton::clicked, this, &PCMainUI::load_new_photos);

    // # checkboxes
    // ## grayscale
    connect(m_mainUI->cbBAndW, &QCheckBox::clicked, this, [=](bool checked){
        m_settings.grayScale = checked;
        update_settings();
    });

    // # buttons
    // ## donate
    connect(m_mainUI->pbDonate, &QPushButton::clicked, this, &PCMainUI::display_donate_window);
    // ## help
    connect(m_mainUI->pbHelp, &QPushButton::clicked, this, [=]{

        m_dynUI->wHelp = std::make_unique<QWidget>();
        m_dynUI->wHelp->setWindowIcon(QIcon(":/images/icon"));
        m_dynUI->wHelp->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

        Ui::HelpW help;        
        help.setupUi(m_dynUI->wHelp.get());

        connect(help.pbFAQ, &QPushButton::clicked, this, [=]{
            if(!QDesktopServices::openUrl(QUrl("https://github.com/FlorianLance/PhotosConsigne/wiki/FAQ:-Foire-aux-questions", QUrl::TolerantMode))){
                QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
            }
        });
        connect(help.pbVideosTutorials, &QPushButton::clicked, this, [=]{
            if(!QDesktopServices::openUrl(QUrl("https://github.com/FlorianLance/PhotosConsigne/wiki/Tutoriels", QUrl::TolerantMode))){
                QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
            }
        });

        connect(help.pbReturn, &QPushButton::clicked, this, [=]{
            m_dynUI->wHelp->hide();
        });

        m_dynUI->wHelp->show();
    });
    // ## open PDF
    connect(m_mainUI->pbOpenPDF, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(m_pcPages.pdfFileName))){
            QMessageBox::warning(this, tr("Avertissement"), tr("Le PDF n'a pu être lancé.\nVeuillez vous assurez que vous disposez d'un logiciel de lecture de PDF (ex : AdobeReader, SumatraPDF, FoxitReader...) .\n"),QMessageBox::Ok);
        }
    });
    // ## add photos directory
    connect(m_mainUI->pbAddPhotosDIrectory, &QPushButton::clicked, this, &PCMainUI::add_photos_directory);
    // ## save PDF
    connect(m_mainUI->pbSavePDF, &QPushButton::clicked, this, [&]{
        QString filePath = QFileDialog::getSaveFileName(this, "Entrez le nom du fichier PDF", QDir::homePath(), "PDF (*.pdf)");
        if(filePath.size() > 0){
            m_pcPages.pdfFileName = filePath;
            set_ui_state_for_generating_pdf(false);
            emit start_PDF_generation_signal(m_pcPages);
        }
    });
    // ## left photo
    connect(m_mainUI->pbLeft, &QPushButton::clicked, this, [&]{ // previous image

        int currRow = m_mainUI->lwPhotosList->currentRow();
        if(currRow == -1){
            return;
        }

        int &currId = m_settings.currentPhotoId;
        if(currId == 0){
            currId = m_settings.photosLoaded->size()-1;
        } else{
            currId--;
        }

        define_selected_photo(currId);

    });
    // ## right photo
    connect(m_mainUI->pbRight, &QPushButton::clicked, this, [&]{ // next image

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        int &currId = m_settings.currentPhotoId;
        if(currId == m_settings.photosLoaded->size()-1){
            currId = 0;
        }else{
            currId++;
        }

        define_selected_photo(currId);
    });
    // ## rotate photot to the left
    connect(m_mainUI->pbRotateLeft, &QPushButton::clicked, this, [&] {// rotate left photo

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        SPhoto photo       = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation    =(photo->rotation - 90)%360;
        photo->scaledPhoto = photo->scaledPhoto.transformed(QTransform().rotate(-90));
        update_photo_to_display(photo);
    });
    // ## rotate photot to the right
    connect(m_mainUI->pbRotateRight, &QPushButton::clicked, this, [&]{ // rotate right photo

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        SPhoto photo        = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation     = (photo->rotation + 90)%360;
        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(90));
        update_photo_to_display(photo);
    });
    // ## add again the photo in the list
    connect(m_mainUI->pbAdd, &QPushButton::clicked, this, [&]{// add/remove photo from list

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        m_settings.photosLoaded.get()->at(m_settings.currentPhotoId)->isRemoved = false;

        define_selected_photo(m_settings.currentPhotoId);
        update_settings_with_no_preview();
    });
    // ## remove the photo from the list
    connect(m_mainUI->pbRemove, &QPushButton::clicked, this, [&] {// add/remove photo from list

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        SPhoto currPhoto = m_settings.photosLoaded->at(m_settings.currentPhotoId);
        currPhoto->isRemoved = true;

        int idPhotoSelected = 0;
        if(currPhoto->isADuplicate || currPhoto->isWhiteSpace){ // delete it

            // delete photo
            m_settings.photosLoaded->takeAt(m_settings.currentPhotoId);

            // block signals
            m_mainUI->lwPhotosList->blockSignals(true);
            m_mainUI->lwPhotosList->takeItem(m_settings.currentPhotoId);
            m_mainUI->lwPhotosList->blockSignals(false);

            // insert new consign in UI
            m_dynUI->remove_individual_set(m_settings.currentPhotoId);

            // define selected id photo
            idPhotoSelected = m_settings.currentPhotoId;

            int count = m_mainUI->lwPhotosList->count();
            if(idPhotoSelected >= count){
                idPhotoSelected = count-1;
            }
        }else{

            // define selected id photo
            int currRow = m_mainUI->lwPhotosList->currentRow();
            if(currRow < m_mainUI->lwPhotosList->count()-1){
                idPhotoSelected = currRow + 1;
            }else if(currRow > 0){
                idPhotoSelected = currRow - 1;
            }
        }


        define_selected_photo(idPhotoSelected);
        define_selected_set_from_current_photo();
        display_photo_panel();
        update_settings_with_no_preview();
    });
    // ## duplicate the photo in the list
    connect(m_mainUI->pbDuplicate, &QPushButton::clicked, this, [&]{

        if(m_mainUI->lwPhotosList->currentRow() == -1){
            return;
        }

        // copy current photo
        int currentIndex = m_mainUI->lwPhotosList->currentRow();
        SPhoto duplicatedPhoto = std::make_shared<Photo>(Photo(*(m_settings.photosLoaded.get()->at(currentIndex))));
        duplicatedPhoto->isRemoved    = false;
        duplicatedPhoto->isADuplicate = true;

        // insert it
        m_settings.photosLoaded->insert(currentIndex+1, duplicatedPhoto);        

        // insert new element in photo list widget
        m_mainUI->lwPhotosList->blockSignals(true);
        m_mainUI->lwPhotosList->insertItem(currentIndex+1,"(Copie) " +  duplicatedPhoto->namePhoto);
        m_mainUI->lwPhotosList->blockSignals(false);

        // insert new consign in UI
        m_dynUI->insert_individual_set(currentIndex+1);

        update_settings_with_no_preview();
    });
    // ## change photo order (minus)
    connect(m_mainUI->pbOrderMinus, &QPushButton::clicked, this, [&]{

        int row = m_mainUI->lwPhotosList->currentRow();
        if(row < 0){
            qWarning() << "-Warning: minus order photo -> bad index";
            return;
        }

        swap_loaded_set(row,row-1);

        define_selected_photo(row-1);
        define_selected_set_from_current_photo();
        display_photo_panel();        
        update_settings_with_no_preview();
    });
    // ## change photo order (plus)
    connect(m_mainUI->pbOrderPlus, &QPushButton::clicked, this, [&]{

        int row = m_mainUI->lwPhotosList->currentRow();
        if(row < 0){
            qWarning() << "-Warning: plus order photo -> bad index";
            return;
        }

        swap_loaded_set(row,row+1);

        define_selected_photo(row+1);
        define_selected_set_from_current_photo();
        display_photo_panel();       
        update_settings_with_no_preview();
    });

    // # tab widgets
     connect(m_mainUI->twMiddle, &QTabWidget::currentChanged, this, [&](int index){

        if(index == 0){
            define_selected_photo(m_settings.currentPhotoId);
            update_settings_with_no_preview();
        }else{
            update_settings();
        }
     });
     // # list widget
     // ## photos list
     connect(m_mainUI->lwPhotosList, &QListWidget::clicked, this, [&](QModelIndex index){

         m_settings.previousPhotoId = m_settings.currentPhotoId;
         if(index.row() == m_settings.currentPhotoId && index.row() < m_settings.photosLoaded->size()){
            define_selected_photo(index.row());
            define_selected_set_from_current_photo();
            define_selected_page_from_current_photo();
            update_settings_with_no_preview();
            display_photo_panel();
         }
     });
    connect(m_mainUI->lwPhotosList, &QListWidget::currentRowChanged, this, [&](int row){

        m_settings.previousPhotoId = m_settings.currentPhotoId;
        if(row < m_settings.photosLoaded->size()){
            define_selected_photo(row);            
            define_selected_set_from_current_photo();
            define_selected_page_from_current_photo();            
            update_settings_with_no_preview();
            display_photo_panel();
        }
    });
    // ## pages list
    connect(m_mainUI->lwPagesList, &QListWidget::currentRowChanged, this, [&](int row){

        m_settings.previousPageId = m_settings.currentPageId;
        m_dynUI->previewW->draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(row != -1){
            define_selected_page(row);
            display_preview_panel();
            update_settings();
        }
    });
    connect(m_mainUI->lwPagesList, &QListWidget::clicked, this, [&](QModelIndex index){

        m_settings.previousPageId = m_settings.currentPageId;
        m_dynUI->previewW->draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(index.row() != -1){
            define_selected_page(index.row());
            display_preview_panel();
            update_settings();
        }
    });

    // # spinboxes
    connect(m_mainUI->sbNbPages, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        m_dynUI->zonesTimer.start(1000);
        update_settings();
    });
    connect(m_mainUI->sbGlobalPhotosNbWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

        int nbMaxPhotos = m_mainUI->sbGlobalPhotosNbWidth->value() * m_mainUI->sbGlobalPhotosNbHeight->value();
        if(nbMaxPhotos < m_mainUI->sbGlobalPhotosNbPerPage->value()){
            Utility::safe_init_spinbox_value(m_mainUI->sbGlobalPhotosNbPerPage, nbMaxPhotos);
        }

        m_dynUI->zonesTimer.start(1000);
        update_settings();
    });
    connect(m_mainUI->sbGlobalPhotosNbHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

        int nbMaxPhotos = m_mainUI->sbGlobalPhotosNbWidth->value() * m_mainUI->sbGlobalPhotosNbHeight->value();
        if(nbMaxPhotos < m_mainUI->sbGlobalPhotosNbPerPage->value()){
            Utility::safe_init_spinbox_value(m_mainUI->sbGlobalPhotosNbPerPage, nbMaxPhotos);
        }

        m_dynUI->zonesTimer.start(1000);
        update_settings();
    });
    connect(m_mainUI->sbGlobalPhotosNbPerPage, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

        int nbMaxPhotos = m_mainUI->sbGlobalPhotosNbWidth->value() * m_mainUI->sbGlobalPhotosNbHeight->value();
        if(nbMaxPhotos < m_mainUI->sbGlobalPhotosNbPerPage->value()){
            Utility::safe_init_spinbox_value(m_mainUI->sbGlobalPhotosNbPerPage, nbMaxPhotos);
        }

        m_dynUI->zonesTimer.start(1000);
        update_settings();
    });

    // # comboBoxes
    connect(m_mainUI->cbDPI, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        update_settings();
    });
    connect(m_mainUI->cbFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        update_settings();
    });
}


void PCMainUI::from_main_module_connections(){

    // to pdf generator worker
    connect(this, &PCMainUI::kill_signal,                       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::kill);
    connect(this, &PCMainUI::start_preview_generation_signal,   m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_preview);
    connect(this, &PCMainUI::start_PDF_generation_signal,       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_PDF);
    connect(this, &PCMainUI::init_document_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::init_document);

    // to photo display worker
    connect(this, &PCMainUI::kill_signal,            m_loadPhotoWorker.get(), &PhotoLoaderWorker::kill);
    connect(this, &PCMainUI::start_loading_photos_signal, m_loadPhotoWorker.get(), &PhotoLoaderWorker::load_photos_directory);
    connect(this, &PCMainUI::stop_loading_photos_signal, m_loadPhotoWorker.get(), &PhotoLoaderWorker::kill);
}


void PCMainUI::from_pdf_generator_worker_connections(){

    PDFGeneratorWorker *worker = m_pdfGeneratorWorker.get();

    // to this
    connect(worker, &PDFGeneratorWorker::end_preview_signal, this, [=](QImage previewImage){
        m_dynUI->previewW->set_image(previewImage);
        m_dynUI->previewW->update();

        display_preview_panel();

        m_mainUI->pbSavePDF->setEnabled(true);

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
            m_mainUI->pbOpenPDF->setEnabled(true);
            m_mainUI->laLoadingText->setText("PDF généré avec succès\n" + m_pcPages.pdfFileName);
        }else{
            m_mainUI->laLoadingText->setText("Echec génération du PDF");
        }

        set_ui_state_for_generating_pdf(true);
    });
    connect(worker, &PDFGeneratorWorker::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PDFGeneratorWorker::set_progress_bar_text_signal,  m_mainUI->laLoadingText, &QLabel::setText);
    connect(worker, &PDFGeneratorWorker::abort_pdf_signal,              this, [=](QString pathFile){

        QMessageBox::warning(this, tr("Avertissement"), tr("Le fichier PDF ") + pathFile + tr(" n'a pu être écrit, il se peut que celui-ci soit en cours d'utilisation par un autre logiciel."),QMessageBox::Ok);
        m_mainUI->laLoadingText->setText("Echec génération du PDF");
        set_ui_state_for_generating_pdf(true);
    });
    connect(worker, &PDFGeneratorWorker::current_pc_selected_signal, this, [=](QRectF pcRectRelative, int totalIdPC){

        if(totalIdPC == -1){
            display_header_panel();
            m_dynUI->previewW->draw_current_pc_rect(totalIdPC, pcRectRelative);
            m_dynUI->previewW->update();
            return;
        }else if(totalIdPC == -2){
            display_footer_panel();
            m_dynUI->previewW->draw_current_pc_rect(totalIdPC, pcRectRelative);
            m_dynUI->previewW->update();
            return;
        }

        m_settings.currentSetId = totalIdPC;
        m_dynUI->previewW->draw_current_pc_rect(totalIdPC, pcRectRelative);
        m_dynUI->previewW->update();

        // update photo list selection
        define_selected_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId);
        update_settings_with_no_preview();
    });
}

void PCMainUI::from_photos_loader_worker_connections(){

    PhotoLoaderWorker *worker = m_loadPhotoWorker.get();

    // # progress bar
    connect(worker, &PhotoLoaderWorker::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(worker, &PhotoLoaderWorker::set_progress_bar_text_signal,  m_mainUI->laLoadingText, &QLabel::setText);
    // # end loading photo
    connect(worker, &PhotoLoaderWorker::photo_loaded_signal,this, [&](SPhoto photo, int indexToInsert){

        // add row in photos list
        m_mainUI->lwPhotosList->insertItem(indexToInsert, photo->namePhoto);
        m_mainUI->twPhotosList->setTabText(1, QString::number(indexToInsert));

        // add loaded photo
        m_settings.photosLoaded->insert(indexToInsert, photo);
        m_dynUI->insert_individual_set(indexToInsert);

        // update current photo row
        m_settings.currentPhotoId = indexToInsert;
        m_mainUI->lwPhotosList->setCurrentRow(m_settings.currentPhotoId);
    });
    // # end loading all photos
    connect(worker, &PhotoLoaderWorker::end_loading_photos_signal,this, [&]{

        // update progress bar
        m_mainUI->laLoadingText->setText("Photos chargées.");
        m_mainUI->progressBarLoading->setValue(1000);

        if( m_settings.photosLoaded->size() > 0){

            m_mainUI->twMiddle->setTabEnabled(0, true);
            m_settings.currentPhotoId = 0;
            update_photo_to_display(m_settings.photosLoaded->at(m_settings.photosLoaded->size()-1));
            display_photo_panel();
        }

        // unlock ui        
        m_isLoadingPhotos = false;
        set_ui_state_for_adding_photos(true);

        update_settings();
    });
}

void PCMainUI::from_UI_elements_connections(){

    PreviewW* previewUI    = m_dynUI->previewW;
    PhotoW* displayPhotoUI = m_dynUI->selectedPhotoW;

    // # UI elements
    // ### update settings request
    connect(m_dynUI->globalSetTextW.textEdit(), &TextEdit::textChanged, this, &PCMainUI::update_settings);
    // ### resource added
    connect(m_dynUI.get(), &UIElements::resource_added_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::add_resource);

    // # preview label
    // ### single click
    connect(previewUI, &PreviewW::click_on_page_signal, this, [&]{
        display_individual_consign_settings_panel();
    });
    connect(previewUI, &PreviewW::click_on_page_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::update_PC_selection);
    // ### double click
    connect(previewUI, &PreviewW::double_click_on_photo_signal, this, [&]{
        if(m_settings.currentSetId < m_settings.photosValided->size()){
            define_selected_photo(m_settings.photosValided->at(m_settings.currentSetId)->loadedId);
            update_settings_with_no_preview();
            display_photo_panel();
        }
    });

    // # image label
    // ### double click
    connect(displayPhotoUI, &PhotoW::double_click_signal, this, [&]{
        define_selected_page_from_current_photo();
        define_selected_set_from_current_photo();
        update_settings();
        display_preview_panel();
    });
}

void PCMainUI::retrieve_global_settings_from_ui(){

    // nb of photos to be used (== nb of pc)
    auto nbPhotosValid          = m_settings.photosValided->size();

    // retrieve global parameters
    // # global nb photos
    m_settings.nbPhotosPageWidth    = m_mainUI->sbGlobalPhotosNbWidth->value();
    m_settings.nbPhotosPageHeight   = m_mainUI->sbGlobalPhotosNbHeight->value();
    m_settings.nbPhotosPerPage      = m_mainUI->sbGlobalPhotosNbPerPage->value();

    // # retrieve nb of pages
    if(!m_settings.resetPages){
        m_settings.nbPages = m_mainUI->sbNbPages->value();
    }else{

        // # compute last page photo nb
        int lastPagePhotosNb = m_settings.photosValided->size() % m_settings.nbPhotosPerPage;
        if(lastPagePhotosNb == 0){
            lastPagePhotosNb = m_settings.nbPhotosPerPage;
        }

        m_settings.nbPages = nbPhotosValid / m_settings.nbPhotosPerPage;
        if(lastPagePhotosNb != 0){
            ++m_settings.nbPages;
        }

        m_mainUI->sbNbPages->blockSignals(true);
        m_mainUI->sbNbPages->setValue(m_settings.nbPages);
        m_mainUI->sbNbPages->blockSignals(false);
        m_settings.resetPages = false;
    }

    // # only global
    m_settings.saveOnlyCurrentPage  = m_mainUI->cbSaveOnlyCurrentPage->isChecked();
    m_settings.PCRatio              = m_dynUI->globalSetStyleW.ui.dsbRatioPC->value();
    m_settings.orientation          = (m_mainUI->cbOrientation->currentIndex() == 0) ? PageOrientation::portrait : PageOrientation::landScape;

    // # set
    // ## text
    m_settings.consignText = m_dynUI->globalSetTextW.html();
    // ## set style
    m_settings.photoAlignment               = Utility::photo_alignment_from_comboBox(m_dynUI->globalSetStyleW.ui.cbPhotoAlignment);
    m_settings.photoAdust                   = Utility::photo_adjust_from_comboBox(m_dynUI->globalSetStyleW.ui.cbAdjustPhoto);
    m_settings.consignPositionFromPhotos    = static_cast<Position>(m_dynUI->globalSetStyleW.ui.cbPositionConsign->currentIndex());

    // # page
    // ## margins
    m_settings.marginsSettings              = m_dynUI->globalMarginsW.settings();
    // ## borders
    m_settings.bordersSettings              = m_dynUI->globalBordersW.settings();
    // ## background
    m_settings.backgroundSettings           = m_dynUI->globalBackgroundW.settings();

    // # header
    // ## enabled
    m_settings.headerSettings.enabled       = m_dynUI->headerW.ui.cbEnableHeader->isChecked();
    // ## background
    m_settings.headerSettings.background    = m_dynUI->headerW.headeBackgroundW.settings();
    // ## text
    m_settings.headerSettings.text          = m_dynUI->headerW.headerTextW.html();
    // ## style
    m_settings.headerSettings.ratio         = m_dynUI->headerW.sectionStyleW.ui.dsbRatioSection->value();

    // # footer
    // ## enabled
    m_settings.footerSettings.enabled       = m_dynUI->footerW.ui.cbEnableFooter->isChecked();
    // ## background
    m_settings.footerSettings.background    = m_dynUI->footerW.footerBackgroundW.settings();
    // ## text
    m_settings.footerSettings.text          = m_dynUI->footerW.footerTextW.html();
    // ## style
    m_settings.footerSettings.ratio         = m_dynUI->footerW.sectionStyleW.ui.dsbRatioSection->value();
}

void PCMainUI::update_settings(){

    DebugMessage dbgMess("update_settings");

    // reconstruct sets
    build_valid_sets();

    // update global settings
    retrieve_global_settings_from_ui();

    // reconstruct the pages from global and individual settings
    build_pages();

    // update defintion txt
    bool landScape = m_settings.orientation == PageOrientation::landScape;
    int currentDPI = m_mainUI->cbDPI->currentText().toInt();
    m_mainUI->laDefinition->setText("<p><b>Définition: </b> " + QString::number(m_pcPages.paperFormat .width_pixels(currentDPI))+ "x" +
                                    QString::number(m_pcPages.paperFormat .height_pixels(currentDPI)) +
                                    " pixels <br /><b>Dimensions: </b> " +
                                    QString::number(landScape ? m_pcPages.paperFormat.sizeMM.height() : m_pcPages.paperFormat.sizeMM.width()) + "x" +
                                    QString::number(landScape ? m_pcPages.paperFormat.sizeMM.width() : m_pcPages.paperFormat.sizeMM.height()) + " mm</p>");


    // update size margins mm
    m_dynUI->globalMarginsW.update_mm_values((m_settings.orientation == PageOrientation::landScape), m_pcPages.paperFormat);

    // update stylesheet of lists
    update_photos_list_style();
    update_pages_list_style();

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
        emit start_preview_generation_signal(m_pcPages, m_settings.currentPageId, m_dynUI->zonesTimer.isActive());
        display_preview_panel();
    }
}

void PCMainUI::display_photo_panel(){
    Utility::safe_init_tab_widget_index(m_mainUI->twMiddle, 0);
}

void PCMainUI::display_preview_panel(){
    Utility::safe_init_tab_widget_index(m_mainUI->twMiddle, 1);
}

void PCMainUI::display_global_consign_panel(){
    Utility::safe_init_tool_box_index(m_mainUI->tbRight, 1);
}

void PCMainUI::display_individual_consign_settings_panel(){    
    Utility::safe_init_tool_box_index(m_mainUI->tbRight, 4);
}

void PCMainUI::display_individual_page_settings_panel(){
    Utility::safe_init_tool_box_index(m_mainUI->tbRight, 3);
}

void PCMainUI::display_header_panel(){
    Utility::safe_init_tool_box_index(m_mainUI->tbRight, 6);
}

void PCMainUI::display_footer_panel(){
    Utility::safe_init_tool_box_index(m_mainUI->tbRight, 7);
}





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

PCMainUI::PCMainUI(QApplication *parent) : m_version("3.0"), m_mainUI(QSharedPointer<Ui::PhotosConsigneMainW>(new Ui::PhotosConsigneMainW())), m_dynUI(std::make_unique<UIElements>(m_mainUI)){

    // init ui
    Q_UNUSED(parent)
    init_misc_elements();
    init_rich_text_edit_widgets();
    init_images_labels_widgets();

    // init workers
    m_displayPhotoWorker = std::make_unique<PhotoDisplayWorker>();
    m_pdfGeneratorWorker = std::make_unique<PDFGeneratorWorker>();

    // connections    
    define_workers_connections();
    define_main_UI_connections();

    // init threads
    m_displayPhotoWorker->moveToThread(&m_displayPhotoWorkerThread);
    m_displayPhotoWorkerThread.start();
    m_pdfGeneratorWorker->moveToThread(&m_pdfGeneratorWorkerThread);
    m_pdfGeneratorWorkerThread.start();

    // update settings with current UI
    update_settings_with_no_preview();
}

PCMainUI::~PCMainUI()
{
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

void PCMainUI::swap_loaded_pc(int currentIndex, int newIndex){

    m_settings.photosLoaded->swap(currentIndex,newIndex);
    m_dynUI->individualConsignsTEditLoaded.insert(newIndex, m_dynUI->individualConsignsTEditLoaded.takeAt(currentIndex));
    m_dynUI->individualConsignsWLoaded.insert(newIndex, m_dynUI->individualConsignsWLoaded.takeAt(currentIndex));

    m_mainUI->lwPhotosList->blockSignals(true);{
        m_mainUI->lwPhotosList->insertItem(newIndex,m_mainUI->lwPhotosList->takeItem(currentIndex));
        m_mainUI->lwPhotosList->setCurrentRow(newIndex);
    }m_mainUI->lwPhotosList->blockSignals(false);

    // rest global ids
    for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){
        m_settings.photosLoaded->at(ii)->globalId = ii;
    }
}



void PCMainUI::set_photos_directory()
{
    QString previousDirectory = m_settings.photosDirectoryPath;
    m_settings.photosDirectoryPath = QFileDialog::getExistingDirectory(this, "Sélection du répertoire d'images", QDir::homePath());

    // no directory selected
    if(m_settings.photosDirectoryPath.size() == 0 ){
        if(previousDirectory.size() != 0){
            m_settings.photosDirectoryPath = previousDirectory; // retrieve precedent path
        }
        return;
    }

    // retrieve dir
    QDir dir(m_settings.photosDirectoryPath);
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
            m_settings.photosDirectoryPath = previousDirectory; // retrieve precedent path
        }

        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune image (jpg,png) n'a pu être trouvée dans ce répertoire, veuillez en selectionner un autre.\n"),QMessageBox::Ok);
        return;
    }
    // path is valid

    // reset pages
    m_mainUI->lwPagesList->clear();
    m_mainUI->lwPagesList->setEnabled(false);
    // reset photos
    m_mainUI->lwPhotosList->clear();
    m_mainUI->lwPhotosList->setEnabled(false);
    // update photos & pages displayed number
    m_mainUI->twPhotosList->setTabText(1, QString::number(0));
    m_mainUI->twPagesList->setTabText(1, QString::number(0));

    // update button and disable it during the loading
    QString styleSheet ="QPushButton{color :rgb(0,106,255); border-style: outset;font-style: normal;font-size: 8pt;background-color:  rgb(220,220,220);} \
                         QPushButton:pressed {background-color: white;background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #f6f7fa, stop: 1 #dadbde);} \
                         QPushButton:hover:!pressed {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa);}";
    QString buttonText = m_settings.photosDirectoryPath;
    if(buttonText.size() > 30){

        int indexMid = buttonText.indexOf('/', buttonText.size()/2);
        if(indexMid > 0){
            buttonText = buttonText.insert(indexMid,'\n');
        }
    }
    m_mainUI->pbChooseDirectory->setMinimumWidth(0);
    m_mainUI->pbChooseDirectory->setStyleSheet(styleSheet);
    m_mainUI->pbChooseDirectory->setText(buttonText);
    m_mainUI->pbChooseDirectory->setEnabled(false);

    // send directoy and files names to the worker to be loaded
    emit send_photos_dir_signal(m_settings.photosDirectoryPath, fileList);
}


void PCMainUI::update_settings_with_no_preview(){

    m_settings.noPreviewGeneration = true;
    update_settings();
    m_settings.noPreviewGeneration = false;
}

void PCMainUI::update_valid_photos()
{
    // build valid arrays
    auto nbPhotosLoaded = m_settings.photosLoaded->size();

    m_settings.photosValided->clear();
    m_settings.photosValided->reserve(nbPhotosLoaded);
    m_dynUI->individualConsignsTEditValided.clear();
    m_dynUI->individualConsignsTEditValided.reserve(nbPhotosLoaded);
    m_dynUI->individualConsignsWValided.clear();
    m_dynUI->individualConsignsWValided.reserve(nbPhotosLoaded);

    for(int ii = 0; ii < nbPhotosLoaded; ++ii){
        if(!m_settings.photosLoaded->at(ii)->isRemoved){

            m_settings.photosValided->push_back(m_settings.photosLoaded->at(ii));
            m_dynUI->individualConsignsTEditValided.push_back(m_dynUI->individualConsignsTEditLoaded.at(ii));
            m_dynUI->individualConsignsWValided.push_back(m_dynUI->individualConsignsWLoaded.at(ii));
        }
    }

    // update current PD displayed if necessary
    auto nbPhotosValid = m_settings.photosValided->size();
    if(m_settings.currentPCDisplayed > nbPhotosValid){
        m_settings.currentPCDisplayed = nbPhotosValid-1;
    }

    // update number of individual text edit widgets    
    if(m_settings.currentPCDisplayed > 0){

        define_selected_pc(m_settings.currentPCDisplayed);
        display_individual_consign_settings_panel();
    }    
}


void PCMainUI::update_pages()
{
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

    // create new pages
    int currentPhotoId = 0;
    for(int ii = 0; ii < m_settings.nbPages; ++ii){

        SPCPage pcPage = std::make_shared<PCPage>(PCPage());

        // define current nb of photos for this page
        auto pageW = m_dynUI->individualPageW[ii];
        bool individualPageSettings = pageW->findChild<QCheckBox*>("cbEnableInvididualPageSettings")->isChecked();
        int nbPhotosPage = individualPageSettings ? (pageW->findChild<QSpinBox*>("sbVSizeGrid")->value()*pageW->findChild<QSpinBox*>("sbHSizeGrid")->value()) : m_settings.nbPhotosPerPage;

        // misc
        pcPage->id = ii;


        // sets
        pcPage->sets.reserve(nbPhotosPage);
        for(int jj = 0; jj < nbPhotosPage; ++jj){

            SPCSet set = std::make_shared<PCSet>(PCSet());
            set->id      = jj;
            set->totalId = currentPhotoId;
            if(currentPhotoId < m_settings.photosValided->size()){
                set->photo = m_settings.photosValided->at(currentPhotoId);
                set->photo->isOnDocument = true; // photo will be on the generated document
                set->consign = std::make_shared<Consign>(Consign());
            }else{
                break;
            }

            // retrieve individual consign
            auto wC = m_dynUI->individualConsignsWValided[currentPhotoId];
            bool individualConsignSettings = wC->findChild<QCheckBox*>("cbEnableIndividualConsign")->isChecked();

            if(!individualConsignSettings){ // global consign
                set->consignPositionFromPhoto   = m_settings.consignPositionFromPhotos;
                set->ratio                      = m_settings.PCRation;
                set->consignOnPhoto             = m_settings.consignOnPhoto;
                set->consign->doc               = m_settings.consignDoc->document();
                set->photo->alignment           = m_settings.photoAlignment;
            }else{ // individual consign

                set->consignPositionFromPhoto = ((!wC->findChild<QPushButton *>("pbConsignTop")->isEnabled())    ? Position::top    :
                                                ((!wC->findChild<QPushButton *>("pbConsignBottom")->isEnabled()) ? Position::bottom :
                                                ((!wC->findChild<QPushButton *>("pbConsignLeft")->isEnabled())   ? Position::left   : Position::right)));
                set->ratio          = wC->findChild<QDoubleSpinBox*>("dsbRatioPC")->value();
                set->consignOnPhoto = wC->findChild<QCheckBox *>("cbWriteOnPhoto")->isChecked();
                set->consign->doc   = m_dynUI->individualConsignsTEditValided[currentPhotoId]->textEdit()->document();

                // image alignment
                int individualPhotosAlignment = 0;
                if(!wC->findChild<QPushButton *>("pbImageAligmentHMiddle")->isEnabled()){ // horizontal center
                    individualPhotosAlignment = individualPhotosAlignment |  Qt::AlignHCenter;
                }else if(!wC->findChild<QPushButton *>("pbImageAligmentRight")->isEnabled()){ // horizontal right
                    individualPhotosAlignment = individualPhotosAlignment |  Qt::AlignRight;
                }else{ // horizontal left
                    individualPhotosAlignment = individualPhotosAlignment |  Qt::AlignLeft;
                }

                if(!wC->findChild<QPushButton *>("pbImageAligmentVMiddle")->isEnabled()){// vertical center
                    individualPhotosAlignment = individualPhotosAlignment | Qt::AlignVCenter;
                }else if(!wC->findChild<QPushButton *>("pbImageAligmentTop")->isEnabled()){// vertical top
                    individualPhotosAlignment = individualPhotosAlignment | Qt::AlignTop;
                }else{
                    individualPhotosAlignment = individualPhotosAlignment | Qt::AlignBottom;
                }
                set->photo->alignment = individualPhotosAlignment;
            }

            pcPage->sets.push_back(set);
            currentPhotoId++;
        }

        // margins
        if(individualPageSettings){
            pc::RatioMargins &margins       = pcPage->margins;
            margins.exteriorMarginsEnabled  = pageW->findChild<QCheckBox*>("cbAddExteriorMargins")->isChecked();
            margins.interiorMarginsEnabled  = pageW->findChild<QCheckBox*>("cbAddInteriorMargins")->isChecked();
            margins.left                    = pageW->findChild<QDoubleSpinBox*>("dsbLeftMargins")->value();
            margins.right                   = pageW->findChild<QDoubleSpinBox*>("dsbRightMargins")->value();
            margins.top                     = pageW->findChild<QDoubleSpinBox*>("dsbTopMargins")->value();
            margins.bottom                  = pageW->findChild<QDoubleSpinBox*>("dsbBottomMargins")->value();
            margins.interWidth              = pageW->findChild<QDoubleSpinBox*>("dsbHorizontalMargins")->value();
            margins.interHeight             = pageW->findChild<QDoubleSpinBox*>("dsbVerticalMargins")->value();

            pcPage->nbPhotosH = pageW->findChild<QSpinBox*>("sbHSizeGrid")->value();
            pcPage->nbPhotosV = pageW->findChild<QSpinBox*>("sbVSizeGrid")->value();

        }else{
            pcPage->margins   = m_settings.margins;
            pcPage->nbPhotosH = m_settings.nbPhotosPageH;
            pcPage->nbPhotosV = m_settings.nbPhotosPageV;
        }

        // title
        if(m_settings.titleAdded && (pcPage->id == 0 || m_settings.titleOnAllPages)){
            if(pcPage->title == nullptr){
                pcPage->title = std::make_shared<Title>(Title());
            }
            pcPage->title->doc     = m_settings.titleDoc->document();
            pcPage->titlePositionFromPC = m_settings.titlePositionFromPC;
            pcPage->ratioWithTitle      = m_settings.ratioTitle;
        }else{
            pcPage->title = nullptr;
        }

        // misc
        pcPage->displayCutLines = m_settings.displayCutLines;

        // add page to pages list widget and update the ui
        m_pcPages.pages.push_back(pcPage);        
        m_mainUI->lwPagesList->addItem(QString::number(ii+1) + ". " + QString::number(pcPage->sets.size()) + ((pcPage->sets.size() > 1 ) ? " images." : " image."));
    }

    // update current row
    if(m_settings.currentPageId != -1){
        if(m_settings.currentPageId >= m_settings.nbPages){
            m_settings.currentPageId = m_settings.nbPages-1;
        }
        m_mainUI->lwPagesList->setCurrentRow(m_settings.currentPageId);
    }

    // update selecte page settings
    if(m_settings.currentPageId >= 0){// && m_settings.currentPageId != m_settings.previousPageId){
        define_current_individual_page_ui();
    }

    if(m_settings.currentPageId == -1){
        m_mainUI->lwPagesList->setCurrentRow(0);
        m_settings.currentPageId = 0;
    }

    // unlock signals
    m_mainUI->lwPagesList->blockSignals(false);
    m_mainUI->twMiddle->blockSignals(false);
}


void PCMainUI::define_current_individual_page_ui(){

    int currentIndex = m_mainUI->tbRight->currentIndex();
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->removeItem(2); // remove current pc tab
    m_mainUI->tbRight->insertItem(2,m_dynUI->individualPageW[m_settings.currentPageId].get(), "PAGE SELECTIONNEE N°" + QString::number(m_settings.currentPageId));
    m_mainUI->tbRight->setCurrentIndex(currentIndex);
    m_mainUI->tbRight->blockSignals(false);
}

void PCMainUI::update_photo_to_display(SPhoto photo)
{
    if(!photo->isWhiteSpace){
        m_dynUI->selectedPhotoW->set_image(QImage(photo->pathPhoto).transformed(QTransform().rotate(photo->rotation)));
        m_dynUI->selectedPhotoW->update();
    }
}

void PCMainUI::define_selected_photo(int index){

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
        update_photo_to_display(m_settings.photosLoaded.get()->at(m_settings.currentPhotoId));
    }

    m_mainUI->lwPhotosList->blockSignals(true);
    m_mainUI->lwPhotosList->setCurrentRow(index);
    m_mainUI->lwPhotosList->blockSignals(false);

    update_photos_list_style();
}

void PCMainUI::define_selected_pc(int index){

    m_settings.currentPCDisplayed = index;

    int currentIndex = m_mainUI->tbRight->currentIndex();
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->removeItem(3); // remove current pc tab
    m_mainUI->tbRight->addItem(m_dynUI->individualConsignsWValided[m_settings.currentPCDisplayed].get(), "ENSEMBLE PHOTO-CONSIGNE SELECTIONNE N°" + QString::number(m_settings.currentPCDisplayed));
    m_mainUI->tbRight->setCurrentIndex(currentIndex);
    m_mainUI->tbRight->blockSignals(false);

}

void PCMainUI::define_selected_pc_from_current_photo()
{
    for(int ii = 0; ii < m_settings.photosValided->size(); ++ii){
        if(m_settings.photosValided->at(ii)->globalId ==  m_settings.currentPhotoId){
            define_selected_pc(ii);
            break;
        }
    }
}

void PCMainUI::define_selected_page(int index){

    m_settings.currentPageId = index;
    m_mainUI->lwPagesList->blockSignals(true);
    m_mainUI->lwPagesList->setCurrentRow(m_settings.currentPageId);
    m_mainUI->lwPagesList->blockSignals(false);

    define_current_individual_page_ui();
}

void PCMainUI::define_selected_page_from_current_photo(){

    int idPage = -1;
    for(const auto &page : m_pcPages.pages){

        for(const auto &set : page->sets){
            if(set->photo->globalId == m_settings.photosLoaded->at(m_settings.currentPhotoId)->globalId){
                idPage = page->id;
                break;
            }
        }
        if(idPage != -1){
            break;
        }
    }

    if(idPage != -1){
        define_selected_page(idPage);
    }
}


void PCMainUI::init_misc_elements(){

    // use designer ui
    m_mainUI->setupUi(this);

    // set icon/title
    setWindowTitle("PhotosConsigne " + m_version + " (générateur de PDF à partir de photos)");
    setWindowIcon(QIcon(":/images/icon"));

    // disable textes info tab
    m_mainUI->twPhotosList->setTabEnabled(1,false);
    m_mainUI->twPagesList->setTabEnabled(1,false);

    m_mainUI->tbRight->setItemEnabled(2, false); // disable current page tab
    m_mainUI->tbRight->setItemEnabled(3, false); // disable current pc tab
    display_global_consign_panel();

    // init definition
    PaperFormat pf(m_mainUI->cbDPI->currentText(),m_mainUI->cbFormat->currentText());
    int currentDPI = m_mainUI->cbDPI->currentText().toInt();
    m_mainUI->laDefWxH->setText(QString::number(pf.widthPixels(currentDPI)) + "x" + QString::number(pf.heightPixels(currentDPI)));
    m_mainUI->laDefTotal->setText("(" + QString::number(pf.widthPixels(currentDPI)*pf.heightPixels(currentDPI)) + " pixels)");
}

void PCMainUI::init_images_labels_widgets(){

    // init photo display widgets
    QHBoxLayout *selectedPhotoLayout = new QHBoxLayout();
    m_mainUI->wSelectedPhotoW->setLayout(selectedPhotoLayout);
    m_dynUI->selectedPhotoW = new ImageLabel();
    selectedPhotoLayout->addWidget(m_dynUI->selectedPhotoW);
    m_mainUI->twMiddle->setTabEnabled(0, false);
    m_mainUI->pbAdd->hide();

    // init preview image widget
    m_dynUI->previewW = new PreviewLabel();
    m_mainUI->vlImagePreview->addWidget(m_dynUI->previewW);
}

void PCMainUI::init_rich_text_edit_widgets(){

    // init text edit widgets
    //  title
    m_dynUI->titleTEdit = new RichTextEdit();
    m_dynUI->titleTEdit->set_doc_locker(&m_dynUI->docLocker);
    m_mainUI->hlTitleBottom->addWidget(m_dynUI->titleTEdit);
    m_dynUI->titleTEdit->setEnabled(false);
    m_dynUI->titleTEdit->init_as_title();
    m_settings.titleDoc = m_dynUI->titleTEdit->textEdit();

    //  global consign
    m_dynUI->globalConsignTEdit = new RichTextEdit();
    m_dynUI->globalConsignTEdit->set_doc_locker(&m_dynUI->docLocker);
    m_dynUI->globalConsignTEdit->init_as_consign();
    m_mainUI->vlGlobalConsign->addWidget(m_dynUI->globalConsignTEdit);
    m_settings.consignDoc =  m_dynUI->globalConsignTEdit->textEdit();
}

void PCMainUI::enable_ui()
{
    m_mainUI->lwPhotosList->setEnabled(true);
    m_mainUI->lwPagesList->setEnabled(true);
    m_mainUI->pbChooseDirectory->setEnabled(true);

    m_mainUI->tbRight->setItemEnabled(2, true); // enable current page tab
    m_mainUI->tbRight->setItemEnabled(3, true); // enable ucrrent pc tab
}

void PCMainUI::update_photos_list_style()
{
    for(int ii = 0; ii < m_mainUI->lwPhotosList->count(); ++ii){

        bool individual = m_dynUI->individualConsignsWLoaded[ii]->findChild<QCheckBox*>("cbEnableIndividualConsign")->isChecked();

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

void PCMainUI::update_pages_list_style()
{
    m_mainUI->twPagesList->setTabText(1, QString::number(m_pcPages.pages.size()));

    QBrush valid;
    valid.setColor(QRgb(qRgb(0,106,255)));
    QBrush invalid;
    invalid.setColor(QRgb(qRgb(255,106,0)));
    for(int ii = 0; ii < m_mainUI->lwPagesList->count(); ++ii){
        m_mainUI->lwPagesList->item(ii)->setForeground((m_pcPages.pages[ii]->sets.size() == 0) ? invalid : valid);
    }
}


void PCMainUI::display_donate_window(){

    m_dynUI->wSupport = std::make_unique<QWidget>();
    m_dynUI->wSupport->setWindowIcon(QIcon(":/images/icon"));

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

void PCMainUI::define_workers_connections()
{
    // ui -> pdf generator worker
    connect(this, &PCMainUI::kill_signal,                       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::kill);
    connect(this, &PCMainUI::start_preview_generation_signal,   m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_preview);
    connect(this, &PCMainUI::start_PDF_generation_signal,       m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::generate_PDF);

    // pdf generator worker -> ui
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::end_preview_signal, this, [=](QImage previewImage){
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
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::end_generation_signal, this, [=](bool success){

        m_mainUI->pbSavePDF->setEnabled(true);
        if(success){
            m_mainUI->pbOpenPDF->setEnabled(true);
            m_mainUI->laLoadingText->setText("PDF généré avec succès\n" + m_pcPages.pdfFileName);
        }else{
            m_mainUI->laLoadingText->setText("Echec génération du PDF");
        }
    });
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::set_progress_bar_text_signal,  m_mainUI->laLoadingText, &QLabel::setText);
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::abort_pdf_signal,              this, [=](QString pathFile){

        QMessageBox::warning(this, tr("Avertissement"), tr("Le fichier PDF ") + pathFile + tr(" n'a pu être écrit, il se peut que celui-ci soit en cours d'utilisation par un autre logiciel."),QMessageBox::Ok);
        m_mainUI->laLoadingText->setText("Echec génération du PDF");
        m_mainUI->pbSavePDF->setEnabled(true);
    });

    // pdf worker -> preview worker
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::current_pc_selected_signal, this, [=](QRectF pcRectRelative, int totalIdPC){

        if(totalIdPC == -1){
            display_title_panel();
        }else{
            define_selected_pc(totalIdPC);
        }

        m_dynUI->previewW->draw_current_pc_rect(totalIdPC, pcRectRelative);
        m_dynUI->previewW->update();

        // update photo list selection
        define_selected_photo(m_settings.photosValided->at(m_settings.currentPCDisplayed)->globalId);
    });

    // preview label -> ui
    connect(m_dynUI->previewW, &PreviewLabel::double_click_on_photo_signal, this, [&]{
        define_selected_photo(m_settings.photosValided->at(m_settings.currentPCDisplayed)->globalId);
        display_photo_panel();
    });
    // preview label -> pdf worker
    connect(m_dynUI->previewW, &PreviewLabel::click_on_page_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::update_PC_selection);
    connect(m_dynUI->previewW, &PreviewLabel::click_on_page_signal, this, [&]{
        display_individual_consign_settings_panel();
    });

    // image label -> ui
    connect(m_dynUI->selectedPhotoW, &ImageLabel::double_click_signal, this, [&]{

        define_selected_page_from_current_photo();
        define_selected_pc_from_current_photo();
        display_preview_panel();
    });

    // ui -> photo display worker
    connect(this, &PCMainUI::kill_signal,            m_displayPhotoWorker.get(), &PhotoDisplayWorker::kill);
    connect(this, &PCMainUI::send_photos_dir_signal, m_displayPhotoWorker.get(), &PhotoDisplayWorker::load_photos);
    // photo display worker -> ui
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::set_progress_bar_text_signal,  m_mainUI->laLoadingText, &QLabel::setText);
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::end_loading_photos_signal,     this, [&](SPhotos photos){

        // reset loaded photos
        m_settings.photosLoaded = photos;

        // reset individuals consigns
        m_dynUI->reset_individual_consigns(m_settings.photosLoaded->size());

        // update ui
        enable_ui();
        update_valid_photos();

        m_settings.resetPages = true;
        update_settings();

        // end loading
        m_mainUI->laLoadingText->setText("Photos chargées.");
        m_mainUI->progressBarLoading->setValue(1000);
    });
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::photo_loaded_signal,this, [&](QString image){

        m_mainUI->lwPhotosList->addItem(QString::number(m_mainUI->lwPhotosList->count()+1) + ". " + image);
        m_mainUI->twPhotosList->setTabText(1, QString::number(m_mainUI->lwPhotosList->count()+1));

        if(m_mainUI->lwPhotosList->count() == 1)
            m_mainUI->lwPhotosList->setCurrentRow(0);
    });
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::end_loading_photos_signal, [&](SPhotos photos){
        m_settings.photosLoaded = photos;

        if( m_settings.photosLoaded->size() > 0){

            m_mainUI->twMiddle->setTabEnabled(0, true);                        
            m_settings.currentPhotoId = 0;            
            update_photo_to_display(photos->at(0));
            display_photo_panel();
        }
    });
}

void PCMainUI::define_main_UI_connections()
{
    // dyn ui elements
    connect(m_dynUI.get(), &UIElements::update_settings_signal, this, &PCMainUI::update_settings);
    connect(m_dynUI.get(), &UIElements::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(m_dynUI.get(), &UIElements::set_progress_bar_text_signal, m_mainUI->laLoadingText, &QLabel::setText);
    connect(m_dynUI.get(), &UIElements::insert_white_space_signal, this, [&](){

        int indexPhoto = m_settings.currentPhotoId;

        // create white photo
        SPhoto whiteSpacePhoto = std::make_shared<Photo>(Photo("", true));

        // insert it
        m_settings.photosLoaded->insert(indexPhoto, whiteSpacePhoto);

        // rest global ids
        for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){
            m_settings.photosLoaded->at(ii)->globalId = ii;
        }

        // insert new element in photo list widget
        m_mainUI->lwPhotosList->blockSignals(true);
        m_mainUI->lwPhotosList->insertItem(indexPhoto,"(Espace blanc)");
        m_mainUI->lwPhotosList->blockSignals(false);

        // insert new consign in UI
        m_dynUI->insert_individual_consign(indexPhoto);

        update_valid_photos();
        update_settings();

        define_selected_photo(indexPhoto);
        define_selected_pc_from_current_photo();
    });

    // timer
    connect(&m_dynUI->zonesTimer, &QTimer::timeout, this, [=]{
        m_dynUI->zonesTimer.stop();
        update_settings();
    });

    // page color
    QPixmap pixColor(16, 16);
    pixColor.fill(Qt::white);
    QAction *actionPageColor = new QAction(pixColor, tr("Couleur de la page"), this);
    connect(actionPageColor, &QAction::triggered, this, [=]{

        QColor col = QColorDialog::getColor(m_settings.pageColor, this, "Choix de la couleur de page");
        if (!col.isValid())
            return;

        QPixmap pix(16, 16);
        pix.fill(col);
        actionPageColor->setIcon(pix);
        m_settings.pageColor = col;
        update_settings();
    });
    m_mainUI->tbColorPage->setDefaultAction(actionPageColor);

    // grayscale
    connect(m_mainUI->cbBAndW, &QCheckBox::clicked, this, [=](bool checked){
        m_settings.grayScale = checked;
        update_settings();
    });

    // # push_button
    connect(m_mainUI->pbDonate, &QPushButton::clicked, this, &PCMainUI::display_donate_window);

    connect(m_mainUI->pbHelp, &QPushButton::clicked, this, [=]{

        m_dynUI->wHelp = std::make_unique<QWidget>();
        m_dynUI->wHelp->setWindowIcon(QIcon(":/images/icon"));

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

    connect(m_mainUI->pbOpenPDF, &QPushButton::clicked, this, [=]{
        bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(m_pcPages.pdfFileName));
        if(!success)
            QMessageBox::warning(this, tr("Avertissement"), tr("Le PDF n'a pu être lancé.\nVeuillez vous assurez que vous disposez d'un logiciel de lecture de PDF (ex : AdobeReader, SumatraPDF, FoxitReader...) .\n"),QMessageBox::Ok);
    });
    connect(m_mainUI->pbChooseDirectory, &QPushButton::clicked, this, &PCMainUI::set_photos_directory);
    connect(m_mainUI->pbSavePDF, &QPushButton::clicked, this, [&]{
        QString filePath = QFileDialog::getSaveFileName(this, "Entrez le nom du fichier PDF", QDir::homePath(), "PDF (*.pdf)");
        if(filePath.size() > 0){
            m_pcPages.pdfFileName = filePath;
            m_mainUI->pbSavePDF->setEnabled(false);
            m_mainUI->pbOpenPDF->setEnabled(false);
            emit start_PDF_generation_signal(&m_dynUI->docLocker, m_settings, m_pcPages);
        }
    });
    connect(m_mainUI->pbLeft, &QPushButton::clicked, this, [&]{ // previous image
        int &currId = m_settings.currentPhotoId;
        if(currId == 0){
            currId = m_settings.photosLoaded->size()-1;
        } else{
            currId--;
        }

       update_photo_to_display(m_settings.photosLoaded.get()->at(currId));
       m_mainUI->lwPhotosList->setCurrentRow(currId);
    });
    connect(m_mainUI->pbRight, &QPushButton::clicked, this, [&]{ // next image

        int &currId = m_settings.currentPhotoId;
        if(currId == m_settings.photosLoaded->size()-1){
            currId = 0;
        }else{
            currId++;
        }

        update_photo_to_display(m_settings.photosLoaded.get()->at(currId));
        m_mainUI->lwPhotosList->setCurrentRow(currId);
    });
    connect(m_mainUI->pbRotateLeft, &QPushButton::clicked, this, [&] // rotate left photo
    {
        SPhoto photo       = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation    =(photo->rotation - 90)%360;
        photo->scaledPhoto = photo->scaledPhoto.transformed(QTransform().rotate(-90));
        update_photo_to_display(photo);
    });
    connect(m_mainUI->pbRotateRight, &QPushButton::clicked, this, [&] // rotate right photo
    {
        SPhoto photo        = m_settings.photosLoaded.get()->at(m_settings.currentPhotoId);
        photo->rotation     = (photo->rotation + 90)%360;
        photo->scaledPhoto  = photo->scaledPhoto.transformed(QTransform().rotate(90));
        update_photo_to_display(photo);
    });
    connect(m_mainUI->pbAdd, &QPushButton::clicked, this, [&]{// add/remove photo from list

        m_settings.photosLoaded.get()->at(m_settings.currentPhotoId)->isRemoved = false;

        define_selected_photo(m_settings.currentPhotoId);
        update_valid_photos();
        update_settings_with_no_preview();
    });

    connect(m_mainUI->pbRemove, &QPushButton::clicked, this, [&] // add/remove photo from list
    {
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
            m_dynUI->remove_individual_consign(m_settings.currentPhotoId);

            // define selected id photo
            idPhotoSelected = m_settings.currentPhotoId;

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
        define_selected_pc_from_current_photo();

        display_photo_panel();

        update_valid_photos();
        update_settings_with_no_preview();
    });
    connect(m_mainUI->pbDuplicate, &QPushButton::clicked, this, [&]{

        // copy current photo
        int currentIndex = m_mainUI->lwPhotosList->currentRow();
        SPhoto duplicatedPhoto = std::make_shared<Photo>(Photo(*(m_settings.photosLoaded.get()->at(currentIndex))));
        duplicatedPhoto->isRemoved = false;
        duplicatedPhoto->isADuplicate = true;

        // insert it
        m_settings.photosLoaded->insert(currentIndex+1, duplicatedPhoto);

        // rest global ids
        for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){
            m_settings.photosLoaded->at(ii)->globalId = ii;
        }

        // insert new element in photo list widget
        m_mainUI->lwPhotosList->blockSignals(true);
        m_mainUI->lwPhotosList->insertItem(currentIndex+1,"(Copie) " + QString::number(currentIndex+1) + ". " + duplicatedPhoto->pathPhoto.split("/").last() + "");
        m_mainUI->lwPhotosList->blockSignals(false);

        // insert new consign in UI
        m_dynUI->insert_individual_consign(currentIndex+1);

        update_valid_photos();
        update_settings_with_no_preview();
    });
    connect(m_mainUI->pbOrderMinus, &QPushButton::clicked, this, [&]{

        int row = m_mainUI->lwPhotosList->currentRow();
        if(row < 0){
            qWarning() << "-Warning: minus order photo -> bad index";
            return;
        }
        swap_loaded_pc(row,row-1);
        define_selected_photo(row-1);
        define_selected_pc_from_current_photo();

        display_photo_panel();
        update_valid_photos();
        update_settings_with_no_preview();
    });
    connect(m_mainUI->pbOrderPlus, &QPushButton::clicked, this, [&]{

        int row = m_mainUI->lwPhotosList->currentRow();
        if(row < 0){
            qWarning() << "-Warning: plus order photo -> bad index";
            return;
        }

        swap_loaded_pc(row,row+1);
        define_selected_photo(row+1);
        define_selected_pc_from_current_photo();

        display_photo_panel();

        update_valid_photos();
        update_settings_with_no_preview();
    });

    // # tab widgets
     connect(m_mainUI->twMiddle, &QTabWidget::currentChanged, this, [&](int index){
        if(index == 1){
            update_settings();
        }else if(index == 0){
            define_selected_photo(m_settings.currentPhotoId);
            display_photo_panel();
        }
     });
     // # list widget
     connect(m_mainUI->lwPhotosList, &QListWidget::clicked, this, [&](QModelIndex index){

         m_settings.previousPhotoId = m_settings.currentPhotoId;
         if(index.row() == m_settings.currentPhotoId && index.row() < m_settings.photosLoaded->size()){
            define_selected_photo(index.row());
            define_selected_pc_from_current_photo();
            define_selected_page_from_current_photo();
            display_photo_panel();
         }
     });
    connect(m_mainUI->lwPhotosList, &QListWidget::currentRowChanged, this, [&](int row){

        m_settings.previousPhotoId = m_settings.currentPhotoId;
        if(row < m_settings.photosLoaded->size()){
            define_selected_photo(row);            
            define_selected_pc_from_current_photo();
            define_selected_page_from_current_photo();
            display_photo_panel();
        }
    });

    connect(m_mainUI->lwPagesList, &QListWidget::currentRowChanged, this, [&](int row){

        m_settings.previousPageId = m_settings.currentPageId;
        m_dynUI->previewW->draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(row != -1){
            define_selected_page(row);
            display_preview_panel();

            if(m_settings.previousPageId != m_settings.currentPageId){

                update_settings();
            }else{
                update_settings_with_no_preview();
            }                        
            display_individual_page_settings_panel();
        }
    });
    connect(m_mainUI->lwPagesList, &QListWidget::clicked, this, [&](QModelIndex index){

        m_settings.previousPageId = m_settings.currentPageId;
        m_dynUI->previewW->draw_current_pc_rect(-1, QRectF()); // remove current rect
        if(index.row() != -1){
            define_selected_page(index.row());
            display_preview_panel();
            if(m_settings.previousPageId != m_settings.currentPageId){
                update_settings();
            }else{
                update_settings_with_no_preview();
            }
            display_individual_page_settings_panel();
        }
    });
    // # associate sliders with spin boxes
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbGlobalRatioPC, m_mainUI->hsGlobalRatioPC);
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbRatioTitle, m_mainUI->hsRatioTitle);
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbLeftMargins, m_mainUI->hsLeftMargins, m_mainUI->dsbRightMargins, m_mainUI->hsRightMargins);
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbTopMargins, m_mainUI->hsTopMargins, m_mainUI->dsbBottomMargins, m_mainUI->hsBottomMargins);
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbHorizontalMargins, m_mainUI->hsHorizontalMargins);
    m_dynUI->associate_double_spinbox_with_slider(m_mainUI->dsbVerticalMargins, m_mainUI->hsVerticalMargins);

    // # associate buttons
    m_dynUI->associate_buttons({m_mainUI->pbGlobalConsignBottom,m_mainUI->pbGlobalConsignLeft,m_mainUI->pbGlobalConsignRight,m_mainUI->pbGlobalConsignTop});
    m_dynUI->associate_buttons({m_mainUI->pbGlobalImageAligmentLeft,m_mainUI->pbGlobalImageAligmentRight, m_mainUI->pbGlobalImageAligmentHMiddle});
    m_dynUI->associate_buttons({m_mainUI->pbGlobalImageAligmentVMiddle,m_mainUI->pbGlobalImageAligmentTop,m_mainUI->pbGlobalImageAligmentBottom});
    m_dynUI->associate_buttons({m_mainUI->pbLandscape,m_mainUI->pbPortrait});

    // # associate checkboxes with UI
    m_dynUI->checkbox_enable_UI(m_mainUI->cbAddExteriorMargins, {m_mainUI->hsLeftMargins, m_mainUI->hsRightMargins, m_mainUI->hsTopMargins, m_mainUI->hsBottomMargins,
                       m_mainUI->dsbLeftMargins, m_mainUI->dsbRightMargins, m_mainUI->dsbTopMargins, m_mainUI->dsbBottomMargins});
    m_dynUI->checkbox_enable_UI(m_mainUI->cbAddInteriorMargins, {m_mainUI->hsHorizontalMargins,m_mainUI->hsVerticalMargins,m_mainUI->dsbHorizontalMargins,m_mainUI->dsbVerticalMargins});
    m_dynUI->checkbox_enable_UI(m_mainUI->cbAddTitle, {m_mainUI->cbAllPagesTitle, m_mainUI->rbBottomTitle, m_mainUI->rbTopTitle,
                                              m_mainUI->rbWriteOnPCTitle, m_mainUI->hsRatioTitle, m_mainUI->dsbRatioTitle, m_dynUI->titleTEdit});

    /// # udpate settings
    m_dynUI->update_settings_buttons({m_mainUI->pbGlobalConsignBottom,m_mainUI->pbGlobalConsignLeft,m_mainUI->pbGlobalConsignRight,m_mainUI->pbGlobalConsignTop,
                            m_mainUI->pbGlobalImageAligmentLeft,m_mainUI->pbGlobalImageAligmentRight, m_mainUI->pbGlobalImageAligmentHMiddle,
                            m_mainUI->pbGlobalImageAligmentVMiddle,m_mainUI->pbGlobalImageAligmentTop,m_mainUI->pbGlobalImageAligmentBottom,
                            m_mainUI->pbLandscape,m_mainUI->pbPortrait},true);
    m_dynUI->update_settings_sliders({m_mainUI->hsGlobalRatioPC, m_mainUI->hsRatioTitle, m_mainUI->hsLeftMargins, m_mainUI->hsRightMargins, m_mainUI->hsTopMargins,
                            m_mainUI->hsBottomMargins, m_mainUI->hsHorizontalMargins, m_mainUI->hsVerticalMargins},true);
    m_dynUI->update_settings_double_spinboxes({m_mainUI->dsbGlobalRatioPC, m_mainUI->dsbRatioTitle, m_mainUI->dsbLeftMargins, m_mainUI->dsbRightMargins,
                                     m_mainUI->dsbTopMargins,m_mainUI->dsbBottomMargins,m_mainUI->dsbHorizontalMargins,m_mainUI->dsbVerticalMargins},true);
    m_dynUI->update_settings_checkboxes({m_mainUI->cbAddExteriorMargins,m_mainUI->cbAddInteriorMargins,
                               m_mainUI->cbAddTitle, m_mainUI->cbAllPagesTitle},true);
    m_dynUI->update_settings_checkboxes({m_mainUI->cbCutLines, m_mainUI->cbSaveOnlyCurrentPage});

    connect(m_mainUI->cbWriteOnPhoto, &QCheckBox::clicked, this, [=](bool checked){

        if(checked){
            m_dynUI->previousGlobalConsignPositionFromPhotos =  m_settings.consignPositionFromPhotos;
        }
        m_mainUI->pbGlobalConsignBottom->setEnabled(!checked);
        m_mainUI->pbGlobalConsignLeft->setEnabled(!checked);
        m_mainUI->pbGlobalConsignRight->setEnabled(!checked);
        m_mainUI->pbGlobalConsignTop->setEnabled(!checked);
        m_mainUI->dsbGlobalRatioPC->setEnabled(!checked);
        m_mainUI->hsGlobalRatioPC->setEnabled(!checked);

        if(!checked){
            switch(m_dynUI->previousGlobalConsignPositionFromPhotos){
                case Position::bottom:
                    m_mainUI->pbGlobalConsignBottom->setEnabled(false);
                break;
                case Position::left:
                    m_mainUI->pbGlobalConsignLeft->setEnabled(false);
                break;
                case Position::right:
                    m_mainUI->pbGlobalConsignRight->setEnabled(false);
                break;
                case Position::top:
                    m_mainUI->pbGlobalConsignTop->setEnabled(false);
                break;
            }
        }

        update_settings();
    });

    m_dynUI->update_settings_format_combo_boxes(m_mainUI->cbDPI, m_mainUI->cbFormat);
    m_dynUI->update_settings_radio_buttons({m_mainUI->rbWriteOnPCTitle});
    connect(m_mainUI->rbTopTitle, &QRadioButton::clicked, this, [=]{
       m_mainUI->hsRatioTitle->setEnabled(true);
       m_mainUI->dsbRatioTitle->setEnabled(true);
    });
    connect(m_mainUI->rbBottomTitle, &QRadioButton::clicked, this, [=]{
       m_mainUI->hsRatioTitle->setEnabled(true);
       m_mainUI->dsbRatioTitle->setEnabled(true);
    });
    connect(m_mainUI->rbWriteOnPCTitle, &QRadioButton::clicked, this, [=]{
       m_mainUI->hsRatioTitle->setEnabled(false);
       m_mainUI->dsbRatioTitle->setEnabled(false);
    });

    m_dynUI->update_settings_radio_buttons({m_mainUI->rbBottomTitle, m_mainUI->rbTopTitle},true);

    connect(m_mainUI->sbNbPages, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        m_dynUI->zonesTimer.start(1000);
        update_valid_photos();
        update_settings();
    });
    connect(m_mainUI->sbNbPhotosH, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        m_dynUI->zonesTimer.start(1000);
        update_valid_photos();
        update_settings();
    });
    connect(m_mainUI->sbNbPhotosV, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
        m_dynUI->zonesTimer.start(1000);
        update_valid_photos();
        update_settings();
    });
    // # list widgets
    connect(m_dynUI->titleTEdit->textEdit(), &TextEdit::textChanged, this, &PCMainUI::update_settings);
    connect(m_dynUI->globalConsignTEdit->textEdit(), &TextEdit::textChanged, this, &PCMainUI::update_settings);
}

void PCMainUI::update_settings(){

    // nb of photos to be used (== nb of pc)
    auto nbPhotosValid          = m_settings.photosValided->size();

    // retrieve global parameters
    // # global nb photos
    m_settings.nbPhotosPageH    = m_mainUI->sbNbPhotosH->value();
    m_settings.nbPhotosPageV    = m_mainUI->sbNbPhotosV->value();
    m_settings.nbPhotosPerPage  = m_settings.nbPhotosPageH * m_settings.nbPhotosPageV;

    // # compute last page photo nb
    int lastPagePhotosNb = m_settings.photosValided->size() % m_settings.nbPhotosPerPage;
    if(lastPagePhotosNb == 0){
        lastPagePhotosNb = m_settings.nbPhotosPerPage;
    }

    // # retrieve nb of pages
    if(!m_settings.resetPages){
        m_settings.nbPages = m_mainUI->sbNbPages->value();
    }else{
        m_settings.nbPages = nbPhotosValid / m_settings.nbPhotosPerPage;
        if(lastPagePhotosNb != 0){
            ++m_settings.nbPages;
        }

        m_mainUI->sbNbPages->blockSignals(true);
        m_mainUI->sbNbPages->setValue(m_settings.nbPages);
        m_mainUI->sbNbPages->blockSignals(false);
        m_settings.resetPages = false;
    }

    // # image alignment
    int globalPhotosAlignment = 0;
    if(!m_mainUI->pbGlobalImageAligmentHMiddle->isEnabled()){ // horizontal center
        globalPhotosAlignment = globalPhotosAlignment |  Qt::AlignHCenter;
    }else if(!m_mainUI->pbGlobalImageAligmentRight->isEnabled()){ // horizontal right
        globalPhotosAlignment = globalPhotosAlignment |  Qt::AlignRight;
    }else{ // horizontal left
        globalPhotosAlignment = globalPhotosAlignment |  Qt::AlignLeft;
    }

    if(!m_mainUI->pbGlobalImageAligmentVMiddle->isEnabled()){// vertical center
        globalPhotosAlignment = globalPhotosAlignment | Qt::AlignVCenter;
    }else if(!m_mainUI->pbGlobalImageAligmentTop->isEnabled()){ // vertical top
        globalPhotosAlignment = globalPhotosAlignment | Qt::AlignTop;
    }else{
        globalPhotosAlignment = globalPhotosAlignment | Qt::AlignBottom;
    }

    m_settings.saveOnlyCurrentPage   = m_mainUI->cbSaveOnlyCurrentPage->isChecked();
    m_settings.PCRation         = m_mainUI->dsbGlobalRatioPC->value();
    m_settings.photoAlignment = globalPhotosAlignment;
    m_settings.orientation     = m_mainUI->pbPortrait->isEnabled() ? PageOrientation::landScape : PageOrientation::portrait;
    m_settings.displayCutLines = m_mainUI->cbCutLines->isChecked();
    m_pcPages.paperFormat            = PaperFormat(m_mainUI->cbDPI->currentText(), m_mainUI->cbFormat->currentText());        

    // # title
    m_settings.titleAdded                   = m_mainUI->cbAddTitle->isChecked();
    m_settings.titleOnAllPages              = m_mainUI->cbAllPagesTitle->isChecked();
    m_settings.ratioTitle             = m_mainUI->dsbRatioTitle->value();
    m_settings.titlePositionFromPC    = (m_mainUI->rbTopTitle->isChecked()) ? Position::top : (m_mainUI->rbBottomTitle->isChecked() ? Position::bottom : Position::on);
    m_settings.titleDoc                     = m_dynUI->titleTEdit->textEdit();

    // # margins
    pc::RatioMargins &margins       = m_settings.margins;
    margins.exteriorMarginsEnabled  = m_mainUI->cbAddExteriorMargins->isChecked();
    margins.interiorMarginsEnabled  = m_mainUI->cbAddInteriorMargins->isChecked();
    margins.left                    = m_mainUI->dsbLeftMargins->value();
    margins.right                   = m_mainUI->dsbRightMargins->value();
    margins.top                     = m_mainUI->dsbTopMargins->value();
    margins.bottom                  = m_mainUI->dsbBottomMargins->value();
    margins.interWidth              = m_mainUI->dsbHorizontalMargins->value();
    margins.interHeight             = m_mainUI->dsbVerticalMargins->value();

    m_settings.consignPositionFromPhotos = (!m_mainUI->pbGlobalConsignTop->isEnabled())    ? Position::top :
                                                ((!m_mainUI->pbGlobalConsignBottom->isEnabled()) ? Position::bottom :
                                                ((!m_mainUI->pbGlobalConsignLeft->isEnabled())   ? Position::left : Position::right));

    // # consigns
    m_settings.consignOnPhoto = m_mainUI->cbWriteOnPhoto->isChecked();
    m_settings.consignDoc     = m_dynUI->globalConsignTEdit->textEdit();

    m_settings.consignsDoc.clear();
    m_settings.consignsDoc.reserve(nbPhotosValid);

    for(const auto &richText : m_dynUI->individualConsignsTEditValided){
        m_settings.consignsDoc.push_back(richText->textEdit());
    }

    // reconstruct the pages
    update_pages();

    // display preview tab
    if(!m_settings.noPreviewGeneration){
        display_preview_panel();
    }

    // check if the preview is already computing and will ask later to a new update if it's the case
    m_previewLocker.lockForWrite();
    bool isPreviewComputing = m_isPreviewComputing;
    m_generatePreviewAgain = isPreviewComputing;
    m_previewLocker.unlock();

    // check timer for displaying the zones
    m_settings.displayZones = m_dynUI->zonesTimer.isActive();

    // update defintion txt
    int currentDPI = m_mainUI->cbDPI->currentText().toInt();
    m_mainUI->laDefWxH->setText(QString::number(m_pcPages.paperFormat .widthPixels(currentDPI)) + "x" + QString::number(m_pcPages.paperFormat .heightPixels(currentDPI)));
    m_mainUI->laDefTotal->setText("(" + QString::number(m_pcPages.paperFormat .widthPixels(currentDPI)*m_pcPages.paperFormat .heightPixels(currentDPI)) + " pixels)");

    update_photos_list_style();
    update_pages_list_style();

    // ask for a preview generation
    if(!m_settings.noPreviewGeneration && !isPreviewComputing){
        m_previewLocker.lockForWrite();
        m_isPreviewComputing = true;
        m_previewLocker.unlock();
        emit start_preview_generation_signal(&m_dynUI->docLocker, m_settings, m_pcPages);
    }
}

void PCMainUI::display_photo_panel(){
    m_mainUI->twMiddle->blockSignals(true);
    m_mainUI->twMiddle->setCurrentIndex(0);
    m_mainUI->twMiddle->blockSignals(false);
}

void PCMainUI::display_preview_panel(){
    m_mainUI->twMiddle->blockSignals(true);
    m_mainUI->twMiddle->setCurrentIndex(1);
    m_mainUI->twMiddle->blockSignals(false);
}

void PCMainUI::display_title_panel(){
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->setCurrentIndex(0);
    m_mainUI->tbRight->blockSignals(false);
}

void PCMainUI::display_global_consign_panel(){
    qDebug() << "display_global_consign_panel";
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->setCurrentIndex(1);
    m_mainUI->tbRight->blockSignals(false);
}

void PCMainUI::display_individual_consign_settings_panel(){
    qDebug() << "display_individual_consign_settings_panel";
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->setCurrentIndex(3);
    m_mainUI->tbRight->blockSignals(false);
}

void PCMainUI::display_individual_page_settings_panel(){
    qDebug() << "display_individual_page_settings_panel";
    m_mainUI->tbRight->blockSignals(true);
    m_mainUI->tbRight->setCurrentIndex(2);
    m_mainUI->tbRight->blockSignals(false);
}





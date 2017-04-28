

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

PCMainUI::PCMainUI(QApplication *parent) : m_version("3.0"), m_mainUI(new Ui::PhotosConsigneMainW), m_dynUI(std::make_unique<UIElements>(m_mainUI)){

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
    update_settings();    
}

PCMainUI::~PCMainUI()
{
    emit kill_signal();

    // kill threads
    m_displayPhotoWorkerThread.quit();
    m_displayPhotoWorkerThread.wait();

    m_pdfGeneratorWorkerThread.quit();
    m_pdfGeneratorWorkerThread.wait();

    delete m_mainUI;
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

        int currentIndex = m_mainUI->tbRight->currentIndex();
        m_mainUI->tbRight->removeItem(3); // remove current pc tab
        m_mainUI->tbRight->addItem(m_dynUI->individualConsignsWValided[m_settings.currentPCDisplayed].get(), "ENSEMBLE PHOTO-CONSIGNE SELECTIONNE N°" + QString::number(m_settings.currentPCDisplayed));
        m_mainUI->tbRight->setCurrentIndex(currentIndex);
    }    

    m_mainUI->twPhotosList->setTabText(1, QString::number(nbPhotosValid));
}

void PCMainUI::update_pages()
{
    // retrieve from UI
    m_settings.nbPhotosPageH = m_mainUI->sbNbPhotosH->value();
    m_settings.nbPhotosPageV = m_mainUI->sbNbPhotosV->value();
    m_settings.nbPhotosPerPage = m_settings.nbPhotosPageH * m_settings.nbPhotosPageV;

    if(m_settings.nbPhotosPerPage == 0){
        return;
    }

    m_mainUI->lwPagesList->blockSignals(true);
    m_mainUI->twMiddle->blockSignals(true);

    // compute pages number
    auto nbPages                = m_settings.photosValided->size() / m_settings.nbPhotosPerPage;
    m_settings.lastPagePhotosNb = m_settings.photosValided->size() % m_settings.nbPhotosPerPage;
    if(m_settings.lastPagePhotosNb != 0){
        ++nbPages;
    }

    if(m_settings.lastPagePhotosNb == 0)
        m_settings.lastPagePhotosNb = m_settings.nbPhotosPerPage;

    // enable preview tab
    m_mainUI->twMiddle->setTabEnabled(1, nbPages != 0);

    // reset pages
    m_settings.currentPageId = m_mainUI->lwPagesList->currentRow();
    m_mainUI->lwPagesList->clear();
    m_pcPages.pages.clear();
    m_pcPages.pages.reserve(nbPages);

    // create new pages
    int currentPhotoId = 0;
    for(int ii = 0; ii < nbPages; ++ii){

        // define current nb of photos for this page
        int nbPhotosPage = m_settings.nbPhotosPerPage;
        if(m_settings.lastPagePhotosNb > 0 && (ii == nbPages -1)){
            nbPhotosPage = m_settings.lastPagePhotosNb;
        }

        SPCPage pcPage = std::make_shared<PCPage>(PCPage());
        // misc
        pcPage->id = ii;
        pcPage->nbPhotosH = m_settings.nbPhotosPageH;
        pcPage->nbPhotosV = m_settings.nbPhotosPageV;

        // sets
        pcPage->sets.reserve(nbPhotosPage);
        for(int jj = 0; jj < nbPhotosPage; ++jj){

            SPCSet set = std::make_shared<PCSet>(PCSet());
            set->id = jj;
            set->totalId = currentPhotoId;
            set->photo = m_settings.photosValided->at(currentPhotoId++);
            set->consign = std::make_shared<Consign>(Consign());
            pcPage->sets.push_back(set);
        }

        m_pcPages.pages.push_back(pcPage);        
        m_mainUI->lwPagesList->addItem(QString::number(ii+1) + ". " + QString::number(nbPhotosPage) + ((nbPhotosPage > 1 ) ? " images." : " image."));

        QBrush brush = m_mainUI->lwPagesList->item(m_mainUI->lwPagesList->count()-1)->foreground();
        brush.setColor(QRgb(qRgb(0,106,255)));
        m_mainUI->lwPagesList->item(m_mainUI->lwPagesList->count()-1)->setForeground(brush);
    }

    // update current row
    if(m_settings.currentPageId != -1){
        if(m_settings.currentPageId >= nbPages){
            m_settings.currentPageId = nbPages-1;
        }
        m_mainUI->lwPagesList->setCurrentRow(m_settings.currentPageId);
    }else{
        m_mainUI->lwPagesList->setCurrentRow(0);
        m_settings.currentPageId = 0;
    }

    // update pages number on the tab text
    m_mainUI->twPagesList->setTabText(1, QString::number(m_pcPages.pages.size()));
    m_mainUI->lwPagesList->blockSignals(false);
    m_mainUI->twMiddle->blockSignals(false);


//    qDebug()<< "reset_pc_pages end: " << std::chrono::duration_cast<std::chrono::milliseconds>
//                             (std::chrono::system_clock::now()-start).count() << " ms";
}

void PCMainUI::update_photo_to_display(SPhoto photo)
{
    m_dynUI->selectedPhotoW->set_image(QImage(photo->pathPhoto).transformed(QTransform().rotate(photo->rotation)));
    m_dynUI->selectedPhotoW->update();
}

void PCMainUI::define_selected_photo(int index, bool showPhotoDisplayTab){

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
    m_mainUI->pbOrderMinus->setEnabled(index != 0);
    m_mainUI->pbOrderPlus->setEnabled(index != nbPhotosLoaded-1);

    // display selected image
    SPhoto currentPhoto = m_settings.photosLoaded.get()->at(index);
    update_photo_to_display(currentPhoto);

    m_mainUI->lwPhotosList->blockSignals(true);
    m_mainUI->lwPhotosList->setCurrentRow(index);
    m_mainUI->lwPhotosList->blockSignals(false);

    // display photo tab
    if(showPhotoDisplayTab){
        m_mainUI->twMiddle->blockSignals(true);
        m_mainUI->twMiddle->setCurrentIndex(0);
        m_mainUI->twMiddle->blockSignals(false);
    }

    // update buttons states and text color
    QBrush brush = m_mainUI->lwPhotosList->currentItem()->foreground();
    if(currentPhoto->isRemoved){        
        brush.setColor(QRgb(qRgb(127,180,255)));
        m_mainUI->pbRemove->hide();
        m_mainUI->pbAdd->show();
    }else{
        brush.setColor(QRgb(qRgb(0,106,255)));
        m_mainUI->pbRemove->show();
        m_mainUI->pbAdd->hide();
    }
    m_mainUI->lwPhotosList->currentItem()->setForeground(brush);
}

void PCMainUI::init_misc_elements(){

    // use designer ui
    m_mainUI->setupUi(this);

    // set icon/title
    setWindowTitle("PhotosConsigne " + m_version + " (générateur de PDF à partir de photos)");
    setWindowIcon(QIcon(":/images/icon"));

    // disable textes info tab
    m_mainUI->twGeneralSettings->setTabEnabled(0,false);
    m_mainUI->twPhotosList->setTabEnabled(1,false);
    m_mainUI->twPagesList->setTabEnabled(1,false);

    m_mainUI->tbRight->setItemEnabled(2, false); // disable current page tab
    m_mainUI->tbRight->setItemEnabled(3, false); // disable current pc tab
    m_mainUI->tbRight->setCurrentIndex(1);       // set global consign tab as current tab

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
    m_settings.globalConsignDoc =  m_dynUI->globalConsignTEdit->textEdit();
}

void PCMainUI::enable_ui()
{
    m_mainUI->lwPhotosList->setEnabled(true);
    m_mainUI->lwPagesList->setEnabled(true);
    m_mainUI->pbChooseDirectory->setEnabled(true);

    m_mainUI->tbRight->setItemEnabled(2, true); // enable current page tab
    m_mainUI->tbRight->setItemEnabled(3, true); // enable ucrrent pc tab
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
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::end_preview_signal,            this, [=](QImage previewImage){
        m_dynUI->previewW->set_image(previewImage);
        m_dynUI->previewW->update();
        m_mainUI->twMiddle->setCurrentIndex(1);
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
    connect(m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::end_generation_signal,         this, [=](bool success){

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
            m_mainUI->tbRight->setCurrentIndex(0); // title tab is now the current tab
        }else{
            m_mainUI->tbRight->removeItem(3); // remove current pc tab
            m_mainUI->tbRight->addItem(m_dynUI->individualConsignsWValided[totalIdPC].get(), "ENSEMBLE PHOTO-CONSIGNE SELECTIONNE N°" + QString::number(totalIdPC));
            m_settings.currentPCDisplayed = totalIdPC;
            m_mainUI->tbRight->setCurrentIndex(3);
        }

        m_dynUI->previewW->draw_current_pc_rect(totalIdPC, pcRectRelative);
        m_dynUI->previewW->update();


        // update photo list selection
        int offset = 0;
        QVector<int> idPhotos;
        for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){
            if(m_settings.photosLoaded.get()->at(ii)->isRemoved){
                ++offset;
            }else{
                idPhotos.push_back(offset++);
            }
        }
        m_mainUI->lwPhotosList->blockSignals(true);
        m_mainUI->lwPhotosList->setCurrentRow(idPhotos[totalIdPC]);
        m_mainUI->lwPhotosList->blockSignals(false);

    });

    // preview label -> ui
    connect(m_dynUI->previewW, &PreviewLabel::double_click_on_photo_signal, this, [&](int idTotalPhoto){

        int offset = 0;
        QVector<int> idPhotos;
        for(int ii = 0; ii < m_settings.photosLoaded->size(); ++ii){
            if(m_settings.photosLoaded.get()->at(ii)->isRemoved){
                ++offset;
            }else{
                idPhotos.push_back(offset++);
            }
        }
        m_mainUI->lwPhotosList->setCurrentRow(idPhotos[idTotalPhoto]);
        m_mainUI->twMiddle->setCurrentIndex(0);
    });

    // preview label -> pdf worker
    connect(m_dynUI->previewW, &PreviewLabel::click_on_page_signal, m_pdfGeneratorWorker.get(), &PDFGeneratorWorker::update_PC_selection);

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
        update_settings();

        // end loading
        m_mainUI->laLoadingText->setText("Photos chargées.");
        m_mainUI->progressBarLoading->setValue(1000);
    });
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::photo_loaded_signal,this, [&](QString image){

        m_mainUI->lwPhotosList->addItem(QString::number(m_mainUI->lwPhotosList->count()+1) + ". " + image);
        m_mainUI->twPhotosList->setTabText(1, QString::number(m_mainUI->lwPhotosList->count()+1));

        QBrush brush = m_mainUI->lwPhotosList->item(m_mainUI->lwPhotosList->count()-1)->foreground();
        brush.setColor(QRgb(qRgb(0,106,255)));
        m_mainUI->lwPhotosList->item(m_mainUI->lwPhotosList->count()-1)->setForeground(brush);

        if(m_mainUI->lwPhotosList->count() == 1)
            m_mainUI->lwPhotosList->setCurrentRow(0);
    });
    connect(m_displayPhotoWorker.get(), &PhotoDisplayWorker::end_loading_photos_signal, [&](SPhotos photos){
        m_settings.photosLoaded = photos;

        if( m_settings.photosLoaded->size() > 0){
            m_mainUI->twMiddle->setTabEnabled(0, true);
            m_mainUI->twMiddle->setCurrentIndex(0);
            m_settings.currentPhotoId = 0;
            update_photo_to_display(photos->at(0));
        }
    });
}

void PCMainUI::define_main_UI_connections()
{
    // dyn ui elements
    connect(m_dynUI.get(), &UIElements::update_settings_signal, this, &PCMainUI::update_settings);
    connect(m_dynUI.get(), &UIElements::set_progress_bar_state_signal, m_mainUI->progressBarLoading, &QProgressBar::setValue);
    connect(m_dynUI.get(), &UIElements::set_progress_bar_text_signal, m_mainUI->laLoadingText, &QLabel::setText);

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

            // update color
            QBrush brush = m_mainUI->lwPhotosList->currentItem()->foreground();
            brush.setColor(QRgb(qRgb(127,180,255)));
            m_mainUI->lwPhotosList->currentItem()->setForeground(brush);
        }

        define_selected_photo(idPhotoSelected, true);
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

        QBrush brush = m_mainUI->lwPhotosList->currentItem()->foreground();
        brush.setColor(QRgb(qRgb(0,106,255)));

        // insert new element in photo list widget
        m_mainUI->lwPhotosList->blockSignals(true);
        m_mainUI->lwPhotosList->insertItem(currentIndex+1,"(Copie) " + QString::number(currentIndex+1) + ". " + duplicatedPhoto->pathPhoto.split("/").last() + "");
        m_mainUI->lwPhotosList->item(currentIndex+1)->setForeground(brush);
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
        define_selected_photo(row-1, true);

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
        define_selected_photo(row+1, true);

        update_valid_photos();
        update_settings_with_no_preview();
    });

    // # tab widgets
     connect(m_mainUI->twMiddle, &QTabWidget::currentChanged, this, [&](int index){
        if(index == 1){
            update_settings();
        }
     });
    // # list widget
//     connect(m_mainUI->lwPhotosList, &QListWidget::clicked, this, [&](QModelIndex index){
//         if(index.row() == m_settings.currentPhotoId && index.row() < m_settings.photosLoaded->size()){
////             qDebug() << "&QListWidget::clicked, this, [&](QModelIndex index){";
//             define_selected_photo(index.row());
//         }
//     });
    connect(m_mainUI->lwPhotosList, &QListWidget::currentRowChanged, this, [&](int row){
        if(row < m_settings.photosLoaded->size()){
//            qDebug() << "&QListWidget::currentRowChanged, this, [&](int row){";
            define_selected_photo(row);
        }
    });

    connect(m_mainUI->lwPagesList, &QListWidget::currentRowChanged, this, [&]
    {
        m_dynUI->previewW->draw_current_pc_rect(-1, QRectF()); // remove current rect

        if(m_mainUI->lwPagesList->currentRow() != -1){
            m_settings.currentPageId = m_mainUI->lwPagesList->currentRow();
            update_settings();
        }

    });
    connect(m_mainUI->lwPagesList, &QListWidget::clicked, this, [&]
    {
        if(m_mainUI->lwPagesList->currentRow() != -1){
            m_settings.currentPageId = m_mainUI->lwPagesList->currentRow();
            update_settings();
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
    connect(m_mainUI->cbAddTitle, &QCheckBox::clicked, [&](bool checked){
        m_mainUI->tbRight->setCurrentIndex(checked ? 0 : 1);
    });

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
            m_dynUI->previousGlobalConsignPositionFromPhotos =  m_settings.globalConsignPositionFromPhotos;
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

    m_dynUI->update_settings_format_combo_boxes({m_mainUI->cbDPI, m_mainUI->cbFormat});
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

void PCMainUI::update_settings()
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    auto nbPhotosValid = m_settings.photosValided->size();
    int currentTabId = m_mainUI->twMiddle->currentIndex();

    // reset all pc pages
    update_pages();

    // image alignment
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
    m_settings.globalPCRatio         = m_mainUI->dsbGlobalRatioPC->value();
    m_settings.globalPhotosAlignment = globalPhotosAlignment;
    m_settings.globalOrientation     = m_mainUI->pbPortrait->isEnabled() ? PageOrientation::landScape : PageOrientation::portrait;
    m_settings.globalDisplayCutLines = m_mainUI->cbCutLines->isChecked();
    m_pcPages.paperFormat            = PaperFormat(m_mainUI->cbDPI->currentText(), m_mainUI->cbFormat->currentText());

    // titles
    m_settings.titleAdded = m_mainUI->cbAddTitle->isChecked();
    m_settings.titleOnAllPages = m_mainUI->cbAllPagesTitle->isChecked();
    m_settings.globalRatioTitle = m_mainUI->dsbRatioTitle->value();
    m_settings.globalTitlePositionFromPC = (m_mainUI->rbTopTitle->isChecked()) ? Position::top : (m_mainUI->rbBottomTitle->isChecked() ? Position::bottom : Position::on);
    m_settings.titleDoc = m_dynUI->titleTEdit->textEdit();

    // margins
    pc::RatioMargins &margins =  m_settings.globalMargins;
    margins.exteriorMarginsEnabled = m_mainUI->cbAddExteriorMargins->isChecked();
    margins.interiorMarginsEnabled = m_mainUI->cbAddInteriorMargins->isChecked();
    margins.left = m_mainUI->dsbLeftMargins->value();
    margins.right = m_mainUI->dsbRightMargins->value();
    margins.top = m_mainUI->dsbTopMargins->value();
    margins.bottom = m_mainUI->dsbBottomMargins->value();
    margins.interWidth = m_mainUI->dsbHorizontalMargins->value();
    margins.interHeight = m_mainUI->dsbVerticalMargins->value();

    m_settings.globalConsignPositionFromPhotos = (!m_mainUI->pbGlobalConsignTop->isEnabled())    ? Position::top :
                                                ((!m_mainUI->pbGlobalConsignBottom->isEnabled()) ? Position::bottom :
                                                ((!m_mainUI->pbGlobalConsignLeft->isEnabled())   ? Position::left : Position::right));

    // consigns
    m_settings.globalConsignOnPhoto = m_mainUI->cbWriteOnPhoto->isChecked();
    m_settings.globalConsignDoc     = m_dynUI->globalConsignTEdit->textEdit();

    m_settings.consignsDoc.clear();
    m_settings.consignsDoc.reserve(nbPhotosValid);

    for(const auto &richText : m_dynUI->individualConsignsTEditValided){
        m_settings.consignsDoc.push_back(richText->textEdit());
    }

    int currPCId = 0;
    for(auto &&pcPage : m_pcPages.pages){

        // misc
        pcPage->margins         = margins;
        pcPage->displayCutLines = m_settings.globalDisplayCutLines;

        // title
        if(m_settings.titleAdded && (pcPage->id == 0 || m_settings.titleOnAllPages)){
            if(pcPage->title == nullptr){
                pcPage->title = std::make_shared<Title>(Title());
            }
            pcPage->title->doc     = m_settings.titleDoc->document();
            pcPage->titlePositionFromPC = m_settings.globalTitlePositionFromPC;
            pcPage->ratioWithTitle      = m_settings.globalRatioTitle;
        }else{
            pcPage->title = nullptr;
        }

        // sets PC
        for(auto &&pc : pcPage->sets){

            auto wC = m_dynUI->individualConsignsWValided[currPCId];

            bool individual = wC->findChild<QCheckBox*>("cbEnableIndividualConsign")->isChecked();// m_individualConsignsTEdit[currPCId]->textEdit()->document()->toPlainText().size() > 0;
            if(!individual){
                pc->consignPositionFromPhoto = m_settings.globalConsignPositionFromPhotos;
                pc->ratio = m_settings.globalPCRatio;
                pc->consignOnPhoto = m_settings.globalConsignOnPhoto;
                pc->consign->doc = m_settings.globalConsignDoc->document();
                pc->photo->alignment = m_settings.globalPhotosAlignment;
                ++currPCId;
                continue;
            }

            pc->consignPositionFromPhoto = ( (!wC->findChild<QPushButton *>("pbConsignTop")->isEnabled())   ? Position::top    :
                                            ((!wC->findChild<QPushButton *>("pbConsignBottom")->isEnabled())? Position::bottom :
                                            ((!wC->findChild<QPushButton *>("pbConsignLeft")->isEnabled())  ? Position::left   : Position::right)));
            pc->ratio = wC->findChild<QDoubleSpinBox*>("dsbRatioPC")->value();
            pc->consignOnPhoto = wC->findChild<QCheckBox *>("cbWriteOnPhoto")->isChecked();
            pc->consign->doc = m_dynUI->individualConsignsTEditValided[currPCId]->textEdit()->document();

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
            pc->photo->alignment = individualPhotosAlignment;

            ++currPCId;
        }
    }

    if(currentTabId > 0 ){
        m_mainUI->twMiddle->blockSignals(true);
        m_mainUI->twMiddle->setCurrentIndex(currentTabId);
        m_mainUI->twMiddle->blockSignals(false);
    }

    if(nbPhotosValid == 0)
        return;

    m_previewLocker.lockForWrite();
    bool isPreviewComputing = m_isPreviewComputing;
    m_generatePreviewAgain = isPreviewComputing;
    m_previewLocker.unlock();


    m_settings.displayZones = m_dynUI->zonesTimer.isActive();
    if(!m_settings.noPreviewGeneration && !isPreviewComputing){
        m_previewLocker.lockForWrite();
        m_isPreviewComputing = true;
        m_previewLocker.unlock();
        emit start_preview_generation_signal(&m_dynUI->docLocker, m_settings, m_pcPages);
    }
//    qDebug()<< "end update settings: " << std::chrono::duration_cast<std::chrono::milliseconds>
//                             (std::chrono::system_clock::now()-start).count() << " ms";

}





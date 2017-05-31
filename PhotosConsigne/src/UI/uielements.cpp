
/**
 * \file UIElements.cpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */


// Qt
#include <QDesktopServices>
#include <QMessageBox>
//#include <QtGlobal>

// local
#include "UIElements.hpp"

using namespace pc;

pc::UIElements::UIElements(QMainWindow *parent) : m_parent(parent) {

    // use designer ui
    mainUI.setupUi(parent);

    // register new types
    qRegisterMetaType<std::shared_ptr<QString>>(        "std::shared_ptr<QString>");
    qRegisterMetaType<std::shared_ptr<QTextDocument>>(  "std::shared_ptr<QTextDocument>");

    // # preview display
    mainUI.hlPreview->addWidget(&previewW);

    // # photo display
    mainUI.hlSelectedPhoto->addWidget(&photoW);

    // # right settings
    mainUI.vlRightSettings->addWidget(&settingsW);        

    // PCMainUI init uI
    // # definition
    PaperFormat pf(mainUI.cbDPI->currentText(),mainUI.cbFormat->currentText(), mainUI.cbOrientation->currentIndex() == 1);
    int currentDPI = mainUI.cbDPI->currentText().toInt();
    mainUI.laDefinition->setTextFormat(Qt::RichText);
    mainUI.laDefinition->setText("<p><b>Définition: </b> " + QString::number(pf.width_pixels(currentDPI)) + "x" + QString::number(pf.height_pixels(currentDPI)) + " pixels</p>");
    // # buttons
    mainUI.pbAdd->hide();
    // # list photos
    mainUI.twPhotosList->setTabEnabled(1,false); // list photos info tab
    // # list pages
    mainUI.twPagesList->setTabEnabled(1,false);  // list pages info tab
    // # tabs middles
    mainUI.twMiddle->setTabEnabled(0, false);

    // # misc
    connect(mainUI.cbSaveOnlyCurrentPage, &QCheckBox::clicked, this, [=]{
        ask_for_update(false);
    });

    connect(mainUI.cbOrientation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        ask_for_update(true);
    });

    // right tab
    connect(&settingsW, &RightSettingsW::settings_updated_signal,       this, &UIElements::ask_for_update);
    connect(&settingsW, &RightSettingsW::resource_added_signal,         this, &UIElements::resource_added_signal);
    connect(&settingsW, &RightSettingsW::set_progress_bar_state_signal, this, &UIElements::set_progress_bar_state_signal);
    connect(&settingsW, &RightSettingsW::set_progress_bar_text_signal,  this, &UIElements::set_progress_bar_text_signal);

    // main window
    // # progress
    connect(this, &UIElements::set_progress_bar_state_signal,   mainUI.progressBarLoading, &QProgressBar::setValue);
    connect(this, &UIElements::set_progress_bar_text_signal,    mainUI.laLoadingText, &QLabel::setText);
    // # buttons
    // ## donate
    connect(mainUI.pbDonate,    &QPushButton::clicked, this, &UIElements::display_donate_window);
    // ## help
    connect(mainUI.pbHelp,      &QPushButton::clicked, this, &UIElements::display_help_window);
}


void UIElements::ask_for_update(bool displayZones){
    if(displayZones){
        zonesTimer.start(1000);
    }
    emit settings_updated_signal();
}


void UIElements::display_donate_window(){

    supportW = std::make_unique<QWidget>();
    supportW->setWindowIcon(QIcon(":/images/icon"));
    supportW->setWindowModality(Qt::ApplicationModal);
    supportW->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    Ui::SupportUI support;
    support.setupUi(supportW.get());

    delete support.laPatreon;
    delete support.pbPatreon;
    delete support.hlPatreon;
    connect(support.pbPaypal, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("https://www.paypal.me/PhotosConsigne", QUrl::TolerantMode))){
            QMessageBox::warning(m_parent, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
        }
    });
    connect(support.pbMail, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("mailto:?to=lance.florian@protonmail.com&subject=PhotosConsigne&body=", QUrl::TolerantMode))){}
    });
    connect(support.pbReturn, &QPushButton::clicked, this, [=]{
        supportW->hide();
    });

    supportW->show();
}

void UIElements::display_help_window(){

    helpW = std::make_unique<QWidget>();
    helpW->setWindowIcon(QIcon(":/images/icon"));
    helpW->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    Ui::HelpUI help;
    help.setupUi(helpW.get());

    connect(help.pbFAQ, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("https://github.com/FlorianLance/PhotosConsigne/wiki/FAQ:-Foire-aux-questions", QUrl::TolerantMode))){
            QMessageBox::warning(m_parent, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
        }
    });
    connect(help.pbVideosTutorials, &QPushButton::clicked, this, [=]{
        if(!QDesktopServices::openUrl(QUrl("https://github.com/FlorianLance/PhotosConsigne/wiki/Tutoriels", QUrl::TolerantMode))){
            QMessageBox::warning(m_parent, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
        }
    });

    connect(help.pbReturn, &QPushButton::clicked, this, [=]{
        helpW->hide();
    });

    helpW->show();
}

void UIElements::update_global_settings(GlobalDocumentSettings &settings) const{

    // # retrieve nb of pages
    settings.nbPages                      = mainUI.sbNbPages->value();

    // # only global
    settings.paperFormat                  = PaperFormat(mainUI.cbDPI->currentText(), mainUI.cbFormat->currentText(), mainUI.cbOrientation->currentIndex() == 1);
    settings.saveOnlyCurrentPage          = mainUI.cbSaveOnlyCurrentPage->isChecked();
    settings.PCRatio                      = settingsW.globalSetW.setStyleW.ui.dsbRatioPC->value();

    // # set
    // ## text
    settings.consignText                  = settingsW.globalSetW.setTextW.html();
    // ## set style
    settings.photoAlignment               = Utility::photo_alignment_from_comboBox(settingsW.globalSetW.setStyleW.ui.cbPhotoAlignment);
    settings.photoAdust                   = Utility::photo_adjust_from_comboBox(settingsW.globalSetW.setStyleW.ui.cbAdjustPhoto);
    settings.consignPositionFromPhotos    = static_cast<Position>(settingsW.globalSetW.setStyleW.ui.cbPositionConsign->currentIndex());

    // # page
    // ## margins
    settings.marginsSettings              = settingsW.globalPageW.marginsW.settings();
    // ## borders
    settings.bordersSettings              = settingsW.globalPageW.bordersW.settings();
    // ## background
    settings.backgroundSettings           = settingsW.globalPageW.backgroundW.settings();
    // ## sets
    settings.setsSettings                 = settingsW.globalPageW.setsW.settings();

    // # header
    // ## enabled
    settings.headerSettings.enabled       = settingsW.headerW.ui.cbEnableHeader->isChecked();
    // ## background
    settings.headerSettings.background    = settingsW.headerW.headeBackgroundW.settings();
    // ## text
    settings.headerSettings.text          = settingsW.headerW.headerTextW.html();
    // ## style
    settings.headerSettings.ratio         = settingsW.headerW.sectionStyleW.ui.dsbRatioSection->value();

    // # footer
    // ## enabled
    settings.footerSettings.enabled       = settingsW.footerW.ui.cbEnableFooter->isChecked();
    // ## background
    settings.footerSettings.background    = settingsW.footerW.footerBackgroundW.settings();
    // ## text
    settings.footerSettings.text          = settingsW.footerW.footerTextW.html();
    // ## style
    settings.footerSettings.ratio         = settingsW.footerW.sectionStyleW.ui.dsbRatioSection->value();
}


void UIElements::set_ui_state_for_adding_photos(bool state){

    if(!state){
        mainUI.pbAddPhotosDIrectory->setIcon(QIcon(":/images/photos/cancel"));
        mainUI.pbAddPhotosDIrectory->setText("Stop");
    }else{
        mainUI.pbAddPhotosDIrectory->setIcon(QIcon(":/images/dirImages"));
        mainUI.pbAddPhotosDIrectory->setText(tr("Ajouter un dossier"));
    }

    // tabs
    // # middle
    mainUI.twMiddle->setEnabled(state);
    // # right
    settingsW.ui.tbRight->setEnabled(state);
    // # action
    // ## photos
    mainUI.pbInsertNewImage->setEnabled(state);
    mainUI.pbInsertNewWhiteSpace->setEnabled(state);
    mainUI.pbRemoveAllPhoto->setEnabled(state);
    // ## pdf
    mainUI.pbSavePDF->setEnabled(state);
    // # photos list
    mainUI.lwPhotosList->setEnabled(state);
    // # pages list
    mainUI.lwPagesList->setEnabled(state);
    // # document
    mainUI.tabWDocument->setEnabled(state);

    if( mainUI.lwPhotosList->count()> 0){
        mainUI.twMiddle->setTabEnabled(0, true);
    }
}

void UIElements::set_ui_state_for_generating_pdf(bool state){
    mainUI.tabActions->setEnabled(state);
}

void UIElements::update_photos_list(const GlobalDocumentSettings &settings){

    mainUI.lwPhotosList->blockSignals(true);

    SPhotos photos = settings.photosLoaded;

    mainUI.lwPhotosList->clear();
    mainUI.twPhotosList->setTabText(1, QString::number(photos->size()));

    for(int ii = 0; ii < photos->size(); ++ii){

        QString ext = photos->at(ii)->isADuplicate ? " (copie)" : "";
        mainUI.lwPhotosList->addItem(QString::number(ii+1) + ". " + photos->at(ii)->namePhoto + ext);

        bool individual = settingsW.setsLoadedW[ii]->ui.cbEnableIndividualConsign->isChecked();
        QBrush brush = mainUI.lwPhotosList->item(ii)->foreground();
        SPhoto photo = photos->at(ii);

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

        if(ii == settings.currentPhotoId){
            if(photo->isRemoved){
                mainUI.pbRemove->hide();
                mainUI.pbAdd->show();
            }else{
                mainUI.pbRemove->show();
                mainUI.pbAdd->hide();
            }
        }

        mainUI.lwPhotosList->item(ii)->setForeground(brush);
    }

    mainUI.lwPhotosList->setCurrentRow(settings.currentPhotoId);
    mainUI.lwPhotosList->blockSignals(false);
}

void UIElements::update_pages_list(const PCPages &pcPages){

    mainUI.lwPagesList->blockSignals(true);

    int currentIndex = mainUI.lwPagesList->currentRow();
    mainUI.lwPagesList->clear();
    mainUI.twPagesList->setTabText(1, QString::number(pcPages.pages.size()));

    for(int ii = 0; ii < pcPages.pages.size(); ++ii){

        mainUI.lwPagesList->addItem(QString::number(ii+1) + ". " + QString::number(pcPages.pages[ii]->sets.size()) + ((pcPages.pages[ii]->sets.size() > 1 ) ? " ensembles." : " ensemble.") );

        bool individual = settingsW.pagesW[ii]->ui.cbEnableIndividualPage->isChecked();
        QBrush brush =  mainUI.lwPagesList->item(ii)->foreground();
        if(pcPages.pages[ii]->sets.size() == 0){  // invalid
            brush.setColor(QRgb(qRgb(255,106,0)));
        }else if(!individual){                  // global
            brush.setColor(QRgb(qRgb(0,106,255)));
        }else{                                  // individual
            brush.setColor(QRgb(qRgb(0,0,255)));
        }

        mainUI.lwPagesList->item(ii)->setForeground(brush);
    }

    if(currentIndex >= pcPages.pages.size() && currentIndex != 0){
        currentIndex = pcPages.pages.size()-1;
    }
    mainUI.lwPagesList->setCurrentRow(currentIndex);
    mainUI.lwPagesList->blockSignals(false);
}

void UIElements::update_UI(const GlobalDocumentSettings &settings){

    // update defition/dim labels
    int currentDPI = mainUI.cbDPI->currentText().toInt();
    mainUI.laDefinition->setText("<p><b>Définition: </b> " + QString::number(settings.paperFormat .width_pixels(currentDPI))+ "x" +
                                    QString::number(settings.paperFormat .height_pixels(currentDPI)) +
                                    " pixels <br /><b>Dimensions: </b> " +
                                    QString::number(settings.paperFormat.sizeMM.width()) + "x" +
                                    QString::number(settings.paperFormat.sizeMM.height()) + " mm</p>");

    // update size margins mm
    settingsW.globalPageW.marginsW.update_mm_values(settings.paperFormat);
    for(auto &&pageW : settingsW.pagesW){
        pageW->marginsW.update_mm_values(settings.paperFormat);
     }

    if(settings.photosLoaded->size() > 0){
        SPhoto currentDisplayedPhoto = settings.photosLoaded->at(settings.currentPhotoId);
        QString textInfo = "<p><b>Chemin:</b> "  + currentDisplayedPhoto->pathPhoto + "<br /><b>Taille photo:</b> " +
                QString::number(currentDisplayedPhoto->info.size()*0.000001, 'f', 2 ) + "Mo - <b>Dimensions:</b> " +
                QString::number(currentDisplayedPhoto->size().width())  + "x" +
                QString::number(currentDisplayedPhoto->size().height()) + "</p>";
        mainUI.laPhotoInfo->setTextFormat(Qt::RichText);
        mainUI.laPhotoInfo->setText(textInfo);
    }

    // custom page format
    settingsW.globalPageW.setsW.customW.update_format(settings.paperFormat);
    for(int ii = 0; ii < settingsW.pagesW.size(); ++ii){
        settingsW.pagesW[ii]->setsW.customW.update_format(settings.paperFormat);
    }

    // display current dynamic ui
    if(settings.photosLoaded->size() > 0){
        display_current_individual_page_ui(settings);
        display_current_individual_set_ui(settings);
    }
}

void UIElements::display_current_individual_set_ui(const GlobalDocumentSettings &settings){

    if(settingsW.setsValidedW.size() == 0){ // empty
        return;
    }

    if(settings.currentSetId >= settingsW.setsValidedW.size()){ // id too big
        qWarning() << "-ERROR: display_current_individual_set_ui  " << settings.currentSetId;
        return;
    }

    if(settingsW.setsValidedW[settings.currentSetId]->id == settings.currentSetIdDisplayed){ // already displayed
        return;
    }

    int currentIndex = settingsW.ui.tbRight->currentIndex();
    settingsW.ui.tbRight->blockSignals(true);
    settingsW.ui.tbRight->removeItem(4);
    settingsW.ui.tbRight->insertItem(4, settingsW.setsValidedW[settings.currentSetId].get(),"ENSEMBLE (Photo+Texte) N°" + QString::number(settings.currentSetId+1));
    settingsW.ui.tbRight->setItemIcon(4, QIcon(":/images/sets/set"));
    settingsW.ui.tbRight->setCurrentIndex(currentIndex);
    settingsW.ui.tbRight->blockSignals(false);
}

void UIElements::display_current_individual_page_ui(const GlobalDocumentSettings &settings){

    if(settingsW.pagesW.size() == 0){ // empty
        return;
    }

    if(settings.currentPageId >= settingsW.pagesW.size()){ // id too big
        qWarning() << "-ERROR: display_current_individual_page_ui  " << settings.currentPageId;
        return;
    }

    if(settings.previousPageId == settings.currentPageIdDisplayed){ // already displayed
        return;
    }

    mainUI.lwPagesList->blockSignals(true);
    mainUI.lwPagesList->setCurrentRow(settings.currentPageId);
    mainUI.lwPagesList->blockSignals(false);

    int currentIndex = settingsW.ui.tbRight->currentIndex();
    settingsW.ui.tbRight->blockSignals(true);
    settingsW.ui.tbRight->removeItem(3);
    settingsW.ui.tbRight->insertItem(3, settingsW.pagesW[settings.currentPageId].get(), "PAGE N°" + QString::number(settings.currentPageId+1));
    settingsW.ui.tbRight->setItemIcon(3, QIcon(":/images/pages/page"));
    settingsW.ui.tbRight->setCurrentIndex(currentIndex);
    settingsW.ui.tbRight->blockSignals(false);
}


void UIElements::display_photo_panel(){
    Utility::safe_init_tab_widget_index(mainUI.twMiddle, 0);
}

void UIElements::display_preview_panel(){
    Utility::safe_init_tab_widget_index(mainUI.twMiddle, 1);
}





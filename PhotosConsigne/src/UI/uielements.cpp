
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
    PaperFormat pf(mainUI.cbDPI->currentText(),mainUI.cbFormat->currentText());
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
    settings.saveOnlyCurrentPage          = mainUI.cbSaveOnlyCurrentPage->isChecked();
    settings.PCRatio                      = settingsW.globalSetW.setStyleW.ui.dsbRatioPC->value();
    settings.orientation                  = (mainUI.cbOrientation->currentIndex() == 0) ? PageOrientation::portrait : PageOrientation::landScape;

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

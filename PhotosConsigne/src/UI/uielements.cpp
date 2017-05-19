
/**
 * \file UIElements.cpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */


// local
#include "UIElements.hpp"

using namespace pc;

pc::UIElements::UIElements(QWidget *parent, std::shared_ptr<Ui::PhotosConsigneMainW> mainUI):
    m_mUI(mainUI), m_parent(parent) {

    // register new types
    qRegisterMetaType<std::shared_ptr<QString>>("std::shared_ptr<QString>");
    qRegisterMetaType<std::shared_ptr<QTextDocument>>("std::shared_ptr<QTextDocument>");

    // PCMainUI add widgets
    // # photo display
    selectedPhotoW = new PhotoW();
    m_mUI->hlSelectedPhoto->addWidget(selectedPhotoW);

    // # header
    m_mUI->vlHeader->addWidget(&headerW);
    // # footer
    m_mUI->vlFooter->addWidget(&footerW);
    // # preview display
    previewW = new PreviewW();
    m_mUI->hlPreview->addWidget(previewW);
    // # global background
    m_mUI->vlGlobalBackground->addWidget(&globalBackgroundW);
    // # global borders
    m_mUI->vlGlobalBorders->addWidget(&globalBordersW);
    // # global margins
    m_mUI->vlGlobalMargins->addWidget(&globalMarginsW);
    // # global set style
    m_mUI->vlGlobalSetStyle->addWidget(&globalSetStyleW);
    // # global consign
    QHBoxLayout *globalConsignLayout = new QHBoxLayout();
    globalConsignLayout->setContentsMargins(3,3,3,3);
    m_mUI->pageAllSetsText->setLayout(globalConsignLayout);
    globalSetTextW.init_style(RichTextType::globalConsign);
    globalConsignLayout->addWidget(&globalSetTextW);

    // PCMainUI init uI
    // # definition
    PaperFormat pf(m_mUI->cbDPI->currentText(),m_mUI->cbFormat->currentText());
    int currentDPI = m_mUI->cbDPI->currentText().toInt();
    m_mUI->laDefinition->setTextFormat(Qt::RichText);
    m_mUI->laDefinition->setText("<p><b>Définition: </b> " + QString::number(pf.width_pixels(currentDPI)) + "x" + QString::number(pf.height_pixels(currentDPI)) + " pixels</p>");
    // # buttons
    m_mUI->pbAdd->hide();
    // # list photos
    m_mUI->twPhotosList->setTabEnabled(1,false); // list photos info tab
    // # list pages
    m_mUI->twPagesList->setTabEnabled(1,false);  // list pages info tab
    // # tabs middles
    m_mUI->twMiddle->setTabEnabled(0, false);
    // # tabs right
    m_mUI->tbRight->setItemEnabled(2, false);
    m_mUI->tbRight->setItemEnabled(5, false);

    // PCMainUI connections
    // # header
    connect(&headerW, &HeaderW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # footer
    connect(&footerW, &FooterW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global background
    connect(&globalBackgroundW, &BackgroundW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global borders
    connect(&globalBordersW, &BordersW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global margins
    connect(&globalMarginsW, &MarginsW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global set style
    connect(&globalSetStyleW, &MarginsW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global consign
    connect(globalSetTextW.textEdit(), &TextEdit::resource_added_signal, this, &UIElements::resource_added_signal);

    // # misc
    connect(m_mUI->cbSaveOnlyCurrentPage, &QCheckBox::clicked, this, [=]{
        emit update_settings_signal();
    });

    connect(m_mUI->cbOrientation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        zonesTimer.start(1000);
        emit update_settings_signal();
    });

    // test
//    CustomPageW *test =  new CustomPageW();
//    m_mUI->hlTest->addWidget(test);
}

pc::UIElements::~UIElements(){

    pageW.clear();
    setsLoadedW.clear();
    setsValidedW.clear();
}

void pc::UIElements::insert_individual_set(int index){

    setsLoadedW.insert(index,std::make_shared<SetW>());
    SIndividualSetW &setW = setsLoadedW[index];
    setW->setEnabled(true);
    Ui::SetUI &ui = setW->ui;

    connect(setW->setTextW.textEdit(), &TextEdit::resource_added_signal, this, &UIElements::resource_added_signal);

    // # set style
    connect(setW.get(), &SetW::settings_updated_signal, this, &UIElements::ask_for_update);

    // # init/copy settings
    connect(ui.pbAllGlobalParameters, &QPushButton::clicked, this, [&]{
        for(auto &&set : setsValidedW){
            Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, false);
            set->ui.frameSet->setEnabled(false);
        }
        emit update_settings_signal();
    });

    connect(ui.pbAllIndividualParameters, &QPushButton::clicked, this, [&]{
        for(auto &&set : setsValidedW){
            Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, true);
            set->ui.frameSet->setEnabled(true);
        }
        emit update_settings_signal();
    });

    connect(ui.pbCopyGlobalToIndividuals, &QPushButton::clicked, this, [&]{

        for(auto &&set : setsLoadedW){
            set->init_with_another_set(globalSetStyleW, globalSetTextW, true);
        }

        emit update_settings_signal();
    });
    connect(ui.pbCopyIndividualToGlobal, &QPushButton::clicked, this, [&]{
        setW->copy_to_another_set(globalSetStyleW, globalSetTextW, true);
    });

    // init new individual ui with global ui
    setW->init_with_another_set(globalSetStyleW, globalSetTextW);
}

void pc::UIElements::remove_individual_set(int index){
    setsLoadedW.removeAt(index);
}

void pc::UIElements::reset_individual_sets(int nbPhotos){

    qreal currentState = 750.;
    qreal offset = 250. / nbPhotos;

    // clean
    setsLoadedW.clear();

    // insert new consigns
    for(int ii = 0; ii < nbPhotos; ++ii){

        emit set_progress_bar_text_signal("Consigne individuelle n°" + QString::number(ii));
        insert_individual_set(ii);
        currentState += offset;
        emit set_progress_bar_state_signal(static_cast<int>(currentState));
    }
}

void pc::UIElements::update_individual_pages(const GlobalDocumentSettings &settings){

    DebugMessage dbgMess("update_individual_pages");
    int diff = pageW.size() - settings.nbPages;
    if(diff < 0){ // add -diff pages

        for(int ii = 0; ii < -diff;++ ii){

            pageW.push_back(std::make_shared<PageW>());

            SPageW pageUI = pageW.last();
            pageUI->init_with_global_settings(settings);
            connect(pageUI.get(), &PageW::settings_updated_signal, this, &UIElements::ask_for_update);

        }

    }else if(diff > 0){ // remove diff pages

        for(int ii = 0; ii < -diff;++ ii){
            pageW.pop_back();
        }
    }
}


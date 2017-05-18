
/**
 * \file UIElements.cpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */


// local
#include "UIElements.hpp"


// Qt
#include <QColorDialog>

using namespace pc;

pc::UIElements::UIElements(QWidget *parent, std::shared_ptr<Ui::PhotosConsigneMainW> mainUI) : globalConsignUI(RichTextUI(RichTextType::globalConsign)),
    m_mUI(mainUI), m_parent(parent) {

    // register new types
    qRegisterMetaType<std::shared_ptr<QString>>("std::shared_ptr<QString>");
    qRegisterMetaType<std::shared_ptr<QTextDocument>>("std::shared_ptr<QTextDocument>");

    // PCMainUI add widgets
    // # photo display
    selectedPhotoW = new PhotoW();
    m_mUI->hlSelectedPhoto->addWidget(selectedPhotoW);
    // # preview display
    previewW = new PreviewW();
    m_mUI->vlImagePreview->addWidget(previewW);
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
    globalConsignLayout->addWidget(globalConsignUI.richTextEdit.get());
    // ## title
    m_mUI->hlTitleBottom->addWidget(titleUI.richTextEdit.get());

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

    // PCMainUI connections
    // # global background
    connect(&globalBackgroundW, &BackgroundW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global borders
    connect(&globalBordersW, &BordersW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global margins
    connect(&globalMarginsW, &MarginsW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global set style
    connect(&globalSetStyleW, &MarginsW::settings_updated_signal, this, &UIElements::ask_for_update);
    // # global consign
    connect(globalConsignUI.richTextEdit.get(), &RichTextEdit::html_updated_signal, this, [&](std::shared_ptr<QString> html){
        globalConsignUI.html = html;
    });
    connect(globalConsignUI.richTextEdit->textEdit(), &TextEdit::resource_added_signal, this, &UIElements::resource_added_signal);
    // # title
    connect(titleUI.richTextEdit.get(), &RichTextEdit::html_updated_signal, this, [&](std::shared_ptr<QString> html){
        titleUI.html = html;
    });
    connect(titleUI.richTextEdit->textEdit(), &TextEdit::resource_added_signal, this, &UIElements::resource_added_signal);


    // # associate sliders with spin boxes
    Utility::associate_double_spinbox_with_slider(m_mUI->dsbRatioTitle, m_mUI->hsRatioTitle);

    // # associate checkboxes with UI
    Utility::checkbox_enable_UI(m_mUI->cbAddTitle, {m_mUI->cbAllPagesTitle, m_mUI->rbBottomTitle, m_mUI->rbTopTitle,
                                              m_mUI->rbWriteOnPCTitle, m_mUI->hsRatioTitle, m_mUI->dsbRatioTitle, titleUI.richTextEdit.get(), m_mUI->pbRatioTitlePage});
    // # title
    connect(m_mUI->cbAddTitle, &QCheckBox::clicked, this, [=]{
        zonesTimer.start(1000);
        emit update_settings_signal();
    });
    connect(m_mUI->cbSaveOnlyCurrentPage, &QCheckBox::clicked, this, [=]{
        emit update_settings_signal();
    });

    connect(m_mUI->hsRatioTitle, &QSlider::valueChanged, this, [=]{
        zonesTimer.start(1000);
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
    qDebug() << "destructor UIElements 1";
    individualPageUI.clear();
    individualSetsLoadedUI.clear();
    individuaSetsValidedUI.clear();    
    qDebug() << "destructor UIElements 2";
}

void pc::UIElements::insert_individual_set(int index){

    DebugMessage dbgMess("insert_individual_consign");
    individualSetsLoadedUI.insert(index,std::make_shared<IndividualSetW>());
    SIndividualSetW &setW = individualSetsLoadedUI[index];
    setW->setEnabled(true);
    Ui::IndividualSetUI &ui = setW->ui;

    connect(setW->setTextW.richTextEdit.get(), &RichTextEdit::html_updated_signal, this, [&](std::shared_ptr<QString> html){
        setW->setTextW.html = html;
    });    
    connect(setW->setTextW.richTextEdit->textEdit(), &TextEdit::resource_added_signal, this, &UIElements::resource_added_signal);

    // # set style
    connect(setW.get(), &IndividualSetW::settings_updated_signal, this, &UIElements::ask_for_update);


    connect(setW->setTextW.richTextEdit->textEdit(), &TextEdit::textChanged, this, &UIElements::update_settings_signal);


    connect(ui.pbAllGlobalParameters, &QPushButton::clicked, this, [&]{
        for(auto &&set : individuaSetsValidedUI){
            set->ui.cbEnableIndividualConsign->blockSignals(true);
            set->ui.cbEnableIndividualConsign->setChecked(false);
            set->ui.cbEnableIndividualConsign->blockSignals(false);
            set->ui.frameSet->setEnabled(false);
        }
        emit update_settings_signal();
    });

    connect(ui.pbAllIndividualParameters, &QPushButton::clicked, this, [&]{
        for(auto &&set : individuaSetsValidedUI){
            set->ui.cbEnableIndividualConsign->blockSignals(true);
            set->ui.cbEnableIndividualConsign->setChecked(true);
            set->ui.cbEnableIndividualConsign->blockSignals(false);
            set->ui.frameSet->setEnabled(true);
        }
        emit update_settings_signal();
    });

    connect(ui.pbCopyGlobalToIndividuals, &QPushButton::clicked, this, [&]{

        for(auto &&set : individualSetsLoadedUI){
            set->init_with_another_set(globalSetStyleW, globalConsignUI);
        }

        emit update_settings_signal();
    });

    connect(ui.pbCopyIndividualToGlobal, &QPushButton::clicked, this, [&]{
        setW->copy_to_another_set(globalSetStyleW, globalConsignUI);
    });

    // init new individual ui with global ui
    setW->init_with_another_set(globalSetStyleW, globalConsignUI);
}

void pc::UIElements::remove_individual_set(int index){

    DebugMessage dbgMess("remove_individual_consign");
    individualSetsLoadedUI.removeAt(index);
}

void pc::UIElements::reset_individual_sets(int nbPhotos){

    DebugMessage dbgMess("reset_individual_consigns");
    qreal currentState = 750.;
    qreal offset = 250. / nbPhotos;

    // clean
    individualSetsLoadedUI.clear();

    // insert new consigns
    for(int ii = 0; ii < nbPhotos; ++ii){

        emit set_progress_bar_text_signal("Consigne individuelle n°" + QString::number(ii));
        insert_individual_set(ii);
        currentState += offset;
        emit set_progress_bar_state_signal(static_cast<int>(currentState));
    }
}

void pc::UIElements::update_individual_pages(const GlobalData &settings){

    DebugMessage dbgMess("update_individual_pages");
    int diff = individualPageUI.size() - settings.nbPages;
    if(diff < 0){ // add -diff pages

        for(int ii = 0; ii < -diff;++ ii){

            individualPageUI.push_back(std::make_shared<PageW>());

            SPageUI pageUI = individualPageUI.last();
            pageUI->init_with_global_settings(settings);
            connect(pageUI.get(), &PageW::settings_updated_signal, this, &UIElements::ask_for_update);

        }

    }else if(diff > 0){ // remove diff pages

        for(int ii = 0; ii < -diff;++ ii){
            individualPageUI.pop_back();
        }
    }
}


TitleUI::TitleUI(){

    richTextEdit->init_as_title();
    richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
    richTextEdit->setEnabled(false);
    html = std::make_shared<QString>(richTextEdit->textEdit()->document()->toHtml());
}



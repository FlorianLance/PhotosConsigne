

#include "PageW.hpp"

static int pageUICounter = 0;

pc::PageW::PageW() : SettingsW(){

    ui.setupUi(this);
    ui.framePage->setEnabled(false);
    ui.vlBackground->addWidget(&backgroundW);
    ui.vlMargins->addWidget(&marginsW);
    ui.vlSets->addWidget(&pageSetsW);
    ui.vlMisc->addWidget(&miscW);
    backgroundW.hide();
    marginsW.hide();
    miscW.hide();

    init_checkboxes_connections({ui.cbEnableIndividualPage}, true);
    Utility::checkbox_enable_UI(ui.cbEnableIndividualPage, {ui.framePage});

    connect(ui.tabPage, &QTabWidget::currentChanged, this, [&]{

        backgroundW.hide();
        marginsW.hide();
        pageSetsW.hide();
        miscW.hide();

        switch (ui.tabPage->currentIndex()) {
        case 0:
            pageSetsW.show();
            break;
        case 1:
            marginsW.show();
            break;
        case 2:
            backgroundW.show();
            break;
        case 3:
            miscW.show();
            break;
        }
    });
    ui.tabPage->setCurrentIndex(0);

    // # background
    connect(&backgroundW, &BackgroundW::settings_updated_signal, this, &PageW::settings_updated_signal);
    // # margins
    connect(&marginsW, &MarginsW::settings_updated_signal, this, &PageW::settings_updated_signal);
    // # sets
    connect(&pageSetsW, &PageSetsW::settings_updated_signal, this, &PageW::settings_updated_signal);
    // # misc
    connect(&miscW, &MiscPageW::settings_updated_signal, this, &PageW::settings_updated_signal);

    id = pageUICounter++;
}


void pc::PageW::init_ui(pc::PageW &p1, const pc::PageW &p2){

    p1.pageSetsW.ui.leNamePage->setText(p2.pageSetsW.ui.leNamePage->text());

    // init ui with
    PageSetsW::init_ui(p1.pageSetsW, p2.pageSetsW);
    BackgroundW::init_ui(p1.backgroundW, p2.backgroundW);    
    MarginsW::init_ui(p1.marginsW, p2.marginsW);
    MiscPageW::init_ui(p1.miscW, p2.miscW);
}

bool pc::PageW::individual() const noexcept{
    return ui.cbEnableIndividualPage->isChecked();
}

void pc::PageW::set_enabled(bool enabled) noexcept{
    Utility::safe_init_checkboxe_checked_state(ui.cbEnableIndividualPage, enabled);
    ui.framePage->setEnabled(enabled);
}

void pc::PageW::write_to_xml(QXmlStreamWriter &xml) const {

    xml.writeStartElement("Page");
    xml.writeAttribute("id", QString::number(id));
    xml.writeAttribute("global", QString::number(global));
    xml.writeAttribute("enabled", global ? "0" : QString::number(ui.cbEnableIndividualPage->isChecked()));
    backgroundW.write_to_xml(xml);
    marginsW.write_to_xml(xml);
    pageSetsW.write_to_xml(xml);
    miscW.write_to_xml(xml);
    xml.writeEndElement();
}

void pc::PageW::load_from_xml(QXmlStreamReader &xml){

    id = xml.attributes().value("id").toInt();
    global = xml.attributes().value("global").toInt() == 1;
    if(!global){
        Utility::safe_init_checkboxe_checked_state(ui.cbEnableIndividualPage, xml.attributes().value("enabled").toInt() == 1);
    }

    QXmlStreamReader::TokenType token = QXmlStreamReader::TokenType::StartElement;
    while(!xml.hasError()) {

        if(token == QXmlStreamReader::TokenType::EndElement && xml.name() == "Page"){
            break;
        }else if(token == QXmlStreamReader::TokenType::StartElement){

            if(xml.name() == "MiscPage"){
                miscW.load_from_xml(xml);
            }else if(xml.name() == "Background"){
                backgroundW.load_from_xml(xml);
            }else if(xml.name() == "Margins"){
                marginsW.load_from_xml(xml);
            }else if(xml.name() == "PageSets"){
                pageSetsW.load_from_xml(xml);
            }
        }

        token = xml.readNext();
    }
}


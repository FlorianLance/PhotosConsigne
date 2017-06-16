

#pragma once

/**
 * \file SectionStyleW.hpp
 * \brief defines pc::SectionStyleW/Ui::SectionStyleUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "SettingsW.hpp"

// generated ui
#include "ui_SectionStyle.h"

namespace Ui {
    class SectionStyleWUI;
}

namespace pc{


struct SectionStyleW : public SettingsW{

    SectionStyleW() : SettingsW(){

        ui.setupUi(this);
        Utility::associate_double_spinbox_with_slider(ui.dsbRatioSection, ui.hsRatioSection);
        init_sliders_connections({ui.hsRatioSection}, true);
        init_doublespinboxes_connections({ui.dsbRatioSection}, true);
    }

    void write_to_xml(QXmlStreamWriter &xml) const{
        xml.writeStartElement("SectionStyle");
        xml.writeAttribute("ratio", QString::number(ui.dsbRatioSection->value()));
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_double_spinbox_value(ui.dsbRatioSection, xml.attributes().value("ratio").toDouble());
        Utility::safe_init_slider_value(ui.hsRatioSection, static_cast<int>(ui.dsbRatioSection->value()*10000));
    }


    Ui::SectionStyleUI ui;
};
}

/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/



#pragma once

/**
 * \file SetStyleW.hpp
 * \brief defines pc::SetStyleW/Ui::SetStyleUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "ImagePositionW.hpp"

// generated ui
#include "ui_SetStyle.h"

namespace Ui {
    class SetStyleUI;
}

namespace pc{


struct SetStyleW : public SettingsW{

    SetStyleW() : SettingsW(){

        ui.setupUi(this);

        ui.hlImagePosition->addWidget(&imagePositionW);

        Utility::associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);
        Utility::associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);

        init_comboboxes_connections({ui.cbPositionConsign}, true);
        init_sliders_connections({ui.hsRatioPC}, true);
        init_doublespinboxes_connections({ui.dsbRatioPC}, true);

        connect(&imagePositionW, &ImagePositionW::settings_updated_signal, this, &SetStyleW::settings_updated_signal);
    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("SetStyle");
        xml.writeAttribute("ratioPC", QString::number(ui.dsbRatioPC->value()));
        xml.writeAttribute("position", QString::number(ui.cbPositionConsign->currentIndex()));
        imagePositionW.write_to_xml(xml);
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_combo_box_index(ui.cbPositionConsign, xml.attributes().value("position").toInt());
        Utility::safe_init_double_spinbox_value(ui.dsbRatioPC, xml.attributes().value("ratioPC").toDouble());
        Utility::safe_init_slider_value(ui.hsRatioPC, static_cast<int>(ui.dsbRatioPC->value()*10000));
    }

    static void init_ui(pc::SetStyleW &s1, const pc::SetStyleW &s2){

        Utility::safe_init_combo_box_index(s1.ui.cbPositionConsign,   s2.ui.cbPositionConsign->currentIndex());
        Utility::safe_init_double_spinbox_value(s1.ui.dsbRatioPC,     s2.ui.dsbRatioPC->value());
        Utility::safe_init_slider_value(s1.ui.hsRatioPC,              s2.ui.hsRatioPC->value());        
        ImagePositionW::init_ui(s1.imagePositionW, s2.imagePositionW);
    }

    void update_settings(Settings &settings) const{

        StyleSettings &style = dynamic_cast<StyleSettings&>(settings);
        style.ratioTextPhoto            = ui.dsbRatioPC->value();
        style.textPositionFromPhotos    = static_cast<Position>(ui.cbPositionConsign->currentIndex());
        imagePositionW.update_settings(style.imagePosition);
    }

    Ui::SetStyleUI ui;
    ImagePositionW imagePositionW;
};
}

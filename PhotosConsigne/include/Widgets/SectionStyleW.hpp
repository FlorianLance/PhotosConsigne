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

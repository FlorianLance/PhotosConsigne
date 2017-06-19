
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
 * \file HeaderW.hpp
 * \brief defines pc::HeaderW/Ui::HeaderUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "RichTextEditW.hpp"
#include "BackgroundW.hpp"
#include "SectionStyleW.hpp"

// generated ui
#include "ui_Header.h"

namespace Ui {
    class HeaderUI;
}

namespace pc{


struct HeaderW : public SettingsW{

    HeaderW() : SettingsW(){

        // init ui
        ui.setupUi(this);
        ui.tabHeader->setEnabled(true);
        ui.frameHeader->setEnabled(false);

        // enable
        init_checkboxes_connections({ui.cbEnableHeader}, true);
        Utility::checkbox_enable_UI(ui.cbEnableHeader, {ui.frameHeader});

        // text
        richTextW.init_style(RichTextType::header);
        ui.vlHeaderText->addWidget(&richTextW);
        connect(richTextW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // background
        ui.vlHeaderBackground->addWidget(&backgroundW);
        connect(&backgroundW, &BackgroundW::settings_updated_signal, this, &HeaderW::settings_updated_signal);

        // section style
        ui.vlStyle->addWidget(&sectionStyleW);
        connect(&sectionStyleW, &SectionStyleW::settings_updated_signal, this, &HeaderW::settings_updated_signal);

        ui.tabHeader->setCurrentIndex(0);
        connect(ui.tabHeader, &QTabWidget::currentChanged, this, [&]{

            if(ui.vlHeader->count() > 2){
                delete  ui.vlHeader->takeAt( ui.vlHeader->count() - 1);
            }

            switch (ui.tabHeader->currentIndex()) {
            case 0:
                ui.tabHeader->setMaximumHeight(2000);
                break;
            case 1:
                ui.vlHeader->addStretch();
                ui.tabHeader->setMaximumHeight(130);
                break;
            case 2:
                ui.vlHeader->addStretch();;
                ui.tabHeader->setMaximumHeight(600);
                break;
            }
        });
    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("Header");
        xml.writeAttribute("enabled", QString::number(ui.cbEnableHeader->isChecked()));
        richTextW.write_to_xml(xml);
        backgroundW.write_to_xml(xml);
        sectionStyleW.write_to_xml(xml);
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_checkboxe_checked_state(ui.cbEnableHeader,xml.attributes().value("enabled").toInt() == 1);
        QXmlStreamReader::TokenType token = QXmlStreamReader::TokenType::StartElement;
        while(!xml.hasError()) {

            if(token == QXmlStreamReader::TokenType::EndElement && xml.name() == "Header"){
                break;
            }else if(token == QXmlStreamReader::TokenType::StartElement){

                if(xml.name() == "RichText"){
                    richTextW.load_from_xml(xml);
                }else if(xml.name() == "Background"){
                    backgroundW.load_from_xml(xml);
                }else if(xml.name() == "SectionStyle"){
                    sectionStyleW.load_from_xml(xml);
                }
            }

            token = xml.readNext();
        }
    }


    void update_settings(Settings &settings) const{

        HeaderSettings &header = dynamic_cast<HeaderSettings&>(settings);
        header.enabled     = ui.cbEnableHeader->isChecked();
        backgroundW.update_settings(header.background);
        header.text.html   = richTextW.html();
        header.ratio       = sectionStyleW.ui.dsbRatioSection->value();
    }


    // ui
    Ui::HeaderUI       ui;

    // sub widgets
    RichTextEditW   richTextW;
    BackgroundW     backgroundW;
    SectionStyleW   sectionStyleW;
};
}



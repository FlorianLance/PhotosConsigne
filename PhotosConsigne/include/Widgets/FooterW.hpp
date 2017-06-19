
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
 * \file FooterW.hpp
 * \brief defines pc::FooterW/Ui::FooterUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "RichTextEditW.hpp"
#include "BackgroundW.hpp"
#include "SectionStyleW.hpp"

// generated ui
#include "ui_Footer.h"

namespace Ui {
    class FooterUI;
}

namespace pc{


struct FooterW : public SettingsW{

    FooterW() : SettingsW(){

        // init ui
        ui.setupUi(this);
        ui.tabFooter->setEnabled(true);
        ui.frameFooter->setEnabled(false);

        // enable
        init_checkboxes_connections({ui.cbEnableFooter}, true);
        Utility::checkbox_enable_UI(ui.cbEnableFooter, {ui.frameFooter});

        // text
        richTextW.init_style(RichTextType::footer);
        ui.vlFooterText->addWidget(&richTextW);
        connect(richTextW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // background
        ui.vlFooterBackground->addWidget(&backgroundW);
        connect(&backgroundW, &BackgroundW::settings_updated_signal, this, &FooterW::settings_updated_signal);

        // section style
        ui.vlStyle->addWidget(&sectionStyleW);
        connect(&sectionStyleW, &SectionStyleW::settings_updated_signal, this, &FooterW::settings_updated_signal);

        ui.tabFooter->setCurrentIndex(0);
        connect(ui.tabFooter, &QTabWidget::currentChanged, this, [&]{

            if(ui.vlFooter->count() > 2){
                delete  ui.vlFooter->takeAt( ui.vlFooter->count() - 1);
            }

            switch (ui.tabFooter->currentIndex()) {
            case 0:
                ui.tabFooter->setMaximumHeight(2000);
                break;
            case 1:
                ui.vlFooter->addStretch();
                ui.tabFooter->setMaximumHeight(130);
                break;
            case 2:
                ui.vlFooter->addStretch();;
                ui.tabFooter->setMaximumHeight(600);
                break;
            }
        });
    }

    void update_UI(){

    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("Footer");
        xml.writeAttribute("enabled", QString::number(ui.cbEnableFooter->isChecked()));
        richTextW.write_to_xml(xml);
        backgroundW.write_to_xml(xml);
        sectionStyleW.write_to_xml(xml);
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_checkboxe_checked_state(ui.cbEnableFooter,xml.attributes().value("enabled").toInt() == 1);

        QXmlStreamReader::TokenType token = QXmlStreamReader::TokenType::StartElement;
        while(!xml.hasError()) {

            if(token == QXmlStreamReader::TokenType::EndElement && xml.name() == "Footer"){
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

        FooterSettings &footer = dynamic_cast<FooterSettings&>(settings);
        footer.enabled     = ui.cbEnableFooter->isChecked();
        backgroundW.update_settings(footer.background);
        footer.ratio       = sectionStyleW.ui.dsbRatioSection->value();
        footer.text.html   = richTextW.html();
    }

    // ui
    Ui::FooterUI    ui;

    // sub widgets
    RichTextEditW   richTextW;
    BackgroundW     backgroundW;
    SectionStyleW   sectionStyleW;

    QSpacerItem *spacerItem = nullptr;
};
}



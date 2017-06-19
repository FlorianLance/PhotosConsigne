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
 * \file SetW.hpp
 * \brief defines pc::SetW/Ui::SetUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "RichTextEditW.hpp"
#include "SetStyleW.hpp"
#include "BordersW.hpp"
#include "MiscW.hpp"

// generated ui
#include "ui_Set.h"

namespace Ui {
    class SetUI;
}

namespace pc{

static int setUICounter = 0;

struct SetW;
using SSetW = std::shared_ptr<SetW>;

struct SetW : public SettingsW{

    Q_OBJECT

public:

    SetW() : SettingsW(){

        // init ui
        ui.setupUi(this);
        ui.tabSet->setEnabled(true);
        ui.frameSet->setEnabled(false);

        // enable
        init_checkboxes_connections({ui.cbEnableIndividualConsign});        
        Utility::checkbox_enable_UI(ui.cbEnableIndividualConsign, {ui.frameSet});

        // text
        textW.init_style(RichTextType::individualConsign);
        ui.vlText->addWidget(&textW);
        ui.vlText->setAlignment(Qt::AlignmentFlag::AlignTop);
        connect(textW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // style
        ui.vlSetStyle->addWidget(&styleW);
        ui.vlSetStyle->setAlignment(Qt::AlignmentFlag::AlignTop);
        connect(&styleW, &SetStyleW::settings_updated_signal, this, &SetW::settings_updated_signal);

        // borders
        ui.vlSetBorders->addWidget(&bordersW);
        ui.vlSetBorders->setAlignment(Qt::AlignmentFlag::AlignTop);
        connect(&bordersW, &BordersW::settings_updated_signal, this, &SetW::settings_updated_signal);

        // misc
        ui.vlMisc->addWidget(&miscW);
        ui.vlMisc->setAlignment(Qt::AlignmentFlag::AlignTop);
        connect(&miscW, &SetStyleW::settings_updated_signal, this, &SetW::settings_updated_signal);

        //  tab
        ui.tabSet->setCurrentIndex(0);
        connect(ui.tabSet, &QTabWidget::currentChanged, this, [&]{

            if(ui.vlSet->count() > (global ? 1 : 2)){
                delete  ui.vlSet->takeAt( ui.vlSet->count() - 1);
            }

            switch (ui.tabSet->currentIndex()) {
            case 0:
                ui.tabSet->setMaximumHeight(2000);
                break;
            case 1:
                ui.tabSet->setMaximumHeight(600);
                ui.vlSet->addStretch();
                break;
            case 2:
                ui.vlSet->addStretch();
                ui.tabSet->setMaximumHeight(160);
                break;
            case 3:
                ui.vlSet->addStretch();
                ui.tabSet->setMaximumHeight(global ? 150 : 100);
                break;
            }
        });

        id = setUICounter++;
    }


    static void init_ui(pc::SetW &s1, const pc::SetW &s2, bool copyHtml){ 

        s1.textW.init_with_another(s2.textW, copyHtml ? s2.textW.html() : nullptr);
        SetStyleW::init_ui(s1.styleW, s2.styleW);
        BordersW::init_ui(s1.bordersW, s2.bordersW);
    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("Set");
        xml.writeAttribute("id", QString::number(id));
        xml.writeAttribute("global", QString::number(global));
        xml.writeAttribute("enabled", global ? "0" : QString::number(ui.cbEnableIndividualConsign->isChecked()));
        textW.write_to_xml(xml);
        bordersW.write_to_xml(xml);
        styleW.write_to_xml(xml);
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        id = xml.attributes().value("id").toInt();
        global = xml.attributes().value("global").toInt() == 1;
        if(!global){
            Utility::safe_init_checkboxe_checked_state(ui.cbEnableIndividualConsign, xml.attributes().value("enabled").toInt() == 1);
        }

        QXmlStreamReader::TokenType token = QXmlStreamReader::TokenType::StartElement;
        while(!xml.hasError()) {

            if(token == QXmlStreamReader::TokenType::EndElement && xml.name() == "Set"){
                break;
            }else if(token == QXmlStreamReader::TokenType::StartElement){

                if(xml.name() == "RichText"){
                    textW.load_from_xml(xml);
                }else if(xml.name() == "SetStyle"){
                    styleW.load_from_xml(xml);
                }else if(xml.name() == "Borders"){
                    bordersW.load_from_xml(xml);
                }
            }

            token = xml.readNext();
        }
    }


    void update_settings(Settings &settings) const{

        SetSettings &set = dynamic_cast<SetSettings&>(settings);
        set.text.html  = textW.html();
        styleW.update_settings(set.style);
        bordersW.update_settings(set.borders);
    }

    int id = 0;
    bool global = false;

    // ui
    Ui::SetUI       ui;

    // sub widgets
    RichTextEditW   textW;
    SetStyleW       styleW;
    BordersW        bordersW;
    MiscSetW        miscW;

};
}



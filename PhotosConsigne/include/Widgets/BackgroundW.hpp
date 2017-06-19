
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
 * \file BackgroundW.hpp
 * \brief defines pc::BackgroundW/Ui::BackgroundUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "SettingsW.hpp"
#include "DegradedW.hpp"
#include "ImagePositionW.hpp"

// generated ui
#include "ui_Background.h"

namespace Ui {
    class BackgroundUI;
}

namespace pc{


struct BackgroundW : public SettingsW{

    BackgroundW() : SettingsW(){

        ui.setupUi(this);

        // position
        ui.hlBackgroundImageSettings->addWidget(&imagePositionW);
        connect(&imagePositionW, &ImagePositionW::settings_updated_signal, this, &BackgroundW::settings_updated_signal);

        // degraded
        ui.vlDegraded->addWidget(&degradedW);
        connect(&degradedW, &DegradedW::settings_updated_signal, this, &BackgroundW::settings_updated_signal);

        init_checkboxes_connections({ui.cbUseBackgroundImage},false);
        init_image_dialog_connections(ui.tbBackgroundImage, tr("Image du fond"), tr("Choisissez l'image qui sera affichée dans le fond"), photo, QSize(45,45));

        ui.framePattern->hide();
        ui.line1->hide();
    }

    void update_settings(Settings &settings) const{

        BackGroundSettings &background = dynamic_cast<BackGroundSettings&>(settings);
        background.displayPhoto       = ui.cbUseBackgroundImage->isChecked();
        background.photo              = photo;
        imagePositionW.update_settings(background.imagePosition);
        degradedW.update_settings(background.colors);
    }

    static void init_ui(BackgroundW &b1, const BackgroundW &b2){

        b1.photo        = b2.photo;
        b1.ui.tbBackgroundImage->setIcon(       b2.ui.tbBackgroundImage->icon());

        Utility::safe_init_checkboxe_checked_state(b1.ui.cbUseBackgroundImage,      b2.ui.cbUseBackgroundImage->isChecked());
        ImagePositionW::init_ui(b1.imagePositionW, b2.imagePositionW);
        DegradedW::init_ui(b1.degradedW, b2.degradedW);
    }

    void write_to_xml(QXmlStreamWriter &xml) const{
        xml.writeStartElement("Background");
        xml.writeAttribute("pathPhoto", ((photo != nullptr) ? photo->pathPhoto : ""));
        xml.writeAttribute("photoEnabled", QString::number(ui.cbUseBackgroundImage->isChecked()));
        degradedW.write_to_xml(xml);
        imagePositionW.write_to_xml(xml);
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        QString path = xml.attributes().value("pathPhoto").toString();
        if(path.size() > 0){
            photo = std::make_shared<Photo>(path);
        }
        Utility::safe_init_checkboxe_checked_state(ui.cbUseBackgroundImage,xml.attributes().value("photoEnabled").toInt() == 1);

        QXmlStreamReader::TokenType token = QXmlStreamReader::TokenType::StartElement;
        while(!xml.hasError()) {

            if(token == QXmlStreamReader::TokenType::EndElement && xml.name() == "Background"){
                break;
            }else if(token == QXmlStreamReader::TokenType::StartElement){

                if(xml.name() == "Degraded"){
                    degradedW.load_from_xml(xml);
                }else if(xml.name() == "ImagePosition"){
                    imagePositionW.load_from_xml(xml);
                }
            }

            token = xml.readNext();
        }
    }


    SPhoto photo        = nullptr;
    Ui::BackgroundUI ui;

    DegradedW degradedW;
    ImagePositionW imagePositionW;
};
}

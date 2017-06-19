
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
 * \file MiscW.hpp
 * \brief defines MiscPageW/MiscSetW/Ui::MiscPageUI/Ui::MiscSetUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"

// generated ui
#include "ui_MiscPage.h"
#include "ui_MiscSet.h"

namespace Ui {
    class MiscPageUI;
    class MiscSetUI;
}

namespace pc{

struct MiscPageW : public SettingsW{

    Q_OBJECT

public :

    MiscPageW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        init_checkboxes_connections({ui.cbDoNotShowFooter,ui.cbDoNotShowHeader}, true);
        init_pushbuttons_connections({ui.pbGlobal, ui.pbPerPage, ui.pbReplaceLocalWIthGlobal, ui.pbReplaceGlobalWithLocal});

        connect(ui.pbGlobal, &QPushButton::clicked, this, &MiscPageW::all_global_pages_signal);
        connect(ui.pbPerPage, &QPushButton::clicked, this, &MiscPageW::all_individual_pages_signal);

        connect(ui.pbReplaceLocalWIthGlobal, &QPushButton::clicked, this, &MiscPageW::replace_individual_with_global_signal);
        connect(ui.pbReplaceGlobalWithLocal, &QPushButton::clicked, this, &MiscPageW::replace_global_with_individual_signal);
    }

    static void init_ui(MiscPageW &p1, const MiscPageW &p2){

        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDoNotShowHeader, p2.ui.cbDoNotShowHeader->isChecked());
        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDoNotShowFooter, p2.ui.cbDoNotShowFooter->isChecked());
    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("MiscPage");
        xml.writeAttribute("showHeader", QString::number(ui.cbDoNotShowHeader->isChecked()));
        xml.writeAttribute("showFooter", QString::number(ui.cbDoNotShowFooter->isChecked()));
        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_checkboxe_checked_state(ui.cbDoNotShowHeader, xml.attributes().value("showHeader").toInt() == 1);
        Utility::safe_init_checkboxe_checked_state(ui.cbDoNotShowFooter, xml.attributes().value("showFooter").toInt() == 1);
    }

    void update_settings(Settings &settings) const{

        MiscSettings &misc = dynamic_cast<MiscSettings&>(settings);
        misc.doNotDisplayHeader = ui.cbDoNotShowHeader->isChecked();
        misc.doNotDisplayFooter = ui.cbDoNotShowFooter->isChecked();
    }

signals:

    void all_individual_pages_signal();
    void all_global_pages_signal();
    void replace_global_with_individual_signal();
    void replace_individual_with_global_signal();

public :

    // ui
    Ui::MiscPageUI ui;
};

struct MiscSetW : public SettingsW{

    Q_OBJECT

public :

    MiscSetW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        init_pushbuttons_connections({ui.pbGlobal, ui.pbPerPage, ui.pbReplaceLocalWIthGlobal, ui.pbReplaceGlobalWithLocal});

        connect(ui.pbGlobal, &QPushButton::clicked, this, &MiscSetW::all_global_pages_signal);
        connect(ui.pbPerPage, &QPushButton::clicked, this, &MiscSetW::all_individual_pages_signal);

        connect(ui.pbReplaceLocalWIthGlobal, &QPushButton::clicked, this, &MiscSetW::replace_individual_with_global_signal);
        connect(ui.pbReplaceGlobalWithLocal, &QPushButton::clicked, this, &MiscSetW::replace_global_with_individual_signal);
    }



signals:

    void all_individual_pages_signal();
    void all_global_pages_signal();
    void replace_global_with_individual_signal();
    void replace_individual_with_global_signal();

public :

    // ui
    Ui::MiscSetUI ui;
};


}



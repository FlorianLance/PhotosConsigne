
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
        headerTextW.init_style(RichTextType::header);
        ui.vlHeaderText->addWidget(&headerTextW);
        connect(headerTextW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // background
        ui.vlHeaderBackground->addWidget(&headeBackgroundW);
        connect(&headeBackgroundW, &BackgroundW::settings_updated_signal, this, &HeaderW::settings_updated_signal);

        // section style
        ui.vlStyle->addWidget(&sectionStyleW);
        connect(&sectionStyleW, &SectionStyleW::settings_updated_signal, this, &HeaderW::settings_updated_signal);
    }

    // ui
    Ui::HeaderUI       ui;

    // sub widgets
    RichTextEditW   headerTextW;
    BackgroundW     headeBackgroundW;
    SectionStyleW   sectionStyleW;
};
}



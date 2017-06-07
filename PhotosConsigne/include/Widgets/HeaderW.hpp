
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
                ui.tabHeader->setMaximumHeight(130);
                break;
            }
        });
    }

    void update_settings(HeaderSettings &settings) const{

        settings.enabled     = ui.cbEnableHeader->isChecked();
        headeBackgroundW.update_settings(settings.background);
        settings.text.html   = headerTextW.html();
        settings.ratio       = sectionStyleW.ui.dsbRatioSection->value();
    }


    // ui
    Ui::HeaderUI       ui;

    // sub widgets
    RichTextEditW   headerTextW;
    BackgroundW     headeBackgroundW;
    SectionStyleW   sectionStyleW;
};
}



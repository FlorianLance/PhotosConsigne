
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
        footerTextW.init_style(RichTextType::footer);
        ui.vlFooterText->addWidget(&footerTextW);
        connect(footerTextW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // background
        ui.vlFooterBackground->addWidget(&footerBackgroundW);
        connect(&footerBackgroundW, &BackgroundW::settings_updated_signal, this, &FooterW::settings_updated_signal);

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
                ui.tabFooter->setMaximumHeight(130);
                break;
            }
        });
    }


    void update_settings(FooterSettings &settings) const{

        settings.enabled     = ui.cbEnableFooter->isChecked();
        footerBackgroundW.update_settings(settings.background);
        settings.ratio       = sectionStyleW.ui.dsbRatioSection->value();
        settings.text.html   = footerTextW.html();
    }

    // ui
    Ui::FooterUI    ui;

    // sub widgets
    RichTextEditW   footerTextW;
    BackgroundW     footerBackgroundW;
    SectionStyleW   sectionStyleW;

    QSpacerItem *spacerItem = nullptr;
};
}



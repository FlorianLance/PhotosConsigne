
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

    void update_settings(SetSettings &settings) const{

        settings.text.html  = textW.html();
        styleW.update_settings(settings.style);
        bordersW.update_settings(settings.borders);
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



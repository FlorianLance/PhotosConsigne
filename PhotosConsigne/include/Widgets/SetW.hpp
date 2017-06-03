
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

    SetW() : SettingsW(){

        // init ui
        ui.setupUi(this);
        ui.tabSet->setEnabled(true);
        ui.frameSet->setEnabled(false);

        // enable
        init_checkboxes_connections({ui.cbEnableIndividualConsign});
        init_pushbuttons_connections({ui.pbAllGlobalParameters, ui.pbAllIndividualParameters, ui.pbCopyGlobalToIndividuals, ui.pbCopyIndividualToGlobal});
        Utility::checkbox_enable_UI(ui.cbEnableIndividualConsign, {ui.frameSet});

        // text
        setTextW.init_style(RichTextType::individualConsign);
        ui.vlText->addWidget(&setTextW);
        connect(setTextW.textEdit(), &TextEdit::textChanged, this, [=]{
            emit settings_updated_signal(false);
        });

        // style
        ui.vlSetStyle->addWidget(&setStyleW);
        connect(&setStyleW, &SetStyleW::settings_updated_signal, this, &SetW::settings_updated_signal);

        id = setUICounter++;
    }

    virtual ~SetW(){
//        DebugMessage("~SetW");
    }

    static void init_ui(pc::SetW &s1, const pc::SetW &s2, bool copyHtml){

        // style
        Ui::SetStyleUI &ui1         = s1.setStyleW.ui;
        const Ui::SetStyleUI &ui2   = s2.setStyleW.ui;
        Utility::safe_init_combo_box_index(ui1.cbPositionConsign,   ui2.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(ui1.cbAdjustPhoto,       ui2.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(ui1.cbPhotoAlignment,    ui2.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(ui1.dsbRatioPC,     ui2.dsbRatioPC->value());
        Utility::safe_init_slider_value(ui1.hsRatioPC,              ui2.hsRatioPC->value());        

        // text
        s1.setTextW.init_with_another(s2.setTextW, copyHtml ? s2.setTextW.html() : nullptr);
    }


    int id;

    // ui
    Ui::SetUI       ui;

    // sub widgets
    RichTextEditW   setTextW;
    SetStyleW       setStyleW;
};
}



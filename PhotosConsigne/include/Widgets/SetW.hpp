
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
using SIndividualSetW = std::shared_ptr<SetW>;

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

    void init_with_another_set(const SetStyleW &initSetStyle, const RichTextEditW &initSetText, bool copyHtml = false){

        // style
        Ui::SetStyleUI &setStyleUI           = setStyleW.ui;
        const Ui::SetStyleUI &initSetStyleUI = initSetStyle.ui;
        Utility::safe_init_combo_box_index(setStyleUI.cbPositionConsign, initSetStyleUI.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(setStyleUI.cbAdjustPhoto, initSetStyleUI.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(setStyleUI.cbPhotoAlignment, initSetStyleUI.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(setStyleUI.dsbRatioPC, initSetStyleUI.dsbRatioPC->value());
        Utility::safe_init_slider_value(setStyleUI.hsRatioPC, initSetStyleUI.hsRatioPC->value());

        // text
        setTextW.init_with_another(initSetText, copyHtml ? initSetText.html() : nullptr);
    }

    void copy_to_another_set(SetStyleW &copySetStyle, RichTextEditW &copySetText, bool copyHtml = false){

        // style
        Ui::SetStyleUI &setStyleUI           = setStyleW.ui;
        const Ui::SetStyleUI &initSetStyleUI = copySetStyle.ui;
        Utility::safe_init_combo_box_index(initSetStyleUI.cbPositionConsign, setStyleUI.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(initSetStyleUI.cbAdjustPhoto, setStyleUI.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(initSetStyleUI.cbPhotoAlignment, setStyleUI.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(initSetStyleUI.dsbRatioPC, setStyleUI.dsbRatioPC->value());
        Utility::safe_init_slider_value(initSetStyleUI.hsRatioPC, setStyleUI.hsRatioPC->value());

        // text
        copySetText.init_with_another(setTextW, copyHtml ? setTextW.html() : nullptr);
    }

    int id;

    // ui
    Ui::SetUI       ui;

    // sub widgets
    RichTextEditW   setTextW;
    SetStyleW       setStyleW;
};
}



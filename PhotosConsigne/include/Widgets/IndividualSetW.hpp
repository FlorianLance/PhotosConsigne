
#pragma once

/**
 * \file IndividualSetUI.hpp
 * \brief defines pc::IndividualSetW/Ui::IndividualSetUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "Utility.hpp"
#include "RichTextUI.hpp"
#include "SetStyleW.hpp"

// generated ui
#include "ui_IndividualSet.h"

namespace Ui {
    class IndividualSetUI;
}

namespace pc{

static int setUICounter = 0;

struct IndividualSetW;
using SIndividualSetW = std::shared_ptr<IndividualSetW>;

struct IndividualSetW : public SettingsW{

    IndividualSetW() : SettingsW(), setTextW(RichTextUI(RichTextType::individualConsign)){

        ui.setupUi(this);
        ui.tabSet->setEnabled(true);
        ui.frameSet->setEnabled(false);
        init_checkboxes_connections({ui.cbEnableIndividualConsign});
        Utility::checkbox_enable_UI(ui.cbEnableIndividualConsign, {ui.frameSet});

        // text
        QVBoxLayout *layout = new QVBoxLayout();
        ui.tabSetText->setLayout(layout);
        layout->setContentsMargins(QMargins(3,3,3,3));
        layout->addWidget(setTextW.richTextEdit.get());

        // style
        ui.vlSetStyle->addWidget(&setStyleW);

        id = setUICounter++;
    }

    void init_with_another_set(const SetStyleW &initSetStyle, const RichTextUI &initSetText, bool copyHtml = false){

        // style
        Ui::SetStyleUI &setStyleUI           = setStyleW.ui;
        const Ui::SetStyleUI &initSetStyleUI = initSetStyle.ui;
        Utility::safe_init_combo_box_index(setStyleUI.cbPositionConsign, initSetStyleUI.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(setStyleUI.cbAdjustPhoto, initSetStyleUI.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(setStyleUI.cbPhotoAlignment, initSetStyleUI.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(setStyleUI.dsbRatioPC, initSetStyleUI.dsbRatioPC->value());
        Utility::safe_init_slider_value(setStyleUI.hsRatioPC, initSetStyleUI.hsRatioPC->value());

        // text
        setTextW.richTextEdit->init_with_another(initSetText.richTextEdit.get(), copyHtml ? initSetText.html.get() : nullptr);
        if(copyHtml){
            setTextW.html = std::make_shared<QString>(*initSetText.html);
        }
    }

    void copy_to_another_set(SetStyleW &copySetStyle, RichTextUI &copySetText, bool copyHtml = false){

        // style
        Ui::SetStyleUI &setStyleUI           = setStyleW.ui;
        const Ui::SetStyleUI &initSetStyleUI = copySetStyle.ui;
        Utility::safe_init_combo_box_index(initSetStyleUI.cbPositionConsign, setStyleUI.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(initSetStyleUI.cbAdjustPhoto, setStyleUI.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(initSetStyleUI.cbPhotoAlignment, setStyleUI.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(initSetStyleUI.dsbRatioPC, setStyleUI.dsbRatioPC->value());
        Utility::safe_init_slider_value(initSetStyleUI.hsRatioPC, setStyleUI.hsRatioPC->value());

        // text
        copySetText.richTextEdit->init_with_another(setTextW.richTextEdit.get(), copyHtml ? setTextW.html.get() : nullptr);
        if(copyHtml){
            copySetText.html = std::make_shared<QString>(*setTextW.html);
        }
    }


    int id;
    RichTextUI  setTextW;
    SetStyleW   setStyleW;
    Ui::IndividualSetUI ui;            
};
}



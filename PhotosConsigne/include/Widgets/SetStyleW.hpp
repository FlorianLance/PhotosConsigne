

#pragma once

/**
 * \file SetStyleW.hpp
 * \brief defines pc::SetStyleW/Ui::SetStyleUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "SettingsW.hpp"

// generated ui
#include "ui_SetStyle.h"

namespace Ui {
    class SetStyleUI;
}

namespace pc{


struct SetStyleW : public SettingsW{

    SetStyleW() : SettingsW(){

        ui.setupUi(this);
        Utility::associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);
        Utility::associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);
        init_sliders_connections({ui.hsRatioPC}, true);
        init_doublespinboxes_connections({ui.dsbRatioPC}, true);
        init_comboboxes_connections({ui.cbPhotoAlignment}, false);
        init_comboboxes_connections({ui.cbPositionConsign, ui.cbAdjustPhoto}, true);
    }

    static void init_ui(pc::SetStyleW &s1, const pc::SetStyleW &s2){

        Utility::safe_init_combo_box_index(s1.ui.cbPositionConsign,   s2.ui.cbPositionConsign->currentIndex());
        Utility::safe_init_combo_box_index(s1.ui.cbAdjustPhoto,       s2.ui.cbAdjustPhoto->currentIndex());
        Utility::safe_init_combo_box_index(s1.ui.cbPhotoAlignment,    s2.ui.cbPhotoAlignment->currentIndex());
        Utility::safe_init_double_spinbox_value(s1.ui.dsbRatioPC,     s2.ui.dsbRatioPC->value());
        Utility::safe_init_slider_value(s1.ui.hsRatioPC,              s2.ui.hsRatioPC->value());
    }

    void update_settings(StyleSettings &settings) const{

        settings.ratioTextPhoto            = ui.dsbRatioPC->value();
        settings.photoAlignment            = Utility::photo_alignment_from_comboBox(ui.cbPhotoAlignment);
        settings.photoAdust                = Utility::photo_adjust_from_comboBox(ui.cbAdjustPhoto);
        settings.textPositionFromPhotos    = static_cast<Position>(ui.cbPositionConsign->currentIndex());
    }

    Ui::SetStyleUI ui;
};
}

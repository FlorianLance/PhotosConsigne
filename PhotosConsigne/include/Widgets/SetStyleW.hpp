

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

    Ui::SetStyleUI ui;
};
}

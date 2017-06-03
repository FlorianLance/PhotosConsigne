

#pragma once

/**
 * \file SectionStyleW.hpp
 * \brief defines pc::SectionStyleW/Ui::SectionStyleUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "SettingsW.hpp"

// generated ui
#include "ui_SectionStyle.h"

namespace Ui {
    class SectionStyleWUI;
}

namespace pc{


struct SectionStyleW : public SettingsW{

    SectionStyleW() : SettingsW(){

        ui.setupUi(this);
        Utility::associate_double_spinbox_with_slider(ui.dsbRatioSection, ui.hsRatioSection);
        init_sliders_connections({ui.hsRatioSection}, true);
        init_doublespinboxes_connections({ui.dsbRatioSection}, true);
    }

    virtual ~SectionStyleW(){
//        DebugMessage("~SectionStyleW");
    }

    Ui::SectionStyleUI ui;
};
}

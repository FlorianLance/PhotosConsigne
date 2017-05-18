
#pragma once

/**
 * \file MarginsW.hpp
 * \brief defines pc::MarginsW/Ui::MarginsUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"

// generated ui
#include "ui_Margins.h"

namespace Ui {
    class MarginsUI;
}

namespace pc{

struct MarginsW : public SettingsW{

    MarginsW() : SettingsW(){

        ui.setupUi(this);
        Utility::associate_double_spinbox_with_slider(ui.dsbLeftMargins, ui.hsLeftMargins, ui.dsbRightMargins, ui.hsRightMargins);
        Utility::associate_double_spinbox_with_slider(ui.dsbTopMargins, ui.hsTopMargins, ui.dsbBottomMargins, ui.hsBottomMargins);
        Utility::associate_double_spinbox_with_slider(ui.dsbHorizontalMargins, ui.hsHorizontalMargins);
        Utility::associate_double_spinbox_with_slider(ui.dsbVerticalMargins, ui.hsVerticalMargins);
        Utility::checkbox_enable_UI(ui.cbAddExteriorMargins, {ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins, ui.hsBottomMargins,
                           ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins, ui.dsbBottomMargins});
        Utility::checkbox_enable_UI(ui.cbAddInteriorMargins, {ui.hsHorizontalMargins,ui.hsVerticalMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins});

        init_checkboxes_connections({ui.cbAddExteriorMargins,ui.cbAddInteriorMargins}, true);
        init_sliders_connections({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins},true);
        init_doublespinboxes_connections({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins},true);

    }

    pc::RatioMargins settings(){

        pc::RatioMargins margins;
        margins.exteriorMarginsEnabled  = ui.cbAddExteriorMargins->isChecked();
        margins.interiorMarginsEnabled  = ui.cbAddInteriorMargins->isChecked();
        margins.left                    = ui.dsbLeftMargins->value();
        margins.right                   = ui.dsbRightMargins->value();
        margins.top                     = ui.dsbTopMargins->value();
        margins.bottom                  = ui.dsbBottomMargins->value();
        margins.interWidth              = ui.dsbHorizontalMargins->value();
        margins.interHeight             = ui.dsbVerticalMargins->value();
        return margins;
    }

    QColor color = qRgba(0,0,0,0);
    Ui::MarginsUI ui;
};
}

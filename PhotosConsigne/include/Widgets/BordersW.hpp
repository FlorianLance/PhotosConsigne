
#pragma once

/**
 * \file BordersW.hpp
 * \brief defines pc::BordersW/Ui::BordersUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"

// generated ui
#include "ui_Borders.h"

namespace Ui {
    class BordersUI;
}

namespace pc{

struct BordersW : public SettingsW {

    BordersW() : SettingsW(){

        ui.setupUi(this);        
        init_checkboxes_connections({ui.cbEnableBorders}, false);
        init_color_dialog_connections(ui.tbColorBorder, tr("Couleur de la bordure"), tr("Choix de la couleur de bordure"), &color, QSize(50,50));
        init_comboboxes_connections({ui.cbBordersLineWidth, ui.cbBordersLineStyle, ui.cbBordersJoin,}, false);
    }

    BordersSettings settings(){

        BordersSettings settings;
        settings.display = ui.cbEnableBorders->isChecked();
        settings.width   = Utility::borders_line_width_from_comboBox(ui.cbBordersLineWidth);
        settings.pen.setColor(color);
        settings.pen.setStyle(Utility::borders_line_style_from_comboBox(ui.cbBordersLineStyle));
        settings.pen.setJoinStyle(Utility::borders_join_style_from_comboBox(ui.cbBordersJoin));
        return settings;
    }

    QColor color = qRgba(0,0,0,0);

    Ui::BordersUI ui;
};
}

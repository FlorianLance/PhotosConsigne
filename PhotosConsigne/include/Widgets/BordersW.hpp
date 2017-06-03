
#pragma once

/**
 * \file BordersW.hpp
 * \brief defines pc::BordersW/Ui::BordersUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"
#include "GlobalDocumentSettings.hpp"

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

    virtual ~BordersW(){
//        DebugMessage("~BordersW");
    }

    BordersSettings settings() const{

        BordersSettings settings;
        settings.display = ui.cbEnableBorders->isChecked();
        settings.width   = Utility::borders_line_width_from_comboBox(ui.cbBordersLineWidth);
        settings.pen.setColor(color);
        settings.pen.setStyle(Utility::borders_line_style_from_comboBox(ui.cbBordersLineStyle));
        settings.pen.setJoinStyle(Utility::borders_join_style_from_comboBox(ui.cbBordersJoin));
        return settings;
    }

    static void init_ui(BordersW &b1, const BordersW &b2){
        Utility::safe_init_checkboxe_checked_state(b1.ui.cbEnableBorders, b2.ui.cbEnableBorders->isChecked());
        Utility::safe_init_combo_box_index(b1.ui.cbBordersLineWidth, b2.ui.cbBordersLineWidth->currentIndex());
        Utility::safe_init_combo_box_index(b1.ui.cbBordersLineStyle, b2.ui.cbBordersLineStyle->currentIndex());
        Utility::safe_init_combo_box_index(b1.ui.cbBordersJoin, b2.ui.cbBordersJoin->currentIndex());
        b1.ui.tbColorBorder->setIcon(b2.ui.tbColorBorder->icon());
        b1.color = b2.color;
    }


    QColor color = qRgba(0,0,0,0);

    Ui::BordersUI ui;
};
}

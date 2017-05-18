
#pragma once

/**
 * \file BackgroundW.hpp
 * \brief defines pc::BackgroundW/Ui::BackgroundUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
// # widgets
#include "SettingsW.hpp"

// generated ui
#include "ui_Background.h"

namespace Ui {
    class BackgroundUI;
}

namespace pc{


struct BackgroundW : public SettingsW{

    BackgroundW() : SettingsW(){

        ui.setupUi(this);
        init_checkboxes_connections({ui.cbUseBackgroundImage, ui.cbDrawBackgoundPattern},false);
        init_comboboxes_connections({ui.cbAdjustBackgroundImage, ui.cbBackgroundPattern, ui.cbBackgroundImageAligment}, false);
        init_color_dialog_connections(ui.tbColorPage, tr("Couleur de la page"), tr("Choix de la couleur de page"), &color, QSize(50,50));
        init_color_dialog_connections(ui.tbBackgoundPatternColor, tr("Couleur du motif de fond"), tr("Choix de la couleur du motif de fond"), &colorPattern, QSize(50,50));
        init_image_dialog_connections(ui.tbBackgroundImage, tr("Image du fond"), tr("Choisissez l'image qui sera affichée dans le fond"), photo, QSize(45,45));
    }

    BackGroundSettings settings(){

        BackGroundSettings settings;

        settings.displayPhoto       = ui.cbUseBackgroundImage->isChecked();
        settings.displayPattern     = ui.cbDrawBackgoundPattern->isChecked();
        settings.color              = color;
        settings.colorPattern       = colorPattern;
        settings.photo              = photo;
        settings.patternBrushStyle  = Utility::pattern_style_comboBox(ui.cbBackgroundPattern);

        if(settings.photo != nullptr){
            settings.photo->alignment = Utility::photo_alignment_from_comboBox(ui.cbBackgroundImageAligment);
            settings.photo->adjust    = Utility::photo_adjust_from_comboBox(ui.cbAdjustBackgroundImage);
        }

        return settings;
    }

    QColor color        = qRgba(255,255,255,255);
    QColor colorPattern = qRgba(255,255,255,255);
    SPhoto photo        = nullptr;
    Ui::BackgroundUI ui;
};
}

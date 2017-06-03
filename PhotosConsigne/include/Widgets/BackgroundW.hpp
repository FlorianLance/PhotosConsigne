
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
#include "GlobalDocumentSettings.hpp"

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

    virtual ~BackgroundW(){
//        DebugMessage("~BackgroundW");
    }

    BackGroundSettings settings() const{

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

    static void init_ui(BackgroundW &b1, const BackgroundW &b2){

        b1.color        = b2.color;
        b1.colorPattern = b2.colorPattern;
        b1.photo        = b2.photo;
        b1.ui.tbColorPage->setIcon(             b2.ui.tbColorPage->icon());
        b1.ui.tbBackgoundPatternColor->setIcon( b2.ui.tbBackgoundPatternColor->icon());
        b1.ui.tbBackgroundImage->setIcon(       b2.ui.tbBackgroundImage->icon());

        Utility::safe_init_checkboxe_checked_state(b1.ui.cbUseBackgroundImage,      b2.ui.cbUseBackgroundImage->isChecked());
        Utility::safe_init_checkboxe_checked_state(b1.ui.cbDrawBackgoundPattern,    b2.ui.cbDrawBackgoundPattern->isChecked());

        Utility::safe_init_combo_box_index(b1.ui.cbAdjustBackgroundImage,           b2.ui.cbAdjustBackgroundImage->currentIndex());
        Utility::safe_init_combo_box_index(b1.ui.cbBackgroundPattern,               b2.ui.cbBackgroundPattern->currentIndex());
        Utility::safe_init_combo_box_index(b1.ui.cbBackgroundImageAligment,         b2.ui.cbBackgroundImageAligment->currentIndex());
    }



    QColor color        = qRgba(255,255,255,255);
    QColor colorPattern = qRgba(255,255,255,255);
    SPhoto photo        = nullptr;
    Ui::BackgroundUI ui;
};
}

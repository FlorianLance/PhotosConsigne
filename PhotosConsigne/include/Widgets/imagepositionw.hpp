#pragma once

/**
 * \file ImagePositionW.hpp
 * \brief defines ...
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"

// generated ui
#include "ui_ImagePosition.h"

namespace Ui {
    class ImagePositionUI;
}

namespace pc{

struct ImagePositionW : public SettingsW{

    Q_OBJECT

public :

    ImagePositionW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        // adjust
        connect(ui.cbAdjust, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&]{

            ui.dsbScale->blockSignals(true);
            switch (static_cast<PhotoAdjust>(ui.cbAdjust->currentIndex())) {
            case PhotoAdjust::center:
                ui.dsbScale->setEnabled(true);                
                ui.dsbScale->setMinimum(0.01);
                ui.dsbScale->setMaximum(1.);
                ui.dsbScale->setValue(previousCenterAdjustScaling);
                ui.framePosition->setEnabled(true);
                break;
            case PhotoAdjust::extend:
                ui.dsbScale->setEnabled(false);
                ui.dsbScale->setMinimum(0.01);
                ui.dsbScale->setMaximum(1.);
                ui.dsbScale->setValue(1.);
                ui.framePosition->setEnabled(false);
                break;
            case PhotoAdjust::fill:
                ui.dsbScale->setEnabled(true);
                ui.dsbScale->setMinimum(1.);
                ui.dsbScale->setMaximum(2.);
                ui.dsbScale->setValue(previousFillAdjustScaling);
                ui.framePosition->setEnabled(true);
                break;
            case PhotoAdjust::adjust:
                ui.dsbScale->setEnabled(false);
                ui.dsbScale->setMinimum(1.);
                ui.dsbScale->setMaximum(1.);
                ui.dsbScale->setValue(1.);
                ui.framePosition->setEnabled(true);
                break;
            case PhotoAdjust::mosaic:
                ui.dsbScale->setEnabled(true);
                ui.dsbScale->setMinimum(0.01);
                ui.dsbScale->setMaximum(1.);
                ui.dsbScale->setValue(previousMosaicAdjustScaling);
                ui.framePosition->setEnabled(true);
                break;
            }
            ui.dsbScale->blockSignals(false);

            emit settings_updated_signal(false);
        });

        // scale
        connect(ui.dsbScale, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged), this, [&]{

            switch (static_cast<PhotoAdjust>(ui.cbAdjust->currentIndex())) {
            case PhotoAdjust::center:
                previousCenterAdjustScaling = ui.dsbScale->value();
                break;
            case PhotoAdjust::fill:
                previousFillAdjustScaling = ui.dsbScale->value();
                break;
            case PhotoAdjust::mosaic:
                previousMosaicAdjustScaling = ui.dsbScale->value();
                break;
            default:
                break;
            }

            emit settings_updated_signal(false);
        });

        // position pattern
        connect(ui.cbPosition, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&]{

            switch (static_cast<PhotoPosition>(ui.cbPosition->currentIndex())) {
            case PhotoPosition::top_left:
                Utility::safe_init_slider_value(ui.vsPositionV, 1000);
                Utility::safe_init_slider_value(ui.hsPositionH, 0);
                break;
            case PhotoPosition::top_center:
                Utility::safe_init_slider_value(ui.vsPositionV, 1000);
                Utility::safe_init_slider_value(ui.hsPositionH, 500);
                break;
            case PhotoPosition::top_right:
                Utility::safe_init_slider_value(ui.vsPositionV, 1000);
                Utility::safe_init_slider_value(ui.hsPositionH, 1000);
                break;
            case PhotoPosition::middle_left:
                Utility::safe_init_slider_value(ui.vsPositionV, 500);
                Utility::safe_init_slider_value(ui.hsPositionH, 0);
                break;
            case PhotoPosition::middle_center:
                Utility::safe_init_slider_value(ui.vsPositionV, 500);
                Utility::safe_init_slider_value(ui.hsPositionH, 500);
                break;
            case PhotoPosition::middle_right:
                Utility::safe_init_slider_value(ui.vsPositionV, 500);
                Utility::safe_init_slider_value(ui.hsPositionH, 1000);
                break;
            case PhotoPosition::bottom_left:
                Utility::safe_init_slider_value(ui.vsPositionV, 0);
                Utility::safe_init_slider_value(ui.hsPositionH, 0);
                break;
            case PhotoPosition::bottom_center:
                Utility::safe_init_slider_value(ui.vsPositionV, 0);
                Utility::safe_init_slider_value(ui.hsPositionH, 500);
                break;
            case PhotoPosition::bottom_right:
                Utility::safe_init_slider_value(ui.vsPositionV, 0);
                Utility::safe_init_slider_value(ui.hsPositionH, 1000);
                break;
            case PhotoPosition::custom:
                Utility::safe_init_slider_value(ui.vsPositionV, previousVPosition);
                Utility::safe_init_slider_value(ui.hsPositionH, previousHPosition);
                break;
            }
            emit settings_updated_signal(true);
        });

        // custom position
        connect(ui.hsPositionH, &QSlider::valueChanged, this, [&](int value){
            Utility::safe_init_combo_box_index(ui.cbPosition, static_cast<int>(PhotoPosition::custom));
            Utility::safe_init_slider_value(ui.vsPositionV, previousVPosition);
            previousHPosition = value;
            emit settings_updated_signal(true);
        });
        connect(ui.vsPositionV, &QSlider::valueChanged, this, [&](int value){
            Utility::safe_init_combo_box_index(ui.cbPosition, static_cast<int>(PhotoPosition::custom));
            Utility::safe_init_slider_value(ui.hsPositionH, previousHPosition);
            previousVPosition = value;
            emit settings_updated_signal(true);
        });
    }

    static void init_ui(ImagePositionW &i1, const ImagePositionW &i2){

        Utility::safe_init_slider_value(i1.ui.vsPositionV, i2.ui.vsPositionV->value());
        Utility::safe_init_slider_value(i1.ui.hsPositionH, i2.ui.hsPositionH->value());
        Utility::safe_init_combo_box_index(i1.ui.cbPosition, i2.ui.cbPosition->currentIndex());
        Utility::safe_init_combo_box_index(i1.ui.cbAdjust, i2.ui.cbAdjust->currentIndex());

        i1.ui.dsbScale->blockSignals(true);
        i1.ui.dsbScale->setEnabled(i2.ui.dsbScale->isEnabled());
        i1.ui.dsbScale->setMinimum(i2.ui.dsbScale->minimum());
        i1.ui.dsbScale->setMaximum(i2.ui.dsbScale->maximum());
        i1.ui.dsbScale->setValue(i2.ui.dsbScale->value());
        i1.ui.dsbScale->blockSignals(false);

        i1.previousHPosition = i2.previousHPosition;
        i1.previousVPosition = i2.previousVPosition;

        i1.previousCenterAdjustScaling  = i2.previousCenterAdjustScaling;
        i1.previousFillAdjustScaling    = i2.previousFillAdjustScaling;
        i1.previousMosaicAdjustScaling  = i2.previousMosaicAdjustScaling;
    }

    void update_settings(ImagePositionSettings &settings) const{

        settings.adjustment = static_cast<PhotoAdjust>(ui.cbAdjust->currentIndex());
        settings.alignment   = static_cast<PhotoPosition>(ui.cbPosition->currentIndex());
        settings.scale      = ui.dsbScale->value();
        settings.xPos       = 0.001 * ui.hsPositionH->value();
        settings.yPos       = 0.001 * ui.vsPositionV->value();
    }


signals:


public :

    // ui
    Ui::ImagePositionUI ui;

    int previousHPosition = 500;
    int previousVPosition = 500;

    qreal previousCenterAdjustScaling = 1.;
    qreal previousFillAdjustScaling = 1.;
    qreal previousMosaicAdjustScaling = 1.;
};
}

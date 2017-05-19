
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

#include <QDebug>

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
        Utility::associate_double_spinbox_with_slider(ui.dsbFooterMargin, ui.hsFooterMargin, ui.dsbHeaderMargin, ui.hsHeaderMargin);
        Utility::checkbox_enable_UI(ui.cbAddExteriorMargins, {ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins, ui.hsBottomMargins,
                           ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins, ui.dsbBottomMargins});
        Utility::checkbox_enable_UI(ui.cbAddInteriorMargins, {ui.hsHorizontalMargins,ui.hsVerticalMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins});
        Utility::checkbox_enable_UI(ui.cbAddFooterHeaderMargins, {ui.hsFooterMargin, ui.hsHeaderMargin});

        init_checkboxes_connections({ui.cbAddExteriorMargins,ui.cbAddInteriorMargins,ui.cbAddFooterHeaderMargins}, true);
        init_sliders_connections({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins, ui.hsFooterMargin, ui.hsHeaderMargin},true);
        init_doublespinboxes_connections({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins, ui.dsbFooterMargin, ui.dsbHeaderMargin},true);
    }

    RatioMargins settings(){

        RatioMargins margins;
        margins.exteriorMarginsEnabled      = ui.cbAddExteriorMargins->isChecked();
        margins.interiorMarginsEnabled      = ui.cbAddInteriorMargins->isChecked();
        margins.footerHeaderMarginEnabled   = ui.cbAddFooterHeaderMargins->isChecked();
        margins.left                        = ui.dsbLeftMargins->value();
        margins.right                       = ui.dsbRightMargins->value();
        margins.top                         = ui.dsbTopMargins->value();
        margins.bottom                      = ui.dsbBottomMargins->value();
        margins.interWidth                  = ui.dsbHorizontalMargins->value();
        margins.interHeight                 = ui.dsbVerticalMargins->value();
        margins.footer                      = ui.dsbFooterMargin->value();
        margins.header                      = ui.dsbHeaderMargin->value();
        return margins;
    }

    void update_mm_values(bool landScape, const PaperFormat &format){

        qDebug() << "update_mm_values -> " << format.sizeMM;

        qreal currentHeight = format.sizeMM.height();
        qreal currentWidth  = format.sizeMM.width();
        if(landScape){
            std::swap(currentHeight,currentWidth);
        }

        int left=0,right=0,top =0,bottom =0;
        if(ui.cbAddExteriorMargins->isChecked()){
            left    = static_cast<int>(currentWidth  * ui.dsbLeftMargins->value());
            right   = static_cast<int>(currentWidth  * ui.dsbRightMargins->value());
            top     = static_cast<int>(currentHeight * ui.dsbTopMargins->value());
            bottom  = static_cast<int>(currentHeight * ui.dsbBottomMargins->value());
            currentWidth -= (left + right);
            currentHeight-= (top  + bottom);
        }
        ui.laLeftMM->setText(QString::number(left) + " mm");
        ui.laRightMM->setText(QString::number(right) + " mm");
        ui.laBottomMM->setText(QString::number(bottom) + " mm");
        ui.laTopMM->setText(QString::number(top) + " mm");

        int footer =0, header = 0;
        if(ui.cbAddFooterHeaderMargins->isChecked()){
            footer = static_cast<int>(currentHeight * ui.dsbFooterMargin->value());
            header = static_cast<int>(currentHeight * ui.dsbHeaderMargin->value());
            currentHeight-= (footer + header);
        }
        ui.laFooterMM->setText(QString::number(footer) + " mm");
        ui.laHeaderMM->setText(QString::number(header) + " mm");

        int internV = 0, internH = 0;
        if(ui.cbAddInteriorMargins->isChecked()){
            internV = static_cast<int>(currentHeight * ui.dsbVerticalMargins->value());
            internH = static_cast<int>(currentWidth * ui.dsbHorizontalMargins->value());
        }
        ui.laVInternMM->setText(QString::number(internV) + " mm");
        ui.laHInternMM->setText(QString::number(internH) + " mm");
    }

    QColor color = qRgba(0,0,0,0);
    Ui::MarginsUI ui;
};
}

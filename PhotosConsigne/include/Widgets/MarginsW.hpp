
#pragma once

/**
 * \file MarginsW.hpp
 * \brief defines pc::MarginsW/Ui::MarginsUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"
#include "GlobalDocumentSettings.hpp"

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
        Utility::checkbox_enable_UI(ui.cbAddFooterHeaderMargins, {ui.hsFooterMargin, ui.hsHeaderMargin, ui.dsbFooterMargin, ui.dsbHeaderMargin});

        init_checkboxes_connections({ui.cbAddExteriorMargins,ui.cbAddInteriorMargins,ui.cbAddFooterHeaderMargins}, true);
        init_sliders_connections({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins, ui.hsFooterMargin, ui.hsHeaderMargin},true);
        init_doublespinboxes_connections({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins, ui.dsbFooterMargin, ui.dsbHeaderMargin},true);
    }


    MarginsSettings settings() const noexcept {

        MarginsSettings margins;
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

    void update_mm_values(const PaperFormat &format){

        qreal currentHeight = format.sizeMM.height();
        qreal currentWidth  = format.sizeMM.width();

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

    static void init_ui(MarginsW &m1, const MarginsW &m2){

        bool externEnabled          = m2.ui.cbAddExteriorMargins->isChecked();
        bool internEnabled          = m2.ui.cbAddInteriorMargins->isChecked();
        bool footerHeaderEnabled    = m2.ui.cbAddFooterHeaderMargins->isChecked();

        Utility::safe_init_checkboxe_checked_state(m1.ui.cbAddExteriorMargins,      externEnabled);
        Utility::safe_init_checkboxe_checked_state(m1.ui.cbAddInteriorMargins,      internEnabled);
        Utility::safe_init_checkboxe_checked_state(m1.ui.cbAddFooterHeaderMargins,  footerHeaderEnabled);

        // double spin boxes
        Utility::safe_init_double_spinbox_value(m1.ui.dsbBottomMargins, m2.ui.dsbBottomMargins->value());
        m1.ui.dsbBottomMargins->setEnabled(externEnabled);
        Utility::safe_init_double_spinbox_value(m1.ui.dsbTopMargins, m2.ui.dsbTopMargins->value());
        m1.ui.dsbTopMargins->setEnabled(externEnabled);
        Utility::safe_init_double_spinbox_value(m1.ui.dsbLeftMargins, m2.ui.dsbLeftMargins->value());
        m1.ui.dsbLeftMargins->setEnabled(externEnabled);
        Utility::safe_init_double_spinbox_value(m1.ui.dsbRightMargins, m2.ui.dsbRightMargins->value());
        m1.ui.dsbRightMargins->setEnabled(externEnabled);

        Utility::safe_init_double_spinbox_value(m1.ui.dsbVerticalMargins, m2.ui.dsbVerticalMargins->value());
        m1.ui.dsbVerticalMargins->setEnabled(internEnabled);
        Utility::safe_init_double_spinbox_value(m1.ui.dsbHorizontalMargins, m2.ui.dsbHorizontalMargins->value());
        m1.ui.dsbHorizontalMargins->setEnabled( internEnabled);

        Utility::safe_init_double_spinbox_value(m1.ui.dsbHeaderMargin, m2.ui.dsbHeaderMargin->value());
        m1.ui.dsbHeaderMargin->setEnabled(footerHeaderEnabled);
        Utility::safe_init_double_spinbox_value(m1.ui.dsbFooterMargin, m2.ui.dsbFooterMargin->value());
        m1.ui.dsbFooterMargin->setEnabled(footerHeaderEnabled);

        // sliders
        Utility::safe_init_slider_value(m1.ui.hsBottomMargins, m2.ui.hsBottomMargins->value());
        m1.ui.hsBottomMargins->setEnabled(externEnabled);
        Utility::safe_init_slider_value(m1.ui.hsTopMargins, m2.ui.hsTopMargins->value());
        m1.ui.hsTopMargins->setEnabled(externEnabled);
        Utility::safe_init_slider_value(m1.ui.hsLeftMargins,  m2.ui.hsLeftMargins->value());
        m1.ui.hsLeftMargins->setEnabled(externEnabled);
        Utility::safe_init_slider_value(m1.ui.hsRightMargins,  m2.ui.hsRightMargins->value());
        m1.ui.hsRightMargins->setEnabled(externEnabled);

        Utility::safe_init_slider_value(m1.ui.hsVerticalMargins,  m2.ui.hsVerticalMargins->value());
        m1.ui.hsVerticalMargins->setEnabled(internEnabled);
        Utility::safe_init_slider_value(m1.ui.hsHorizontalMargins,  m2.ui.hsHorizontalMargins->value());
        m1.ui.hsHorizontalMargins->setEnabled(internEnabled);

        Utility::safe_init_slider_value(m1.ui.hsHeaderMargin,  m2.ui.hsHeaderMargin->value());
        m1.ui.hsHeaderMargin->setEnabled(footerHeaderEnabled);
        Utility::safe_init_slider_value(m1.ui.hsFooterMargin,  m2.ui.hsFooterMargin->value());
        m1.ui.hsFooterMargin->setEnabled(footerHeaderEnabled);
    }


    QColor color = qRgba(0,0,0,0);
    Ui::MarginsUI ui;
};
}


/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

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
        Utility::associate_double_spinbox_with_slider(ui.dsbFooterMargin, ui.hsFooterMargin, ui.dsbHeaderMargin, ui.hsHeaderMargin);
        Utility::checkbox_enable_UI(ui.cbAddExteriorMargins, {ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins, ui.hsBottomMargins,
                           ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins, ui.dsbBottomMargins});
        Utility::checkbox_enable_UI(ui.cbAddInteriorMargins, {ui.hsHorizontalMargins,ui.hsVerticalMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins});
        Utility::checkbox_enable_UI(ui.cbAddFooterHeaderMargins, {ui.hsFooterMargin, ui.hsHeaderMargin, ui.dsbFooterMargin, ui.dsbHeaderMargin});

        init_checkboxes_connections({ui.cbAddExteriorMargins,ui.cbAddInteriorMargins,ui.cbAddFooterHeaderMargins}, true);
        init_sliders_connections({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins, ui.hsFooterMargin, ui.hsHeaderMargin},true);
        init_doublespinboxes_connections({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins, ui.dsbFooterMargin, ui.dsbHeaderMargin},true);
    }

    void write_to_xml(QXmlStreamWriter &xml) const{

        xml.writeStartElement("Margins");
        xml.writeAttribute("exterior", QString::number(ui.cbAddExteriorMargins->isChecked()));
        xml.writeAttribute("interior", QString::number(ui.cbAddInteriorMargins->isChecked()));
        xml.writeAttribute("headerFooter", QString::number(ui.cbAddFooterHeaderMargins->isChecked()));

        xml.writeAttribute("left", QString::number(ui.dsbLeftMargins->value()));
        xml.writeAttribute("right", QString::number(ui.dsbRightMargins->value()));
        xml.writeAttribute("top", QString::number(ui.dsbTopMargins->value()));
        xml.writeAttribute("bottom", QString::number(ui.dsbBottomMargins->value()));

        xml.writeAttribute("interW", QString::number(ui.dsbHorizontalMargins->value()));
        xml.writeAttribute("interH", QString::number(ui.dsbVerticalMargins->value()));
        xml.writeAttribute("footer", QString::number(ui.dsbFooterMargin->value()));
        xml.writeAttribute("header", QString::number(ui.dsbHeaderMargin->value()));

        xml.writeEndElement();
    }

    void load_from_xml(QXmlStreamReader &xml){

        Utility::safe_init_checkboxe_checked_state(ui.cbAddExteriorMargins, xml.attributes().value("exterior").toInt() == 1);
        Utility::safe_init_checkboxe_checked_state(ui.cbAddInteriorMargins, xml.attributes().value("interior").toInt() == 1);
        Utility::safe_init_checkboxe_checked_state(ui.cbAddFooterHeaderMargins, xml.attributes().value("headerFooter").toInt() == 1);

        Utility::safe_init_double_spinbox_value(ui.dsbLeftMargins, xml.attributes().value("left").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbRightMargins, xml.attributes().value("right").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbTopMargins, xml.attributes().value("top").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbBottomMargins, xml.attributes().value("bottom").toDouble());

        Utility::safe_init_slider_value(ui.hsLeftMargins, static_cast<int>(ui.dsbLeftMargins->value()*10000));
        Utility::safe_init_slider_value(ui.hsRightMargins, static_cast<int>(ui.dsbRightMargins->value()*10000));
        Utility::safe_init_slider_value(ui.hsTopMargins, static_cast<int>(ui.dsbTopMargins->value()*10000));
        Utility::safe_init_slider_value(ui.hsBottomMargins, static_cast<int>(ui.dsbBottomMargins->value()*10000));

        Utility::safe_init_double_spinbox_value(ui.dsbHorizontalMargins, xml.attributes().value("interW").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbVerticalMargins, xml.attributes().value("interH").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbFooterMargin, xml.attributes().value("footer").toDouble());
        Utility::safe_init_double_spinbox_value(ui.dsbHeaderMargin, xml.attributes().value("header").toDouble());

        Utility::safe_init_slider_value(ui.hsHorizontalMargins, static_cast<int>(ui.dsbHorizontalMargins->value()*10000));
        Utility::safe_init_slider_value(ui.hsVerticalMargins, static_cast<int>(ui.dsbVerticalMargins->value()*10000));
        Utility::safe_init_slider_value(ui.hsFooterMargin, static_cast<int>(ui.dsbFooterMargin->value()*10000));
        Utility::safe_init_slider_value(ui.hsHeaderMargin, static_cast<int>(ui.dsbHeaderMargin->value()*10000));
    }

    void update_settings(Settings &settings) const{

        MarginsSettings &margins = dynamic_cast<MarginsSettings&>(settings);
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


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
 * \file CustomPageW.hpp
 * \brief defines CustomPageW
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "SettingsW.hpp"

// Qt
#include <QGridLayout>

namespace pc {


class CustomPageW : public SettingsW{


    Q_OBJECT

public:

    CustomPageW();


    void init(int gridH, int gridV);

    static void init_ui(CustomPageW &p1, const CustomPageW &p2);

    void update_ui_with_settings(const SetsPositionSettings &pos);

    void update_format(const PaperFormat &format);

    void paintEvent(QPaintEvent *event);

    void write_to_xml(QXmlStreamWriter &xml) const;
    void load_from_xml(QXmlStreamReader &xml);


public slots:

    void validate_current_set();

    void remove_last_set();

signals:

    void current_state_signal(QPoint p1, QPoint p2, int validSets);

public :

    QSize sizeGrid;

    QVector<QRectF> relativePositions;
    QRectF currRelativePos;

    QPoint currentFirstPoint;
    QPoint currentSecondPoint;

private :


    QVector<QVector<QCheckBox*>> m_points;
    QGridLayout *m_layout = nullptr;

    QFrame m_frame;

};
}

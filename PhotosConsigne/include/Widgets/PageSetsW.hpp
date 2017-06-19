
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
 * \file PageSetsW.hpp
 * \brief defines pc::PageSetsW/Ui::PageSetsUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "CustomPageW.hpp"

// generated ui
#include "ui_PageSets.h"

// Qt
#include <QDirIterator>

namespace Ui {
    class PageSetsUI;
}


namespace pc{


class PageSetsW : public SettingsW{

    Q_OBJECT

public :
    PageSetsW();

    void read_pos_files();

    static void init_ui(PageSetsW &p1, const PageSetsW &p2);

    void update_settings(Settings &settings) const;

    void update_grid_sizes_ui();

    void write_to_xml(QXmlStreamWriter &xml) const;

    void load_from_xml(QXmlStreamReader &xml);

public slots :

    void init_ui_with_settings(int id);

signals :

    void reset_pos_signal();

public :

    // ui
    Ui::PageSetsUI    ui;
    CustomPageW customW;

    QVector<QDoubleSpinBox*> columnsWidthDsb;
    QVector<QDoubleSpinBox*> linesHeightDsb;

    static bool posLoaded;
    static QVector<SetsPositionSettings> predefPositions;

private :

    QVector<QPushButton*> predefButtons;

    QString dsbStyle =
            "QWidget {"
                "color : rgb(0,106,255);"
                 "font:  bold 10 ptCalibri;"
             "}"
            "QWidget:!enabled {"
                "color : rgb(150,150,150);"
                 "font:  bold 10 ptCalibri;"
             "}";
};
}



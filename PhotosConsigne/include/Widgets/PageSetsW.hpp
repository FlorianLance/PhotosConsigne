
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

    void update_settings(SetsPositionSettings &settings) const;

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



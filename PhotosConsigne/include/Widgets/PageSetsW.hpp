
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

//    const QVector<QColor> m_colors = {Qt::blue,Qt::green,Qt::red, Qt::yellow,
//                                      Qt::lightGray, Qt::black, Qt::cyan, Qt::darkGray,
//                                      Qt::magenta, Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
//                                      Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};

    const QVector<QColor> m_colors = {qRgb(96,72,204), qRgb(98,106,213), qRgb(148,154,226), qRgb(173,177,233),
                                     qRgb(196,171,235), qRgb(159,118,222), qRgb(126,70,210), qRgb(154,169,207),
                                     qRgb(109,131,186), qRgb(74,97,157), qRgb(122,164,167), qRgb(148,182,186),
                                     qRgb(197,216,218), qRgb(108,182,255), qRgb(13,134,255), qRgb(0,92,185)};
};
}



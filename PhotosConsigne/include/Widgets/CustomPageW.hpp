

#pragma once

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

    const QVector<QColor> m_colors = {qRgb(96,72,204), qRgb(98,106,213), qRgb(148,154,226), qRgb(173,177,233),
                                     qRgb(196,171,235), qRgb(159,118,222), qRgb(126,70,210), qRgb(154,169,207),
                                     qRgb(109,131,186), qRgb(74,97,157), qRgb(122,164,167), qRgb(148,182,186),
                                     qRgb(197,216,218), qRgb(108,182,255), qRgb(13,134,255), qRgb(0,92,185)};

    QVector<QVector<QCheckBox*>> m_points;
    QGridLayout *m_layout = nullptr;

    QFrame m_frame;

};
}

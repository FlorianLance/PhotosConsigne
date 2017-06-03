

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

    virtual ~CustomPageW(){
//        DebugMessage("~CustomPageW");
    }

    void init(int gridH, int gridV);

    static void init_ui(CustomPageW &p1, const CustomPageW &p2);


    void update_format(const PaperFormat &format);

    void paintEvent(QPaintEvent *event);

public slots:

    void validate_current_set();

    void remove_last_set();

signals:

    void current_state_signal(QPoint p1, QPoint p2, int validSets);

public :

    QVector<QRectF> relativePositions;
    QRectF currRelativePos;

    QPoint currentFirstPoint;
    QPoint currentSecondPoint;

private :

    const QVector<QColor> m_colors = {Qt::blue,Qt::green,Qt::red, Qt::yellow,
                                      Qt::lightGray, Qt::black, Qt::cyan, Qt::darkGray,
                                      Qt::magenta, Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
                                      Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};

    QVector<QVector<QCheckBox*>> m_points;
    QGridLayout *m_layout = nullptr;

    QFrame m_frame;

};
}

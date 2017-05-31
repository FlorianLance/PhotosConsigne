

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

    void update_format(const PaperFormat &format);

    void paintEvent(QPaintEvent *event);

public slots:

    void validate_current_set();

    void remove_last_set();

signals:

    void current_state_signal(QPoint p1, QPoint p2, int validSets);

private :
    bool is_adding_set = false;




    QVector<QRectF> m_relativePositions;

    QPoint currentFirstPoint;
    QPoint currentSecondPoint;

    QVector<QVector<QCheckBox*>> m_points;
    QGridLayout *m_layout = nullptr;

    QFrame m_frame;

};
}

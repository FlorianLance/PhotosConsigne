

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

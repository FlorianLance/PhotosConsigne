

#pragma once

// local
#include "SettingsW.hpp"

namespace pc {



class CustomPageW : public SettingsW{


    Q_OBJECT

public:

    CustomPageW();


    void init(int nbPhotos, int gridH, int gridV);


    QVector<QVector<QCheckBox*>> m_points;
};
}

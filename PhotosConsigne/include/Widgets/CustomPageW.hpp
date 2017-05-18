
#include <QWidget>

#pragma once


class CustomPageW : public QWidget{


    Q_OBJECT

public:

    CustomPageW(QWidget *parent = 0);


    void init(int nbPhotos, int gridH, int gridV);

};

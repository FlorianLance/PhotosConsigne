
#pragma once


// qt
#include <QMainWindow>
#include <QThread>
#include <QReadWriteLock>
#include <QModelIndex>
#include <QCoreApplication>
#include <QTime>

// debug
#include <QDebug>

#include "imagelabel.h"


enum Alignment { Left, Right, Center};

class InterfaceWorker : public QObject
{
    Q_OBJECT

    public :

        InterfaceWorker(ImageLabel *preview);

    public slots :

        void checkEvent();

        void loadImages(QString path, QStringList imagesList);

        void generatePDF(QString pdfFileName);

        void generatePreview(int currentRowPhoto);

        void updateParameters(int nbImagesPage,double ratio,QFont font, QString text, QColor textColor, int imageAlignment, int textAlignment);

        void updateRotationImage(int index, bool rightRotation);

        void sendPhoto(int index);

    signals :

        void unlockSignal();

        void setProgressBarState(int);

        void displayPhoto(QImage);




    private :


        void addContourPreview(QImage &image);


        bool m_exit;

        QColor m_textColor;

        QStringList m_imagesDir;
        QVector<QImage> m_loadedImages;

        Alignment m_textAlignment;
        Alignment m_imageAlignment;

        int m_nbImagesPage;
        double m_ratio;
        QFont m_font;
        QString m_text;

        ImageLabel *m_preview;
};





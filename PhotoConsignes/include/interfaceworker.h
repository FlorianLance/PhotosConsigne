
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


enum Alignment { BottomLeft, BottomRight, TopLeft, TopRight, Bottom, Top, CenterLeft, CenterRight, Center};

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

        void updateParameters(QVector<bool> removePhotoList,int nbImagesPageV, int nbImagesPageH, double ratio,QFont font, QString text, QColor textColor, int imageAlignment, int textAlignment, bool orientation,
                              double leftMargin, double rightMargin, double topMargin, double bottomMargin, double betweenMargin, bool cutLines, bool zExternMargins, bool zInterMargins, bool zPhotos,  bool zConsignes);

        void updateRotationImage(int index, bool rightRotation);

        void sendPhoto(int index);

    signals :

        void unlockSignal();

        void setProgressBarState(int);

        void displayPhoto(QImage);




    private :


        void addContourPreview(QImage &image);


        bool m_exit;
        bool m_landScapeOrientation;
        bool m_isAllPhotoRemoved;
        bool m_cutLines;
        bool m_zExternMargins;
        bool m_zInterMargins;
        bool m_zPhotos;
        bool m_zConsignes;

        QColor m_textColor;

        QStringList m_imagesDir;
        QVector<QImage> m_loadedImages;

        int m_textAlignment;
        int m_imageAlignment;

        int m_nbImagesPageH;
        int m_nbImagesPageV;

        double m_ratio;
        double m_leftMargin;
        double m_rightMargin;
        double m_topMargin;
        double m_bottomMargin;
        double m_betweenMargin;

        QVector<bool> m_removePhotoList;

        QFont m_font;
        QString m_text;

        ImageLabel *m_preview;
};





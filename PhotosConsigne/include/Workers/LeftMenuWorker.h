
#pragma once

// Qt
#include <QObject>

// local
#include "Utility.h"


namespace pc {

/**
 * @brief ...
 */
class LeftMenuWorker : public QObject
{
    Q_OBJECT

public :

    /**
     * @brief LeftMenuWorker
     */
    LeftMenuWorker()
    {
        qRegisterMetaType<SImage>("SImage");
        qRegisterMetaType<SImages>("SImages");
        qRegisterMetaType<SPDFSettings>("SPDFSettings");
        qRegisterMetaType<Page>("Page");
    }


public slots :

    /**
     * @brief loadImages
     * @param path
     * @param imagesList
     */
    void loadImages(QString path, QStringList imagesList)
    {
        qDebug() << "loadImages";

        emit setProgressBarStateSignal(0);
        emit setProgressBarTextSignal("Chargement des photos...");

        auto nbImages = imagesList.size();

        int idPhoto = 0;
        qreal offset = 1000 / nbImages;
        qreal currentState = 0;

        m_images = SImages(new QList<SImage>);
        m_images->reserve(nbImages);
        for(auto &&imageName : imagesList)
        {
            emit setProgressBarTextSignal("Chargement de " + imageName);

            SImage image(new Image(path + "/" + imageName));

            if(!image->scaledImage.isNull())
            {
                m_images->push_back(image);
                emit photoLoadedSignal(imageName);
            }
            else
                emit setProgressBarTextSignal("Echec chargement photo n°" + QString::number(idPhoto));

            currentState += offset;
            emit setProgressBarStateSignal(static_cast<int>(currentState));
            idPhoto++;
        }

        emit setProgressBarStateSignal(1000);
        emit setProgressBarTextSignal("Photos chargées.");
        emit endLoadingImagesSignal(m_images);
    }


signals :

    /**
     * @brief setProgressBarStateSignal
     * @param state
     */
    void setProgressBarStateSignal(int state);

    /**
     * @brief setProgressBarTextSignal
     * @param text
     */
    void setProgressBarTextSignal(QString text);

    /**
     * @brief endLoadingImagesSignal
     */
    void endLoadingImagesSignal(SImages images);

    /**
     * @brief photoLoadedSignal
     * @param photoName
     */
    void photoLoadedSignal(QString photoName);

private :


    SImages m_images = nullptr;
};
}

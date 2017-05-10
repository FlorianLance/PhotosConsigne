
/**
 * \file PhotoLoaderWorker.hpp
 * \brief defines PhotoLoaderWorker
 * \author Florian Lance
 * \date 04/04/2017
 */

#pragma once

// Qt
#include <QObject>
#include <QPixmap>
#include <QPdfWriter>
#include <QPainter>
#include <QPrinter>
#include <QMessageBox>
#include <QCoreApplication>

// local
#include "Utility.hpp"


namespace pc {

class PhotoLoaderWorker : public QObject{

    Q_OBJECT

public :

    PhotoLoaderWorker(){

        qRegisterMetaType<SPhoto>("SPhoto");
        qRegisterMetaType<SPhotos>("SPhotos");
        qRegisterMetaType<GlobalData>("GlobalData");
        qRegisterMetaType<PCPages>("PCPages");
        qRegisterMetaType<PCPage>("PCPage");
        qRegisterMetaType<QVector<QRectF>>("QVector<QRectF>");
        qRegisterMetaType<QReadWriteLock *>("QReadWriteLock *");
    }

    ~PhotoLoaderWorker(){}

public slots :

    void load_photos(QString path, QStringList photosList){

        emit set_progress_bar_state_signal(0);
        emit set_progress_bar_text_signal("Chargement des photos...");

        auto nbPhotos = photosList.size();

        int idPhoto = 0;
        qreal offset = 750 / nbPhotos;
        qreal currentState = 0;

        m_photosLoaded = std::make_shared<QList<SPhoto>>();
        m_photosLoaded->reserve(nbPhotos);
        for(auto &&imageName : photosList)
        {
//            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);

            bool continueLoop;
            m_locker.lockForRead();
            continueLoop = m_continueLoop;
            m_locker.unlock();

            if(!continueLoop)
                break;

            emit set_progress_bar_text_signal("Chargement de " + imageName);

            SPhoto photo = std::make_shared<Photo>(Photo(path + "/" + imageName));
            if(!photo->scaledPhoto.isNull()){
                m_photosLoaded->push_back(photo);
                emit photo_loaded_signal(photo);
            }
            else{
                emit set_progress_bar_text_signal("Echec chargement photo n°" + QString::number(idPhoto));
            }

            currentState += offset;
            emit set_progress_bar_state_signal(static_cast<int>(currentState));
            photo->globalId = idPhoto++;
        }

        emit set_progress_bar_state_signal(750);
        emit end_loading_photos_signal(m_photosLoaded);
    }

    void kill(){

        m_locker.lockForWrite();
        m_continueLoop = false;
        m_locker.unlock();
    }



signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void photo_loaded_signal(SPhoto photo);

    void end_loading_photos_signal(SPhotos photos);


private :

    SPhotos m_photosLoaded = nullptr;

    QReadWriteLock m_locker;
    bool m_continueLoop = true;
};

}

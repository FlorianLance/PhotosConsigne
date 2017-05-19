
/**
 * \file PhotoLoaderWorker.hpp
 * \brief defines PhotoLoaderWorker
 * \author Florian Lance
 * \date 04/04/2017
 */

#pragma once

// local
#include "Photo.hpp"
#include "Utility.hpp"

namespace pc {

class PhotoLoaderWorker : public QObject{

    Q_OBJECT

public :

    PhotoLoaderWorker();

    virtual ~PhotoLoaderWorker();

public slots :

    void load_photos_directory(QStringList photosPath, int startIndexToInsert);

    void kill();

signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void photo_loaded_signal(SPhoto photo, int indexToInsert);

    void end_loading_photos_signal();


private :

    QReadWriteLock m_locker;
    bool m_continueLoop = true;
};

}

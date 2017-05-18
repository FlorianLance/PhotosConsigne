
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

    PhotoLoaderWorker();

    ~PhotoLoaderWorker(){
        qDebug() << "destructor PhotoLoaderWorker";
    }

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

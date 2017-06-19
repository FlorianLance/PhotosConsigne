
/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/


/**
 * \file PhotoLoaderWorker.cpp
 * \brief defines PhotoLoaderWorker
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "PhotoLoaderWorker.hpp"
#include "DocumentElements.hpp"

// Qt
#include <QCoreApplication>

pc::PhotoLoaderWorker::PhotoLoaderWorker(){

    qRegisterMetaType<SPhoto>("SPhoto");
    qRegisterMetaType<SPhotos>("SPhotos");
    qRegisterMetaType<GlobalSettings>("GlobalDocumentSettings");
    qRegisterMetaType<PCPages>("PCPages");
    qRegisterMetaType<PCPage>("PCPage");
    qRegisterMetaType<SPCPage>("SPCPage");
    qRegisterMetaType<QVector<QRectF>>("QVector<QRectF>");
    qRegisterMetaType<QReadWriteLock *>("QReadWriteLock *");
}


void pc::PhotoLoaderWorker::load_photos_directory(QStringList photosPath, int startIndexToInsert){


    emit set_progress_bar_state_signal(0);
    emit set_progress_bar_text_signal("Chargement des photos...");

    m_locker.lockForWrite();
    m_continueLoop = true;
    m_locker.unlock();

    auto nbPhotos = photosPath.size();

    int idPhoto = startIndexToInsert;
    qreal offset = 750 / nbPhotos;
    qreal currentState = 0;

    SPhotos photosLoaded = std::make_shared<QList<SPhoto>>();
    photosLoaded->reserve(nbPhotos);
    for(const auto &photoPath : photosPath){

        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);

        bool continueLoop;
        m_locker.lockForRead();
        continueLoop = m_continueLoop;
        m_locker.unlock();

        if(!continueLoop){
            break;
        }


        QString splitPath = photoPath;
        if(splitPath.size() > 30){
//            splitPath = splitPath.insert(30, "\n");
        }
        emit set_progress_bar_text_signal("Chargement de " + splitPath);
        SPhoto photo = std::make_shared<Photo>(Photo(photoPath));
        if(!photo->scaledPhoto.isNull()){
            photosLoaded->push_back(photo);
            emit photo_loaded_signal(photo, idPhoto);
        }
        else{
            emit set_progress_bar_text_signal("Echec chargement photo n°" + QString::number(idPhoto));
        }

        currentState += offset;
        ++idPhoto;
        emit set_progress_bar_state_signal(static_cast<int>(currentState));
    }

    emit set_progress_bar_state_signal(750);
    emit end_loading_photos_signal();
}

void pc::PhotoLoaderWorker::kill(){
    qDebug() << "pc::PhotoLoaderWorker::kill()";
    m_locker.lockForWrite();
    m_continueLoop = false;
    m_locker.unlock();
    qDebug() << "end pc::PhotoLoaderWorker::kill()";
}

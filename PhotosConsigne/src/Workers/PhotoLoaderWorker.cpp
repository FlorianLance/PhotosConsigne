

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

pc::PhotoLoaderWorker::~PhotoLoaderWorker(){
    DebugMessage("~PhotoLoaderWorker");
}

void pc::PhotoLoaderWorker::load_photos_directory(QStringList photosPath, int startIndexToInsert){

    DebugMessage dbgMess("load_photos_directory");

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

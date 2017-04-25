
#pragma once

#include <thirdparty/asyncfuture/asyncfuture.h>

#include <QtConcurrent>
#include <QObject>
#include <QFuture>
#include <QImage>

// Example code in the article of "Multithreaded Programming with Future & Promise"
// http://blog.qt.io/blog/2017/04/18/multithreaded-programming-future-promise/#comments

/* Step 1 - An ImageReader class */

//class ImageReader1 : public QObject {
//public:
//    QFuture<QImage> read(const QString& fileName);
//};

//QFuture<QImage> ImageReader1::read(const QString &fileName)
//{
//    auto readImageWorker = [](const QString &fileName) {
//        QImage image;
//        image.load(fileName);
//        return image;
//    };

//    return QtConcurrent::run(readImageWorker, fileName);
//}

// Example of use
//{
//    ImageReader reader;
//    QFuture<QImage> future = reader.read(INPUT);

//    QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>();

//    QObject::connect(watcher, &QFutureWatcher<QImage>::finished,
//            [=]() {
//        setImage(future.result());
//    });

//    watcher->setFuture(future);
//}

/* Step 2 - Image Caching (without AsyncFuture) */

//class ImageReader2 : public QObject {
//public:
//    bool isCached(const QString& fileName) const;

//    QImage readCache(const QString& fileName) const;

//    QFuture<QImage> read(const QString& fileName);

//private:
//    QMap<QString,QImage> m_cache;
//};

//QFuture<QImage> ImageReader2::read(const QString &fileName)
//{
//    auto readImageWorker = [](const QString &fileName) {
//        QImage image;
//        image.load(fileName);
//        return image;
//    };

//    QFuture<QImage> future = QtConcurrent::run(readImageWorker, fileName);

//    QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>(this);

//    auto updateCache = [=]() {
//        m_cache[fileName] = future.result();
//        watcher->deleteLater();
//    };

//    connect(watcher, &QFutureWatcher<QImage>::finished, updateCache);

//    watcher->setFuture(future);

//    return future;
//}

//bool ImageReader2::isCached(const QString &fileName) const
//{
//    return m_cache.contains(fileName);
//}

//QImage ImageReader2::readCache(const QString &fileName) const
//{
//    return m_cache[fileName];
//}

/* Step 3 - Image Caching with AsyncFuture */

//class ImageReader3 : public QObject {
//public:
//    QFuture<QImage> read(const QString& fileName);

//private:
//    QMap<QString,QImage> m_cache;
//    QMap<QString, QFuture<QImage>> m_futures;
//};

//QFuture<QImage> ImageReader3::read(const QString &fileName)
//{
//    if (m_cache.contains(fileName)) {
//        // Cache hit. Return an already finished QFuture object with the image
//        auto defer = AsyncFuture::deferred<QImage>();
//        defer.complete(m_cache[fileName]);
//        return defer.future();
//    }

//    if (m_futures.contains(fileName)) {
//        // It is loading. Return the running QFuture
//        return m_futures[fileName];
//    }

//    auto readImageWorker = [](const QString &fileName) {
//        QImage image;
//        image.load(fileName);
//        return image;
//    };

//    auto updateCache = [=](QImage result) {
//        m_cache[fileName] = result;
//        m_futures.remove(fileName);
//        return result;
//    };

//    QFuture<QImage> future = AsyncFuture::observe(QtConcurrent::run(readImageWorker, fileName)).subscribe(updateCache).future();

//    m_futures[fileName] = future;
//    return future;
//}

/* Step 4 - ImageReader with readScaled function */

//class ImageReader4 : public QObject {
//public:
//    QFuture<QImage> read(const QString& fileName);
//    QFuture<QImage> readScaled(const QString& fileName, const QSize& size);

//private:
//    QMap<QString,QImage> m_cache;
//    QMap<QString, QFuture<QImage>> m_futures;
//};

//QFuture<QImage> ImageReader4::read(const QString &fileName)
//{
//    if (m_cache.contains(fileName)) {
//        // Cache hit. Return an already finished QFuture object with the image
//        auto defer = AsyncFuture::deferred<QImage>();
//        defer.complete(m_cache[fileName]);
//        return defer.future();
//    }

//    if (m_futures.contains(fileName)) {
//        // It is loading. Return the running QFuture
//        return m_futures[fileName];
//    }

//    auto readImageWorker = [](const QString &fileName) {
//        QImage image;
//        image.load(fileName);
//        return image;
//    };

//    auto updateCache = [=](QImage result) {
//        m_cache[fileName] = result;
//        m_futures.remove(fileName);
//        return result;
//    };

//    QFuture<QImage> future = AsyncFuture::observe(QtConcurrent::run(readImageWorker, fileName)).subscribe(updateCache).future();

//    m_futures[fileName] = future;
//    return future;
//}

//QFuture<QImage> ImageReader4::readScaled(const QString &fileName, const QSize &size)
//{
//    auto scaleImageWorker = [=](QImage input) {
//        // Run in worker thread
//        return input.scaled(size);
//    };

//    auto callback = [=](QImage result) {
//        // Run in main thread
//        return QtConcurrent::run(scaleImageWorker, result);
//    };

//    QFuture<QImage> input = read(fileName);

//    QFuture<QImage> output = AsyncFuture::observe(input).subscribe(callback).future();

//    return output;
//}


#include "Utility.h"

class ImageReader5 : public QObject {

    Q_OBJECT

public :

    ImageReader5()
    {
        qRegisterMetaType<pc::SPhotos>("SPhotos");
//        qRegisterMetaType<GlobalParameters>("GlobalParameters");
//        qRegisterMetaType<PCPages>("PCPages");
//        qRegisterMetaType<PCPage>("PCPage");
//        qRegisterMetaType<QVector<QRectF>>("QVector<QRectF>");
//        qRegisterMetaType<QReadWriteLock *>("QReadWriteLock *");
    }



    QFuture<pc::SPhotos> read_photos(const QString& directory, const QStringList &files){

        auto readImagesWorker = [this](const QString& directory, const QStringList &files) {
    //        QImage image;
            pc::SPhotos photos = std::make_shared<QList<pc::SPhoto>>();

            emit set_progress_bar_state_signal(0);
//            emit set_progress_bar_text_signal("Chargement des photos...");

            auto nbPhotos = files.size();

            int idPhoto = 0;
            qreal offset = 750 / nbPhotos;
            qreal currentState = 0;
            photos->reserve(nbPhotos);

            for(auto &&photoName : files)
            {
//                emit set_progress_bar_text_signal("Chargement de " + photoName);

                pc::SPhoto photo = std::make_shared<pc::Photo>(pc::Photo(directory + "/" + photoName));
                if(!photo->scaledPhoto.isNull()){
                    photos->push_back(photo);
//                    emit photo_loaded_signal(photoName);
                }
                else{
//                    emit set_progress_bar_text_signal("Echec chargement photo n°" + QString::number(idPhoto));
                }

                currentState += offset;
                emit set_progress_bar_state_signal(static_cast<int>(currentState));
                idPhoto++;
            }

            emit set_progress_bar_state_signal(750);
//            emit end_loading_photos_signal(m_photosLoaded);

            return photos;
        };

        QFuture<pc::SPhotos> future = QtConcurrent::run(readImagesWorker, directory, files);//AsyncFuture::observe(QtConcurrent::run(readImagesWorker, directory, files)).s
        return future;
    }

//    QFuture<pc::SPhoto> read(const QString& fileName){

//        if (m_cache.contains(fileName)) {
//            // Cache hit. Return an already finished QFuture object with the image
//            auto defer = AsyncFuture::deferred<pc::SPhoto>();
//            defer.complete(m_cache[fileName]);
//            return defer.future();
//        }

//        if (m_futures.contains(fileName)) {
//            // It is loading. Return the running QFuture
//            return m_futures[fileName];
//        }

//        auto readImageWorker = [](const QString &fileName) {
//    //        QImage image;
//            pc::SPhoto image = std::make_shared<pc::Photo>(pc::Photo(fileName));
//    //        image.load(fileName);
//            return image;
//        };

//        auto updateCache = [=](pc::SPhoto result) {
//            m_cache[fileName] = result;
//            m_futures.remove(fileName);
//            return result;
//        };

//        QFuture<pc::SPhoto> future = AsyncFuture::observe(QtConcurrent::run(readImageWorker, fileName)).subscribe(updateCache).future();

//        m_futures[fileName] = future;
//        return future;
//    }

signals :

    void set_progress_bar_state_signal(int state);

//    void set_progress_bar_text_signal(QString text);

//    void end_loading_photos_signal(pc::SPhotos photos);

//    void photo_loaded_signal(QString photoName);

private:
//    QMap<QString,pc::SPhoto> m_cache;
//    QMap<QString, QFuture<pc::SPhoto>> m_futures;
};


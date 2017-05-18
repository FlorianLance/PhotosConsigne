
#include "PreviewW.hpp"

// Qt
#include <QTime>

using namespace pc;


void PreviewWorker::stop_loop(){

    m_locker.lockForWrite();
    m_isLooping = false;
    m_locker.unlock();
}

void PreviewWorker::loop_update(){

    m_locker.lockForWrite();
    bool isLooping = m_isLooping;
    m_isLooping = true;
    m_locker.unlock();

    if(isLooping){
        return;
    }

    while (true){

        m_locker.lockForRead();
        isLooping = m_isLooping;
        m_locker.unlock();

        if(!isLooping){
            break;
        }

        QTime dieTime = QTime::currentTime().addMSecs(33);
        while( QTime::currentTime() < dieTime){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 33);
        }

        emit update_preview_signal();
    }
}

PreviewW::PreviewW(){

    connect(&m_rectTimer, &QTimer::timeout, this, [&]{
        m_rectTimer.stop();
        emit stop_update_loop_signal();
        update();
    });

    m_worker = std::make_unique<PreviewWorker>();
    connect(m_worker.get(), &PreviewWorker::update_preview_signal, this, [=]{
        update();
    });

    connect(this, &PreviewW::start_update_loop_signal, m_worker.get(), &PreviewWorker::loop_update);
    connect(this, &PreviewW::stop_update_loop_signal, m_worker.get(), &PreviewWorker::stop_loop);

    m_worker->moveToThread(&m_workerThread);
    m_workerThread.start();
}

PreviewW::~PreviewW(){
    m_workerThread.quit();
    m_workerThread.wait();
}

void PreviewW::draw_current_pc_rect(int idRect, QRectF pcRectRelative){

    m_currentPCRectId = idRect;
    m_pcRectRelative = pcRectRelative;
    m_rectTimer.start(2000);
    emit start_update_loop_signal();
}



#include "PreviewW.hpp"

// Qt
#include <QTime>
#include <QCoreApplication>
#include <QPainter>

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

void PreviewW::set_page(SPCPage previewPage){
    m_previewPage       = previewPage;
}

void PreviewW::set_current_pc(int idPC){

    if(m_previewPage == nullptr){
        return;
    }

    if(idPC == -1){
        return;
    }


    if(m_currentRectId == idPC){ // rectangle already selected
        return;
    }

    m_currentRectId = -1;

    int idSet = 0;
    for(int ii = 0; ii < m_previewPage->sets.size(); ++ii){
        if(m_previewPage->sets[ii]->totalId == idPC){
            m_currentRectId = idPC;
            idSet = ii;
            break;
        }
    }

    if(m_currentRectId == -1){
        return;
    }

    m_rectTimer.start(2000);
    emit start_update_loop_signal();
    auto set =  m_previewPage->sets[idSet];
    QRectF rectPage = m_previewPage->rectOnPage;
    m_rectRelative = QRectF(set->rectOnPage.x()/rectPage.width(), set->rectOnPage.y()/rectPage.height(),
                          set->rectOnPage.width()/rectPage.width(), set->rectOnPage.height()/rectPage.height());
}

void PreviewW::mousePressEvent(QMouseEvent *ev){

    bool inside = m_imageRect.contains(ev->pos());
    if(inside){ // click inside preview image

        if(m_previewPage == nullptr){
            return;
        }

        QPointF posRelative =(ev->pos() - m_imageRect.topLeft());
        posRelative.setX(posRelative.x()/m_imageRect.width());
        posRelative.setY(posRelative.y()/m_imageRect.height());

        QRectF rectPage = m_previewPage->rectOnPage;
        QPointF realPos(rectPage.x() + posRelative.x()*rectPage.width(), rectPage.y() + posRelative.y()*rectPage.height());

        int previousRectId = m_currentRectId;
        m_currentRectId = -1;
        for(auto &&set : m_previewPage->sets){
            if(set->rectOnPage.contains(realPos)){
                if(set->totalId > m_currentRectId){
                    m_currentRectId = set->totalId;
                    m_rectRelative = QRectF(set->rectOnPage.x()/rectPage.width(), set->rectOnPage.y()/rectPage.height(),
                                          set->rectOnPage.width()/rectPage.width(), set->rectOnPage.height()/rectPage.height());
                }
            }
        }

        if(m_currentRectId > -1){ // click on set

            if(m_doubleClickTimer.isActive() && m_currentRectId == previousRectId){ // second click on same pc before timer finish
                emit double_click_on_photo_signal(m_currentRectId);
            }else{ // reset timer
                m_doubleClickTimer.start(300);
                m_rectTimer.start(2000);
                emit start_update_loop_signal();
                emit current_pc_selected_signal(m_currentRectId);
            }
        }else{

            if(m_previewPage->header->rectOnPage.contains(realPos)){ // click on header
                m_rectRelative = QRectF(m_previewPage->header->rectOnPage.x()/rectPage.width(), m_previewPage->header->rectOnPage.y()/rectPage.height(),
                                        m_previewPage->header->rectOnPage.width()/rectPage.width(), m_previewPage->header->rectOnPage.height()/rectPage.height());
                m_rectTimer.start(2000);
                m_currentRectId = -2;

                emit start_update_loop_signal();
                emit current_pc_selected_signal(m_currentRectId);

            }else if(m_previewPage->footer->rectOnPage.contains(realPos)){ // click on footer
                m_rectRelative = QRectF(m_previewPage->footer->rectOnPage.x()/rectPage.width(), m_previewPage->footer->rectOnPage.y()/rectPage.height(),
                                        m_previewPage->footer->rectOnPage.width()/rectPage.width(), m_previewPage->footer->rectOnPage.height()/rectPage.height());
                m_rectTimer.start(2000);
                m_currentRectId = -3;

                emit start_update_loop_signal();
                emit current_pc_selected_signal(m_currentRectId);
            }
        }
    }
}

void PreviewW::paintEvent(QPaintEvent *event){

    PhotoW::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if(m_rectRelative.width() > 0 && m_rectTimer.isActive() && m_currentRectId != -1){

        m_currentPCRect = QRectF(m_imageRect.x() + m_rectRelative.x()*m_imageRect.width(),
                                 m_imageRect.y() + m_rectRelative.y()*m_imageRect.height(),
                                 m_rectRelative.width()*m_imageRect.width(),
                                 m_rectRelative.height()*m_imageRect.height());

        int alpha = (m_rectTimer.remainingTime() > 1500) ? 90 : (90*m_rectTimer.remainingTime()/1500.);
        painter.fillRect(m_currentPCRect, QColor(0,0,255,alpha));
    }
}


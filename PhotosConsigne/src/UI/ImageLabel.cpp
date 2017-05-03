/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file imageLabel.cpp
 * \brief defines ImageLabel
 * \author Florian Lance
 * \date 01/11/15
 */

// Qt
#include <QPainter>
#include <QPaintEvent>
#include <QTime>

// local
#include "ImageLabel.hpp"


ImageLabel::ImageLabel(QWidget *parent) : QWidget(parent) {

    connect(&m_doubleClickTimer, &QTimer::timeout, this, [&]{
        m_doubleClickTimer.stop();
    });
}

void ImageLabel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize imageSize = m_image.size();
    imageSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QImage scaledImage = m_image.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imageRect = QRectF(width()*0.5-scaledImage.size().width()*0.5,height()*0.5-scaledImage.size().height()*0.5,
                         scaledImage.width(), scaledImage.height());



    painter.drawImage(m_imageRect.x(), m_imageRect.y(), scaledImage);
}

const QImage* ImageLabel::Image() const {
    return &m_image;
}

void ImageLabel::set_image (const QImage &image){
    m_image = image;
}


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

PreviewLabel::PreviewLabel(){

    connect(&m_rectTimer, &QTimer::timeout, this, [&]{
        m_rectTimer.stop();
        emit stop_update_loop_signal();
        update();
    });

    m_worker = std::make_unique<PreviewWorker>();
    connect(m_worker.get(), &PreviewWorker::update_preview_signal, this, [=]{
        update();
    });

    connect(this, &PreviewLabel::start_update_loop_signal, m_worker.get(), &PreviewWorker::loop_update);
    connect(this, &PreviewLabel::stop_update_loop_signal, m_worker.get(), &PreviewWorker::stop_loop);

    m_worker->moveToThread(&m_workerThread);
    m_workerThread.start();
}

PreviewLabel::~PreviewLabel(){
    m_workerThread.quit();
    m_workerThread.wait();
}

void PreviewLabel::draw_current_pc_rect(int idRect, QRectF pcRectRelative){

    m_currentPCRectId = idRect;
    m_pcRectRelative = pcRectRelative;
    m_rectTimer.start(2000);
    emit start_update_loop_signal();
}

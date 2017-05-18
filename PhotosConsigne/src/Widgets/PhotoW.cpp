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
#include "PhotoW.hpp"


PhotoW::PhotoW(QWidget *parent) : QWidget(parent) {

    connect(&m_doubleClickTimer, &QTimer::timeout, this, [&]{
        m_doubleClickTimer.stop();
    });
}

void PhotoW::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (m_image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    QSize widgetSize = event->rect().size();
    QSize imageSize = m_image.size();
//    imageSize.scale(event->rect().size(), Qt::KeepAspectRatio);
    imageSize.scale(QSize(widgetSize.width()-2, widgetSize.height()-2), Qt::KeepAspectRatio);

    QImage scaledImage = m_image.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_imageRect = QRectF(width()*0.5-scaledImage.size().width()*0.5,height()*0.5-scaledImage.size().height()*0.5,
                         scaledImage.width(), scaledImage.height());

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);    
    painter.drawImage(static_cast<int>(m_imageRect.x()), static_cast<int>(m_imageRect.y()), scaledImage);
    painter.drawRect(QRectF(m_imageRect.x()-1, m_imageRect.y(), scaledImage.width()+1, scaledImage.height()+1));
}

const QImage* PhotoW::Image() const {
    return &m_image;
}

void PhotoW::set_image (const QImage &image){
    m_image = image;
}



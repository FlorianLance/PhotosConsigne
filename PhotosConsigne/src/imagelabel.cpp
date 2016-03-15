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

// Photos Consigne
#include "ImageLabel.h"


ImageLabel::ImageLabel(QWidget *parent) :
    QWidget(parent)
{}

void ImageLabel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (pix.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = pix.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPixmap scaledPix = pix.scaled(pixSize,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation
                                   );

    painter.drawPixmap(width()/2-scaledPix.size().width()/2,height()/2-scaledPix.size().height()/2,scaledPix);
//    painter.drawPixmap(QPoint(), scaledPix);

}

const QPixmap* ImageLabel::pixmap() const {
    return &pix;
}

void ImageLabel::setPixmap (const QPixmap &pixmap){
    pix = pixmap;
}

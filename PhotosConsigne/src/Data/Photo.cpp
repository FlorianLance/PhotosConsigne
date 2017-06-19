

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
 * \file Photos.cpp
 * \brief defines Photos
 * \author Florian Lance
 * \date 04/04/2017
 */

// Qt
#include <QDebug>

// local
#include "Photo.hpp"


using namespace pc;

pc::Photo::Photo(QImage image){

    scaledPhoto  = image;
    originalSize = scaledPhoto.size();
}

pc::Photo::Photo(const QString &path, bool isWhiteSpace) : isWhiteSpace(isWhiteSpace), pathPhoto(path){

    constexpr int maxHeight = 800;
    constexpr int maxWidth  = 800;

    if(isWhiteSpace){
        namePhoto = "Espace transparent";
    }else{

        info            = QFileInfo(path);
        scaledPhoto     = QImage(path);
        originalSize    = scaledPhoto.size();

        if(!scaledPhoto.isNull()){

            namePhoto = pathPhoto.split('/').last().split('.').first();
            if(scaledPhoto.size().width() > maxWidth)
                scaledPhoto = scaledPhoto.scaledToWidth(maxWidth);
            if(scaledPhoto.size().height() > maxHeight)
                scaledPhoto = scaledPhoto.scaledToHeight(maxHeight);
        }
        else{
            namePhoto = "Erreur";
            qWarning() << "-Error: photo not loaded: " << pathPhoto;
        }
    }
}

void pc::Photo::draw(QPainter &painter, const ImagePositionSettings &position, const QRectF &rectPhoto, const ExtraPCInfo &infos, const QSizeF &pageSize){

    if(isWhiteSpace){
        return;
    }

    if(scaledPhoto.isNull()){
        qWarning() << "-Error: photo is null, can't be drawn";
        return;
    }

    if(infos.preview){
        draw_small(painter, position, rectPhoto, scaledPhoto, infos, pageSize);
    }else{
        if(rectPhoto.width() > 32000 || rectPhoto.height() > 32000){
//            draw_huge(painter, rectPhoto);
            qWarning() << "-Error: Format too huge: " << namePhoto << " can't be drawn.";
        }else{
//            draw_small(painter, rectPhoto, (pathPhoto.size() > 0) ? QImage(pathPhoto) : scaledPhoto, infos, pageSize);
            draw_small(painter, position, rectPhoto, (pathPhoto.size() > 0) ? QImage(pathPhoto).transformed((QTransform().rotate(rotation))) : scaledPhoto, infos, pageSize);
        }
    }
}

QRectF pc::Photo::draw_small(QPainter &painter, const ImagePositionSettings &position, const QRectF &rectPhoto, const QImage &photo, const ExtraPCInfo &infos, const QSizeF &pageSize){

    int startX, startY;
    qreal newX, newY, newWidth, newHeight;
    QImage photoToDraw;

    int rectPhotoWidth  = static_cast<int>(rectPhoto.width());
    int rectPhotoHeight = static_cast<int>(rectPhoto.height());

    // scale and rotate photo
    qreal scaleCenter;
    switch(position.adjustment){

        case PhotoAdjust::adjust:

            photoToDraw = photo.scaled(rectPhotoWidth,rectPhotoHeight, Qt::KeepAspectRatio);

            {
                qreal diffX = rectPhoto.width() - photoToDraw.width();
                qreal diffY = rectPhoto.height() - photoToDraw.height();
                newX      = static_cast<int>(diffX * position.xPos) + rectPhoto.x();
                newY      = static_cast<int>(diffY * (1. - position.yPos)) + rectPhoto.y();
                newWidth  = photoToDraw.width();
                newHeight = photoToDraw.height();
            }

        break;
        case PhotoAdjust::extend:

            photoToDraw = photo.scaled(rectPhotoWidth, rectPhotoHeight, Qt::IgnoreAspectRatio);
            newX      = rectPhoto.x();
            newY      = rectPhoto.y();
            newWidth  = photoToDraw.width();
            newHeight = photoToDraw.height();

        break;
        case PhotoAdjust::center:

//            if(photo.width() > rectPhoto.width() || photo.height() > rectPhoto.height()){
//                scaleCenter = position.scale;//(position.scale < 1.) ? position.scale : 1.;
//                qDebug() << "rescale";
//                photoToDraw = photo.scaled(static_cast<int>(rectPhotoWidth*position.scale), static_cast<int>(rectPhotoHeight*position.scale), Qt::KeepAspectRatio);
//            }else{
//                photoToDraw = photo;
//            }

            photoToDraw = photo.scaled(static_cast<int>(rectPhotoWidth*position.scale), static_cast<int>(rectPhotoHeight*position.scale), Qt::KeepAspectRatio);


            {
                qreal diffX = rectPhoto.width() - photoToDraw.width();
                qreal diffY = rectPhoto.height() - photoToDraw.height();
                newX      = static_cast<int>(diffX * position.xPos) + rectPhoto.x();
                newY      = static_cast<int>(diffY * (1. - position.yPos)) + rectPhoto.y();
                newWidth  = photoToDraw.width();
                newHeight = photoToDraw.height();
            }

        break;
        case PhotoAdjust::fill:

            scaleCenter = (position.scale > 1.) ? position.scale : 1.;
            photoToDraw = photo.scaled(static_cast<int>(rectPhotoWidth*scaleCenter), static_cast<int>(rectPhotoHeight*scaleCenter), Qt::KeepAspectRatioByExpanding);

            startX = static_cast<int>((photoToDraw.width() - rectPhoto.width())*position.xPos);
            startY = static_cast<int>((photoToDraw.height() - rectPhoto.height())*(1. - position.yPos));

            photoToDraw = photoToDraw.copy(QRect(startX, startY, static_cast<int>(rectPhoto.width()), static_cast<int>(rectPhoto.height())));
            newX      = rectPhoto.x();
            newY      = rectPhoto.y();
            newWidth  = photoToDraw.width();
            newHeight = photoToDraw.height();

        break;
        case PhotoAdjust::mosaic:

            QSize scaledSize(infos.factorUpscale* position.scale*photo.width(), infos.factorUpscale*position.scale*photo.height());

            if(!infos.preview){
                scaledSize.setWidth(scaledSize.width() *(1.*scaledPhoto.width()/photo.width()));
                scaledSize.setHeight(scaledSize.height() *(1.*scaledPhoto.height()/photo.height()));
            }

            QImage tile = photo.scaled(scaledSize, Qt::IgnoreAspectRatio);
            if(tile.width() > rectPhoto.width() || tile.height() > rectPhoto.height()){
                tile = tile.scaled(rectPhotoWidth, rectPhotoHeight, Qt::KeepAspectRatio);
            }

            int nbTilesH = rectPhotoWidth  / tile.width()  + ((rectPhotoWidth % tile.width() > 0) ? 1 : 0);
            int nbTilesV = rectPhotoHeight / tile.height() + ((rectPhotoHeight % tile.height() > 0) ? 1 : 0);

            QImage tiles(tile.width()*nbTilesH, tile.height()*nbTilesV, QImage::Format_ARGB32);
            QPainter tilePainter(&tiles);
            for(int ii = 0; ii < nbTilesH; ++ii){
                for(int jj = 0; jj < nbTilesV; ++jj){
                    tilePainter.drawImage(QRectF(ii*tile.width(), jj* tile.height(), tile.width(), tile.height()), tile, QRectF(0,0,tile.width(), tile.height()));
                }
            }
            tilePainter.end();


//            startX = 0; // left
//            if(alignment & Qt::AlignHCenter){ // right
//                startX = static_cast<int>((tiles.width() - rectPhoto.width())*0.5);
//            }else if(alignment & Qt::AlignRight){ // h center
//                startX = static_cast<int>(tiles.width() - rectPhoto.width());
//            }

//            startY = 0; // top
//            if(alignment & Qt::AlignVCenter){ // V center
//                startY = static_cast<int>((tiles.height() - rectPhoto.height())*0.5);
//            }else if(alignment & Qt::AlignBottom){ // bottom
//                startY = static_cast<int>(tiles.height() - rectPhoto.height());
//            }

            startX = static_cast<int>((tiles.width() - rectPhoto.width())*position.xPos);
            startY = static_cast<int>((tiles.height() - rectPhoto.height())*(1. - position.yPos));


            photoToDraw = tiles.copy(QRect(startX,startY,rectPhotoWidth,rectPhotoHeight));

            newX      = rectPhoto.x();
            newY      = rectPhoto.y();
            newWidth  = photoToDraw.width();
            newHeight = photoToDraw.height();

        break;
    }


    // draw image
    QRectF newRectPhoto(newX, newY, newWidth, newHeight);
    painter.drawImage(newRectPhoto, photoToDraw, QRectF(0.,0.,photoToDraw.width(),photoToDraw.height()));

    // draw size
    if(infos.displaySizes && newWidth > 100 && newHeight > 50){

        QPen pen;
        pen.setColor(Qt::red);
        painter.setPen(pen);
        QFont font;

        QString sizeImageStr = QString::number(newRectPhoto.width()/pageSize.width()*infos.paperFormat.sizeMM.width(), 'i',0) + "x" +
                QString::number(newRectPhoto.height()/pageSize.height()*infos.paperFormat.sizeMM.height(), 'i', 0) +"(mm)";
        font.setPointSizeF(newRectPhoto.width()/sizeImageStr.size());
        painter.setFont(font);
        painter.drawText(QRectF(newX, newY, newWidth, newHeight),  Qt::AlignCenter,sizeImageStr);
    }

    return newRectPhoto;
}


void pc::Photo::draw_huge_photo_whith_tiles(QPainter &painter, const QImage &photoToUpscale, const QRectF &rectPhoto){

    QSizeF upscaledSize(rectPhoto.width(),rectPhoto.height());

    qreal ratioW = 1.*photoToUpscale.width()  / upscaledSize.width();
    qreal ratioH = 1.*photoToUpscale.height() / upscaledSize.height();
    if(ratioW > ratioH){
        upscaledSize = QSizeF(upscaledSize.width(), photoToUpscale.height()/ratioW);
    }else{
        upscaledSize = QSizeF(photoToUpscale.width()/ratioH, upscaledSize.height());
    }

    // draw tiles
    int widthUpPerTile  = (upscaledSize.width() /10);
    int heightUpPerTile = (upscaledSize.height()/10);

    int widthUpTileRemainder  = static_cast<int>(upscaledSize.width())%widthUpPerTile;
    int heightUpTileRemainder = static_cast<int>(upscaledSize.height())%heightUpPerTile;

    int nbTilesH            = upscaledSize.width()/widthUpPerTile;
    int widthPerTile        = photoToUpscale.width()/nbTilesH;
    int widthTileRemainder  = static_cast<int>(photoToUpscale.width())%widthPerTile;

    int nbTilesV            = upscaledSize.height()/heightUpPerTile;
    int heightPerTile       = photoToUpscale.height()/nbTilesV;
    int heightTileRemainder = static_cast<int>(photoToUpscale.height())%heightPerTile;

    int nbTotalTilesH = nbTilesH + (widthTileRemainder  > 0 ? 1:0);
    int nbTotalTilesV = nbTilesV + (heightTileRemainder > 0 ? 1:0);

    for(int ii = 0; ii < nbTotalTilesH; ++ii){

        bool lastTileH   = (widthTileRemainder > 0) && (ii == nbTotalTilesH-1);
        bool lastTileUpH = (widthUpTileRemainder > 0) && (ii == nbTotalTilesH-1);

        for (int jj = 0; jj < nbTotalTilesV; ++jj){

            bool lastTileV = (heightTileRemainder > 0) && (jj == nbTotalTilesV-1);
            bool lastTileUpV = (heightUpTileRemainder > 0) && (jj == nbTotalTilesV-1);
            QRectF itRect(ii* widthPerTile, jj * heightPerTile, lastTileH ? widthTileRemainder : widthPerTile, lastTileV ?  heightTileRemainder : heightPerTile);
            QRectF itUpRect(ii* widthUpPerTile, jj * heightUpPerTile, lastTileUpH ? widthUpTileRemainder : widthUpPerTile, lastTileUpV ? heightUpTileRemainder : heightUpPerTile);
            size_t offset = itRect.x() * photoToUpscale.depth() / 8 + itRect.y() * photoToUpscale.bytesPerLine();

            painter.drawImage(rectPhoto.x()+itUpRect.x(),rectPhoto.y()+itUpRect.y(), QImage(photoToUpscale.bits() + offset, itRect.width(),itRect.height(),
                                                                                            photoToUpscale.bytesPerLine(), photoToUpscale.format()).scaled(itUpRect.width(),itUpRect.height()));
        }
    }
}

void pc::Photo::draw_huge(QPainter &painter, const QRectF &rectPhoto){

    Q_UNUSED(painter)
    Q_UNUSED(rectPhoto)
//    // TODO
//    QImage rotatedPhoto =  (pathPhoto.size() > 0 ? QImage(pathPhoto) : scaledPhoto).transformed(QTransform().rotate(rotation));
////    qreal newX = rectPhoto.x(), newY = rectPhoto.y();
////    draw_huge_photo_whith_tiles(painter, rotatedPhoto, QRectF(newX,newY,rectPhoto.width(),rectPhoto.height()));

//    qreal ratio;
//    qreal ratioW = (rectPhoto.width()  / 5000);
//    qreal ratioH = (rectPhoto.height() / 5000);

//    qreal smallWidth, smallHeight;
//    if(ratioW < ratioH){
//        ratio = ratioW;
//    }else{
//        ratio = ratioH;
//    }

//    smallWidth  = (rectPhoto.width() /ratio);
//    smallHeight = (rectPhoto.height()/ratio);

//    QImage smallPhoto(smallWidth, smallHeight, QImage::Format_ARGB32);
//    smallPhoto.save("G:/t.png");
//    QPainter smallPainter(&smallPhoto);
//    QRectF newRectPhoto = draw_small(smallPainter, QRectF(0.,0.,smallWidth,smallHeight), rotatedPhoto, false, false, QSizeF(), QSizeF());

//    draw_huge_photo_whith_tiles(painter, smallPhoto, QRectF(newRectPhoto.x()*ratio, newRectPhoto.y()*ratio, rectPhoto.width()*ratio, rectPhoto.height()*ratio));
}

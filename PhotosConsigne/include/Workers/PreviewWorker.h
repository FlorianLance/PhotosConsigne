
#pragma once

// Qt
#include <QObject>
#include <QImage>
#include <QPainter>

// local
#include "Utility.h"


namespace pc{

/**
 * @brief ...
 */
class PreviewWorker : public QObject
{
    Q_OBJECT

public :

    /**
     * @brief PreviewWorker
     */
    PreviewWorker()
    {
        qRegisterMetaType<QPixmap>("QPixmap");
        qRegisterMetaType<PreviewZonesDisplay>("PreviewZonesDisplay");                
    }

public slots :

    void generatePreview(SImages images, SPDFSettings settings, Page page, PreviewZonesDisplay zones)
    {
        qDebug() << "generatePreview";

        int width, height;
        if(settings->pageOrientation == PageOrientation::landScape)
        {
            width = 3507;
            height = 2480;
        }
        else
        {
            width = 2480;
            height = 3507;
        }

        QImage image(width, height, QImage::Format_RGB32);

        if(zones.externMargins)
            image.fill(Qt::GlobalColor::red);
        else
            image.fill(Qt::GlobalColor::white);
        QPainter painter(&image);

        // total
        int heightTotal = height * (1.0 - settings->margins.bottom - settings->margins.top);
        int widthTotal = width * (1.0 - settings->margins.left - settings->margins.right);

       // bewteen margin
        int heightMargin =  heightTotal * settings->margins.interHeight;
        int widthMargin  = widthTotal * settings->margins.interWidth;

        // photo and margin
        int heightPhotoAndConsigne = (heightTotal  - (settings->nbImagesPageV-1) * heightMargin) / settings->nbImagesPageV;
        int widthPhotoAndConsigne = (widthTotal  - (settings->nbImagesPageH-1) * widthMargin) / settings->nbImagesPageH;

        // sizes
        //  consign
        int heightConsigne = heightPhotoAndConsigne * settings->ratioPhotosConsign;
        int widthConsigne = widthPhotoAndConsigne;
        //  photo
        int heightPhoto = (1-settings->ratioPhotosConsign) *  heightPhotoAndConsigne;
        int widthPhoto = widthPhotoAndConsigne;

        // positions
        int vPositionConsigne, hPositionConsigne, vPositionPhoto, hPositionPhoto;
        if(settings->globalConsignPositionFromImages == Position::top)
        {
            vPositionConsigne = settings->margins.top * height;
            hPositionConsigne = settings->margins.left * width;

            vPositionPhoto = vPositionConsigne + heightConsigne;
            hPositionPhoto = settings->margins.left * width;
        }
        else
        {
            vPositionPhoto = settings->margins.top * height;
            hPositionPhoto = settings->margins.left * width;

            vPositionConsigne = vPositionPhoto + heightPhoto;
            hPositionConsigne = settings->margins.left * width;
        }


        QRect rect = QRect(settings->margins.left* width, settings->margins.top* height, (1-settings->margins.left-settings->margins.right)* width, (1-settings->margins.top-settings->margins.bottom)* height);
        if(zones.externMargins)
        {
            painter.fillRect(rect, Qt::white);
            painter.drawRect(rect);
        }

        // draw margins
        if(zones.InterMargins)
        {
            int startPositionV, startPositionH;
            if(settings->globalConsignPositionFromImages == Position::bottom)
            {
                startPositionV = vPositionPhoto;
                startPositionH = hPositionPhoto;
            }
            else
            {
                startPositionV = vPositionConsigne;
                startPositionH = hPositionConsigne;
            }

            for(int ii = 0; ii < settings->nbImagesPageV -1; ++ii)
            {
                QRect rect = QRect(0, startPositionV + (ii+1)*heightPhotoAndConsigne + ii*heightMargin, width, heightMargin);
                painter.fillRect(rect, Qt::green);
            }

            for(int ii = 0; ii < settings->nbImagesPageH-1; ++ii)
            {
                QRect rect = QRect(startPositionH + (ii+1)*widthPhotoAndConsigne + ii*widthMargin, 0, widthMargin, height);
                painter.fillRect(rect, Qt::green);
            }
        }

        // draw cut lines
        if(settings->displayCutLines)
        {
            QPen currentPen = painter.pen();
            currentPen.setStyle(Qt::DashLine);
            currentPen.setWidth(1);
            painter.setPen(currentPen);
            for(int ii = 0; ii < settings->nbImagesPageV-1; ++ii)
            {
                painter.drawLine(0, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2, width, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2);
            }
            for(int ii = 0; ii < settings->nbImagesPageH-1; ++ii)
            {
                painter.drawLine(hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2, 0, hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2,height);
            }
            currentPen.setStyle(Qt::SolidLine);
            painter.setPen(currentPen);
        }

        // define font/pen
        painter.setPen(settings->globalConsignColor);
        painter.setFont(settings->globalConsignFont);

        for(int ii = 0; ii < settings->nbImagesPageV; ++ii)
        {
            for(int jj = 0; jj < settings->nbImagesPageH; ++jj)
            {
                int offsetBetweenVMargin = ii * heightMargin;
                int offsetBetweenHMargin = jj * widthMargin;

                // draw consigne rectangle
                if(zones.consigns)
                {
                    QRect rect = QRect(offsetBetweenHMargin  + hPositionConsigne + jj*widthPhotoAndConsigne,
                                       offsetBetweenVMargin + vPositionConsigne + ii*heightPhotoAndConsigne,widthConsigne, heightConsigne);
                    painter.fillRect(rect, Qt::cyan);
                }

                // draw photo rectangle
                if(zones.photos)
                {
                    rect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                 offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii,widthPhoto, heightPhoto);
                    painter.fillRect(rect, Qt::yellow);
                }
            }
        }

        int currPhoto = 0;

        bool allRemoved = true;
        for(auto &&image : (*images))
        {
            if(!image->isRemoved)
            {
                allRemoved = false;
                break;
            }
        }


        for(int ii = 0; ii < settings->nbImagesPageV; ++ii)
        {
            for(int jj = 0; jj < settings->nbImagesPageH; ++jj)
            {
                if(currPhoto >= images->size())
                    break;

                int offsetBetweenVMargin = ii * heightMargin;
                int offsetBetweenHMargin = jj * widthMargin;

                int idPhoto;
                for(int kk = 0; kk < images->size(); ++kk)
                {
                    idPhoto = page.startPhotoId + (currPhoto++);

                    if(idPhoto < images->size())
                        if(!images->at(idPhoto)->isRemoved)
                            break;
                }

                if(idPhoto >= images->size())
                    break;

                // draw consigne
                QString consignText;
                if( images->at(idPhoto)->addedConsign)
                    consignText =images->at(idPhoto)->consign;
                else
                    consignText = settings->globalConsignText;

                painter.drawText(offsetBetweenHMargin  + hPositionConsigne + jj*widthPhotoAndConsigne,
                                 offsetBetweenVMargin + vPositionConsigne + ii*heightPhotoAndConsigne,widthConsigne, heightConsigne, settings->consignAlignment, consignText);

                if(allRemoved)
                    continue;

                // resize image
                QImage rPhoto = images->at(idPhoto)->scaledImage.scaled(widthPhoto,heightPhoto, Qt::KeepAspectRatio);

                // draw images
                QRect photoRect;
                if(settings->imagesAlignment & Qt::AlignBottom)
                {
                    int bottomPhoto = heightPhoto-rPhoto.height();
                    if(settings->imagesAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                          offsetBetweenVMargin + vPositionPhoto + bottomPhoto + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                    }
                    else if(settings->imagesAlignment & Qt::AlignRight)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + widthPhoto-rPhoto.width(),
                                          offsetBetweenVMargin + vPositionPhoto + bottomPhoto + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                    }
                    else
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + (widthPhoto-rPhoto.width())/2,
                                          offsetBetweenVMargin + vPositionPhoto + bottomPhoto + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                    }

                }
                else if(settings->imagesAlignment & Qt::AlignTop) // TOP is good
                {
                    if(settings->imagesAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                    }
                    else if(settings->imagesAlignment & Qt::AlignRight)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + widthPhoto-rPhoto.width(),
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                    }
                    else
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + (widthPhoto-rPhoto.width())/2,
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                    }
                }
                else
                {
                    int centerPhoto = (heightPhoto-rPhoto.height())/2;
                    if(settings->imagesAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne, offsetBetweenVMargin +
                                          vPositionPhoto + heightPhotoAndConsigne*ii + centerPhoto, rPhoto.width(),rPhoto.height());
                    }
                    else if(settings->imagesAlignment & Qt::AlignRight)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + widthPhoto-rPhoto.width(),
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii + centerPhoto, rPhoto.width(),rPhoto.height());
                    }
                    else
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne + (widthPhoto-rPhoto.width())/2,
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii + centerPhoto, rPhoto.width(),rPhoto.height());
                    }
                }

                painter.drawPixmap(photoRect,QPixmap::fromImage(rPhoto));
            }
        }


//        addContourPreview(image);
//        m_preview->setPixmap(QPixmap::fromImage(image));
//        m_preview->update();

//        // unlock ui
//        if(m_loadedImages.size() > 0)
//            emit unlockSignal();



        painter.end();

//        addContourPreview(image);

        emit endPreviewSignal(QPixmap::fromImage(image));
    }


signals :

    void endPreviewSignal(QPixmap preview);
};
}

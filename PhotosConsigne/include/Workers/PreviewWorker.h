
#pragma once

// Qt
#include <QObject>
#include <QImage>
#include <QPainter>
#include <QPdfWriter>

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

private :


    void drawPage(QPainter &painter, const Page &page, const SImages &images, const SPDFSettings &settings, const PreviewZonesDisplay &zones, const bool preview, const int width, const int height)
    {
        // misc
        bool drawTitle          = settings->titleAdded && (page.startImageId == 0 || settings->titleOnAllPages);
        bool titleOnTop         = settings->titlePositionFromPC == Position::top;
        bool consignBeforePhoto = settings->globalConsignPositionFromPhotos == Position::top;
        int nbInterVMargins     = settings->nbImagesPageV - 1;
        int nbInterHMargins     = settings->nbImagesPageH - 1;
        QColor photoZoneColor(112,124,190), consignZoneColor(63,72,204), titleZoneColor(0,121,0);
        QColor interMarginsZoneColor(221,88,0);

        // sizes
        //      title
        int heightTitle             = drawTitle ? (settings->ratioTitle * height) : 0;
        int widthTitle              = drawTitle ? width : 0;

        //      all PC + inter margins + extern margings
        int heightAll               = height - heightTitle;
        int widthAll                = width;

        //      extern margins
        int heightTopMargin         = heightAll * settings->margins.top;
        int heightBottomMargin      = heightAll * settings->margins.bottom;
        int widthLeftMargin         = widthAll  * settings->margins.left;
        int widthRightMargin        = widthAll  * settings->margins.right;

        //      all PC + inter margins
        int heightAllPC             = heightAll - heightTopMargin - heightBottomMargin;
        int widthAllPC              = widthAll  - widthLeftMargin - widthRightMargin;

        //      inter margins
        int heightInterMargin       = heightAllPC * settings->margins.interHeight;
        int widthInterMargin        = widthAllPC  * settings->margins.interWidth;

        //      photo and consign
        int heightPhotoAndConsigne  = (heightAllPC - nbInterVMargins * heightInterMargin) / settings->nbImagesPageV;
        int widthPhotoAndConsigne   = (widthAllPC  - nbInterHMargins * widthInterMargin)  / settings->nbImagesPageH;

        //      consign
        int heightConsigne          = heightPhotoAndConsigne * settings->ratioPhotosConsign;
        int widthConsigne           = widthPhotoAndConsigne;
        //      photo
        int heightPhoto             = (1 - settings->ratioPhotosConsign) *  heightPhotoAndConsigne;
        int widthPhoto              = widthPhotoAndConsigne;

        // positions
        //      first PC
        int vPositionFirstPC    = heightTopMargin + (titleOnTop ? heightTitle : 0);
        int hPositionFirstPC    = widthLeftMargin;

        //      title
        int hPositionTitle      = 0;
        int vPositionTitle      = titleOnTop ? 0 : (height - heightTitle);

        //      extern margins
        int hPositionTopMargin  = 0;
        int vPositionTopMargin  = titleOnTop ? heightTitle : 0;

        //      consign & photo & title
        //          horizontal
        int hPositionPC   = settings->margins.left * width;

        // draw title zone
        if(drawTitle)
        {
            QRect rectTitle(hPositionTitle, vPositionTitle, widthTitle, heightTitle);
            painter.fillRect(rectTitle, zones.titles ? titleZoneColor : Qt::white);
            if(zones.titles)
                painter.drawRect(rectTitle);
        }

        // draw extern margins zone
        if(zones.externMargins)
        {
            QRect rectExternMargins(hPositionTopMargin + widthLeftMargin, vPositionTopMargin + heightTopMargin,
                                    widthAll - widthLeftMargin - widthRightMargin, heightAll - heightTopMargin - heightBottomMargin);

            painter.fillRect(rectExternMargins, Qt::white);
            painter.drawRect(rectExternMargins);
        }

        // draw inter margins zone
        if(zones.InterMargins)
        {
            for(int ii = 0; ii < nbInterVMargins; ++ii)
                painter.fillRect(QRect(0, vPositionFirstPC + (ii+1)*heightPhotoAndConsigne + ii*heightInterMargin, widthAll, heightInterMargin), interMarginsZoneColor);

            for(int ii = 0; ii < nbInterHMargins; ++ii)
                painter.fillRect(QRect(hPositionFirstPC + (ii+1)*widthPhotoAndConsigne + ii*widthInterMargin, vPositionTopMargin, widthInterMargin, heightAll), interMarginsZoneColor);
        }

        // draw cut lines
        if(settings->displayCutLines)
        {
            QPen currentPen = painter.pen();
            currentPen.setStyle(Qt::DashLine);
            currentPen.setWidth(1);
            painter.setPen(currentPen);

            int offset = vPositionFirstPC;
            for(int ii = 0; ii < settings->nbImagesPageV+1; ++ii)
            {
                painter.drawLine(0, offset, widthAll, offset);
                offset += heightPhotoAndConsigne;

                if(ii == 0)
                    offset += heightInterMargin/2;
                else if(ii < settings->nbImagesPageV-1)
                    offset += heightInterMargin;
                else
                    offset += heightInterMargin/2;
            }

            offset = hPositionFirstPC;
            for(int ii = 0; ii < settings->nbImagesPageH+1; ++ii)
            {
                painter.drawLine(offset, vPositionTopMargin, offset, heightAll);
                offset += widthPhotoAndConsigne;

                if(ii == 0)
                    offset += widthInterMargin/2;
                else if(ii < settings->nbImagesPageH-1)
                    offset += widthInterMargin;
                else
                    offset += widthInterMargin/2;
            }

            currentPen.setStyle(Qt::SolidLine);
            painter.setPen(currentPen);
        }



        int offsetV = vPositionFirstPC;
        for(int ii = 0; ii < settings->nbImagesPageV; ++ii)
        {
            int offsetH = hPositionFirstPC;
            for(int jj = 0; jj < settings->nbImagesPageH; ++jj)
            {
                // draw consigne rectangle
                if(zones.consigns)
                {
                    QRect consignRect(offsetH, offsetV + (consignBeforePhoto ? 0: heightPhoto) , widthConsigne, heightConsigne);
                    painter.fillRect(consignRect, consignZoneColor);
                    painter.drawRect(consignRect);
                }

                // draw photo rectangle
                if(zones.photos)
                {
                    QRect photoRect(offsetH, offsetV +  (consignBeforePhoto ? heightConsigne : 0), widthPhoto, heightPhoto);
                    painter.fillRect(photoRect, photoZoneColor);
                    painter.drawRect(photoRect);
                }

                offsetH += widthPhotoAndConsigne + widthInterMargin;
            }

            offsetV += heightPhotoAndConsigne + heightInterMargin;
        }

        // draw title
        if(drawTitle)
        {
            painter.setPen(settings->titleColor);
            painter.setFont(settings->titleFont);
            painter.drawText(hPositionTitle, vPositionTitle, widthTitle, heightTitle, settings->titleAlignment, settings->titleText);
        }

        // define text font/pen
        painter.setPen(settings->globalConsignColor);
        painter.setFont(settings->globalConsignFont);

        bool allRemoved = true;
        for(auto &&image : (*images))
        {
            if(!image->isRemoved)
            {
                allRemoved = false;
                break;
            }
        }

        int currPhoto = 0;
        for(int ii = 0; ii < settings->nbImagesPageV; ++ii)
        {
            if(allRemoved)
                continue;

            for(int jj = 0; jj < settings->nbImagesPageH; ++jj)
            {
                if(currPhoto >= images->size())
                    break;

                int offsetBetweenVMargin = ii * heightInterMargin;
                int offsetBetweenHMargin = jj * widthInterMargin;

                int idPhoto;
                for(int kk = 0; kk < images->size(); ++kk)
                {
                    idPhoto = page.startImageId + (currPhoto++);

                    if(idPhoto < images->size())
                        if(!images->at(idPhoto)->isRemoved)
                            break;
                }

                if(idPhoto >= images->size())
                    break;

                // draw consigne
                QString consignText;
                if( images->at(idPhoto)->addedText)
                    consignText =images->at(idPhoto)->text;
                else if (settings->useImageNameAsText)
                    consignText = images->at(idPhoto)->pathImage.split("/").last();
                else
                    consignText = settings->globalConsignText;

                painter.drawText(offsetBetweenHMargin + hPositionPC + jj*widthPhotoAndConsigne,
                                 offsetBetweenVMargin + vPositionFirstPC + (consignBeforePhoto ? 0 : heightPhoto ) + ii*heightPhotoAndConsigne,
                                 widthConsigne, heightConsigne, settings->consignAlignment, consignText);


                // resize image
                QImage rPhoto;
                QSize imageSize = images->at(idPhoto)->scaledImage.size();
                if((imageSize.width() < widthPhoto || imageSize.height() < heightPhoto) && !preview)
                    rPhoto = QImage(images->at(idPhoto)->pathImage).transformed(QTransform().rotate(images->at(idPhoto)->rotation)).scaled(widthPhoto,heightPhoto, Qt::KeepAspectRatio);
                else
                    rPhoto = images->at(idPhoto)->scaledImage.scaled(widthPhoto,heightPhoto, Qt::KeepAspectRatio);


                // draw images
                QRect photoRect;

                int alignHOffset = offsetBetweenHMargin + hPositionPC + jj*widthPhotoAndConsigne +  (settings->imagesAlignment & Qt::AlignLeft ? 0 : (settings->imagesAlignment & Qt::AlignRight ? widthPhoto-rPhoto.width() : (widthPhoto-rPhoto.width())/2));
                int alignVOffset = offsetBetweenVMargin + vPositionFirstPC + (consignBeforePhoto ? heightConsigne : 0 );

                if(settings->imagesAlignment & Qt::AlignBottom)
                    photoRect = QRect(alignHOffset, alignVOffset + heightPhoto-rPhoto.height() + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                else if(settings->imagesAlignment & Qt::AlignTop) // TOP is good
                    photoRect = QRect(alignHOffset, alignVOffset + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                else
                    photoRect = QRect(alignHOffset,alignVOffset + heightPhotoAndConsigne*ii + (heightPhoto-rPhoto.height())/2, rPhoto.width(),rPhoto.height());


                painter.drawPixmap(photoRect,QPixmap::fromImage(rPhoto));
            }
        }

    }


public slots :


    /**
     * @brief Kill worker loop
     */
    void kill()
    {
        m_locker.lockForWrite();
        m_continueLoop = false;
        m_locker.unlock();
    }

    /**
     * @brief generatePreview
     * @param images
     * @param settings
     * @param page
     * @param zones
     */
    void generatePreview(SImages images, SPDFSettings settings, Page page, PreviewZonesDisplay zones)
    {
        bool landScape          = settings->pageOrientation == PageOrientation::landScape;
        int width               = landScape ? settings->paperFormat.heighRatio*300 : settings->paperFormat.widthRatio*300;
        int height              = landScape ? settings->paperFormat.widthRatio*300 : settings->paperFormat.heighRatio*300;

        // create preview image
        QImage image(width, height, QImage::Format_RGB32);

        // fill background
        QColor externMarginsZoneColor(151,0,0);
        image.fill(zones.externMargins ? externMarginsZoneColor : Qt::GlobalColor::white);

        QPainter painter(&image);
            drawPage(painter, page, images, settings, zones, true, width, height);
        painter.end();


        emit endPreviewSignal(QPixmap::fromImage(image));
    }


    /**
     * @brief generatePDF
     * @param pdfFileName
     * @param images
     * @param settings
     * @param zones
     */
    void generatePDF(QString pdfFileName, SImages images, SPDFSettings settings, QVector<Page> pages, PreviewZonesDisplay zones)
    {
        emit setProgressBarStateSignal(0);
        bool landScape = settings->pageOrientation == PageOrientation::landScape;

        // init writer
        QPdfWriter pdfWriter(pdfFileName);

        // define format
        pdfWriter.setPageSize(settings->paperFormat.format);

        // define dpi resolution
        pdfWriter.setResolution(settings->paperFormat.definition);

        // disable zones
        zones.consigns = zones.externMargins = zones.InterMargins = zones.photos = zones.titles = false;

        QPageLayout layout(pdfWriter.pageLayout());
        layout.setOrientation(landScape ? QPageLayout::Landscape : QPageLayout::Portrait);
        pdfWriter.setPageLayout(layout);

        // for each image
        QPagedPaintDevice::Margins margins;
        margins.bottom = 0;
        margins.top = 0;
        margins.left = 0;
        margins.right = 0;
        pdfWriter.setMargins(margins);

        // init painter
        QPainter painter(&pdfWriter);
        int width = pdfWriter.logicalDpiX() * (landScape ? settings->paperFormat.heighRatio : settings->paperFormat.widthRatio);
        int height = pdfWriter.logicalDpiY() * (landScape ? settings->paperFormat.widthRatio : settings->paperFormat.heighRatio);

        float offsetProgress = 1000 / pages.size();
        float currentProgress = 0;
        for(int ii = 0; ii < pages.size(); ++ii)
        {
            bool continueLoop;
            m_locker.lockForRead();
            continueLoop = m_continueLoop;
            m_locker.unlock();

            if(!continueLoop)
                return;

            emit setProgressBarTextSignal("Création page " + QString::number(ii));
            drawPage(painter, pages[ii], images, settings, zones, false, width, height);

            if(ii < pages.size()-1)
                pdfWriter.newPage();

            currentProgress += offsetProgress;
            emit setProgressBarStateSignal(currentProgress);
        }


        // end pdf writing
        painter.end();

        emit setProgressBarTextSignal("Fin de la génération");
        emit setProgressBarStateSignal(1000);
        emit endGenerationSignal(true);
    }




signals :

    void setProgressBarStateSignal(int state);

    void setProgressBarTextSignal(QString text);

    void endPreviewSignal(QPixmap preview);

    void endGenerationSignal(bool finished);

private :

    QReadWriteLock m_locker;
    bool m_continueLoop = true;

};
}

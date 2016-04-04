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
 * \file InterfaceWorker.cpp
 * \brief defines InterfaceWorker
 * \author Florian Lance
 * \date 01/11/15
 */

// Qt
#include <QtWidgets>
#include <QPrinter>
#include <QMarginsF>

// Photos consigne
#include "InterfaceWorker.h"

InterfaceWorker::InterfaceWorker(ImageLabel *preview)
{
    m_preview = preview;
    qRegisterMetaType<QVector<bool>>("QVector<bool>");
    qRegisterMetaType<UIParameters>("UIParameters");
}

void InterfaceWorker::loadImages(QString path, QStringList imagesList)
{
    emit setProgressBarStateSignal(0);

    m_loadedImages.resize(imagesList.count());
    m_individualTextes.resize(imagesList.count());
    for(int ii = 0; ii < imagesList.count(); ++ii)
    {        
        m_loadedImages[ii] = QImage(path + "/" + imagesList[ii]);
        emit setProgressBarStateSignal(ii * (100/imagesList.count()));
    }

    if(imagesList.count() > 0)
        emit unlockSignal();

    emit setProgressBarStateSignal(100);

    if(m_loadedImages.size() > 0)
        emit displayPhotoSignal(m_loadedImages[0]);
}

void InterfaceWorker::generatePDF(QString pdfFileName)
{
    emit setProgressBarStateSignal(0);

    // init writer
    QPdfWriter pdfWriter(pdfFileName);

    // define format
    pdfWriter.setPageSize(QPagedPaintDevice::A4); // 210 x 297 mm, 8.26 x 11.69 inches    

    // define dpi resolution
    pdfWriter.setResolution(300);

    // define layour
    QPageLayout layout(pdfWriter.pageLayout());
    if(m_landScapeOrientation)
    {
        layout.setOrientation(QPageLayout::Landscape);
    }
    else
    {
        layout.setOrientation(QPageLayout::Portrait);
    }
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
    painter.setPen(m_consignColor);
    QFont pdfFont = m_font;

    pdfFont.setPixelSize(m_font.pixelSize());//*15/4);
    painter.setFont(pdfFont);

    int width, height;
    if(m_landScapeOrientation)
    {
        width = 11.69 * pdfWriter.logicalDpiX();
        height = 8.26 * pdfWriter.logicalDpiY();
    }
    else
    {
        width = 8.26 * pdfWriter.logicalDpiX();
        height = 11.69 * pdfWriter.logicalDpiY();
    }

    // total
    int heightTotal = height * (1.0 - m_bottomMargin - m_topMargin);
    int widthTotal = width * (1.0 - m_leftMargin - m_rightMargin);

    // bewteen margin
    int heightMargin =  heightTotal * m_innerMarginHeight;
    int widthMargin  = widthTotal * m_innerMarginWidth;

    // photo and margin
    int heightPhotoAndConsigne = (heightTotal  - (m_nbImagesPageV-1) * heightMargin) / m_nbImagesPageV;
    int widthPhotoAndConsigne = (widthTotal  - (m_nbImagesPageH-1) * widthMargin) / m_nbImagesPageH;


    // consigne
    //  size
    int heightConsigne = heightPhotoAndConsigne * m_ratio;
    int widthConsigne = widthPhotoAndConsigne;
    //  position
    int vPositionConsigne = m_topMargin * height;
    int hPositionConsigne = m_leftMargin * width;
    // photo
    //  size
    int heightPhoto = (1-m_ratio) *  heightPhotoAndConsigne;
    int widthPhoto = widthPhotoAndConsigne;
    //  position
    int vPositionPhoto = vPositionConsigne + heightConsigne;
    int hPositionPhoto = m_leftMargin * width;

    // compute nb photos removes
    int photosRemoved = 0;
    for(int ii = 0; ii < m_removedImageList.count();++ii)
    {
        if(m_removedImageList[ii])
            photosRemoved++;
    }

    int nbPhotosPerPage = m_nbImagesPageV*m_nbImagesPageH;
    int nbPhotosTotal = m_loadedImages.size()-photosRemoved;
    int nbPages = nbPhotosTotal / nbPhotosPerPage;
    int rest = nbPhotosTotal % nbPhotosPerPage;
    if(nbPages == 0 || rest != 0)
        ++nbPages;

    int idPhoto = 0;
    for(int aa = 0; aa < nbPages; ++aa)
    {        
        // draw cut lines
        if(m_cutLines)
        {
            QPen currentPen = painter.pen();
            currentPen.setStyle(Qt::DashLine);
            currentPen.setWidth(1);
            painter.setPen(currentPen);
            for(int ii = 0; ii < m_nbImagesPageV-1; ++ii)
            {
                painter.drawLine(0, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2, width, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2);
            }
            for(int ii = 0; ii < m_nbImagesPageH-1; ++ii)
            {
                painter.drawLine(hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2, 0, hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2,height);
            }
            currentPen.setStyle(Qt::SolidLine);
            painter.setPen(currentPen);
        }

        for(int ii = 0; ii < m_nbImagesPageV; ++ii)
        {
            for(int jj = 0; jj < m_nbImagesPageH; ++jj)
            {
                if(idPhoto >= m_loadedImages.size())
                    break;

                int offsetBetweenVMargin = ii * heightMargin;
                int offsetBetweenHMargin = jj * widthMargin;

                for(int kk = 0; kk < m_loadedImages.size(); ++kk)
                {
                    if(!m_removedImageList[idPhoto])
                    {
                        break;
                    }

                    ++idPhoto;
                }
                if(idPhoto >= m_loadedImages.size())
                    break;


                // draw consigne
                painter.drawText(offsetBetweenHMargin  + hPositionConsigne + jj*widthPhotoAndConsigne,
                                 offsetBetweenVMargin + vPositionConsigne + ii*heightPhotoAndConsigne,widthConsigne, heightConsigne, m_consignAlignment, m_consignText);

                if(m_isAllPhotoRemoved)
                    continue;

                // resize image
                QImage rPhoto = m_loadedImages[idPhoto].scaled(widthPhoto,heightPhoto, Qt::KeepAspectRatio);

                // draw images
                QRect photoRect;
                if(m_imageAlignment & Qt::AlignBottom)
                {
                    int bottomPhoto = heightPhoto-rPhoto.height();
                    if(m_imageAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                          offsetBetweenVMargin + vPositionPhoto + bottomPhoto + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                    }
                    else if(m_imageAlignment & Qt::AlignRight)
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
                else if(m_imageAlignment & Qt::AlignTop) // TOP is good
                {
                    if(m_imageAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                          offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                    }
                    else if(m_imageAlignment & Qt::AlignRight)
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
                    if(m_imageAlignment & Qt::AlignLeft)
                    {
                        photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne, offsetBetweenVMargin +
                                          vPositionPhoto + heightPhotoAndConsigne*ii + centerPhoto, rPhoto.width(),rPhoto.height());
                    }
                    else if(m_imageAlignment & Qt::AlignRight)
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

                emit setProgressBarStateSignal(idPhoto * (100/m_loadedImages.size()));

                ++idPhoto;
            }
        }

        if(idPhoto == m_loadedImages.size())
        {
            break;
        }

        if(aa != nbPages -1)
        {
            pdfWriter.newPage();
        }
    }

    // end pdf writing
    painter.end();

    // unlock ui
    if(m_loadedImages.size() > 0)
        emit unlockSignal();

    emit setProgressBarStateSignal(100);

    emit pdfGeneratedSignal();
}

void InterfaceWorker::generatePreview(int currentRowPhoto)
{
    int width, height;
    if(m_landScapeOrientation)
    {
//        width = 1500;
//        height = width*0.7070;
        width = 3507;
        height = 2480;
    }
    else
    {
//        height = 1500;
//        width = height*0.7070;
        width = 2480;
        height = 3507;
    }

    QImage image(width, height, QImage::Format_RGB32);

    if(m_zExternMargins)
        image.fill(Qt::GlobalColor::red);
    else
        image.fill(Qt::GlobalColor::white);
    QPainter painter(&image);

    // total
    int heightTotal = height * (1.0 - m_bottomMargin - m_topMargin);
    int widthTotal = width * (1.0 - m_leftMargin - m_rightMargin);

    // bewteen margin
    int heightMargin =  heightTotal * m_innerMarginHeight;
    int widthMargin  = widthTotal * m_innerMarginWidth;

    // photo and margin
    int heightPhotoAndConsigne = (heightTotal  - (m_nbImagesPageV-1) * heightMargin) / m_nbImagesPageV;
    int widthPhotoAndConsigne = (widthTotal  - (m_nbImagesPageH-1) * widthMargin) / m_nbImagesPageH;


    // consigne
    //  size
    int heightConsigne = heightPhotoAndConsigne * m_ratio;
    int widthConsigne = widthPhotoAndConsigne;
    //  position
    int vPositionConsigne = m_topMargin * height;
    int hPositionConsigne = m_leftMargin * width;
    // photo
    //  size
    int heightPhoto = (1-m_ratio) *  heightPhotoAndConsigne;
    int widthPhoto = widthPhotoAndConsigne;
    //  position
    int vPositionPhoto = vPositionConsigne + heightConsigne;
    int hPositionPhoto = m_leftMargin * width;

    QRect rect = QRect(m_leftMargin* width, m_topMargin* height, (1-m_leftMargin-m_rightMargin)* width, (1-m_topMargin-m_bottomMargin)* height);

    if(m_zExternMargins)
    {
        painter.fillRect(rect, Qt::white);
        painter.drawRect(rect);
    }

    // draw margins
    if(m_zInterMargins)
    {
        for(int ii = 0; ii < m_nbImagesPageV-1; ++ii)
        {
            QRect rect = QRect(0, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin, width, heightMargin);
            painter.fillRect(rect, Qt::green);
        }

        for(int ii = 0; ii < m_nbImagesPageH-1; ++ii)
        {
            QRect rect = QRect(hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin, 0, widthMargin, height);
            painter.fillRect(rect, Qt::green);
        }
    }

    // draw cut lines
    if(m_cutLines)
    {
        QPen currentPen = painter.pen();
        currentPen.setStyle(Qt::DashLine);
        currentPen.setWidth(1);
        painter.setPen(currentPen);
        for(int ii = 0; ii < m_nbImagesPageV-1; ++ii)
        {
            painter.drawLine(0, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2, width, vPositionConsigne + (ii+1)*heightPhotoAndConsigne + ii*heightMargin + heightMargin/2);
        }
        for(int ii = 0; ii < m_nbImagesPageH-1; ++ii)
        {
            painter.drawLine(hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2, 0, hPositionConsigne + (ii+1)*widthPhotoAndConsigne + ii*widthMargin + widthMargin/2,height);
        }
        currentPen.setStyle(Qt::SolidLine);
        painter.setPen(currentPen);
    }

    // define font/pen
    painter.setPen(m_consignColor);
    painter.setFont(m_font);



    for(int ii = 0; ii < m_nbImagesPageV; ++ii)
    {
        for(int jj = 0; jj < m_nbImagesPageH; ++jj)
        {
            int offsetBetweenVMargin = ii * heightMargin;
            int offsetBetweenHMargin = jj * widthMargin;

            // draw consigne rectangle
            if(m_zConsigns)
            {
                QRect rect = QRect(offsetBetweenHMargin  + hPositionConsigne + jj*widthPhotoAndConsigne,
                                   offsetBetweenVMargin + vPositionConsigne + ii*heightPhotoAndConsigne,widthConsigne, heightConsigne);
                painter.fillRect(rect, Qt::cyan);
            }

            // draw photo rectangle
            if(m_zPhotos)
            {
                rect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                             offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii,widthPhoto, heightPhoto);
                painter.fillRect(rect, Qt::yellow);
            }
        }
    }


    int currPhoto = 0;
    for(int ii = 0; ii < m_nbImagesPageV; ++ii)
    {
        for(int jj = 0; jj < m_nbImagesPageH; ++jj)
        {
            if(currPhoto >= m_loadedImages.size())
                break;

            int offsetBetweenVMargin = ii * heightMargin;
            int offsetBetweenHMargin = jj * widthMargin;

            int idPhoto;
            for(int kk = 0; kk < m_loadedImages.size(); ++kk)
            {
                idPhoto = (currentRowPhoto + (currPhoto++) )% m_loadedImages.size();

                if(!m_removedImageList[idPhoto])
                {
                    break;
                }
            }


            // draw consigne
            painter.drawText(offsetBetweenHMargin  + hPositionConsigne + jj*widthPhotoAndConsigne,
                             offsetBetweenVMargin + vPositionConsigne + ii*heightPhotoAndConsigne,widthConsigne, heightConsigne, m_consignAlignment, m_consignText);

            if(m_isAllPhotoRemoved)
                continue;

            // resize image
            QImage rPhoto = m_loadedImages[idPhoto].scaled(widthPhoto,heightPhoto, Qt::KeepAspectRatio);

            // draw images
            QRect photoRect;
            if(m_imageAlignment & Qt::AlignBottom)
            {
                int bottomPhoto = heightPhoto-rPhoto.height();
                if(m_imageAlignment & Qt::AlignLeft)
                {
                    photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                      offsetBetweenVMargin + vPositionPhoto + bottomPhoto + (heightPhotoAndConsigne)*ii, rPhoto.width(),rPhoto.height());
                }
                else if(m_imageAlignment & Qt::AlignRight)
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
            else if(m_imageAlignment & Qt::AlignTop) // TOP is good
            {
                if(m_imageAlignment & Qt::AlignLeft)
                {
                    photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne,
                                      offsetBetweenVMargin + vPositionPhoto + heightPhotoAndConsigne*ii, rPhoto.width(),rPhoto.height());
                }
                else if(m_imageAlignment & Qt::AlignRight)
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
                if(m_imageAlignment & Qt::AlignLeft)
                {
                    photoRect = QRect(offsetBetweenHMargin + hPositionPhoto + jj*widthPhotoAndConsigne, offsetBetweenVMargin +
                                      vPositionPhoto + heightPhotoAndConsigne*ii + centerPhoto, rPhoto.width(),rPhoto.height());
                }
                else if(m_imageAlignment & Qt::AlignRight)
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

    painter.end();

    addContourPreview(image);
    m_preview->setPixmap(QPixmap::fromImage(image));
    m_preview->update();

    // unlock ui
    if(m_loadedImages.size() > 0)
        emit unlockSignal();
}

void InterfaceWorker::updateParameters(UIParameters params)
{
    m_nbImagesPageV = params.nbImagesPageV;
    m_nbImagesPageH = params.nbImagesPageH;
    m_ratio = params.ratio;
    m_font = params.font;
    m_consignText = params.consignText;
    m_consignColor = params.consignColor;
    m_imageAlignment = params.imageAlignment;
    m_consignAlignment = params.consignAlignment | Qt::TextWordWrap;
    m_landScapeOrientation = !params.orientation;

    m_leftMargin = params.leftMargin;
    m_rightMargin = params.rightMargin;
    m_topMargin = params.topMargin;
    m_bottomMargin = params.bottomMargin;
    m_innerMarginWidth = params.interMarginWidth;
    m_innerMarginHeight = params.interMarginHeight;
    m_removedImageList = params.removePhotoList;

    m_cutLines = params.cutLines;
    m_zExternMargins = params.zExternMargins;
    m_zInterMargins = params.zInterMargins;
    m_zPhotos = params.zPhotos;
    m_zConsigns = params.zConsigns;

    m_params = params;


    m_isAllPhotoRemoved = true;
    for(int ii = 0; ii < m_removedImageList.count(); ++ii)
    {
        if(!m_removedImageList[ii])
        {
            m_isAllPhotoRemoved = false;
            break;
        }
    }
}

void InterfaceWorker::updateRotationImage(int index, bool rightRotation)
{
    QTransform t;
    if(rightRotation)
        m_loadedImages[index] = m_loadedImages[index].transformed(t.rotate(90));
    else
        m_loadedImages[index] = m_loadedImages[index].transformed(t.rotate(-90));

    sendPhoto(index);
}

void InterfaceWorker::sendPhoto(QImage image)
{
    emit displayPhotoSignal(image);
}


void InterfaceWorker::saveProfile()
{
    if(m_params.saveProfileTo(m_profilePath))
    {
        emit endSaveProfileSignal("");
    }
    else
    {
        // TODO : manage error
    }
}

void InterfaceWorker::loadProfile(QString pathProfileFile)
{
    if(m_params.loadProfile(pathProfileFile))
    {
        m_profilePath = pathProfileFile;
        QStringList elements = pathProfileFile.split('/');
        QString profileName = elements[elements.size()-1];
        emit endLoadProfileSignal(profileName, m_params);
    }
    else
    {
        // TODO : manage error
    }
}

void InterfaceWorker::loadDefaultProfile(QString defaultProfileFile)
{
    if(m_params.loadProfile(defaultProfileFile))
    {
        QStringList elements = defaultProfileFile.split('/');
        QString profileName = elements[elements.size()-1];
        emit endLoadProfileSignal(profileName, m_params);
    }
    else
    {
        // TODO : manage error
    }
}

void InterfaceWorker::saveDefaultProfile()
{
    QString defaultProfilPath = QDir::currentPath();
    if(QDir(defaultProfilPath + "/resources").exists())
    {
        defaultProfilPath += "/resources/profils/default.profil";
    }
    else
    {
        defaultProfilPath += "/../PhotosConsigne/resources/profils/default.profil";
    }

    if(!m_params.saveProfileTo(defaultProfilPath))
    {
        // TODO : manage error
    }
}

void InterfaceWorker::saveProfileTo(QString pathProFile)
{
    if(m_params.saveProfileTo(pathProFile))
    {
        m_profilePath = pathProFile;

        QStringList elements = pathProFile.split('/');
        QString profileName = elements[elements.size()-1];

        emit endSaveProfileSignal(profileName);
    }
    else
    {
        // TODO : manage error
    }
}

void InterfaceWorker::sendPhoto(int index)
{
    if(index < m_loadedImages.count() && index >= 0)
        emit displayPhotoSignal(m_loadedImages[index]);
}

void InterfaceWorker::addContourPreview(QImage &image)
{
    QImage img(image.width()+2, image.height() +2, QImage::Format_RGB32);
    img.fill(Qt::GlobalColor::black);

    QPainter painter(&img);
    painter.drawImage(1,1, image);
    painter.end();

    image = img;
}

bool UIParameters::saveProfileTo(const QString &pathProFile)
{
    QFile proFile(pathProFile);
    if(proFile.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&proFile);
        stream << "PhotosConsigne parameters :\n";
        stream << orientation << " " << cutLines << " " << zExternMargins << " " << zInterMargins << " " << zPhotos  << " " << zConsigns << endl;
        stream << nbImagesPageV << " " << nbImagesPageH << " " << imageAlignment << " " << consignAlignment  << endl;
        stream << ratio << " " << leftMargin << " " << rightMargin << " " << topMargin  << " " << bottomMargin << " " << interMarginWidth << " " << interMarginHeight << endl;
        stream << font.pixelSize() << " " << font.italic() << " " << font.bold() << " " << font.family() << " " << consignColor.red() << " " << consignColor.green() << " " << consignColor.blue() <<  endl;
        return true;
    }

    return false;
}

bool UIParameters::loadProfile(const QString &pathProFile)
{
    QFile proFile(pathProFile);
    if (proFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&proFile);

       QString line = in.readLine();
       line = in.readLine();
       QStringList elements = line.split(' ');

       if(elements.size() != 6)
           return false;

       orientation = (elements[0].toInt() == 1);
       cutLines = (elements[1].toInt() == 1);
       zExternMargins = (elements[2].toInt() == 1);
       zInterMargins = (elements[3].toInt() == 1);
       zPhotos = (elements[4].toInt() == 1);
       zConsigns = (elements[5].toInt() == 1);

       line = in.readLine();
       elements = line.split(' ');
       if(elements.size() != 4)
           return false;

       nbImagesPageV = elements[0].toInt();
       nbImagesPageH = elements[1].toInt();
       imageAlignment = elements[2].toInt();
       consignAlignment = elements[3].toInt();

       line = in.readLine();
       elements = line.split(' ');
       if(elements.size() != 7)
           return false;

       ratio = elements[0].toDouble();
       leftMargin = elements[1].toDouble();
       rightMargin = elements[2].toDouble();
       topMargin = elements[3].toDouble();
       bottomMargin = elements[4].toDouble();
       interMarginWidth = elements[5].toDouble();
       interMarginHeight = elements[6].toDouble();

       line = in.readLine();
       elements = line.split(' ');
       if(elements.size() != 7)
           return false;

       font.setPixelSize(elements[0].toInt());
       font.setItalic((elements[1].toInt() == 1));
       font.setBold((elements[2].toInt() == 1));
       font.setFamily(elements[3]);
       consignColor = QColor(elements[4].toInt(),elements[5].toInt(),elements[6].toInt());

       proFile.close();
       return true;
    }

    return false;
}





#include "InterfaceWorker.h"


#include <QtWidgets>
#include <QPrinter>
#include <QtGui>

#include <QMarginsF>

//int QScreenTools::cvtInch2Px( qreal in )
//{
//    qreal px = in * mDpi;

//    return (int)(px+0.5);
//}
//    qreal mDpi = QApplication::primaryScreen()->physicalDotsPerInch();
//    qDebug() << QString::number(mDpi) << " " ;//<< QString::number(mDpi);

//    mDpi * pdfWriter.logicalDpiX()

//        #define MM2INCH 0.0393700787401575
//        #define INCH2MM 25.4

//        QScreenTools::QScreenTools()
//        {
//        mDpi = QApplication::primaryScreen()->physicalDotsPerInch();
//        }

//        int QScreenTools::cvtMm2Px( qreal mm )
//        {
//        qreal inch = mm*MM2INCH;

//        return cvtInch2Px( inch );
//        }

//        int QScreenTools::cvtInch2Px( qreal in )
//        {
    //        qreal px = in * mDpi;

    //        return (int)(px+0.5);
//        }



InterfaceWorker::InterfaceWorker(ImageLabel *preview){m_preview = preview; m_exit = false;}

void InterfaceWorker::checkEvent()
{
//    while()
//    {

//    }
}

void InterfaceWorker::loadImages(QString path, QStringList imagesList)
{
    emit setProgressBarState(0);

    m_loadedImages.resize(imagesList.count());
    for(int ii = 0; ii < imagesList.count(); ++ii)
    {
        m_loadedImages[ii] = QImage(path + "/" + imagesList[ii]);
        emit setProgressBarState(ii * (100/imagesList.count()));
    }

    if(imagesList.count() > 0)
        emit unlockSignal();

    emit setProgressBarState(100);

    if(m_loadedImages.size() > 0)
        emit displayPhoto(m_loadedImages[0]);
}

void InterfaceWorker::generatePDF(QString pdfFileName)
{
    emit setProgressBarState(0);

    // init writer
    QPdfWriter pdfWriter(pdfFileName);

    // define format
    pdfWriter.setPageSize(QPagedPaintDevice::A4); // 210 x 297 mm, 8.26 x 11.69 inches

    // for each image
    QPagedPaintDevice::Margins margins;
    margins.bottom = 0;
    margins.top = 0;
    margins.left = 0;
    margins.right = 0;
    pdfWriter.setMargins(margins);

    // init painter
    QPainter painter(&pdfWriter);
    painter.setPen(m_textColor);
    QFont pdfFont = m_font;
    pdfFont.setPixelSize(m_font.pixelSize() *15);
    painter.setFont(pdfFont);

    int width = 8.26 *pdfWriter.logicalDpiX();// -  0.0393701 *marginss.left -  0.0393701*marginss.right;
    int height = 11.69*pdfWriter.logicalDpiY();//-   0.0393701 *marginss.top - 0.0393701*marginss.bottom;
    int heightPhotoAndConsigne = height / m_nbImagesPage;
    int heightConsigne = heightPhotoAndConsigne * m_ratio;
    int heightPhoto = heightPhotoAndConsigne * (1-m_ratio);
    int nbPages = m_loadedImages.size() /  m_nbImagesPage;
    if(m_loadedImages.size()%0 != 0)
    {
        nbPages++;
    }

    int option = Qt::TextWordWrap;
    if(m_textAlignment == Left)
    {
        option = option | Qt::AlignLeft;
    }
    else if(m_textAlignment == Right)
    {
        option = option | Qt::AlignRight;
    }
    else
    {
        option = option | Qt::AlignCenter;
    }

    int idPhoto = 0;
    for(int jj = 0; jj < nbPages; ++jj)
    {
        if(idPhoto == m_loadedImages.size())
            break;

        for(int ii = 0; ii < m_nbImagesPage; ++ii)
        {
            // text
            painter.drawText(0,ii*heightPhotoAndConsigne,width, heightConsigne, option, m_text);

            // image
            QImage resizedPhoto = m_loadedImages[idPhoto++].scaled(width,heightPhoto, Qt::KeepAspectRatio);
            QRect photoRect;
            if(m_imageAlignment == Center)
            {
                photoRect = QRect(width/2-resizedPhoto.width()/2,  heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height()); // heightPhoto/2-resizedPhoto.height()/2 +
            }
            else if(m_imageAlignment == Left)
            {
                photoRect = QRect(0, heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height());
            }
            else
            {
                photoRect = QRect(width-resizedPhoto.width(), heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height());
            }

            painter.drawPixmap(photoRect,QPixmap::fromImage(resizedPhoto));

            if(idPhoto == m_loadedImages.size())
            {
                break;
            }

            emit setProgressBarState(idPhoto * (100/m_loadedImages.size()));
        }

        if(idPhoto == m_loadedImages.size())
        {
            break;
        }

        if(jj != nbPages -1)
        {
            pdfWriter.newPage();
        }
    }

    // end pdf writing
    painter.end();

    // unlock ui
    if(m_loadedImages.size() > 0)
        emit unlockSignal();

    emit setProgressBarState(100);
}

void InterfaceWorker::generatePreview(int currentRowPhoto)
{
    int height = 1000;
    int width = height*0.7070;

    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::GlobalColor::white);
    QPainter painter(&image);
    painter.setPen(m_textColor);
    painter.setFont(m_font);


    // for each image
    int heightPhotoAndConsigne = height / m_nbImagesPage;
    int heightConsigne = heightPhotoAndConsigne * m_ratio;
    int heightPhoto = heightPhotoAndConsigne * (1-m_ratio);

    int option = Qt::TextWordWrap;
    if(m_textAlignment == Left)
    {
        option = option | Qt::AlignLeft;
    }
    else if(m_textAlignment == Right)
    {
        option = option | Qt::AlignRight;
    }
    else
    {
        option = option | Qt::AlignCenter;
    }

    for(int ii = 0; ii < m_nbImagesPage; ++ii)
    {
        // text
        painter.drawText(0,ii*heightPhotoAndConsigne,width, heightConsigne, option, m_text);

        // image
        int idPhoto = (currentRowPhoto+ii)%m_loadedImages.size();
        QImage resizedPhoto = m_loadedImages[idPhoto].scaled(width,heightPhoto, Qt::KeepAspectRatio);

        QRect photoRect;
        if(m_imageAlignment == Center)
        {
            photoRect = QRect(width/2-resizedPhoto.width()/2,  heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height()); // heightPhoto/2-resizedPhoto.height()/2 +
        }
        else if(m_imageAlignment == Left)
        {
            photoRect = QRect(0, heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height());
        }
        else
        {
            photoRect = QRect(width-resizedPhoto.width(), heightConsigne + (heightPhotoAndConsigne)*ii, resizedPhoto.width(),resizedPhoto.height());
        }


        painter.drawPixmap(photoRect,QPixmap::fromImage(resizedPhoto));
    }

    painter.end();

    addContourPreview(image);
    m_preview->setPixmap(QPixmap::fromImage(image));
    m_preview->update();

    // unlock ui
    if(m_loadedImages.size() > 0)
        emit unlockSignal();
}

void InterfaceWorker::updateParameters(int nbImagesPage, double ratio, QFont font, QString text, QColor textColor, int imageAlignment, int textAlignment)
{
    m_nbImagesPage = nbImagesPage;
    m_ratio = ratio;
    m_font = font;
    m_text = text;
    m_textColor = textColor;
    m_imageAlignment = (Alignment) imageAlignment;
    m_textAlignment = (Alignment) textAlignment;
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



void InterfaceWorker::sendPhoto(int index)
{
    if(index < m_loadedImages.count() && index >= 0)
        emit displayPhoto(m_loadedImages[index]);
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

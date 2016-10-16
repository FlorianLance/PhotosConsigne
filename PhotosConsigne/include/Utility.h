

#pragma once


// std
#include <memory>

// debug
#include <QDebug>

// Qt
#include <QImage>
#include <QList>
#include <QFont>
#include <QReadWriteLock>
#include <QPagedPaintDevice>


namespace pc
{

    class Image
    {
    public :

        Image() = delete;

        Image(const QString &path) : pathImage(path)
        {
            constexpr int maxHeight = 800;
            constexpr int maxWidth = 800;

            scaledImage = QImage(path);

            if(!scaledImage.isNull())
            {
                if(scaledImage.size().width() > maxWidth)
                    scaledImage = scaledImage.scaledToWidth(maxWidth);
                if(scaledImage.size().height() > maxHeight)
                    scaledImage = scaledImage.scaledToHeight(maxHeight);
            }
        }

        QImage scaledImage;
        QString pathImage;


        bool isRemoved = false;
        bool addedText = false;
        int rotation = 0;
        QString text = "";
    };

    struct Page
    {
        int startImageId = 0;
        int nbImages = 0;
    };


    using SImage = std::shared_ptr<Image>;
    using SImages = std::shared_ptr<QList<SImage>>;


    struct RatioMargins
    {
        double left = 0.02;
        double right = 0.02;
        double top = 0.02;
        double bottom = 0.02;
        double interWidth = 0.02;
        double interHeight = 0.02;
    };

    enum class PageOrientation { landScape = 0, portrait = 1};


    enum class Position { top = 0, bottom = 1};

    struct PreviewZonesDisplay
    {
        bool externMargins = false;
        bool InterMargins = false;
        bool titles = false;
        bool photos = false;
        bool consigns = false;
    };


    struct PaperFormat
    {
        int formatId = 4;
        int definitionId = 3;

        double widthRatio  = 8.26;
        double heighRatio = 11.69;


        int definition = 300;
        QPagedPaintDevice::PageSize format = QPagedPaintDevice::PageSize::A4;

        void updateDefinition(QString definitionStr)
        {
            definition = definitionStr.toInt();
            switch(definition)
            {
                case 72:
                    definitionId = 0;
                break;
                case 96:
                    definitionId = 1;
                break;
                case 150:
                    definitionId = 2;
                break;
                case 300:
                    definitionId = 3;
                break;
                case 600:
                    definitionId = 4;
                break;
                case 900:
                    definitionId = 5;
                break;
                case 1200:
                    definitionId = 6;
                break;
                case 2400:
                    definitionId = 7;
                break;
            }
        }

        void updateFormat(QString formatStr)
        {
            if(formatStr == "A0")
            {
                formatId = 0;
                widthRatio  = 33.1;
                heighRatio = 46.8;
                format = QPagedPaintDevice::PageSize::A0;
            }
            else if(formatStr == "A1")
            {
                formatId = 1;
                widthRatio  = 23.4;
                heighRatio = 33.1;
                format = QPagedPaintDevice::PageSize::A1;
            }
            else if(formatStr == "A2")
            {
                formatId = 2;
                widthRatio  = 16.5;
                heighRatio = 23.4;
                format = QPagedPaintDevice::PageSize::A2;
            }
            else if(formatStr == "A3")
            {
                formatId = 3;
                widthRatio  = 11.7;
                heighRatio = 16.5;
                format = QPagedPaintDevice::PageSize::A3;
            }
            else if(formatStr == "A4")
            {
                formatId = 4;
                widthRatio  = 8.27;
                heighRatio = 11.69;
                format = QPagedPaintDevice::PageSize::A4;
            }
            else if(formatStr == "A5")
            {
                formatId = 5;
                widthRatio  = 5.83;
                heighRatio = 8.27;
                format = QPagedPaintDevice::PageSize::A5;
            }
            else if(formatStr == "A6")
            {
                formatId = 6;
                widthRatio  = 4.13;
                heighRatio = 5.83;
                format = QPagedPaintDevice::PageSize::A6;
            }
            else if(formatStr == "A7")
            {
                formatId = 7;
                widthRatio  = 2.91;
                heighRatio = 4.13;
                format = QPagedPaintDevice::PageSize::A7;
            }
            else if(formatStr == "A8")
            {
                formatId = 8;
                widthRatio  = 2.05;
                heighRatio = 2.91;
                format = QPagedPaintDevice::PageSize::A8;
            }
            else if(formatStr == "A9")
            {
                formatId = 9;
                widthRatio  = 1.46;
                heighRatio = 2.05;
                format = QPagedPaintDevice::PageSize::A9;
            }
            else if(formatStr == "B0")
            {
                formatId = 10;
                widthRatio  = 39.4;
                heighRatio = 55.7;
                format = QPagedPaintDevice::PageSize::B0;
            }
            else if(formatStr == "B1")
            {
                formatId = 11;
                widthRatio  = 27.8;
                heighRatio = 39.4;
                format = QPagedPaintDevice::PageSize::B1;
            }
            else if(formatStr == "B2")
            {
                formatId = 12;
                widthRatio  = 19.7;
                heighRatio = 27.8;
                format = QPagedPaintDevice::PageSize::B2;
            }
            else if(formatStr == "B3")
            {
                formatId = 13;
                widthRatio  = 13.9;
                heighRatio = 19.7;
                format = QPagedPaintDevice::PageSize::B3;
            }
            else if(formatStr == "B4")
            {
                formatId = 14;
                widthRatio  = 9.84;
                heighRatio = 13.9;
                format = QPagedPaintDevice::PageSize::B4;
            }
            else if(formatStr == "B5")
            {
                formatId = 15;
                widthRatio  = 6.93;
                heighRatio = 9.87;
                format = QPagedPaintDevice::PageSize::B5;
            }
            else if(formatStr == "B6")
            {
                formatId = 16;
                widthRatio  = 4.92;
                heighRatio = 6.93;
                format = QPagedPaintDevice::PageSize::B6;
            }
            else if(formatStr == "B7")
            {
                formatId = 17;
                widthRatio  = 3.46;
                heighRatio = 4.92;
                format = QPagedPaintDevice::PageSize::B7;
            }
            else if(formatStr == "B8")
            {
                formatId = 18;
                widthRatio  = 2.44;
                heighRatio = 3.46;
                format = QPagedPaintDevice::PageSize::B8;
            }
            else if(formatStr == "B9")
            {
                formatId = 19;
                widthRatio  = 1.73;
                heighRatio = 2.44;
                format = QPagedPaintDevice::PageSize::B9;
            }
        }
    };


    struct PDFSettings
    {
        PDFSettings()
        {
            titleFont.setPixelSize(75);
            globalConsignFont.setPixelSize(50);
        }

        bool useImageNameAsText = false;
        bool displayCutLines = false;
        bool titleAdded = false;
        bool titleOnAllPages = false;
        int nbImagesPageV = 3, nbImagesPageH= 2;
        double ratioPhotosConsign = 0.15;
        double ratioTitle = 0.08;

        int imagesAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        int consignAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        int titleAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        PageOrientation pageOrientation = PageOrientation::portrait;

        RatioMargins margins;

        Position globalConsignPositionFromPhotos = Position::top;
        QFont globalConsignFont;
        QColor globalConsignColor = Qt::black;
        QString globalConsignText = "Tapez la consigne ici.";

        Position titlePositionFromPC = Position::top;
        QFont titleFont;
        QColor titleColor = Qt::black;
        QString titleText = "TITRE";

        PaperFormat paperFormat;
    };

    using SPDFSettings = std::shared_ptr<PDFSettings>;

}

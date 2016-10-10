

#pragma once


// std
#include <memory>

// debug
#include <QDebug>

// Qt
#include <QImage>
#include <QList>
#include <QFont>


namespace pc
{

    class Image
    {
    public :

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
        bool addedConsign = false;
        QString consign = "";
    };

    struct Page
    {
        int startPhotoId = 0;
        int nbPhotos = 0;
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
        bool title = false;
        bool photos = false;
        bool consigns = false;
    };

    struct PDFSettings
    {
        bool displayCutLines = false;
        bool titleAdded = false;
        bool titleOnAllPages = false;
        int nbImagesPageV = 2, nbImagesPageH= 1;
        double ratioPhotosConsign = 0.15;

        int imagesAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        int consignAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        int titleAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        PageOrientation pageOrientation = PageOrientation::portrait;

        RatioMargins margins;

        Position globalConsignPositionFromImages = Position::top;
        QFont globalConsignFont;
        QColor globalConsignColor = Qt::black;
        QString globalConsignText = "Tapez la consigne ici.";

        Position titlePositionFromPage = Position::top;
        QFont titleFont;
        QColor titleColor = Qt::black;
        QString titleText = "TITRE";
    };

    using SPDFSettings = std::shared_ptr<PDFSettings>;

}

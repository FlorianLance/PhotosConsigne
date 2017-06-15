
#pragma once

// local
#include "Photo.hpp"


namespace pc {

    // define enums
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};
    enum class ColorType { color1 = 0, color2 = 1, degraded = 2};
//    enum class DegradedType {padSpread = 0, reflectSpread =1, repeatSpread = 2, radPadSpread = 3, radReflectSpread = 4, radRepeatSpread = 5, conical = 6};
    enum class DegradedType {padSpread = 0, radPadSpread = 1, conical = 2};

    // define classes
    struct MarginsSettings{

        bool exteriorMarginsEnabled;
        bool interiorMarginsEnabled;
        bool footerHeaderMarginEnabled;
        qreal left;
        qreal right;
        qreal top;
        qreal bottom;
        qreal interWidth;
        qreal interHeight;
        qreal footer;
        qreal header;
    };

    struct BordersSettings{

        bool display = false;
        qreal width = 1.;
        QPen pen;
    };

    struct MiscSettings{

        bool doNotDisplayHeader;
        bool doNotDisplayFooter;
    };

    struct SetsPositionSettings{

        bool customMode;

        int nbPhotos;
        int nbPhotosH;
        int nbPhotosV;

        QVector<qreal> columnsWidth;
        QVector<qreal> linesHeight;

        QVector<QRectF> relativePosCustom;
    };

    struct ColorsSettings{

        ColorType type;

        QColor color1;
        QColor color2;

        QPointF start;
        QPointF end;
        DegradedType degradedType;
    };

    struct BackGroundSettings{

        bool displayPhoto = false;
//        bool displayPattern = false;

//        Qt::BrushStyle patternBrushStyle = Qt::BDiagPattern;

//        QColor color = Qt::white;
//        QColor colorPattern = Qt::black;

        ImagePositionSettings imagePosition;
        ColorsSettings colors;
        SPhoto photo = nullptr;
    };


    struct StyleSettings{

        ImagePositionSettings imagePosition;

//        int photoAlignment;
        qreal ratioTextPhoto;
//        qreal scalePhoto;
        Position textPositionFromPhotos;
//        PhotoAdjust photoAdust = PhotoAdjust::adjust;
    };

    struct PhotosSettings{

        int previousId     = 0;
        int currentId      = 0;    /**< id of the current selcted photo (photo list widget) */
        QString lastDirectoryPath= "";
        SPhotos loaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
        SPhotos valided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    };


    struct TextSettings{
        std::shared_ptr<QString> html = nullptr;
    };

    struct HeaderSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct FooterSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct SetSettings{

        int currentId            = 0;    /**< id of the current set (Photo + consign) */
        int currentIdDisplayed   = -1;

        StyleSettings style;
        BordersSettings borders;
        TextSettings text;
    };

    struct PageSettings{

        int previousId         = -2;
        int currentId          =  0;    /**< id of the current selected page (page list widget) */
        int nb                 = 1;     /**< number of pages for the document */

        MarginsSettings margins;
        BackGroundSettings background;
        SetsPositionSettings positions;
        MiscSettings misc;        
    };

    struct DocumentSettings{

        bool grayScale           = false;
        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;

        PaperFormat paperFormat;
    };


    struct GlobalSettings{

        // photos
        PhotosSettings photos;

        // pages
        PageSettings pages;

        // sets
        SetSettings sets;

        // header
        HeaderSettings header;

        // footer
        FooterSettings footer;

        // document
        DocumentSettings document;
    };

}

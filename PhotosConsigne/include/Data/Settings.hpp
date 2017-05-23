
#pragma once

// local
#include "Photo.hpp"


namespace pc {

    // define enums
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};

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

    struct PageSetsSettings{

        int nbPhotos;
        int nbPhotosH;
        int nbPhotosV;
    };

    struct BackGroundSettings{

        bool displayPhoto = false;
        bool displayPattern = false;

        Qt::BrushStyle patternBrushStyle = Qt::BDiagPattern;

        QColor color = Qt::white;
        QColor colorPattern = Qt::black;

        SPhoto photo = nullptr;
    };

    struct HeaderSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        std::shared_ptr<QString> text = nullptr;
    };

    struct FooterSettings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        std::shared_ptr<QString> text = nullptr;
    };




}

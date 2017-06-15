
#pragma once

// std
#include <memory>

// Qt
#include <QImage>
#include <QFileInfo>
#include <QPainter>

// local
#include "RectPageItem.hpp"
#include "PaperFormat.hpp"
#include "DebugMessage.hpp"


namespace pc
{





    // define aliases
    struct Photo;
    using SPhoto  = std::shared_ptr<Photo>;

    using Photos  = QList<SPhoto>;
    using SPhotos = std::shared_ptr<Photos>;

    // define enums
    enum class PhotoAdjust { center = 0, extend = 1, fill = 2, adjust = 3, mosaic = 4};
    enum class PhotoPosition {top_left = 0, top_center = 1, top_right = 2,
                              middle_left = 3, middle_center = 4, middle_right = 5,
                             bottom_left = 6, bottom_center = 7, bottom_right = 8, custom = 9};

    struct ImagePositionSettings{

        qreal xPos = 0.5;
        qreal yPos = 0.5;
        qreal scale = 1.;
        PhotoPosition alignment = PhotoPosition::middle_center;
        PhotoAdjust adjustment = PhotoAdjust::adjust;
    };

    struct ExtraPCInfo{

        ExtraPCInfo(){}

        QFileInfo fileInfo;
        QString namePCAssociatedPhoto = "";

        bool preview        = false;
        bool displaySizes   = false;
        qreal factorUpscale = 1.;
        PaperFormat paperFormat;

        int pageNum       = -1;
        int pagesNb       = -1;
        int photoNum      = -1;
        int photoPCNum    = -1;
        int photoTotalNum = -1;
//        QColor pageColor = Qt::white;
    };




    static int convert_photo_position_to_qt_alignment(PhotoPosition position){

        int align = 0;
        switch (position) {
        case PhotoPosition::top_left:
            align |= Qt::AlignmentFlag::AlignLeft;
            align |= Qt::AlignmentFlag::AlignTop;
            break;
        case PhotoPosition::top_center:
            align |= Qt::AlignmentFlag::AlignHCenter;
            align |= Qt::AlignmentFlag::AlignTop;
            break;
        case PhotoPosition::top_right:
            align |= Qt::AlignmentFlag::AlignRight;
            align |= Qt::AlignmentFlag::AlignTop;
            break;
        case PhotoPosition::middle_left:
            align |= Qt::AlignmentFlag::AlignLeft;
            align |= Qt::AlignmentFlag::AlignVCenter;
            break;
        case PhotoPosition::middle_center:
            align |= Qt::AlignmentFlag::AlignHCenter;
            align |= Qt::AlignmentFlag::AlignVCenter;
            break;
        case PhotoPosition::middle_right:
            align |= Qt::AlignmentFlag::AlignRight;
            align |= Qt::AlignmentFlag::AlignVCenter;
            break;
        case PhotoPosition::bottom_left:
            align |= Qt::AlignmentFlag::AlignLeft;
            align |= Qt::AlignmentFlag::AlignBottom;
            break;
        case PhotoPosition::bottom_center:
            align |= Qt::AlignmentFlag::AlignHCenter;
            align |= Qt::AlignmentFlag::AlignBottom;
            break;
        case PhotoPosition::bottom_right:
            align |= Qt::AlignmentFlag::AlignRight;
            align |= Qt::AlignmentFlag::AlignBottom;
            break;
        default:
            break;
        }

        return align;
    }


    struct Photo : public RectPageItem {

        Photo() = delete;

        Photo(const Photo &photo) = default;

        Photo(QImage image);

        Photo(const QString &path, bool isWhiteSpace = false);

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }

        QSize size() const noexcept {return originalSize;}

        QSize scaled_size() const noexcept {return scaledPhoto.size();}

        void draw(QPainter &painter, const ImagePositionSettings &position,  const QRectF &rectPhoto, const ExtraPCInfo &infos, const QSizeF &pageSize = QSizeF());

    private:

        QRectF draw_small(QPainter &painter, const ImagePositionSettings &position, const QRectF &rectPhoto, const QImage &photo, const ExtraPCInfo &infos, const QSizeF &pageSize);

        void draw_huge_photo_whith_tiles(QPainter &painter, const QImage &photoToUpscale, const QRectF &rectPhoto);

        void draw_huge(QPainter &painter, const QRectF &rectPhoto);

    public:

        bool isWhiteSpace = false;
        bool isADuplicate = false;
        bool isRemoved    = false;
        bool isOnDocument = false;

        int rotation = 0;
        int loadedId = 0;  // global id from all loaded photos
        int id       = -1; // id from all valid photos
        int pageId   = -1;


//        qreal scale  = 1.;
//        PhotoPosition position = PhotoPosition::middle_center;
//        PhotoAdjust adjust = PhotoAdjust::adjust;

        QSize originalSize;
        QString pathPhoto;
        QString namePhoto;
        QFileInfo info;
        QImage scaledPhoto;
    };
}

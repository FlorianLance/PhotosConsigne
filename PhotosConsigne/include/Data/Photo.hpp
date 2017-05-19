
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


namespace pc
{

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
        QColor pageColor = Qt::white;
    };

    // define aliases
    struct Photo;
    using SPhoto  = std::shared_ptr<Photo>;

    using Photos  = QList<SPhoto>;
    using SPhotos = std::shared_ptr<Photos>;

    // define enums
    enum class PhotoAdjust { center = 0, extend = 1, fill = 2, adjust = 3, mosaic = 4};

    struct Photo : public RectPageItem {

        Photo() = delete;

        virtual ~Photo(){}

        Photo(const Photo &photo) = default;

        Photo(QImage image);

        Photo(const QString &path, bool isWhiteSpace = false);

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }

        QSize size() const noexcept {return originalSize;}

        QSize scaled_size() const noexcept {return scaledPhoto.size();}

        void draw(QPainter &painter, const QRectF &rectPhoto, const ExtraPCInfo &infos, const QSizeF &pageSize = QSizeF());

    private:

        QRectF draw_small(QPainter &painter, const QRectF &rectPhoto, const QImage &photo, const ExtraPCInfo &infos, const QSizeF &pageSize);

        void draw_huge_photo_whith_tiles(QPainter &painter, const QImage &photoToUpscale, const QRectF &rectPhoto);

        void draw_huge(QPainter &painter, const QRectF &rectPhoto);

    public:

        bool isWhiteSpace = false;
        bool isADuplicate = false;
        bool isRemoved    = false;
        bool isOnDocument = false;

        int alignment;
        int rotation = 0;
        int loadedId = 0;  // global id from all loaded photos
        int id       = -1; // id from all valid photos
        PhotoAdjust adjust = PhotoAdjust::adjust;

        QSize originalSize;
        QString pathPhoto;
        QString namePhoto;
        QFileInfo info;
        QImage scaledPhoto;
    };
}

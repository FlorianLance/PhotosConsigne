
#pragma once

// std
#include <memory>

// Qt
#include <QImage>
#include <QFileInfo>
#include <QPainter>

// local
#include "RectPageItem.hpp"

namespace pc
{
    // define aliases
    struct Photo;
    using SPhoto  = std::shared_ptr<Photo>;

    using Photos  = QList<SPhoto>;
    using SPhotos = std::shared_ptr<Photos>;

    // define enums
    enum class PhotoAdjust { center = 0, extend = 1, fill = 2, adjust = 3, mosaic = 4};

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

        void draw(QPainter &painter, const QRectF &rectPhoto, bool preview, bool drawImageSize = false, const QSizeF &pageSizeMM = QSizeF(), const QSizeF &pageSize = QSizeF());

    private:

        QRectF draw_small(QPainter &painter, const QRectF &rectPhoto, const QImage &photo, bool preview, bool drawImageSize, const QSizeF &pageSizeMM, const QSizeF &pageSize);

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

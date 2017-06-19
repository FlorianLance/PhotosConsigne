
/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once


/**
 * \file Photo.hpp
 * \brief defines Photo
 * \author Florian Lance
 * \date 04/04/2017
 */


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

        QSize originalSize;
        QString pathPhoto;
        QString namePhoto;
        QFileInfo info;
        QImage scaledPhoto;
    };
}

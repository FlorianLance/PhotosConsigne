

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
 * \file RectPageItem.hpp
 * \brief defines RectPageItem/Settings/ImagePositionSettings/ExtraPCInfo
 * \author Florian Lance
 * \date 04/04/2017
 */


// local
#include "PaperFormat.hpp"

// Qt
#include <QRectF>
#include <QFileInfo>
#include <QColor>
#include <QVector>

namespace pc {

    // define enums
    enum class PhotoAdjust { center = 0, extend = 1, fill = 2, adjust = 3, mosaic = 4};
    enum class PhotoPosition {top_left = 0, top_center = 1, top_right = 2,
                              middle_left = 3, middle_center = 4, middle_right = 5,
                             bottom_left = 6, bottom_center = 7, bottom_right = 8, custom = 9};
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};
    enum class ColorType { color1 = 0, color2 = 1, degraded = 2};
//    enum class DegradedType {padSpread = 0, reflectSpread =1, repeatSpread = 2, radPadSpread = 3, radReflectSpread = 4, radRepeatSpread = 5, conical = 6};
    enum class DegradedType {padSpread = 0, radPadSpread = 1, conical = 2};

    static const QVector<QColor> UIColors = {qRgb(96,72,204), qRgb(98,106,213), qRgb(148,154,226), qRgb(173,177,233),
                                                   qRgb(196,171,235), qRgb(159,118,222), qRgb(126,70,210), qRgb(154,169,207),
                                                   qRgb(109,131,186), qRgb(74,97,157), qRgb(122,164,167), qRgb(148,182,186),
                                                   qRgb(197,216,218), qRgb(108,182,255), qRgb(13,134,255), qRgb(0,92,185)};

    class StrUtility{

    public :
        static QString to_str(const QColor &color){
            return QString::number(color.red()) +  " " + QString::number(color.green()) + " " + QString::number(color.blue()) + " " + QString::number(color.alpha());
        }

        static QString to_str(const QPoint &pt){
            return QString::number(pt.x()) +  " " + QString::number(pt.y());
        }

        static QString to_str(const QPointF &pt){
            return QString::number(pt.x()) +  " " + QString::number(pt.y());
        }

        static QString to_str(const QSize &size){
            return QString::number(size.width()) +  " " + QString::number(size.height());
        }

        static QString to_str(const QRect &rect){
            return QString::number(rect.x()) +  " " + QString::number(rect.y()) + " " + QString::number(rect.width()) + " " + QString::number(rect.height());
        }

        static QString to_str(const QRectF &rect){
            return QString::number(rect.x()) +  " " + QString::number(rect.y()) + " " + QString::number(rect.width()) + " " + QString::number(rect.height());
        }
    };

    struct RectPageItem{
        QRectF rectOnPage;
        virtual void compute_sizes(QRectF upperRect) = 0;
        virtual ~RectPageItem(){}
    };


    struct Settings{
        virtual ~Settings(){}
    };

    struct ImagePositionSettings : public Settings{

        qreal xPos = 0.5;
        qreal yPos = 0.5;
        qreal scale = 1.;
        PhotoPosition alignment = PhotoPosition::middle_center;
        PhotoAdjust adjustment = PhotoAdjust::adjust;
    };

    struct ExtraPCInfo{

        QFileInfo fileInfo;
        QString namePCAssociatedPhoto = "";
        QString pageName;

        bool preview        = false;
        bool displaySizes   = false;
        qreal factorUpscale = 1.;
        PaperFormat paperFormat;

        int pageNum       = -1;
        int pagesNb       = -1;
        int photoNum      = -1;
        int photoPCNum    = -1;
        int photoTotalNum = -1;
    };
}

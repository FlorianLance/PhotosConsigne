
#pragma once

// Qt
#include <QPagedPaintDevice>

namespace pc {
    struct PaperFormat
    {
        bool landscape    = false;
        int dpi           = 300;
        qreal widthRatio  = 8.263;
        qreal heightRatio = 11.693;
        QSizeF sizeMM;
        QPagedPaintDevice::PageSize format = QPagedPaintDevice::PageSize::A4;

        PaperFormat(){}

        PaperFormat(QString dpiStr, QString formatStr, bool landScape);

        int64_t width_pixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(widthRatio*wantedDpi);
        }

        int64_t height_pixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(heightRatio*wantedDpi);
        }
    };


}


#pragma once

// Qt
#include <QRectF>

struct RectPageItem{
    QRectF rectOnPage;
    virtual void compute_sizes(QRectF upperRect) = 0;
};

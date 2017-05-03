
#include "Utility.h"



void pc::PCPage::compute_sizes(QRectF upperRect){
    rectOnPage = std::move(upperRect);

    // title rect and sets with margins rect
    QRectF titleRect(0.,0.,0.,0.);
    if(title != nullptr){
        if(titlePositionFromPC == Position::on){
            titleRect = rectOnPage;
            setsAndMarginsRect = rectOnPage;
        }else{
            qreal heightTitle = ratioWithTitle * rectOnPage.height();
            titleRect = QRectF(0, (titlePositionFromPC == Position::top) ? 0 : rectOnPage.height() - heightTitle,
                               rectOnPage.width(), heightTitle);
            setsAndMarginsRect = QRectF(0., (titlePositionFromPC == Position::top) ? titleRect.height() : 0.,
                                        rectOnPage.width(), rectOnPage.height()- titleRect.height());
        }

        title->compute_sizes(QRectF(titleRect));
    }else{
        setsAndMarginsRect = rectOnPage;
    }

    // extern margins
    qreal heightTopMargin = 0., heightBottomMargin = 0., widthLeftMargin = 0., widthRightMargin = 0.;
    if(margins.exteriorMarginsEnabled){
        heightTopMargin     = setsAndMarginsRect.height() * margins.top;
        heightBottomMargin  = setsAndMarginsRect.height() * margins.bottom;
        widthLeftMargin     = setsAndMarginsRect.width()  * margins.left;
        widthRightMargin    = setsAndMarginsRect.width()  * margins.right;
    }

    // sets
    setsRect = QRectF(setsAndMarginsRect.x() + widthLeftMargin, setsAndMarginsRect.y() + heightTopMargin,
                      setsAndMarginsRect.width() - widthLeftMargin - widthRightMargin, setsAndMarginsRect.height() - heightTopMargin - heightBottomMargin);

    int nbInterVMargins     = nbPhotosV - 1;
    int nbInterHMargins     = nbPhotosH - 1;
    qreal heightInterMargin = 0., widthInterMargin = 0.;
    if(margins.interiorMarginsEnabled){
        if(nbInterVMargins > 0){
            heightInterMargin = setsRect.height()/nbInterVMargins * margins.interHeight;
        }
        if(nbInterHMargins > 0){
            widthInterMargin  = setsRect.width()/nbInterHMargins  * margins.interWidth;
        }
    }

    // PC
    qreal widthPhotoAndConsign   = (setsRect.width()  - nbInterHMargins * widthInterMargin)  / nbPhotosH;
    qreal heightPhotoAndConsign  = (setsRect.height() - nbInterVMargins * heightInterMargin) / nbPhotosV;

    int currPC = 0;
    interMarginsRects.clear();
    interMarginsRects.reserve(sets.size());
    qreal offsetV = setsRect.y();
    for(int ii = 0; ii < nbPhotosV; ++ii){
        qreal offsetH = setsRect.x();
        for(int jj = 0; jj < nbPhotosH; ++jj){
            if(currPC >= sets.size())
                break;

            sets[currPC]->compute_sizes(QRectF(offsetH, offsetV, widthPhotoAndConsign, heightPhotoAndConsign));
            interMarginsRects.push_back(QRectF(offsetH, offsetV,
                                               (jj < nbPhotosH-1) ? widthPhotoAndConsign  + widthInterMargin  : widthPhotoAndConsign,
                                               (ii < nbPhotosV-1) ? heightPhotoAndConsign + heightInterMargin : heightPhotoAndConsign));

            offsetH += widthPhotoAndConsign + widthInterMargin;
            ++currPC;
        }

        offsetV += heightPhotoAndConsign + heightInterMargin;
    }

    // horizontal cut lines
    horizontalCutLines.clear();
    horizontalCutLines.push_back(QLineF(QPointF(0,setsRect.y()), QPointF(rectOnPage.width(),setsRect.y())));
    horizontalCutLines.push_back(QLineF(QPointF(0,setsRect.y()+setsRect.height()), QPointF(rectOnPage.width(),setsRect.y()+setsRect.height())));

    for(int ii = 1; ii < nbPhotosV; ++ii){
        qreal startV = setsRect.y() + ii*(heightPhotoAndConsign + heightInterMargin) - 0.5*heightInterMargin;
        horizontalCutLines.push_back(QLineF(QPointF(0,startV), QPointF(rectOnPage.width(),startV)));
    }

    // vertical cut lines
    verticalCutLines.clear();
    verticalCutLines.push_back(QLineF(QPointF(widthLeftMargin ,setsRect.y()), QPointF(widthLeftMargin, setsRect.y() + setsRect.height())));
    verticalCutLines.push_back(QLineF(QPointF(rectOnPage.width()-widthRightMargin,setsRect.y()), QPointF(rectOnPage.width()-widthRightMargin, setsRect.y() + setsRect.height())));
    for(int ii = 1; ii < nbPhotosH; ++ii){
        qreal startH = setsRect.x() + ii*(widthPhotoAndConsign + widthInterMargin) - 0.5*widthInterMargin;
        verticalCutLines.push_back(QLineF(QPointF(startH,setsRect.y()), QPointF(startH, setsRect.y() + setsRect.height())));
    }
}

void pc::PCSet::compute_sizes(QRectF upperRect){
    rectOnPage = std::move(upperRect);

    //  consign
    qreal heightConsigneV = rectOnPage.height() * (1. - ratio);
    qreal widthConsigneV  = rectOnPage.width();
    qreal heightConsigneH = rectOnPage.height();
    qreal widthConsigneH  = rectOnPage.width() * (1. - ratio);
    //  photo
    qreal heightPhotoV    = ratio * rectOnPage.height();
    qreal widthPhotoV     = rectOnPage.width();
    qreal heightPhotoH    = rectOnPage.height();
    qreal widthPhotoH     = ratio * rectOnPage.width();

    QRectF consignRect, photoRect;
    if(consignOnPhoto){
        photoRect = consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), rectOnPage.width(), rectOnPage.height());
    }else{
        switch (consignPositionFromPhoto) {
        case Position::top:
            consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), widthConsigneV, heightConsigneV);
            photoRect   = QRectF(rectOnPage.x(), rectOnPage.y() + heightConsigneV, widthPhotoV, heightPhotoV);
            break;
        case Position::left:
            consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), widthConsigneH, heightConsigneH);
            photoRect   = QRectF(rectOnPage.x() + widthConsigneH, rectOnPage.y(), widthPhotoH, heightPhotoH);
            break;
        case Position::right:
            consignRect = QRectF(rectOnPage.x() + widthPhotoH, rectOnPage.y(), widthConsigneH, heightConsigneH);
            photoRect   = QRectF(rectOnPage.x(), rectOnPage.y(), widthPhotoH, heightPhotoH);
            break;
        case Position::bottom:
            consignRect = QRectF(rectOnPage.x(), rectOnPage.y() + heightPhotoV, widthConsigneV, heightConsigneV);
            photoRect   = QRectF(rectOnPage.x(), rectOnPage.y(), widthPhotoV, heightPhotoV);
            break;
        default:
            break;
        }
    }

    consign->compute_sizes(consignRect);
    photo->compute_sizes(photoRect);
}

pc::Photo::Photo(const QString &path, bool isWhiteSpace) : isWhiteSpace(isWhiteSpace), pathPhoto(path){

    constexpr int maxHeight = 800;
    constexpr int maxWidth = 800;;

    if(isWhiteSpace){
        namePhoto = "Espace vide";
    }else{

        info            = QFileInfo(path);
        scaledPhoto     = QImage(path);
        originalSize    = scaledPhoto.size();

        if(!scaledPhoto.isNull()){

            namePhoto = pathPhoto.split('/').last().split('.').first();
            if(scaledPhoto.size().width() > maxWidth)
                scaledPhoto = scaledPhoto.scaledToWidth(maxWidth);
            if(scaledPhoto.size().height() > maxHeight)
                scaledPhoto = scaledPhoto.scaledToHeight(maxHeight);
        }
        else{
            namePhoto = "Erreur";
            qWarning() << "-Error: photo not loaded: " << pathPhoto;
        }
    }
}

void pc::Photo::draw(QPainter &painter, const QRectF &rectPhoto, bool preview, bool drawImageSize, pc::PaperFormat format, QRectF pageRect){

    if(isWhiteSpace){
        return;
    }

    if(scaledPhoto.isNull()){
        qWarning() << "-Error: photo is null, can't be drawn";
        return;
    }

    if(preview){
        draw(painter, rectPhoto, scaledPhoto, true, drawImageSize, format, pageRect);
    }else{
        if(rectPhoto.width() > 5000 || rectPhoto.height() > 5000){
            draw_huge(painter, rectPhoto);
        }else{
            draw(painter, rectPhoto, QImage(pathPhoto), false, false, format, pageRect);
        }
    }
}

void pc::Photo::draw(QPainter &painter, QRectF rectPhoto, QImage photo, bool preview, bool drawImageSize, pc::PaperFormat format, QRectF pageRect){

    QImage rotatedPhoto = preview ? photo.scaled(rectPhoto.width(),rectPhoto.height(), Qt::KeepAspectRatio) :
                                    (photo.transformed(QTransform().rotate(rotation))).scaled(rectPhoto.width(),rectPhoto.height(), Qt::KeepAspectRatio);

    qreal newX     = rectPhoto.x(), newY = rectPhoto.y();
    qreal newWidth = rotatedPhoto.width(), newHeight = rotatedPhoto.height();
    if(rotatedPhoto.width() < rectPhoto.width()){
        if(alignment & Qt::AlignRight){
            newX     += rectPhoto.width()-rotatedPhoto.width();
        }
        else if(alignment & Qt::AlignHCenter){
            newX     += rectPhoto.width()*0.5  - rotatedPhoto.width()*0.5;
        }
    }
    if(rotatedPhoto.height() < rectPhoto.height()){
        if(alignment & Qt::AlignBottom){
            newY      += rectPhoto.height()-rotatedPhoto.height();
        }
        else if(alignment & Qt::AlignVCenter){
            newY      += rectPhoto.height()*0.5-rotatedPhoto.height()*0.5;
        }
    }

    if((rectPhoto.width()-rotatedPhoto.width()) > (rectPhoto.height()-rotatedPhoto.height())){
        newHeight = rectPhoto.height();
        newY      = rectPhoto.y();
    }else{
        newWidth = rectPhoto.width();
        newX     = rectPhoto.x();
    }


    //            QImage rotatedPhoto2 = photo.copy(QRect(0,0,newWidth,newHeight));

    // draw image
    QRectF newRectPhoto(newX, newY, newWidth, newHeight);
    painter.drawImage(newRectPhoto, rotatedPhoto, QRectF(0.,0.,rotatedPhoto.width(),rotatedPhoto.height()));

    if(drawImageSize && newWidth > 100 && newHeight > 50){

        QPen pen;
        pen.setColor(Qt::red);
        painter.setPen(pen);
        QFont font;

        QString sizeImageStr = QString::number(newRectPhoto.width()/pageRect.width()*format.sizeMM.width(), 'i',0) + "x" +
                QString::number(newRectPhoto.height()/pageRect.height()*format.sizeMM.height(), 'i', 0) +"(mm)";
        font.setPointSizeF(newRectPhoto.width()/sizeImageStr.size());
        painter.setFont(font);


        painter.drawText(QRectF(newX, newY, newWidth, newHeight),  Qt::AlignCenter,sizeImageStr);
    }
}

void pc::Photo::draw_huge(QPainter &painter, QRectF rectPhoto){

    QImage rotatedPhoto = QImage(pathPhoto).transformed(QTransform().rotate(rotation));
    QSizeF upscaledSize(rectPhoto.width(),rectPhoto.height());
    QSizeF size = rotatedPhoto.size();

    qreal ratioW = 1.*size.width() / upscaledSize.width();
    qreal ratioH = 1.*size.height() / upscaledSize.height();

    if(ratioW > ratioH){
        upscaledSize = QSizeF(upscaledSize.width(), size.height()/ratioW);
    }else{
        upscaledSize = QSizeF(size.width()/ratioH, upscaledSize.height());
    }

    // alignment
    qreal newX = rectPhoto.x(), newY = rectPhoto.y();
    if(upscaledSize.width() < rectPhoto.width()){
        if(alignment & Qt::AlignRight){
            newX = rectPhoto.x() + rectPhoto.width()-upscaledSize.width();
        }
        if(alignment & Qt::AlignHCenter){
            newX = rectPhoto.x() + rectPhoto.width()*0.5 - upscaledSize.width()*0.5;
        }
    }
    if(upscaledSize.height() < rectPhoto.height()){
        if(alignment & Qt::AlignBottom){
            newY = rectPhoto.y() + rectPhoto.height()-upscaledSize.height();
        }
        if(alignment & Qt::AlignVCenter){
            newY = rectPhoto.y() + rectPhoto.height()*0.5-upscaledSize.height()*0.5;
        }
    }
    rectPhoto = QRectF(newX,newY,rectPhoto.width(),rectPhoto.height());

    // draw tiles
    int widthUpPerTile  = (upscaledSize.width() /10);
    int heightUpPerTile = (upscaledSize.height()/10);

    int widthUpTileRemainder  = static_cast<int>(upscaledSize.width())%widthUpPerTile;
    int heightUpTileRemainder = static_cast<int>(upscaledSize.height())%heightUpPerTile;

    int nbTilesH            = upscaledSize.width()/widthUpPerTile;
    int widthPerTile        = size.width()/nbTilesH;
    int widthTileRemainder  = static_cast<int>(size.width())%widthPerTile;

    int nbTilesV            = upscaledSize.height()/heightUpPerTile;
    int heightPerTile       = size.height()/nbTilesV;
    int heightTileRemainder = static_cast<int>(size.height())%heightPerTile;

    int nbTotalTilesH = nbTilesH + (widthTileRemainder  > 0 ? 1:0);
    int nbTotalTilesV = nbTilesV + (heightTileRemainder > 0 ? 1:0);

    for(int ii = 0; ii < nbTotalTilesH; ++ii){

        bool lastTileH   = (widthTileRemainder > 0) && (ii == nbTotalTilesH-1);
        bool lastTileUpH = (widthUpTileRemainder > 0) && (ii == nbTotalTilesH-1);

        for (int jj = 0; jj < nbTotalTilesV; ++jj){

            bool lastTileV = (heightTileRemainder > 0) && (jj == nbTotalTilesV-1);
            bool lastTileUpV = (heightUpTileRemainder > 0) && (jj == nbTotalTilesV-1);
            QRectF itRect(ii* widthPerTile, jj * heightPerTile, lastTileH ? widthTileRemainder : widthPerTile, lastTileV ?  heightTileRemainder : heightPerTile);
            QRectF itUpRect(ii* widthUpPerTile, jj * heightUpPerTile, lastTileUpH ? widthUpTileRemainder : widthUpPerTile, lastTileUpV ? heightUpTileRemainder : heightUpPerTile);
            size_t offset = itRect.x() * rotatedPhoto.depth() / 8 + itRect.y() * rotatedPhoto.bytesPerLine();

            painter.drawImage(rectPhoto.x()+itUpRect.x(),rectPhoto.y()+itUpRect.y(), QImage(rotatedPhoto.bits() + offset, itRect.width(),itRect.height(),
                                                                                            rotatedPhoto.bytesPerLine(), rotatedPhoto.format()).scaled(itUpRect.width(),itUpRect.height()));
        }
    }
}

pc::PaperFormat::PaperFormat(QString dpiStr, QString formatStr){

    dpi = dpiStr.toInt();

    if(formatStr == "A4"){
        widthRatio =  8.2633 ;
        heightRatio =  11.6933 ;
        format = QPagedPaintDevice::PageSize::A4;
        sizeMM = QSizeF(210.00,297.00);
    }else if(formatStr == "B5"){
        widthRatio =  6.9300 ;
        heightRatio =  9.8467 ;
        format = QPagedPaintDevice::PageSize::B5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "Legal"){
        widthRatio =  8.5000 ;
        heightRatio =  14.0000 ;
        format = QPagedPaintDevice::PageSize::Legal;
        sizeMM = QSizeF(216.00,356.00);
    }else if(formatStr == "Executive"){
        widthRatio =  7.5000 ;
        heightRatio =  10.0000 ;
        format = QPagedPaintDevice::PageSize::Executive;
        sizeMM = QSizeF(191.00,254.00);
    }else if(formatStr == "A0"){
        widthRatio =  33.1100 ;
        heightRatio =  46.8067 ;
        format = QPagedPaintDevice::PageSize::A0;
        sizeMM = QSizeF(841.00,1189.00);
    }else if(formatStr == "A1"){
        widthRatio =  23.3900 ;
        heightRatio =  33.1100 ;
        format = QPagedPaintDevice::PageSize::A1;
        sizeMM = QSizeF(594.00,841.00);
    }else if(formatStr == "A2"){
        widthRatio =  16.5433 ;
        heightRatio =  23.3900 ;
        format = QPagedPaintDevice::PageSize::A2;
        sizeMM = QSizeF(420.00,594.00);
    }else if(formatStr == "A3"){
        widthRatio =  11.6933 ;
        heightRatio =  16.5433 ;
        format = QPagedPaintDevice::PageSize::A3;
        sizeMM = QSizeF(297.00,420.00);
    }else if(formatStr == "A5"){
        widthRatio =  5.8333 ;
        heightRatio =  8.2633 ;
        format = QPagedPaintDevice::PageSize::A5;
        sizeMM = QSizeF(148.00,210.00);
    }else if(formatStr == "A6"){
        widthRatio =  4.1267 ;
        heightRatio =  5.8333 ;
        format = QPagedPaintDevice::PageSize::A6;
        sizeMM = QSizeF(105.00,148.00);
    }else if(formatStr == "A7"){
        widthRatio =  2.9167 ;
        heightRatio =  4.1267 ;
        format = QPagedPaintDevice::PageSize::A7;
        sizeMM = QSizeF(74.00,105.00);
    }else if(formatStr == "A8"){
        widthRatio =  2.0567 ;
        heightRatio =  2.9167 ;
        format = QPagedPaintDevice::PageSize::A8;
        sizeMM = QSizeF(52.00,74.00);
    }else if(formatStr == "A9"){
        widthRatio =  1.4600 ;
        heightRatio =  2.0567 ;
        format = QPagedPaintDevice::PageSize::A9;
        sizeMM = QSizeF(37.00,52.00);
    }else if(formatStr == "B0"){
        widthRatio =  39.3767 ;
        heightRatio =  55.6667 ;
        format = QPagedPaintDevice::PageSize::B0;
        sizeMM = QSizeF(1000.00,1414.00);
    }else if(formatStr == "B1"){
        widthRatio =  27.8333 ;
        heightRatio =  39.3767 ;
        format = QPagedPaintDevice::PageSize::B1;
        sizeMM = QSizeF(707.00,1000.00);
    }else if(formatStr == "B10"){
        widthRatio =  1.2233 ;
        heightRatio =  1.7367 ;
        format = QPagedPaintDevice::PageSize::B10;
        sizeMM = QSizeF(31.00,44.00);
    }else if(formatStr == "B2"){
        widthRatio =  19.6800 ;
        heightRatio =  27.8333 ;
        format = QPagedPaintDevice::PageSize::B2;
        sizeMM = QSizeF(500.00,707.00);
    }else if(formatStr == "B3"){
        widthRatio =  13.9033 ;
        heightRatio =  19.6800 ;
        format = QPagedPaintDevice::PageSize::B3;
        sizeMM = QSizeF(353.00,500.00);
    }else if(formatStr == "B4"){
        widthRatio =  9.8467 ;
        heightRatio =  13.9033 ;
        format = QPagedPaintDevice::PageSize::B4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "B6"){
        widthRatio =  4.9167 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::B6;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "B7"){
        widthRatio =  3.4600 ;
        heightRatio =  4.9167 ;
        format = QPagedPaintDevice::PageSize::B7;
        sizeMM = QSizeF(88.00,125.00);
    }else if(formatStr == "B8"){
        widthRatio =  2.4433 ;
        heightRatio =  3.4600 ;
        format = QPagedPaintDevice::PageSize::B8;
        sizeMM = QSizeF(62.00,88.00);
    }else if(formatStr == "B9"){
        widthRatio =  1.7367 ;
        heightRatio =  2.4433 ;
        format = QPagedPaintDevice::PageSize::B9;
        sizeMM = QSizeF(44.00,62.00);
    }else if(formatStr == "Folio"){
        widthRatio =  8.2633 ;
        heightRatio =  12.9867 ;
        format = QPagedPaintDevice::PageSize::Folio;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "Tabloid"){
        widthRatio =  11.0000 ;
        heightRatio =  17.0000 ;
        format = QPagedPaintDevice::PageSize::Tabloid;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "Custom"){
        widthRatio =  11.0000 ;
        heightRatio =  17.0000 ;
        format = QPagedPaintDevice::PageSize::Custom;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "A10"){
        widthRatio =  1.0133 ;
        heightRatio =  1.4600 ;
        format = QPagedPaintDevice::PageSize::A10;
        sizeMM = QSizeF(26.00,37.00);
    }else if(formatStr == "A3Extra"){
        widthRatio =  12.6800 ;
        heightRatio =  17.5267 ;
        format = QPagedPaintDevice::PageSize::A3Extra;
        sizeMM = QSizeF(322.00,445.00);
    }else if(formatStr == "A4Extra"){
        widthRatio =  9.2633 ;
        heightRatio =  12.6933 ;
        format = QPagedPaintDevice::PageSize::A4Extra;
        sizeMM = QSizeF(236.00,322.00);
    }else if(formatStr == "A4Plus"){
        widthRatio =  8.2633 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::A4Plus;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "A4Small"){
        widthRatio =  8.2633 ;
        heightRatio =  11.6933 ;
        format = QPagedPaintDevice::PageSize::A4Small;
        sizeMM = QSizeF(210.00,297.00);
    }else if(formatStr == "A5Extra"){
        widthRatio =  6.8333 ;
        heightRatio =  9.2767 ;
        format = QPagedPaintDevice::PageSize::A5Extra;
        sizeMM = QSizeF(174.00,235.00);
    }else if(formatStr == "B5Extra"){
        widthRatio =  7.9167 ;
        heightRatio =  10.8600 ;
        format = QPagedPaintDevice::PageSize::B5Extra;
        sizeMM = QSizeF(201.00,276.00);
    }else if(formatStr == "JisB0"){
        widthRatio =  40.5567 ;
        heightRatio =  57.3200 ;
        format = QPagedPaintDevice::PageSize::JisB0;
        sizeMM = QSizeF(1030.00,1456.00);
    }else if(formatStr == "JisB1"){
        widthRatio =  28.6667 ;
        heightRatio =  40.5567 ;
        format = QPagedPaintDevice::PageSize::JisB1;
        sizeMM = QSizeF(728.00,1030.00);
    }else if(formatStr == "JisB2"){
        widthRatio =  20.2767 ;
        heightRatio =  28.6667 ;
        format = QPagedPaintDevice::PageSize::JisB2;
        sizeMM = QSizeF(515.00,728.00);
    }else if(formatStr == "JisB3"){
        widthRatio =  14.3333 ;
        heightRatio =  20.2767 ;
        format = QPagedPaintDevice::PageSize::JisB3;
        sizeMM = QSizeF(364.00,515.00);
    }else if(formatStr == "JisB4"){
        widthRatio =  10.1267 ;
        heightRatio =  14.3333 ;
        format = QPagedPaintDevice::PageSize::JisB4;
        sizeMM = QSizeF(257.00,364.00);
    }else if(formatStr == "JisB5"){
        widthRatio =  7.1667 ;
        heightRatio =  10.1267 ;
        format = QPagedPaintDevice::PageSize::JisB5;
        sizeMM = QSizeF(182.00,257.00);
    }else if(formatStr == "JisB6"){
        widthRatio =  5.0433 ;
        heightRatio =  7.1667 ;
        format = QPagedPaintDevice::PageSize::JisB6;
        sizeMM = QSizeF(128.00,182.00);
    }else if(formatStr == "JisB7"){
        widthRatio =  3.5833 ;
        heightRatio =  5.0433 ;
        format = QPagedPaintDevice::PageSize::JisB7;
        sizeMM = QSizeF(91.00,128.00);
    }else if(formatStr == "JisB8"){
        widthRatio =  2.5133 ;
        heightRatio =  3.5833 ;
        format = QPagedPaintDevice::PageSize::JisB8;
        sizeMM = QSizeF(64.00,91.00);
    }else if(formatStr == "JisB9"){
        widthRatio =  1.7633 ;
        heightRatio =  2.5133 ;
        format = QPagedPaintDevice::PageSize::JisB9;
        sizeMM = QSizeF(45.00,64.00);
    }else if(formatStr == "JisB10"){
        widthRatio =  1.2633 ;
        heightRatio =  1.7633 ;
        format = QPagedPaintDevice::PageSize::JisB10;
        sizeMM = QSizeF(32.00,45.00);
    }else if(formatStr == "AnsiC"){
        widthRatio =  17.0000 ;
        heightRatio =  22.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiC;
        sizeMM = QSizeF(432.00,559.00);
    }else if(formatStr == "AnsiD"){
        widthRatio =  22.0000 ;
        heightRatio =  34.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiD;
        sizeMM = QSizeF(559.00,864.00);
    }else if(formatStr == "AnsiE"){
        widthRatio =  34.0000 ;
        heightRatio =  44.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiE;
        sizeMM = QSizeF(864.00,1118.00);
    }else if(formatStr == "LegalExtra"){
        widthRatio =  9.5000 ;
        heightRatio =  15.0000 ;
        format = QPagedPaintDevice::PageSize::LegalExtra;
        sizeMM = QSizeF(241.00,381.00);
    }else if(formatStr == "LetterExtra"){
        widthRatio =  9.5000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::LetterExtra;
        sizeMM = QSizeF(241.00,305.00);
    }else if(formatStr == "LetterPlus"){
        widthRatio =  8.5000 ;
        heightRatio =  12.6933 ;
        format = QPagedPaintDevice::PageSize::LetterPlus;
        sizeMM = QSizeF(216.00,322.00);
    }else if(formatStr == "LetterSmall"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::LetterSmall;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "TabloidExtra"){
        widthRatio =  12.0000 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::TabloidExtra;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchA"){
        widthRatio =  9.0000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::ArchA;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "ArchB"){
        widthRatio =  12.0000 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::ArchB;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchC"){
        widthRatio =  18.0000 ;
        heightRatio =  24.0000 ;
        format = QPagedPaintDevice::PageSize::ArchC;
        sizeMM = QSizeF(457.00,610.00);
    }else if(formatStr == "ArchD"){
        widthRatio =  24.0000 ;
        heightRatio =  36.0000 ;
        format = QPagedPaintDevice::PageSize::ArchD;
        sizeMM = QSizeF(610.00,914.00);
    }else if(formatStr == "ArchE"){
        widthRatio =  36.0000 ;
        heightRatio =  48.0000 ;
        format = QPagedPaintDevice::PageSize::ArchE;
        sizeMM = QSizeF(914.00,1219.00);
    }else if(formatStr == "Imperial7x9"){
        widthRatio =  7.0000 ;
        heightRatio =  9.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial7x9;
        sizeMM = QSizeF(178.00,229.00);
    }else if(formatStr == "Imperial8x10"){
        widthRatio =  8.0000 ;
        heightRatio =  10.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial8x10;
        sizeMM = QSizeF(203.00,254.00);
    }else if(formatStr == "Imperial9x11"){
        widthRatio =  9.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial9x11;
        sizeMM = QSizeF(229.00,279.00);
    }else if(formatStr == "Imperial9x12"){
        widthRatio =  9.0000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial9x12;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "Imperial10x11"){
        widthRatio =  10.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x11;
        sizeMM = QSizeF(254.00,279.00);
    }else if(formatStr == "Imperial10x13"){
        widthRatio =  10.0000 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x13;
        sizeMM = QSizeF(254.00,330.00);
    }else if(formatStr == "Imperial10x14"){
        widthRatio =  10.0000 ;
        heightRatio =  14.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x14;
        sizeMM = QSizeF(254.00,356.00);
    }else if(formatStr == "Imperial12x11"){
        widthRatio =  12.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial12x11;
        sizeMM = QSizeF(305.00,279.00);
    }else if(formatStr == "Imperial15x11"){
        widthRatio =  15.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial15x11;
        sizeMM = QSizeF(381.00,279.00);
    }else if(formatStr == "ExecutiveStandard"){
        widthRatio =  7.2500 ;
        heightRatio =  10.5000 ;
        format = QPagedPaintDevice::PageSize::ExecutiveStandard;
        sizeMM = QSizeF(184.00,267.00);
    }else if(formatStr == "Note"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Note;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "Quarto"){
        widthRatio =  8.4733 ;
        heightRatio =  10.8333 ;
        format = QPagedPaintDevice::PageSize::Quarto;
        sizeMM = QSizeF(216.00,275.00);
    }else if(formatStr == "Statement"){
        widthRatio =  5.5000 ;
        heightRatio =  8.5000 ;
        format = QPagedPaintDevice::PageSize::Statement;
        sizeMM = QSizeF(140.00,216.00);
    }else if(formatStr == "SuperA"){
        widthRatio =  8.9300 ;
        heightRatio =  14.0133 ;
        format = QPagedPaintDevice::PageSize::SuperA;
        sizeMM = QSizeF(227.00,356.00);
    }else if(formatStr == "SuperB"){
        widthRatio =  12.0000 ;
        heightRatio =  19.1667 ;
        format = QPagedPaintDevice::PageSize::SuperB;
        sizeMM = QSizeF(305.00,487.00);
    }else if(formatStr == "Postcard"){
        widthRatio =  3.9433 ;
        heightRatio =  5.8200 ;
        format = QPagedPaintDevice::PageSize::Postcard;
        sizeMM = QSizeF(100.00,148.00);
    }else if(formatStr == "DoublePostcard"){
        widthRatio =  7.8767 ;
        heightRatio =  5.8200 ;
        format = QPagedPaintDevice::PageSize::DoublePostcard;
        sizeMM = QSizeF(200.00,148.00);
    }else if(formatStr == "Prc16K"){
        widthRatio =  5.7500 ;
        heightRatio =  8.4733 ;
        format = QPagedPaintDevice::PageSize::Prc16K;
        sizeMM = QSizeF(146.00,215.00);
    }else if(formatStr == "Prc32K"){
        widthRatio =  3.8200 ;
        heightRatio =  5.9433 ;
        format = QPagedPaintDevice::PageSize::Prc32K;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "Prc32KBig"){
        widthRatio =  3.8200 ;
        heightRatio =  5.9433 ;
        format = QPagedPaintDevice::PageSize::Prc32KBig;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "FanFoldUS"){
        widthRatio =  14.8767 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldUS;
        sizeMM = QSizeF(378.00,279.00);
    }else if(formatStr == "FanFoldGerman"){
        widthRatio =  8.5000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldGerman;
        sizeMM = QSizeF(216.00,305.00);
    }else if(formatStr == "FanFoldGermanLegal"){
        widthRatio =  8.5000 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldGermanLegal;
        sizeMM = QSizeF(216.00,330.00);
    }else if(formatStr == "EnvelopeB4"){
        widthRatio =  9.8333 ;
        heightRatio =  13.9033 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "EnvelopeB5"){
        widthRatio =  6.9300 ;
        heightRatio =  9.8467 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "EnvelopeB6"){
        widthRatio =  6.9300 ;
        heightRatio =  4.9167 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB6;
        sizeMM = QSizeF(176.00,125.00);
    }else if(formatStr == "EnvelopeC0"){
        widthRatio =  36.0967 ;
        heightRatio =  51.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC0;
        sizeMM = QSizeF(917.00,1297.00);
    }else if(formatStr == "EnvelopeC1"){
        widthRatio =  25.5133 ;
        heightRatio =  36.0967 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC1;
        sizeMM = QSizeF(648.00,917.00);
    }else if(formatStr == "EnvelopeC2"){
        widthRatio =  18.0267 ;
        heightRatio =  25.5133 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC2;
        sizeMM = QSizeF(458.00,648.00);
    }else if(formatStr == "EnvelopeC3"){
        widthRatio =  12.7500 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC3;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeC4"){
        widthRatio =  9.0133 ;
        heightRatio =  12.7500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC4;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopeC6"){
        widthRatio =  4.4867 ;
        heightRatio =  6.3767 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC6;
        sizeMM = QSizeF(114.00,162.00);
    }else if(formatStr == "EnvelopeC65"){
        widthRatio =  4.5000 ;
        heightRatio =  9.0000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC65;
        sizeMM = QSizeF(114.00,229.00);
    }else if(formatStr == "EnvelopeC7"){
        widthRatio =  3.1933 ;
        heightRatio =  4.4867 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC7;
        sizeMM = QSizeF(81.00,114.00);
    }else if(formatStr == "Envelope9"){
        widthRatio =  3.8767 ;
        heightRatio =  8.8767 ;
        format = QPagedPaintDevice::PageSize::Envelope9;
        sizeMM = QSizeF(98.00,225.00);
    }else if(formatStr == "Envelope11"){
        widthRatio =  4.5000 ;
        heightRatio =  10.3767 ;
        format = QPagedPaintDevice::PageSize::Envelope11;
        sizeMM = QSizeF(114.00,264.00);
    }else if(formatStr == "Envelope12"){
        widthRatio =  4.7500 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Envelope12;
        sizeMM = QSizeF(121.00,279.00);
    }else if(formatStr == "Envelope14"){
        widthRatio =  5.0000 ;
        heightRatio =  11.5000 ;
        format = QPagedPaintDevice::PageSize::Envelope14;
        sizeMM = QSizeF(127.00,292.00);
    }else if(formatStr == "EnvelopeMonarch"){
        widthRatio =  3.8767 ;
        heightRatio =  7.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeMonarch;
        sizeMM = QSizeF(98.00,191.00);
    }else if(formatStr == "EnvelopePersonal"){
        widthRatio =  3.6267 ;
        heightRatio =  6.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopePersonal;
        sizeMM = QSizeF(92.00,165.00);
    }else if(formatStr == "EnvelopeChou3"){
        widthRatio =  4.7233 ;
        heightRatio =  9.2500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeChou3;
        sizeMM = QSizeF(120.00,235.00);
    }else if(formatStr == "EnvelopeChou4"){
        widthRatio =  3.5433 ;
        heightRatio =  8.0700 ;
        format = QPagedPaintDevice::PageSize::EnvelopeChou4;
        sizeMM = QSizeF(90.00,205.00);
    }else if(formatStr == "EnvelopeInvite"){
        widthRatio =  8.6667 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopeInvite;
        sizeMM = QSizeF(220.00,220.00);
    }else if(formatStr == "EnvelopeItalian"){
        widthRatio =  4.3333 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopeItalian;
        sizeMM = QSizeF(110.00,230.00);
    }else if(formatStr == "EnvelopeKaku2"){
        widthRatio =  9.4433 ;
        heightRatio =  13.0700 ;
        format = QPagedPaintDevice::PageSize::EnvelopeKaku2;
        sizeMM = QSizeF(240.00,332.00);
    }else if(formatStr == "EnvelopeKaku3"){
        widthRatio =  8.5000 ;
        heightRatio =  10.9033 ;
        format = QPagedPaintDevice::PageSize::EnvelopeKaku3;
        sizeMM = QSizeF(216.00,277.00);
    }else if(formatStr == "EnvelopePrc1"){
        widthRatio =  4.0133 ;
        heightRatio =  6.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc1;
        sizeMM = QSizeF(102.00,165.00);
    }else if(formatStr == "EnvelopePrc2"){
        widthRatio =  4.0133 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc2;
        sizeMM = QSizeF(102.00,176.00);
    }else if(formatStr == "EnvelopePrc3"){
        widthRatio =  4.9167 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc3;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "EnvelopePrc4"){
        widthRatio =  4.3333 ;
        heightRatio =  8.1933 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc4;
        sizeMM = QSizeF(110.00,208.00);
    }else if(formatStr == "EnvelopePrc5"){
        widthRatio =  4.3333 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc5;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "EnvelopePrc6"){
        widthRatio =  4.7233 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc6;
        sizeMM = QSizeF(120.00,230.00);
    }else if(formatStr == "EnvelopePrc7"){
        widthRatio =  6.3067 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc7;
        sizeMM = QSizeF(160.00,230.00);
    }else if(formatStr == "EnvelopePrc8"){
        widthRatio =  4.7233 ;
        heightRatio =  12.1667 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc8;
        sizeMM = QSizeF(120.00,309.00);
    }else if(formatStr == "EnvelopePrc9"){
        widthRatio =  9.0133 ;
        heightRatio =  12.7500 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc9;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopePrc10"){
        widthRatio =  12.7500 ;
        heightRatio =  18.0267 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc10;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeYou4"){
        widthRatio =  4.1400 ;
        heightRatio =  9.2500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeYou4;
        sizeMM = QSizeF(105.00,235.00);
        //            }else if(formatStr == "LastPageSize"){
        //               widthRatio =  4.1400 ;
        //               heightRatio =  9.2500 ;
        //               format = QPagedPaintDevice::PageSize::LastPageSize;
        //               sizeMM = QSizeF(105.00,235.00);
        //            }else if(formatStr == "NPageSize"){
        //               widthRatio =  4.1400 ;
        //               heightRatio =  9.2500 ;
        //               format = QPagedPaintDevice::PageSize::NPageSize;
        //               sizeMM = QSizeF(105.00,235.00);
        //            }else if(formatStr == "NPaperSize"){
        //               widthRatio =  4.1400 ;
        //               heightRatio =  9.2500 ;
        //               format = QPagedPaintDevice::PageSize::NPaperSize;
        //               sizeMM = QSizeF(105.00,235.00);
    }else if(formatStr == "AnsiA" || formatStr == "Letter"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiA;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "AnsiB" || formatStr == "Ledger"){
        widthRatio =  17.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiB;
        sizeMM = QSizeF(432.00,279.00);
    }else if(formatStr == "EnvelopeC5" || formatStr == "C5E"){
        widthRatio =  6.3767 ;
        heightRatio =  9.0133 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC5;
        sizeMM = QSizeF(162.00,229.00);
    }else if(formatStr == "EnvelopeDL" || formatStr == "DLE"){
        widthRatio =  4.3333 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopeDL;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "Envelope10" || formatStr == "Comm10E"){
        widthRatio =  4.1267 ;
        heightRatio =  9.5000 ;
        format = QPagedPaintDevice::PageSize::Envelope10;
        sizeMM = QSizeF(105.00,241.00);
    }
}

void pc::UIElements::insert_individual_consign(int index, bool whiteSpace){

    std::shared_ptr<QWidget>      SQWidget      = std::make_shared<QWidget>();
    std::shared_ptr<RichTextEdit> SRichTextEdit = std::make_shared<RichTextEdit>();

    SRichTextEdit->set_doc_locker(&docLocker);
    SRichTextEdit->init_as_individual_consign();

    individualConsignsWLoaded.insert(index,SQWidget);
    individualConsignsTEditLoaded.insert(index, SRichTextEdit);
//    previousIndividualConsignPositionFromPhotos.insert(index, Position::top);

    Ui::IndividualConsignW ui;
    ui.setupUi(SQWidget.get());
    ui.vlIndividualConsign->addWidget(SRichTextEdit.get());
    associate_buttons({ui.pbConsignBottom, ui.pbConsignLeft, ui.pbConsignRight, ui.pbConsignTop});
    associate_buttons({ui.pbImageAligmentRight, ui.pbImageAligmentLeft, ui.pbImageAligmentHMiddle});
    associate_buttons({ui.pbImageAligmentBottom, ui.pbImageAligmentVMiddle, ui.pbImageAligmentTop});
    update_settings_buttons({ui.pbConsignBottom, ui.pbConsignLeft, ui.pbConsignRight, ui.pbConsignTop,
                             ui.pbImageAligmentRight, ui.pbImageAligmentLeft, ui.pbImageAligmentHMiddle,
                             ui.pbImageAligmentBottom, ui.pbImageAligmentVMiddle, ui.pbImageAligmentTop}, true);
    update_settings_sliders({ui.hsRatioPC}, true);
    update_settings_double_spinboxes({ui.dsbRatioPC}, true);
    update_settings_checkboxes({ui.cbEnableIndividualConsign});

    checkbox_enable_UI(ui.cbEnableIndividualConsign, {ui.wTop});
    connect(SRichTextEdit.get()->textEdit(), &TextEdit::textChanged, this, &UIElements::update_settings_signal);
    connect(ui.cbWriteOnPhoto, &QCheckBox::clicked, this, [=](bool checked){

        ui.hsRatioPC->setEnabled(!checked);
        ui.dsbRatioPC->setEnabled(!checked);
        ui.pbConsignBottom->setEnabled(!checked);
        ui.pbConsignLeft->setEnabled(!checked);
        ui.pbConsignRight->setEnabled(!checked);
        ui.pbConsignTop->setEnabled(!checked);

        if(!checked){
            ui.pbConsignBottom->setEnabled(false);
        }

        emit update_settings_signal();
    });

    ui.pbInsertWhiteSpace->setEnabled(!whiteSpace);
    connect(ui.pbInsertWhiteSpace, &QPushButton::clicked, [=]{
        emit insert_white_space_signal();
    });

    associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);

}

void pc::UIElements::remove_individual_consign(int index){

    // remove element
    individualConsignsWLoaded.removeAt(index);
    individualConsignsTEditLoaded.removeAt(index);
//    previousIndividualConsignPositionFromPhotos.removeAt(index);
}

void pc::UIElements::reset_individual_consigns(int nbPhotos){

    qreal currentState = 750.;
    qreal offset = 250. / nbPhotos;

    // clean
    individualConsignsTEditLoaded.clear();
    individualConsignsWLoaded.clear();
//    previousIndividualConsignPositionFromPhotos.clear();

    // insert new consigns
    for(int ii = 0; ii < nbPhotos; ++ii){

        emit set_progress_bar_text_signal("Consigne individuelle n°" + QString::number(ii));
        insert_individual_consign(ii);
        currentState += offset;
        emit set_progress_bar_state_signal(static_cast<int>(currentState));
    }
}

void pc::UIElements::update_individual_pages(const GlobalData &settings)
{
    int diff = individualPageW.size() - settings.nbPages;
    if(diff < 0){ // add -diff pages

        for(int ii = 0; ii < -diff;++ ii){
            std::shared_ptr<QWidget> SQWidget = std::make_shared<QWidget>();
            individualPageW.push_back(SQWidget);

            Ui::IndividualPageW ui;
            ui.setupUi(SQWidget.get());

            // init uit
            ui.sbVSizeGrid->setValue(settings.nbPhotosPageV);
            ui.sbHSizeGrid->setValue(settings.nbPhotosPageH);
            ui.sbNbPhotos->setEnabled(false);

            // define connections...
            // # associate sliders with spin boxes
            associate_double_spinbox_with_slider(ui.dsbLeftMargins, ui.hsLeftMargins, ui.dsbRightMargins, ui.hsRightMargins);
            associate_double_spinbox_with_slider(ui.dsbTopMargins, ui.hsTopMargins, ui.dsbBottomMargins, ui.hsBottomMargins);
            associate_double_spinbox_with_slider(ui.dsbHorizontalMargins, ui.hsHorizontalMargins);
            associate_double_spinbox_with_slider(ui.dsbVerticalMargins, ui.hsVerticalMargins);


            update_settings_checkboxes({ui.cbEnableInvididualPageSettings, ui.cbAddExteriorMargins,ui.cbAddInteriorMargins}, true);
            update_settings_sliders({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins},true);
            update_settings_spinboxes({ui.sbVSizeGrid,ui.sbHSizeGrid,ui.sbNbPhotos}, true);
            update_settings_double_spinboxes({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins},true);
            checkbox_enable_UI(ui.cbAddExteriorMargins, {ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins, ui.hsBottomMargins,
                               ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins, ui.dsbBottomMargins});
            checkbox_enable_UI(ui.cbAddInteriorMargins, {ui.hsHorizontalMargins,ui.hsVerticalMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins});

            checkbox_enable_UI(ui.cbEnableInvididualPageSettings, {ui.laHSize, ui.laNbPhotos, ui.laVSize, ui.dsbLeftMargins, ui.hsLeftMargins, ui.dsbRightMargins, ui.hsRightMargins,
                               ui.dsbTopMargins, ui.hsTopMargins, ui.dsbBottomMargins, ui.hsBottomMargins,ui.dsbHorizontalMargins, ui.hsHorizontalMargins,
                               ui.dsbVerticalMargins, ui.hsVerticalMargins,ui.cbAddExteriorMargins,ui.cbAddInteriorMargins,ui.sbVSizeGrid,ui.sbHSizeGrid});
        }

    }else if(diff > 0){ // remove diff pages

        for(int ii = 0; ii < -diff;++ ii){
            individualPageW.pop_back();
        }
    }
}

void pc::UIElements::update_settings_buttons(QVector<QPushButton *> buttons, bool displayZones){
    for(auto &&button : buttons){
        connect(button, &QPushButton::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_sliders(QVector<QSlider *> sliders, bool displayZones){
    for(auto &&slider : sliders){
        connect(slider, &QSlider::valueChanged, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_spinboxes(QVector<QSpinBox *> spinBoxes, bool displayZones){
    for(auto &&spinBox : spinBoxes){
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_double_spinboxes(QVector<QDoubleSpinBox *> dSpinBoxes, bool displayZones){
    for(auto &&dSpinBox : dSpinBoxes){
        connect(dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_checkboxes(QVector<QCheckBox *> checkBoxes, bool displayZones){
    for(auto &&cb : checkBoxes){
        connect(cb, &QCheckBox::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_format_combo_boxes(QComboBox *comboDpi, QComboBox *comboFormat, bool displayZones){

    connect(comboDpi, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        if(displayZones){
            zonesTimer.start(1000);
        }
        emit update_settings_signal();
    });
    connect(comboFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{

        if(displayZones){
            zonesTimer.start(1000);
        }
        emit update_settings_signal();
    });
}

void pc::UIElements::update_settings_radio_buttons(QVector<QRadioButton *> radioButtons, bool displayZones){
    for(auto &&rb : radioButtons){
        connect(rb, &QRadioButton::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::associate_buttons(QVector<QPushButton *> buttons){
    auto nbButtons = buttons.size();
    for(decltype(nbButtons) ii = 0; ii < nbButtons; ++ii){
        connect(buttons[ii], &QPushButton::clicked, this, [=]{
            for(decltype(nbButtons) jj = 0; jj < nbButtons; ++jj){
                buttons[jj]->setEnabled(ii != jj);
            }
        });
    }
}

void pc::UIElements::associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider){
    connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        slider->blockSignals(true);
        slider->setValue(1000*value);
        slider->blockSignals(false);
    });
    // # sliders
    connect(slider, &QSlider::valueChanged, [=](double value){
        sb->blockSignals(true);
        sb->setValue(value*0.001);
        sb->blockSignals(false);
    });
}

void pc::UIElements::associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2){
    connect(sb1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb2->value() > 1){
            value = 1 - sb2->value();
            sb1->setValue(value);
        }
        slider1->blockSignals(true);
        slider1->setValue(1000*value);
        slider1->blockSignals(false);
    });
    connect(sb2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb1->value() > 1){
            value = 1 - sb1->value();
            sb2->setValue(value);
        }
        slider2->blockSignals(true);
        slider2->setValue(1000*value);
        slider2->blockSignals(false);
    });
    // # sliders
    connect(slider1, &QSlider::valueChanged, [=](double value){
        if(value + slider2->value() > 1000){
            value = 1000 - slider2->value();
            slider1->setValue(value);
        }
        sb1->blockSignals(true);
        sb1->setValue(value*0.001);
        sb1->blockSignals(false);
    });
    connect(slider2, &QSlider::valueChanged, [=](double value){
        if(value + slider1->value() > 1000){
            value = 1000 - slider1->value();
            slider2->setValue(value);
        }
        sb2->blockSignals(true);
        sb2->setValue(value*0.001);
        sb2->blockSignals(false);
    });
}

void pc::UIElements::checkbox_enable_UI(QCheckBox *cb, QVector<QWidget *> widgets, bool inverted){
    connect(cb, &QCheckBox::clicked, this, [=](bool checked){
        for(auto &&widget : widgets)
            widget->setEnabled(inverted ? (!checked) : checked);
    });
}

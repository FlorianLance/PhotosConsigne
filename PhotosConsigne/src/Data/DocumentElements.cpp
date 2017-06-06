

#include "DocumentElements.hpp"

// debug
#include <QDebug>

using namespace pc;

void pc::PCPage::compute_sizes(QRectF upperRect){

    rectOnPage = std::move(upperRect);

    // extern margins
    qreal heightTopMargin = 0., heightBottomMargin = 0., widthLeftMargin = 0., widthRightMargin = 0.;
    if(margins.exteriorMarginsEnabled){
        heightTopMargin     = rectOnPage.height() * margins.top;
        heightBottomMargin  = rectOnPage.height() * margins.bottom;
        widthLeftMargin     = rectOnPage.width()  * margins.left;
        widthRightMargin    = rectOnPage.width()  * margins.right;
    }

    // page minus extern margins
    pageMinusMarginsRect = QRectF(rectOnPage.x() + widthLeftMargin, rectOnPage.y() + heightTopMargin,
                                 rectOnPage.width() - widthLeftMargin - widthRightMargin, rectOnPage.height() - heightTopMargin - heightBottomMargin);

    qreal footerMarginHeight  = margins.footerHeaderMarginEnabled ? margins.footer * pageMinusMarginsRect.height() : 0.;
    qreal headerMarginHeight  = margins.footerHeaderMarginEnabled ? margins.header * pageMinusMarginsRect.height() : 0.;

    // header/footer ratios
    qreal headerFooterSetsHeight = pageMinusMarginsRect.height() - footerMarginHeight - headerMarginHeight;
    qreal footerRatio            = (footer->settings.enabled && !miscSettings.doNotDisplayFooter) ? footer->settings.ratio : 0.;
    qreal headerRatio            = (header->settings.enabled && !miscSettings.doNotDisplayHeader) ? header->settings.ratio : 0.;

    qreal sum = footerRatio + headerRatio;
    if(sum > 1.){
        footerRatio -= sum *0.5;
        headerRatio -= sum *0.5;
    }

    // header/footer/sets heights
    qreal footerHeight = footerRatio * headerFooterSetsHeight;
    qreal headerHeight = headerRatio * headerFooterSetsHeight;
    qreal setsHeight   = headerFooterSetsHeight - footerHeight - headerHeight;

    // header/footer/sets/headerMargins/footerMargins rects
    QRectF headerRect = QRectF( pageMinusMarginsRect.x(),
                                pageMinusMarginsRect.y(),
                                pageMinusMarginsRect.width(),
                                headerHeight);

    marginHeaderRect  = QRectF(pageMinusMarginsRect.x(),
                               headerRect.y() + headerRect.height(),
                               pageMinusMarginsRect.width(),
                               headerMarginHeight);

    setsRect          = QRectF(pageMinusMarginsRect.x(),
                               marginHeaderRect.y() + marginHeaderRect.height(),
                               pageMinusMarginsRect.width(),
                               setsHeight);

    marginFooterRect  = QRectF(pageMinusMarginsRect.x(),
                               setsRect.y() + setsRect.height(),
                               pageMinusMarginsRect.width(),
                               footerMarginHeight);

    QRectF footerRect = QRectF(pageMinusMarginsRect.x(),
                               marginFooterRect.y() + marginFooterRect.height(),
                               pageMinusMarginsRect.width(),
                               footerHeight);

    header->compute_sizes(headerRect);
    footer->compute_sizes(footerRect);

    // intern margins
    int nbInterVMargins     = pageSetsSettings.nbPhotosV - 1;
    int nbInterHMargins     = pageSetsSettings.nbPhotosH - 1;
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
    qreal widthSets   = (setsRect.width()  - nbInterHMargins * widthInterMargin)  / pageSetsSettings.nbPhotosH;
    qreal heightSets  = (setsRect.height() - nbInterVMargins * heightInterMargin) / pageSetsSettings.nbPhotosV;

    int currPC = 0;

    if(!pageSetsSettings.customMode){
        interMarginsRects.clear();
        interMarginsRects.reserve(sets.size());
        qreal offsetV = setsRect.y();
        for(int ii = 0; ii < pageSetsSettings.nbPhotosV; ++ii){
            qreal offsetH = setsRect.x();
            for(int jj = 0; jj < pageSetsSettings.nbPhotosH; ++jj){
                if(currPC >= sets.size())
                    break;

                sets[currPC]->compute_sizes(QRectF(offsetH, offsetV, widthSets, heightSets));
                interMarginsRects.push_back(QRectF(offsetH, offsetV,
                                                   (jj < pageSetsSettings.nbPhotosH-1) ? widthSets  + widthInterMargin  : widthSets,
                                                   (ii < pageSetsSettings.nbPhotosV-1) ? heightSets + heightInterMargin : heightSets));

                offsetH += widthSets + widthInterMargin;
                ++currPC;
            }

            offsetV += heightSets + heightInterMargin;
        }
    }else{

        interMarginsRects.clear();
        for(int ii = 0; ii < pageSetsSettings.relativePosCustom.size(); ++ii){
            if(currPC >= sets.size())
                break;

            QRectF &relRect = pageSetsSettings.relativePosCustom[ii];

            sets[currPC]->compute_sizes(QRectF(setsRect.x() + relRect.x()*setsRect.width(),
                                               setsRect.y() + relRect.y()*setsRect.height(),
                                               relRect.width()*setsRect.width(),
                                               relRect.height()*setsRect.height()));
            ++currPC;
        }
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
    case Position::on:
        photoRect = consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), rectOnPage.width(), rectOnPage.height());
        break;
    }

    consign->compute_sizes(consignRect);
    photo->compute_sizes(photoRect);
}


void Consign::compute_sizes(QRectF upperRect){
    rectOnPage = std::move(upperRect);
}

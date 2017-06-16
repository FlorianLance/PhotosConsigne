
// local
#include "PDFGeneratorWorker.hpp"


// Qt
#include <QCoreApplication>
#include <QVector2D>


using namespace pc;


void PDFGeneratorWorker::draw_zones(QPainter &painter, SPCPage pcPage){

    // fill extern margins rect
    Drawing::draw_filled_rect(painter, pcPage->rectOnPage, qRgb(0,255,0),0.7);

    // remove header/footer/headerMargins/footerMargins/Sets
    Drawing::draw_filled_rect(painter, pcPage->pageMinusMarginsRect, qRgb(255,255,255), 1.);

    // fill header rect
    Drawing::draw_filled_rect(painter, pcPage->header->rectOnPage, qRgb(255,127,39), 76./255.);

    // fill header margins rect
    Drawing::draw_filled_rect(painter, pcPage->marginHeaderRect, qRgb(150,0,0), 0.1);

    // fill footer rect
    Drawing::draw_filled_rect(painter, pcPage->footer->rectOnPage, qRgb(255,127,39), 76./255.);

    // fill footer margins rect
    Drawing::draw_filled_rect(painter, pcPage->marginFooterRect, qRgb(150,0,0), 0.1);

    // sets
    for(auto &&set : pcPage->sets){

        // inter margins
        Drawing::draw_filled_rect(painter, pcPage->interMarginsRects[set->id], qRgb(200,0,0), 0.3);

        // remove set
        Drawing::draw_filled_rect(painter, set->rectOnPage, qRgb(255,255,255), 0.3);

        // photo
        Drawing::draw_filled_rect(painter, set->photo->rectOnPage, qRgb(255,255,0), 0.3);

        // text
        Drawing::draw_filled_rect(painter, set->text->rectOnPage, qRgb(0,0,200), 0.3);
    }
}


void PDFGeneratorWorker::draw_degraded(QPainter &painter, const QRectF &rectPage, const ColorsSettings &colors, const ExtraPCInfo &infos){

    QPointF startR(rectPage.width()*colors.start.x(), rectPage.height()*colors.start.y());
    QPointF endR(rectPage.width()*colors.end.x(), rectPage.height()*colors.end.y());
    switch(colors.degradedType){
    case DegradedType::padSpread:{
            QLinearGradient grad(startR, endR);
            grad.setSpread(QGradient::PadSpread);
            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
            grad.setColorAt(0, colors.color1);
            grad.setColorAt(1, colors.color2);
            painter.fillRect(rectPage,grad);
        }
        break;
//    case DegradedType::reflectSpread:{
//            QLinearGradient grad(startR, endR);
//            grad.setSpread(QGradient::ReflectSpread);
//            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
//            grad.setColorAt(0, colors.color1);
//            grad.setColorAt(1, colors.color2);
//            painter.fillRect(rectPage,grad);
//        }
//        break;
//    case DegradedType::repeatSpread:{
//            QLinearGradient grad(startR, endR);
//            grad.setSpread(QGradient::RepeatSpread);
//            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
//            grad.setColorAt(0, colors.color1);
//            grad.setColorAt(1, colors.color2);
//            painter.fillRect(rectPage,grad);
//        }
//        break;
    case DegradedType::radPadSpread:{
            QRadialGradient grad(startR, static_cast<qreal>(QVector2D(startR-endR).length()));
            grad.setSpread(QGradient::PadSpread);
            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
            grad.setColorAt(0, colors.color1);
            grad.setColorAt(1, colors.color2);
            painter.fillRect(rectPage,grad);
        }
        break;
//    case DegradedType::radReflectSpread:{
//            QRadialGradient grad(startR, static_cast<qreal>(QVector2D(startR-endR).length()));
//            grad.setSpread(QGradient::ReflectSpread);
//            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
//            grad.setColorAt(0, colors.color1);
//            grad.setColorAt(1, colors.color2);
//            painter.fillRect(rectPage,grad);
//        }
//        break;
//    case DegradedType::radRepeatSpread:{
//            QRadialGradient grad(startR, static_cast<qreal>(QVector2D(startR-endR).length()));
//            grad.setSpread(QGradient::RepeatSpread);
//            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
//            grad.setColorAt(0, colors.color1);
//            grad.setColorAt(1, colors.color2);
//            painter.fillRect(rectPage,grad);
//        }
//        break;
    case DegradedType::conical:{
            QConicalGradient grad(startR, static_cast<qreal>(QVector2D(startR-endR).length()));
            grad.setCoordinateMode(QGradient::CoordinateMode::LogicalMode);
            grad.setColorAt(0, colors.color1);
            grad.setColorAt(1, colors.color2);
            painter.fillRect(rectPage,grad);
        }
        break;
    }
}


void PDFGeneratorWorker::draw_backgrounds(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos){

    // page background
    // # color
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    BackGroundSettings &background = pcPage->settings.background;

    // # white background
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::white);
    painter.fillRect(pcPage->rectOnPage,brush);

    // # photo
    if(pcPage->settings.background.displayPhoto){

        if(background.photo != nullptr){
            background.photo->draw(painter, pcPage->settings.background.imagePosition, pcPage->rectOnPage, infos, pcPage->rectOnPage.size());
        }
    }

    // # colors
    if(background.colors.type == ColorType::color1){
        brush.setColor(background.colors.color1);
        painter.fillRect(pcPage->rectOnPage,brush);
    }else if(background.colors.type == ColorType::color2){
        brush.setColor(background.colors.color2);
        painter.fillRect(pcPage->rectOnPage,brush);
    }else{
        draw_degraded(painter, pcPage->rectOnPage, background.colors, infos);
    }

    // # pattern
//    if(background.displayPattern){

//        qreal widthPattern  = m_referenceDPI * infos.paperFormat.widthRatio;
//        qreal heightPattern = m_referenceDPI * infos.paperFormat.heightRatio;
//        QRectF patternRect(0., 0., widthPattern, heightPattern);

//        QImage patternImage(static_cast<int>(patternRect.width()),static_cast<int>(patternRect.height()), QImage::Format_ARGB32);
//        QPainter patternPainter(&patternImage);
//        brush.setStyle(background.patternBrushStyle);
//        brush.setColor(background.colorPattern);
//        patternPainter.fillRect(patternRect, brush);
//        patternPainter.end();

//        Photo patternPhoto(std::move(patternImage));
//        patternPhoto.draw(painter, pcPage->rectOnPage, infos);
//    }

    // header background
    if(pcPage->header->settings.enabled){

        // # color
        brush.setStyle(Qt::SolidPattern);
        if(pcPage->header->settings.background.colors.type == ColorType::color1){
            brush.setColor(pcPage->header->settings.background.colors.color1);
            painter.fillRect(pcPage->header->rectOnPage,brush);
        }else if(pcPage->header->settings.background.colors.type == ColorType::color2){
            brush.setColor(pcPage->header->settings.background.colors.color2);
            painter.fillRect(pcPage->header->rectOnPage,brush);
        }else{
            draw_degraded(painter, pcPage->header->rectOnPage, pcPage->header->settings.background.colors, infos);
        }

        // # photo
        if(pcPage->header->settings.background.displayPhoto){
            if(pcPage->header->settings.background.photo != nullptr){
                pcPage->header->settings.background.photo->draw(painter, pcPage->header->settings.background.imagePosition, pcPage->header->rectOnPage, infos, pcPage->header->rectOnPage.size());
            }
        }
    }

    // footer background
    if(pcPage->footer->settings.enabled){

        // # color
        brush.setStyle(Qt::SolidPattern);
        if(pcPage->footer->settings.background.colors.type == ColorType::color1){
            brush.setColor(pcPage->footer->settings.background.colors.color1);
            painter.fillRect(pcPage->footer->rectOnPage,brush);
        }else if(pcPage->footer->settings.background.colors.type == ColorType::color2){
            brush.setColor(pcPage->footer->settings.background.colors.color2);
            painter.fillRect(pcPage->footer->rectOnPage,brush);
        }else{
            draw_degraded(painter, pcPage->footer->rectOnPage, pcPage->footer->settings.background.colors, infos);
        }

        // # photo
        if(pcPage->footer->settings.background.displayPhoto){

            if(pcPage->footer->settings.background.photo != nullptr){
                pcPage->footer->settings.background.photo->draw(painter,  pcPage->footer->settings.background.imagePosition, pcPage->footer->rectOnPage, infos, pcPage->footer->rectOnPage.size());
            }
        }
    }
}


void PDFGeneratorWorker::draw_contents(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos){

    // PC
    for(auto &&set : pcPage->sets){

        painter.setOpacity(1.);
        SPCSet pcSet = set;
        infos.photoNum   = pcSet->totalId;
        infos.photoPCNum = pcSet->id;
        infos.namePCAssociatedPhoto = pcSet->photo->namePhoto;
        infos.fileInfo = pcSet->photo->info;

        if(!infos.preview){
            emit set_progress_bar_text_signal("Dessin photo-consigne n°" + QString::number(pcSet->totalId));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }

        if(pcSet->settings.style.textPositionFromPhotos != Position::on){

            // draw text
            if(pcSet->text->rectOnPage.width()> 0 && pcSet->text->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, Drawing::format_html_for_generation(*pcSet->settings.text.html, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->text->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->text->rectOnPage);
            }
        }

        // draw photo
        if(pcSet->photo != nullptr){
            if(pcSet->photo->rectOnPage.width() > 0 && pcSet->photo->rectOnPage.height() > 0){ // ############################ costly

                pcSet->photo->draw(painter,pcSet->settings.style.imagePosition ,pcSet->photo->rectOnPage, infos, pcPage->rectOnPage.size());
            }
        }

        if(pcSet->settings.style.textPositionFromPhotos == Position::on){
            // draw text
            if(pcSet->text->rectOnPage.width()> 0 && pcSet->text->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, Drawing::format_html_for_generation(*pcSet->settings.text.html, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->text->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->text->rectOnPage);
            }
        }

        if(pcSet->settings.borders.display){
            pcSet->settings.borders.pen.setWidthF(pcSet->settings.borders.width*infos.factorUpscale);
            painter.setOpacity(1.);
            painter.setPen(pcSet->settings.borders.pen);
            painter.drawRect(pcSet->rectOnPage);
        }

        if(!infos.preview){
            emit set_progress_bar_state_signal(static_cast<int>(1000. * pcSet->totalId/m_totalPC));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }
    }

    // header
    if(pcPage->header->settings.enabled && !pcPage->settings.misc.doNotDisplayHeader){

        // text
        if(pcPage->header->rectOnPage.height() > 0){
            painter.setOpacity(1.);
            infos.photoNum      = 0;
            infos.photoPCNum    = 0;

            if(!infos.preview){
                emit set_progress_bar_text_signal("Dessin haut de page");
                QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
            }

            draw_html(painter, Drawing::format_html_for_generation(*pcPage->header->settings.text.html.get(), infos),
                      QRectF(pcPage->header->rectOnPage.x(),        pcPage->header->rectOnPage.y(),
                             pcPage->header->rectOnPage.width(),    pcPage->rectOnPage.height()),
                      pcPage->header->rectOnPage);
        }
    }

    // footer
    if(pcPage->footer->settings.enabled && !pcPage->settings.misc.doNotDisplayFooter){

        // text
        if(pcPage->footer->rectOnPage.height() > 0){

            painter.setOpacity(1.);
            infos.photoNum      = 0;
            infos.photoPCNum    = 0;

            if(!infos.preview){
                emit set_progress_bar_text_signal("Dessin base de page");
                QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
            }

            draw_html(painter, Drawing::format_html_for_generation(*pcPage->footer->settings.text.html.get(), infos),
                      QRectF(pcPage->footer->rectOnPage.x(),        pcPage->footer->rectOnPage.y(),
                             pcPage->footer->rectOnPage.width(),    pcPage->rectOnPage.height()),
                      pcPage->footer->rectOnPage);
        }
    }
}


void PDFGeneratorWorker::draw_page(QPainter &painter, pc::PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones){

    SPCPage pcPage = pcPages.pages[idPageToDraw];

    ExtraPCInfo infos;
    infos.pagesNb       = pcPages.pages.size();
    infos.pageNum       = idPageToDraw;
//    infos.pageColor     = pcPage->settings.background.color;
    infos.photoNum      = 0;
    infos.photoPCNum    = 0;
    infos.preview       = preview;
    infos.paperFormat   = pcPages.settings.paperFormat;
    infos.factorUpscale = factorUpscale;
    infos.displaySizes  = drawZones;
    infos.pageName      = pcPage->settings.name;

    for(auto &&page : pcPages.pages){
        infos.photoTotalNum += page->sets.size();
    }

    if(drawZones){
        draw_zones(painter,pcPage);
    }
//    else{
        draw_backgrounds(painter, pcPage, infos);
//    }

    draw_contents(painter, pcPage, infos);
}

void PDFGeneratorWorker::draw_html(QPainter &painter, QString html, QRectF upperRect, QRectF docRect){

    if(static_cast<int>(upperRect.width()) == 0 || static_cast<int>(upperRect.height()) == 0){
        return;
    }
    //        QElapsedTimer timer;
    //        timer.start();
    m_doc->setPageSize(QSizeF(upperRect.width(), upperRect.height()));
    m_doc->setHtml(std::move(html));
    painter.translate(QPointF(docRect.x(),docRect.y()));

    m_doc->setIndentWidth(0);
    m_doc->drawContents(&painter, QRectF(0,0,docRect.width(),docRect.height()));
    painter.translate(QPointF(-docRect.x(),-docRect.y()));
    //        qDebug() << "   draw_html " << timer.elapsed() << "ms";
}

void PDFGeneratorWorker::kill(){

    m_locker.lockForWrite();
    m_continueLoop = false;
    m_locker.unlock();
}

void PDFGeneratorWorker::generate_preview(pc::PCPages pcPages, int pageIdToDraw, bool drawZones){

//    DebugMessage dbgMess("generate_preview");
    m_pageToDraw    = pcPages.pages[pageIdToDraw];
    int dpi = pcPages.settings.paperFormat.dpi;
    if(dpi > 300){
        dpi = 300;
    }

    qreal widthPreview   = pcPages.settings.paperFormat.widthRatio  * dpi;
    qreal heightPreview  = pcPages.settings.paperFormat.heightRatio * dpi;

    // create preview image
    QImage image(static_cast<int>(widthPreview), static_cast<int>(heightPreview), QImage::Format_RGB32);
    QPainter painter(&image);

    m_pageToDraw->compute_sizes(QRectF(0 ,0, widthPreview, heightPreview));

    draw_page(painter, pcPages, pageIdToDraw, 1.*dpi/m_referenceDPI, true, drawZones);

    painter.end();

    if(pcPages.settings.grayScale){
        for (int ii = 0; ii < image.height(); ii++) {
            QRgb *pixel = reinterpret_cast<QRgb*>(image.scanLine(ii));
            QRgb *end = pixel + image.width();
            for (; pixel != end; pixel++) {
                int gray = qGray(*pixel);
                *pixel = QColor(gray, gray, gray).rgb();
            }
        }
    }

    emit end_preview_signal(image, m_pageToDraw);
}

void PDFGeneratorWorker::generate_PDF(pc::PCPages pcPages){

    int nbTotalPC = 0;
    for(auto &&page : pcPages.pages){
        nbTotalPC += page->sets.size();
    }

    m_totalPC = nbTotalPC;
    emit set_progress_bar_state_signal(0);

    QPrinter pdfWriter(QPrinter::HighResolution);


    pdfWriter.setOutputFormat(QPrinter::PdfFormat);
    pdfWriter.setOutputFileName(pcPages.pdfFileName);
    pdfWriter.setCreator("created with PhotosConsigne (https://github.com/FlorianLance/PhotosConsigne)");
    pdfWriter.setColorMode(pcPages.settings.grayScale ? QPrinter::ColorMode::GrayScale : QPrinter::ColorMode::Color);

    pdfWriter.setResolution(pcPages.settings.paperFormat.dpi);
    pdfWriter.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pcPages.settings.paperFormat.format)));
    pdfWriter.setPageOrientation(pcPages.settings.paperFormat.landscape ? QPageLayout::Landscape : QPageLayout::Portrait);
    pdfWriter.setPageMargins(QMarginsF(0.,0.,0.,0.));

    // init painter
    QPainter pdfPainter;
    pdfPainter.setRenderHints(QPainter::Antialiasing, true);
    pdfPainter.setPen(Qt::NoPen);
    if(!pdfPainter.begin(&pdfWriter)){
        qWarning() << "-Error: can't write on file: " << pcPages.pdfFileName << ", file may not exists";

        emit abort_pdf_signal(pcPages.pdfFileName);
        return;
    }

    pcPages.compute_all_pages_sizes(pdfWriter.width(), pdfWriter.height());
    for(int ii = 0; ii < pcPages.pages.size(); ++ii){

        if(!pcPages.pages[ii]->drawThisPage){
            continue;
        }

        if(ii > 0){
            pdfWriter.newPage();
        }

        bool continueLoop;
        m_locker.lockForRead();
        continueLoop = m_continueLoop;
        m_locker.unlock();

        if(!continueLoop)
            return;

        emit set_progress_bar_text_signal("Création page " + QString::number(ii));

        qreal factor = 1.*pcPages.settings.paperFormat.dpi/m_referenceDPI;
        draw_page(pdfPainter, pcPages, ii, factor, false, false);

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }

    // end pdf writing
    pdfPainter.end();

    emit set_progress_bar_state_signal(1000);
    emit end_generation_signal(true);
}



void PDFGeneratorWorker::init_document(){
    m_doc = std::make_unique<QTextDocument>();
}

void PDFGeneratorWorker::add_resource(QUrl url, QImage image){
    m_doc->addResource(QTextDocument::ImageResource, std::move(url), std::move(image));
}


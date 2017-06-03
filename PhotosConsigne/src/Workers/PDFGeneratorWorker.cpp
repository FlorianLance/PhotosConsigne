
// local
#include "PDFGeneratorWorker.hpp"


// Qt
#include <QCoreApplication>


using namespace pc;

PDFGeneratorWorker::~PDFGeneratorWorker(){
    DebugMessage("~PDFGeneratorWorker");
}


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
        Drawing::draw_filled_rect(painter, set->rectOnPage, qRgb(255,255,255), 1.);

        // photo
        Drawing::draw_filled_rect(painter, set->photo->rectOnPage, qRgb(255,255,0), 0.3);

        // text
        Drawing::draw_filled_rect(painter, set->consign->rectOnPage, qRgb(0,0,200), 0.3);
    }
}

void PDFGeneratorWorker::draw_backgrounds(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos){

    // page background
    // # color
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(pcPage->backgroundSettings.color);
    painter.fillRect(pcPage->rectOnPage,brush);

    // # photo
    if(pcPage->backgroundSettings.displayPhoto){

        if(pcPage->backgroundSettings.photo != nullptr){
            pcPage->backgroundSettings.photo->draw(painter, pcPage->rectOnPage, infos, pcPage->rectOnPage.size());
        }
    }

    // # pattern
    if(pcPage->backgroundSettings.displayPattern){

        qreal widthPattern  = m_referenceDPI * infos.paperFormat.widthRatio;
        qreal heightPattern = m_referenceDPI * infos.paperFormat.heightRatio;
        QRectF patternRect(0., 0., widthPattern, heightPattern);

        QImage patternImage(static_cast<int>(patternRect.width()),static_cast<int>(patternRect.height()), QImage::Format_ARGB32);
        QPainter patternPainter(&patternImage);
        brush.setStyle(pcPage->backgroundSettings.patternBrushStyle);
        brush.setColor(pcPage->backgroundSettings.colorPattern);
        patternPainter.fillRect(patternRect, brush);
        patternPainter.end();

        Photo patternPhoto(std::move(patternImage));
        patternPhoto.draw(painter, pcPage->rectOnPage, infos);
    }

    // header background
    if(pcPage->header->settings.enabled){

        // # color
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(pcPage->header->settings.background.color);
        painter.fillRect(pcPage->header->rectOnPage, brush);

        // # photo
        if(pcPage->header->settings.background.displayPhoto){
            if(pcPage->header->settings.background.photo != nullptr){
                pcPage->header->settings.background.photo->draw(painter, pcPage->header->rectOnPage, infos, pcPage->header->rectOnPage.size());
            }
        }
    }

    // footer background
    if(pcPage->footer->settings.enabled){

        // # color
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(pcPage->footer->settings.background.color);
        painter.fillRect(pcPage->footer->rectOnPage,brush);

        // # photo
        if(pcPage->footer->settings.background.displayPhoto){

            if(pcPage->footer->settings.background.photo != nullptr){
                pcPage->footer->settings.background.photo->draw(painter, pcPage->footer->rectOnPage, infos, pcPage->footer->rectOnPage.size());
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


        if(pcSet->consignPositionFromPhoto != Position::on){
            // draw consign
            if(pcSet->consign->rectOnPage.width()> 0 && pcSet->consign->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, Drawing::format_html_for_generation(*pcSet->consign->html, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->consign->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->consign->rectOnPage);
            }
        }

        // draw photo
        if(pcSet->photo != nullptr){
            if(pcSet->photo->rectOnPage.width() > 0 && pcSet->photo->rectOnPage.height() > 0){ // ############################ costly

                pcSet->photo->draw(painter, pcSet->photo->rectOnPage, infos, pcPage->rectOnPage.size());
            }
        }

        if(pcSet->consignPositionFromPhoto == Position::on){
            // draw consign
            if(pcSet->consign->rectOnPage.width()> 0 && pcSet->consign->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, Drawing::format_html_for_generation(*pcSet->consign->html, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->consign->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->consign->rectOnPage);
            }
        }

        if(!infos.preview){
            emit set_progress_bar_state_signal(static_cast<int>(1000. * pcSet->totalId/m_totalPC));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }
    }

    // header
    if(pcPage->header->settings.enabled){

        // text
        if(pcPage->header->rectOnPage.height() > 0){
            painter.setOpacity(1.);
            infos.photoNum      = 0;
            infos.photoPCNum    = 0;

            if(!infos.preview){
                emit set_progress_bar_text_signal("Dessin haut de page");
                QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
            }

            draw_html(painter, Drawing::format_html_for_generation(*pcPage->header->settings.text, infos),
                      QRectF(pcPage->header->rectOnPage.x(),        pcPage->header->rectOnPage.y(),
                             pcPage->header->rectOnPage.width(),    pcPage->rectOnPage.height()),
                      pcPage->header->rectOnPage);
        }
    }

    // footer
    if(pcPage->footer->settings.enabled){

        // text
        painter.setOpacity(1.);
        infos.photoNum      = 0;
        infos.photoPCNum    = 0;

        if(!infos.preview){
            emit set_progress_bar_text_signal("Dessin base de page");
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }

        draw_html(painter, Drawing::format_html_for_generation(*pcPage->footer->settings.text, infos),
                  QRectF(pcPage->footer->rectOnPage.x(),        pcPage->footer->rectOnPage.y(),
                         pcPage->footer->rectOnPage.width(),    pcPage->rectOnPage.height()),
                  pcPage->footer->rectOnPage);
    }
}


void PDFGeneratorWorker::draw_page(QPainter &painter, pc::PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones){

    SPCPage pcPage = pcPages.pages[idPageToDraw];

    ExtraPCInfo infos;
    infos.pagesNb       = pcPages.pages.size();
    infos.pageNum       = idPageToDraw;
    infos.pageColor     = pcPage->backgroundSettings.color;
    infos.photoNum      = 0;
    infos.photoPCNum    = 0;
    infos.preview       = preview;
    infos.paperFormat   = pcPages.paperFormat;
    infos.factorUpscale = factorUpscale;
    for(auto &&page : pcPages.pages){
        infos.photoTotalNum += page->sets.size();
    }


    if(drawZones){
        draw_zones(painter,pcPage);
    }else{
        draw_backgrounds(painter, pcPage, infos);
    }

    draw_contents(painter, pcPage, infos);

    // borders
    for(auto &&set : pcPage->sets){

        painter.setOpacity(1.);
        SPCSet pcSet = set;

        if(pcPage->bordersSettings.display){
            pcPage->bordersSettings.pen.setWidthF(pcPage->bordersSettings.width*factorUpscale);
            painter.setPen(pcPage->bordersSettings.pen);
            painter.drawRect(pcSet->rectOnPage);
        }
    }
}

void PDFGeneratorWorker::draw_html(QPainter &painter, QString html, QRectF upperRect, QRectF docRect){

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
    int dpi = pcPages.paperFormat.dpi;
    if(dpi > 300){
        dpi = 300;
    }

    qreal widthPreview   = pcPages.paperFormat.widthRatio  * dpi;
    qreal heightPreview  = pcPages.paperFormat.heightRatio * dpi;

    // create preview image
    QImage image(static_cast<int>(widthPreview), static_cast<int>(heightPreview), QImage::Format_RGB32);
    QPainter painter(&image);

    m_pageToDraw->compute_sizes(QRectF(0 ,0, widthPreview, heightPreview));

    draw_page(painter, pcPages, pageIdToDraw, 1.*dpi/m_referenceDPI, true, drawZones);

    painter.end();

    if(pcPages.grayScale){
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
    pdfWriter.setColorMode(pcPages.grayScale ? QPrinter::ColorMode::GrayScale : QPrinter::ColorMode::Color);

    pdfWriter.setResolution(pcPages.paperFormat.dpi);
    pdfWriter.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pcPages.paperFormat.format)));
    pdfWriter.setPageOrientation(pcPages.paperFormat.landscape ? QPageLayout::Landscape : QPageLayout::Portrait);
    pdfWriter.setPageMargins(QMarginsF(0.,0.,0.,0.));

    qDebug() << "pcPages.paperFormat.landscape: " << pcPages.paperFormat.landscape;
    // init painter
    QPainter pdfPainter;
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

        qreal factor = 1.*pcPages.paperFormat.dpi/m_referenceDPI;
        draw_page(pdfPainter, pcPages, ii, factor, false, false);

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }

    // end pdf writing
    pdfPainter.end();

    emit set_progress_bar_state_signal(1000);
    emit end_generation_signal(true);
}

void PDFGeneratorWorker::update_PC_selection_with_id(int idSet){

//    qDebug() << "update_PC_selection_with_id: " << idSet << " " << m_pageToDraw->sets.size();
//    QRectF rectPage = m_pageToDraw.get()->rectOnPage;

//    SPCSet set = m_pageToDraw->sets[idSet];
//    QRectF pcRectRelative = QRectF(set->rectOnPage.x()/rectPage.width(), set->rectOnPage.y()/rectPage.height(),
//                            set->rectOnPage.width()/rectPage.width(), set->rectOnPage.height()/rectPage.height());
//    emit current_pc_selected_signal(pcRectRelative, idSet);
}

void PDFGeneratorWorker::update_PC_selection_with_pos(QPointF pos){

//    QRectF rectPage = m_pageToDraw->rectOnPage;
//    QPointF realPos(rectPage.x() + pos.x()*rectPage.width(), rectPage.y() + pos.y()*rectPage.height());

//    int id = -1;
//    QRectF pcRectRelative;
//    for(auto &&set : m_pageToDraw->sets){
//        if(set->rectOnPage.contains(realPos)){
//            if(set->totalId > id){
//                id = set->totalId;
//                pcRectRelative = QRectF(set->rectOnPage.x()/rectPage.width(), set->rectOnPage.y()/rectPage.height(),
//                                      set->rectOnPage.width()/rectPage.width(), set->rectOnPage.height()/rectPage.height());
//            }
//        }
//    }
//    if(id > -1){
//        emit current_pc_selected_signal(pcRectRelative, id);
//    }

//    if(m_pageToDraw->header->settings.enabled){
//        if(m_pageToDraw->header->rectOnPage.contains(realPos)){

//            QRectF pcRectRelavive(m_pageToDraw->header->rectOnPage.x()/rectPage.width(), m_pageToDraw->header->rectOnPage.y()/rectPage.height(),
//                                  m_pageToDraw->header->rectOnPage.width()/rectPage.width(), m_pageToDraw->header->rectOnPage.height()/rectPage.height());
//            emit current_pc_selected_signal(pcRectRelavive, -1);
//        }
//    }

//    if(m_pageToDraw->footer->settings.enabled){
//        if(m_pageToDraw->footer->rectOnPage.contains(realPos)){

//            QRectF pcRectRelavive(m_pageToDraw->footer->rectOnPage.x()/rectPage.width(), m_pageToDraw->footer->rectOnPage.y()/rectPage.height(),
//                                  m_pageToDraw->footer->rectOnPage.width()/rectPage.width(), m_pageToDraw->footer->rectOnPage.height()/rectPage.height());
//            emit current_pc_selected_signal(pcRectRelavive, -2);
//        }
//    }
}

void PDFGeneratorWorker::init_document(){
    m_doc = std::make_unique<QTextDocument>();
}

void PDFGeneratorWorker::add_resource(QUrl url, QImage image){
    m_doc->addResource(QTextDocument::ImageResource, std::move(url), std::move(image));
}


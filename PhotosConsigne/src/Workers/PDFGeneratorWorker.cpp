

#include "PDFGeneratorWorker.hpp"

void pc::PDFGeneratorWorker::draw_page(QPainter &painter, pc::PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones)
{
    SPCPage pcPage = pcPages.pages[idPageToDraw];

    // background color
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_pageToDraw->backgroundSettings.color);
    painter.fillRect(pcPage->rectOnPage,brush);

    // bakground image
    if(pcPage->backgroundSettings.displayPhoto){
        if(pcPage->backgroundSettings.photo != nullptr){
            pcPage->backgroundSettings.photo->draw(painter, pcPage->rectOnPage, false, false, pcPages.paperFormat.sizeMM, pcPage->rectOnPage.size());
        }
    }

    // background pattern
    if(pcPage->backgroundSettings.displayPattern){

        qreal widthPattern = m_referenceDPI* ((pcPage->orientation == PageOrientation::portrait) ? pcPages.paperFormat.widthRatio : pcPages.paperFormat.heightRatio);
        qreal heightPattern = m_referenceDPI* ((pcPage->orientation == PageOrientation::portrait) ? pcPages.paperFormat.heightRatio : pcPages.paperFormat.widthRatio);
        QRectF patternRect(0., 0., widthPattern, heightPattern);

        QImage patternImage(static_cast<int>(patternRect.width()),static_cast<int>(patternRect.height()), QImage::Format_ARGB32);
        QPainter patternPainter(&patternImage);
        brush.setStyle(pcPage->backgroundSettings.patternBrushStyle);
        brush.setColor(pcPage->backgroundSettings.colorPattern);
        patternPainter.fillRect(patternRect, brush);
        patternPainter.end();

        Photo patternPhoto(std::move(patternImage));
        patternPhoto.draw(painter, pcPage->rectOnPage, false);
    }

    if(drawZones){

        // zones externs margins + sets
        painter.setOpacity(0.3);
        if(pcPage->setsAndMarginsRect.width() > 0){
            painter.fillRect(pcPage->setsAndMarginsRect, QRgb(qRgb(0,255,0)));
        }

        // zones sets
        painter.setOpacity(1.);
        if(pcPage->setsRect.width() > 0){
            painter.fillRect(pcPage->setsRect, QRgb(qRgb(255,255,255)));
        }

        for(auto &&set : pcPage->sets){

            SPCSet pcSet = set;
            if(pcPage->interMarginsRects[pcSet->id].width() > 0){
                painter.setOpacity(0.3);
                painter.fillRect(pcPage->interMarginsRects[pcSet->id], QRgb(qRgb(200,0,0)));
            }

            if(pcSet->rectOnPage.width() > 0){
                painter.setOpacity(1.);
                painter.fillRect(pcSet->rectOnPage, QRgb(qRgb(255,255,255)));
            }

            // draw photo
            if(pcSet->photo->rectOnPage.width() > 0){
                painter.setOpacity(0.3);
                painter.fillRect(pcSet->photo->rectOnPage, QRgb(qRgb(255,255,0)));
            }
        }
    }

    ExtraPCInfo infos;
    infos.pagesNb    = pcPages.pages.size();
    infos.pageNum    = idPageToDraw;
    infos.pageColor  = pcPage->backgroundSettings.color;
    infos.photoNum   = 0;
    infos.photoPCNum = 0;
    for(auto &&page : pcPages.pages){
        infos.photoTotalNum += page->sets.size();
    }

    // PC
    for(auto &&set : pcPage->sets){

        painter.setOpacity(1.);
        SPCSet pcSet = set;
        infos.photoNum   = pcSet->totalId;
        infos.photoPCNum = pcSet->id;
        infos.namePCAssociatedPhoto = pcSet->photo->namePhoto;
        infos.fileInfo = pcSet->photo->info;

        if(!preview){
            emit set_progress_bar_text_signal("Dessin photo-consigne n°" + QString::number(pcSet->totalId));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }


        if(pcSet->consignPositionFromPhoto != Position::on){
            // draw consign
            if(pcSet->consign->rectOnPage.width()> 0 && pcSet->consign->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, format_html_for_generation(*pcSet->consign->html, factorUpscale, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->consign->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->consign->rectOnPage);
            }
        }

        // draw photo
        if(pcSet->photo != nullptr){
            if(pcSet->photo->rectOnPage.width() > 0 && pcSet->photo->rectOnPage.height() > 0){ // ############################ costly

                pcSet->photo->draw(painter, pcSet->photo->rectOnPage, preview, drawZones, pcPages.paperFormat.sizeMM, pcPage->rectOnPage.size());
            }
        }

        if(pcSet->consignPositionFromPhoto == Position::on){
            // draw consign
            if(pcSet->consign->rectOnPage.width()> 0 && pcSet->consign->rectOnPage.height()>0){ // ############################ costly

                draw_html(painter, format_html_for_generation(*pcSet->consign->html, factorUpscale, infos),
                          QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->consign->rectOnPage.width(), pcPage->rectOnPage.height()),
                          pcSet->consign->rectOnPage);
            }
        }

        if(!preview){
            emit set_progress_bar_state_signal(static_cast<int>(1000. * pcSet->totalId/m_totalPC));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }
    }

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

    // title
    if(pcPage->title != nullptr){
        painter.setOpacity(1.);

        infos.photoNum = 0;
        infos.photoPCNum = 0;

        if(!preview){
            emit set_progress_bar_text_signal("Dessin titre");
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }

        // ############################- costly
        draw_html(painter, format_html_for_generation(*pcPage->title->html, factorUpscale, infos), pcPage->rectOnPage, pcPage->title->rectOnPage);
    }

    if(drawZones){

        // zone title
        infos.photoNum = -1;
        if(pcPage->title != nullptr){
            if(pcPage->titlePositionFromPC != Position::on){
                painter.setOpacity(0.3);
                painter.fillRect(pcPage->title->rectOnPage, QRgb(qRgb(255,127,39)));
            }
        }

        // zones PC
        for(auto &&set : pcPage->sets){

            SPCSet pcSet = set;
            // draw PC
            if(pcSet->consignPositionFromPhoto != Position::on){
                if(pcSet->consign->rectOnPage.width() > 0 && pcSet->consign->rectOnPage.height() > 0){
                    painter.setOpacity(0.3);
                    painter.fillRect(pcSet->consign->rectOnPage, QRgb(qRgb(0,0,200)));
                }
            }
        }
    }
}

void pc::PDFGeneratorWorker::draw_html(QPainter &painter, QString html, QRectF upperRect, QRectF docRect){

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

void pc::PDFGeneratorWorker::kill(){

    qDebug() << "pc::PDFGeneratorWorker::kill()";
    m_locker.lockForWrite();
    m_continueLoop = false;
    m_locker.unlock();
    qDebug() << "end pc::PDFGeneratorWorker::kill()";
}

void pc::PDFGeneratorWorker::generate_preview(pc::PCPages pcPages, int pageIdToDraw, bool drawZones){

    //        DebugMessage dbgMess("generate_preview");
    //        QElapsedTimer timer;
    //        timer.start();

    m_pageToDraw    = pcPages.pages[pageIdToDraw];
    bool landScape  = m_pageToDraw->orientation == PageOrientation::landScape;

    int dpi = pcPages.paperFormat.dpi;
    if(dpi > 150){
        dpi = 150;
    }

    qreal widthPreview   = landScape ? pcPages.paperFormat.heightRatio*dpi : pcPages.paperFormat.widthRatio  * dpi;
    qreal heightPreview  = landScape ? pcPages.paperFormat.widthRatio *dpi : pcPages.paperFormat.heightRatio * dpi;

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

    emit end_preview_signal(image);
}

void pc::PDFGeneratorWorker::generate_PDF(pc::PCPages pcPages){

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

    pdfWriter.setPageMargins(QMarginsF(0.,0.,0.,0.));
    pdfWriter.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pcPages.paperFormat.format)));

    // define dpi resolution
    pdfWriter.setResolution(pcPages.paperFormat.dpi);

    // init painter
    QPainter pdfPainter;
    if(!pdfPainter.begin(&pdfWriter)){
        qWarning() << "-Error: can't write on file: " << pcPages.pdfFileName << ", file may not exists";

        emit abort_pdf_signal(pcPages.pdfFileName);
        return;
    }

    pcPages.compute_all_pages_sizes(pdfWriter.width(), pdfWriter.height());
    for(int ii = 0; ii < pcPages.pages.size(); ++ii){

        bool landScape = pcPages.pages[ii]->orientation == PageOrientation::landScape;
        pdfWriter.setPageOrientation(landScape ? QPageLayout::Landscape : QPageLayout::Portrait);

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

void pc::PDFGeneratorWorker::update_PC_selection(QPointF pos){

    QRectF rectPage = m_pageToDraw.get()->rectOnPage;
    QPointF realPos(rectPage.x() + pos.x()*rectPage.width(), rectPage.y() + pos.y()*rectPage.height());
    for(auto &&set : m_pageToDraw->sets){
        if(set->rectOnPage.contains(realPos)){
            QRectF pcRectRelavive(set->rectOnPage.x()/rectPage.width(), set->rectOnPage.y()/rectPage.height(),
                                  set->rectOnPage.width()/rectPage.width(), set->rectOnPage.height()/rectPage.height());
            emit current_pc_selected_signal(pcRectRelavive, set->totalId);
            return;
        }
    }

    if(m_pageToDraw->title != nullptr){
        if(m_pageToDraw->title->rectOnPage.contains(realPos)){

            QRectF pcRectRelavive(m_pageToDraw->title->rectOnPage.x()/rectPage.width(), m_pageToDraw->title->rectOnPage.y()/rectPage.height(),
                                  m_pageToDraw->title->rectOnPage.width()/rectPage.width(), m_pageToDraw->title->rectOnPage.height()/rectPage.height());
            emit current_pc_selected_signal(pcRectRelavive, -1);
        }
    }
}

void pc::PDFGeneratorWorker::init_document(){
    m_doc = std::make_unique<QTextDocument>();
}

void pc::PDFGeneratorWorker::add_resource(QUrl url, QImage image){
    m_doc->addResource(QTextDocument::ImageResource, std::move(url), std::move(image));
}

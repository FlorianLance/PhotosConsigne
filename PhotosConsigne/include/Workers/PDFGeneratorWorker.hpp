
#pragma once

// local
#include "Utility.hpp"

// Qt
#include <QPrinter>

namespace pc{

class PDFGeneratorWorker : public QObject {

    Q_OBJECT

public :

    PDFGeneratorWorker(){}

    virtual ~PDFGeneratorWorker(){}

    void draw_page(QPainter &painter, GlobalData settings, PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones)
    {
        SPCPage pcPage = pcPages.pages[idPageToDraw];

        // draw cut lines
        if(pcPage->displayCutLines){

            painter.setOpacity(1);
            QPen pen;
            pen.setStyle(Qt::DashLine);
            pen.setColor(Qt::darkGray);
            pen.setWidthF((pcPage->rectOnPage.width()+pcPage->rectOnPage.height())*0.5/1000.);
            painter.setPen(pen);
            for(auto &&hLine : pcPage->horizontalCutLines){
                painter.drawLine(hLine);
            }
            for(auto &&vLine : pcPage->verticalCutLines){
                painter.drawLine(vLine);
            }
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
                if(pcSet->photo->rectOnPage.width()){
                    painter.setOpacity(0.3);
                    painter.fillRect(pcSet->photo->rectOnPage, QRgb(qRgb(255,255,0)));
                }
            }
        }

        ExtraPCInfo infos;
        infos.pagesNb    = pcPages.pages.size();
        infos.pageNum    = idPageToDraw;
        infos.pageColor  = pcPage->backgroundColor;
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


            if(!pcSet->consignOnPhoto){
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
                    painter.fillRect(pcSet->photo->rectOnPage, pcPage->backgroundColor);
                    pcSet->photo->draw(painter, pcSet->photo->rectOnPage, preview, settings.displayZones, pcPages.paperFormat, pcPage->rectOnPage);
                }
            }

            if(pcSet->consignOnPhoto){
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
                if(!pcSet->consignOnPhoto){
                    if(pcSet->consign->rectOnPage.width() > 0 && pcSet->consign->rectOnPage.height() > 0){
                        painter.setOpacity(0.3);
                        painter.fillRect(pcSet->consign->rectOnPage, QRgb(qRgb(0,0,200)));
                    }
                }
            }
        }
    }

    void draw_html(QPainter &painter, QString html, QRectF upperRect, QRectF docRect){

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


public slots :

    void kill()
    {
        m_locker.lockForWrite();
            m_continueLoop = false;
        m_locker.unlock();
    }

    void generate_preview(GlobalData settings, PCPages pcPages)
    {
//        QElapsedTimer timer;
//        timer.start();


        m_pageToDraw = pcPages.pages[settings.currentPageId];
        bool landScape     = settings.orientation == PageOrientation::landScape;


        int dpi = pcPages.paperFormat.dpi;
        if(dpi > 150){
            dpi = 150;
        }

        qreal widthPreview   = landScape ? pcPages.paperFormat.heightRatio*dpi : pcPages.paperFormat.widthRatio  * dpi;
        qreal heightPreview  = landScape ? pcPages.paperFormat.widthRatio *dpi : pcPages.paperFormat.heightRatio * dpi;

        // create preview image
        QImage image(static_cast<int>(widthPreview), static_cast<int>(heightPreview), QImage::Format_RGB32);

        // fill background
        image.fill(m_pageToDraw->backgroundColor);

        QPainter painter(&image);
        m_pageToDraw->compute_sizes(QRectF(0 ,0, widthPreview, heightPreview));

//        qDebug() << "Before draw page" << timer.elapsed() << "ms";
        draw_page(painter, settings, pcPages, settings.currentPageId, 1.*dpi/m_referenceDPI, true, settings.displayZones);
//        qDebug() << "After draw page" << timer.elapsed() << "ms";
        painter.end();


        if(settings.grayScale){
            for (int ii = 0; ii < image.height(); ii++) {
                QRgb *pixel = reinterpret_cast<QRgb*>(image.scanLine(ii));
                QRgb *end = pixel + image.width();
                for (; pixel != end; pixel++) {
                    int gray = qGray(*pixel);
                    *pixel = QColor(gray, gray, gray).rgb();
                }
            }
        }

        QImage pageImage(static_cast<int>(widthPreview), static_cast<int>(heightPreview), QImage::Format_RGB32);
//        QImage pageImage(static_cast<int>(widthPreview+2), static_cast<int>(heightPreview+2), QImage::Format_RGB32);
//        pageImage.fill(Qt::black);
        QPainter painterPage(&pageImage);
//        painterPage.drawImage(1,1,image);
        painterPage.drawImage(0,0,image);
        painterPage.end();

//        qDebug() << "End generate preview" << timer.elapsed() << "ms";
        emit end_preview_signal(pageImage);
    }

    void generate_PDF(GlobalData settings, PCPages pcPages)
    {
        m_totalPC = settings.photosValided->size();
        emit set_progress_bar_state_signal(0);

        QPrinter pdfWriter(QPrinter::HighResolution);
        pdfWriter.setOutputFormat(QPrinter::PdfFormat);
        pdfWriter.setOutputFileName(pcPages.pdfFileName);
        pdfWriter.setCreator("created with PhotosConsigne (https://github.com/FlorianLance/PhotosConsigne)");
        pdfWriter.setColorMode(settings.grayScale ? QPrinter::ColorMode::GrayScale : QPrinter::ColorMode::Color);

        bool landScape = settings.orientation == PageOrientation::landScape;
        QPageLayout pageLayout(QPageSize(static_cast<QPageSize::PageSizeId>(pcPages.paperFormat.format)),
                               landScape ? QPageLayout::Landscape : QPageLayout::Portrait, QMarginsF(0.,0.,0.,0.));
        pdfWriter.setPageLayout(pageLayout);

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

            if(settings.saveOnlyCurrentPage){
                if(settings.currentPageId != ii){
                    continue;
                }
            }

            bool continueLoop;
            m_locker.lockForRead();
            continueLoop = m_continueLoop;
            m_locker.unlock();

            if(!continueLoop)
                return;

            emit set_progress_bar_text_signal("Création page " + QString::number(ii));

            pdfPainter.fillRect(pcPages.pages[ii]->rectOnPage, pcPages.pages[ii]->backgroundColor);
            qreal factor = 1.*pcPages.paperFormat.dpi/m_referenceDPI;
            draw_page(pdfPainter, settings, pcPages, ii, factor, false, false);

            if(ii < pcPages.pages.size()-1 && !settings.saveOnlyCurrentPage){
                pdfWriter.newPage();
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        }

        // end pdf writing
        pdfPainter.end();

        emit set_progress_bar_state_signal(1000);
        emit end_generation_signal(true);
    }


    void update_PC_selection(QPointF pos){

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

    void init_document(){
        m_doc = std::make_unique<QTextDocument>();
    }

    void add_resource(QUrl url, QImage image){
        m_doc->addResource(QTextDocument::ImageResource, std::move(url), std::move(image));
    }

signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void end_preview_signal(QImage preview);

    void end_generation_signal(bool finished);

    void abort_pdf_signal(QString pathPDF);

    void current_pc_selected_signal(QRectF pcRectRelative, int totalIdPC);


private :

    bool m_continueLoop = true;
    const int m_referenceDPI  = 100;
    int m_totalPC = 0;
    QReadWriteLock m_locker;
    SPCPage m_pageToDraw = nullptr;

    std::unique_ptr<QTextDocument> m_doc = nullptr;
};}

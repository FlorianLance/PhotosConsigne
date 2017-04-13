
/**
 * \file PCMainWorker.hpp
 * \brief defines PhotoDisplayWorker,PDFGeneratorWorker
 * \author Florian Lance
 * \date 04/04/2017
 */

#pragma once

// Qt
#include <QObject>
#include <QPixmap>
#include <QPdfWriter>
#include <QPainter>
#include <QPrinter>
#include <QMessageBox>
#include <QCoreApplication>

// local
#include "Utility.h"


namespace pc {

class PhotoDisplayWorker : public QObject
{
    Q_OBJECT

public :

    PhotoDisplayWorker()
    {
        qRegisterMetaType<SPhotos>("SPhotos");
        qRegisterMetaType<GlobalParameters>("GlobalParameters");
        qRegisterMetaType<PCPages>("PCPages");
        qRegisterMetaType<PCPage>("PCPage");
        qRegisterMetaType<QVector<QRectF>>("QVector<QRectF>");
    }

    ~PhotoDisplayWorker(){}

public slots :

    void load_photos(QString path, QStringList photosList)
    {
        emit set_progress_bar_state_signal(0);
        emit set_progress_bar_text_signal("Chargement des photos...");

        auto nbPhotos = photosList.size();

        int idPhoto = 0;
        qreal offset = 1000 / nbPhotos;
        qreal currentState = 0;

        m_photosLoaded = std::make_shared<QList<SPhoto>>();
        m_photosLoaded->reserve(nbPhotos);
        for(auto &&imageName : photosList)
        {
            bool continueLoop;
            m_locker.lockForRead();
            continueLoop = m_continueLoop;
            m_locker.unlock();

            if(!continueLoop)
                break;

            emit set_progress_bar_text_signal("Chargement de " + imageName);

            SPhoto photo = std::make_shared<Photo>(Photo(path + "/" + imageName));
            if(!photo->scaledPhoto.isNull()){
                m_photosLoaded->push_back(photo);
                emit photo_loaded_signal(imageName);
            }
            else{
                emit set_progress_bar_text_signal("Echec chargement photo n°" + QString::number(idPhoto));
            }

            currentState += offset;
            emit set_progress_bar_state_signal(static_cast<int>(currentState));
            idPhoto++;
        }

        emit set_progress_bar_state_signal(1000);
        emit set_progress_bar_text_signal("Photos chargées.");
        emit end_loading_photos_signal(m_photosLoaded);
    }

    void kill()
    {
        m_locker.lockForWrite();
        m_continueLoop = false;
        m_locker.unlock();
    }

signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void end_loading_photos_signal(SPhotos photos);

    void photo_loaded_signal(QString photoName);

private :

    SPhotos m_photosLoaded = nullptr;

    QReadWriteLock m_locker;
    bool m_continueLoop = true;
};

class PDFGeneratorWorker : public QObject
{
    Q_OBJECT

public :

    PDFGeneratorWorker()
    {}

    void draw_page(QPainter &painter, PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones)
    {
        SPCPage pcPage = pcPages.pages[idPageToDraw];
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

            emit set_progress_bar_text_signal("Dessin photo-consigne n°" + QString::number(pcSet->totalId));
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

            // draw photo            
            if(pcSet->photo->rectOnPage.width()){
                pcSet->photo->draw(painter, pcSet->photo->rectOnPage, preview);
            }            

            // draw consign
            if(pcSet->consign->rectOnPage.width()){
                draw_doc_html_with_size_factor(painter, *pcSet->consign->doc, QRectF(pcSet->rectOnPage.x(),pcSet->rectOnPage.y(),pcSet->consign->rectOnPage.width() , pcPage->rectOnPage.height()), pcSet->consign->rectOnPage, factorUpscale, infos);
            }

            if(!preview){
                emit set_progress_bar_state_signal(1000. * pcSet->totalId/m_totalPC);
                QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            }
        }

        // title
        if(pcPage->title != nullptr){
            painter.setOpacity(1.);
            emit set_progress_bar_text_signal("Dessin titre");
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            draw_doc_html_with_size_factor(painter, *pcPage->title->doc, pcPage->rectOnPage, pcPage->title->rectOnPage, factorUpscale, infos);
        }


        if(drawZones){

            // zone title
            infos.photoNum = -1;
            if(pcPage->title != nullptr){
                painter.setOpacity(0.3);
                painter.fillRect(pcPage->title->rectOnPage, QRgb(qRgb(255,127,39)));
            }

            // zones PC
            for(auto &&set : pcPage->sets){

                SPCSet pcSet = set;                
                // draw PC
                if(!pcSet->consignOnPhoto){
                    if(pcSet->consign->rectOnPage.width()){
                        painter.setOpacity(0.3);
                        painter.fillRect(pcSet->consign->rectOnPage, QRgb(qRgb(0,0,200)));
                    }
                }
            }
        }

        // draw cut lines
        if(pcPage->displayCutLines){

            painter.setOpacity(1);
            QPen pen;
            pen.setStyle(Qt::DashLine);
            pen.setColor(Qt::darkGray);
            pen.setWidthF((pcPage->rectOnPage.width()+pcPage->rectOnPage.height())*0.5/500.);
            painter.setPen(pen);
            for(auto &&hLine : pcPage->horizontalCutLines){
                painter.drawLine(hLine);
            }
            for(auto &&vLine : pcPage->verticalCutLines){
                painter.drawLine(vLine);
            }
        }

        return;

//        if(preview)
//            emit send_PC_rects_signal(pcRects, page.id);
    }

    ~PDFGeneratorWorker()
    {}

public slots :

    void kill()
    {
        m_locker.lockForWrite();
            m_continueLoop = false;
        m_locker.unlock();
    }

    void generate_preview(GlobalParameters settings, PCPages pcPages)
    {
        SPCPage pageToDraw = pcPages.pages[settings.currentPageId];
        bool landScape     = settings.globalOrientation == PageOrientation::landScape;
        qreal widthPreview   = landScape ? pcPages.paperFormat.heightRatio*m_previewDPI : pcPages.paperFormat.widthRatio  * m_previewDPI;
        qreal heightPreview  = landScape ? pcPages.paperFormat.widthRatio *m_previewDPI : pcPages.paperFormat.heightRatio * m_previewDPI;

        // create preview image
        QImage image(widthPreview, heightPreview, QImage::Format_RGB32);
        // fill background
        image.fill(Qt::GlobalColor::white);

        QPainter painter(&image);
        pageToDraw->compute_sizes(QRectF(0 ,0, widthPreview, heightPreview));
        draw_page(painter, pcPages, settings.currentPageId, 1., true, settings.displayZones);

        painter.end();

        emit end_preview_signal(image);
    }

    void generate_PDF(GlobalParameters settings, PCPages pcPages)
    {
        m_totalPC = settings.photosValided->size();
        emit set_progress_bar_state_signal(0);        

        QPrinter pdfWriter(QPrinter::PrinterResolution);
        pdfWriter.setOutputFormat(QPrinter::PdfFormat);
        pdfWriter.setOutputFileName(pcPages.pdfFileName);
//        pdfWriter.setColorMode(QPrinter::ColorMode::GrayScale);

        bool landScape = settings.globalOrientation == PageOrientation::landScape;
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

            bool continueLoop;
            m_locker.lockForRead();
            continueLoop = m_continueLoop;
            m_locker.unlock();

            if(!continueLoop)
                return;

            emit set_progress_bar_text_signal("Création page " + QString::number(ii));

            draw_page(pdfPainter, pcPages, ii, pcPages.paperFormat.dpi/m_previewDPI, false, false);

            if(ii < pcPages.pages.size()-1)
                pdfWriter.newPage();                

            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        // end pdf writing
        pdfPainter.end();

        emit set_progress_bar_state_signal(1000);
        emit end_generation_signal(true);
    }


signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void end_preview_signal(QImage preview);

    void end_generation_signal(bool finished);

    void send_PC_rects_signal(QVector<QRectF> pcRects, int currentPageId);

    void abort_pdf_signal(QString pathPDF);

    void generate_again_signal();


private :

    QReadWriteLock m_locker;
    bool m_continueLoop = true;

    const int m_previewDPI  = 100;
    int m_totalPC = 0;
};
}

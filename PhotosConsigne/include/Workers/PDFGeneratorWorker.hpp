
#pragma once

// local
#include "Utility.hpp"
#include "DocumentElements.hpp"

// Qt
#include <QPrinter>
#include <QUrl>
#include <QTextDocument>

namespace pc{

class PDFGeneratorWorker : public QObject {

    Q_OBJECT

public :

    PDFGeneratorWorker(){}

    virtual ~PDFGeneratorWorker();

    void draw_page(QPainter &painter, PCPages &pcPages, const int idPageToDraw, const qreal factorUpscale, const bool preview, const bool drawZones);

    void draw_html(QPainter &painter, QString html, QRectF upperRect, QRectF docRect);


public slots :

    void kill();

    void generate_preview(PCPages pcPages, int pageIdToDraw, bool drawZones);

    void generate_PDF(PCPages pcPages);

    void init_document();

    void add_resource(QUrl url, QImage image);

signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void end_preview_signal(QImage preview, SPCPage previewPage);

    void end_generation_signal(bool finished);

    void abort_pdf_signal(QString pathPDF);

    void current_pc_selected_signal(QRectF pcRectRelative, int totalIdPC);

private :

    void draw_zones(QPainter &painter, SPCPage pcPage);

    void draw_backgrounds(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos);

    void draw_contents(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos);


private :

    bool m_continueLoop = true;
    const int m_referenceDPI  = 100;
    int m_totalPC = 0;
    QReadWriteLock m_locker;
    SPCPage m_pageToDraw = nullptr;

    std::unique_ptr<QTextDocument> m_doc = nullptr;
};}

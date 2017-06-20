/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/


#pragma once

/**
 * \file PDFGeneratorWorker.hpp
 * \brief defines PDFGeneratorWorker
 * \author Florian Lance
 * \date 17/05/2017
 */


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

    void draw_degraded(QPainter &painter, const QRectF &rectPage, const ColorsSettings &colors, const ExtraPCInfo &infos);

    void draw_backgrounds(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos);

    void draw_contents(QPainter &painter, SPCPage pcPage, ExtraPCInfo &infos);


private :

    bool m_continueLoop = true;
    const int m_referenceDPI  = 100;
    int m_totalPC = 0;
    QReadWriteLock m_locker;
    SPCPage m_pageToDraw = nullptr;

    std::unique_ptr<QTextDocument> m_doc = nullptr;

public :
    QVector<QImage> droppedImages;
    QVector<QUrl> droppedUrl;

    QVector<QImage> insertedImages;
    QVector<QUrl> insertedUrl;
};}

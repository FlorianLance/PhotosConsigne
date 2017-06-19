
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

// local
#include "Utility.hpp"

using namespace pc;

QString Drawing::format_html_for_generation(QString html, pc::ExtraPCInfo infos){

//    QElapsedTimer timer;
//    timer.start();

    int index = 0;
//    qDebug() << "HTML BEFORE\n " << html;
    html = html.replace("margin-top:12px; margin-bottom:12px", "margin-top:0px; margin-bottom:0px margin-left:0px margin-right:0px padding:0px");
    html = html.replace("style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\"",
                        "B_#B_#B_#B_");
    QVector<qreal> sizes;
    while (index != -1){
        index = html.indexOf(QString("font-size:"));
        if(index == -1)
            break;

        int indexEndImg = html.indexOf("pt;", index)+3;
        QString subString = html.mid(index, indexEndImg - index);
        qreal sizePolice = (subString.mid(10, subString.size()-13)).toDouble();

        qreal newPoliceSize = (sizePolice*infos.factorUpscale);
        sizes.push_back(newPoliceSize);
        html = html.remove(index, indexEndImg - index);
        html = html.insert(index, "#F_#F_#F_#F_" + QString::number(newPoliceSize)  + "_#F_#F_#F_#F");
    }

    index = 0;
    int currentIdSize = 0;
    while(index != -1){
        index = html.indexOf(QString("#F_#F_#F_#F_"));
        if(index == -1)
            break;
        int indexEndImg = html.indexOf("_#F_#F_#F_#F", index)+12;
        html = html.remove(index, indexEndImg - index);
        html = html.insert(index, "font-size:" + QString::number(sizes[currentIdSize++])  + "pt;");
    }

    html = html.replace("B_#B_#B_#B_","style=\"font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:300; font-style:normal;\"");

    // text-decoration:line-through;  // text-decoration:overline ; oblique // tables
    html = html.replace("$nom_photo$", "$name_photo$");
    html = html.replace("$nom_page$", "$name_page$");
    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$name_photo$"));
        if(index == -1)
            break;
        html = html.remove(index, 12);
        html = html.insert(index, infos.namePCAssociatedPhoto);
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$date$"));
        if(index == -1)
            break;
        html = html.remove(index, 6);
        html = html.insert(index, QDate::currentDate().toString("dd/MM/yyyy"));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$date_photo$"));
        if(index == -1)
            break;
        html = html.remove(index, 12);
        html = html.insert(index, infos.fileInfo.lastModified().toString("dd/MM/yyyy"));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$num_page$"));
        if(index == -1)
            break;
        html = html.remove(index, 10);
        html = html.insert(index, QString::number(infos.pageNum+1));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$num_photo$"));
        if(index == -1)
            break;
        html = html.remove(index, 11);
        html = html.insert(index, QString::number(infos.photoNum+1) + "/" + QString::number(infos.photoTotalNum+1));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$nb_photos$"));
        if(index == -1)
            break;
        html = html.remove(index, 11);
        html = html.insert(index, QString::number(infos.photoTotalNum+1));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$nb_pages$"));
        if(index == -1)
            break;
        html = html.remove(index, 10);
        html = html.insert(index, QString::number(infos.pagesNb));
    }

    index = 0;
    while(index != -1){
        index = html.indexOf(QString("$name_page$"));
        if(index == -1)
            break;
        html = html.remove(index, 11);
        html = html.insert(index, infos.pageName);
    }


    index = 0;
    int currentImage = 0;
    QVector<QString> newImages;
    while(index != -1){
        index = html.indexOf(QString("<img src="));
        if(index == -1)
            break;

        int indexEndImg = html.indexOf("/>", index)+2;
        QString subString = html.mid(index, indexEndImg - index);

        int indexWidth = subString.indexOf("width=");
        int indexheight = subString.indexOf("height=");
        int indexEndHeight = subString.size()-3;
        int indexEndWidth = indexheight- 1;

        QString heightAll = subString.mid(indexheight, indexEndHeight - indexheight);
        QString onlyHeight = heightAll.mid(8);
        onlyHeight.resize(onlyHeight.size()-1);
        QString widthAll = subString.mid(indexWidth, indexEndWidth - indexWidth);
        QString onlyWidth= widthAll.mid(7);
        onlyWidth.resize(onlyWidth.size()-1);

        html = html.remove(index, indexEndImg - index);
        html = html.insert(index, "#I_#I_#I_#I_" + QString::number(currentImage++)  + "_#I_#I_#I_#I");


        newImages.push_back("<img src=" + subString.mid(9, indexWidth-10)
                            + " width=\""    + QString::number(infos.factorUpscale * onlyWidth.toDouble())
                            + "\" height=\"" + QString::number(infos.factorUpscale * onlyHeight.toDouble())+ "\" />");
    }

    index = 0;
    currentImage = 0;
    while(index != -1){

        index = html.indexOf(QRegExp("#I_#I_#I_#I_"));
        if(index == -1)
            break;

        int indexEndImg = html.indexOf("_#I_#I_#I_#I", index)+12;
        html = html.remove(index, indexEndImg - index);
        html = html.insert(index, newImages[currentImage++]);
    }

    //        qDebug() << html;
//    qDebug() << "Format html took" << timer.elapsed() << "milliseconds";
//    qDebug() << "HTML AFTER\n " << html << "\n";
    return html;
}

void Drawing::draw_filled_rect(QPainter &painter, const QRectF &rect, QRgb color, qreal opacity){
    if(rect.width() > 0 && rect.height() > 0){
        painter.setOpacity(opacity);
        painter.fillRect(rect, color);
    }
}

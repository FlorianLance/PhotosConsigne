/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file main.cpp
 * \brief entry point
 * \author Florian Lance
 * \date 01/11/15
 */


// Qt
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDesktopWidget>


// Photos consigne
//#include "MainInterface.h"
#include "MainUI.h"
#include "PCMainUI.hpp"

int main(int argc,char** argv)
{


    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);
    app.installTranslator(&qtTranslator);

    QImage image(25,25, QImage::Format_ARGB32);
    QImage image2;
    QPainter painter(&image);

    QPen pen;
    QBrush brush;

    brush.setColor(qRgba(127,180,255,255));
    brush.setStyle(Qt::SolidPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/SolidPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense1Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense1Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense2Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense2Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense3Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense3Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense4Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense4Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense5Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense5Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense6Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense6Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::Dense7Pattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/Dense7Pattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::HorPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/HorPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::VerPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/HorPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::CrossPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/CrossPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::BDiagPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/BDiagPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::FDiagPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/FDiagPattern.png");
    image.fill(qRgba(0,0,0,0));

    brush.setStyle(Qt::DiagCrossPattern);
    painter.fillRect(0,0,image.width(),image.height(),brush);
    image2 = image.scaled(50,50, Qt::KeepAspectRatio);
    image2.save("G:/DiagCrossPattern.png");
    image.fill(qRgba(0,0,0,0));

//    return 0;
//    QPrinter printer(QPrinter::HighResolution);
//    printer.setOutputFormat(QPrinter::PdfFormat);
//    printer.setOutputFileName("out.pdf");

//    QTextDocument document;

//QString html ="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
//    "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
//    "<head>"
//    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
//    "<title>test</title>"
//    "</head>"
//    "<style>"
//    "body"
//    "{"
//    "background-color:#000;"
//    "width:1280px;"
//    "height:720px;"
//    "margin:0px;"
//    "}"
//    ".text"
//    "{"
//    "color:#FFF;"
//    "width:400px;"
//    //"letter-spacing:150px;"
//    "word-spacing:15px;"
//    "margin-left:30px;"
//    "font-size:200px;"
//    "font-weight:200;"
//    "}"
//    "</style>"
//    "<body>"
//    "<br /><br />"
//    "<div class=\"text\">test test test</div>"
//    "</body>"
//    "</html>";

//////    QString html = "<iframe width=\"420\" height=\"315\" src=\"https://www.youtube.com/embed/XGSy3_Czz8k\"></iframe>";


//    document.setHtml(html);

//    document.print( &printer );
//    return 0;




//    QFont font = app.font();
//    font.setPixelSize(11);
//    app.setFont(font);


//    QPdfWriter pdfW("G:/test.pdf");
//    pdfW.setPageSize(QPagedPaintDevice::A4);


//    QVector<QPagedPaintDevice::PageSize> sizesPages {
//        // Existing Qt sizes
//        QPagedPaintDevice::PageSize::A4,
//        QPagedPaintDevice::PageSize::B5,
//        QPagedPaintDevice::PageSize::Letter,
//        QPagedPaintDevice::PageSize::Legal,
//        QPagedPaintDevice::PageSize::Executive,
//        QPagedPaintDevice::PageSize::A0,
//        QPagedPaintDevice::PageSize::A1,
//        QPagedPaintDevice::PageSize::A2,
//        QPagedPaintDevice::PageSize::A3,
//        QPagedPaintDevice::PageSize::A5,
//        QPagedPaintDevice::PageSize::A6,
//        QPagedPaintDevice::PageSize::A7,
//        QPagedPaintDevice::PageSize::A8,
//        QPagedPaintDevice::PageSize::A9,
//        QPagedPaintDevice::PageSize::B0,
//        QPagedPaintDevice::PageSize::B1,
//        QPagedPaintDevice::PageSize::B10,
//        QPagedPaintDevice::PageSize::B2,
//        QPagedPaintDevice::PageSize::B3,
//        QPagedPaintDevice::PageSize::B4,
//        QPagedPaintDevice::PageSize::B6,
//        QPagedPaintDevice::PageSize::B7,
//        QPagedPaintDevice::PageSize::B8,
//        QPagedPaintDevice::PageSize::B9,
//        QPagedPaintDevice::PageSize::C5E,
//        QPagedPaintDevice::PageSize::Comm10E,
//        QPagedPaintDevice::PageSize::DLE,
//        QPagedPaintDevice::PageSize::Folio,
//        QPagedPaintDevice::PageSize::Ledger,
//        QPagedPaintDevice::PageSize::Tabloid,
//        QPagedPaintDevice::PageSize::Custom,
//        QPagedPaintDevice::PageSize::A10,
//        QPagedPaintDevice::PageSize::A3Extra,
//        QPagedPaintDevice::PageSize::A4Extra,
//        QPagedPaintDevice::PageSize::A4Plus,
//        QPagedPaintDevice::PageSize::A4Small,
//        QPagedPaintDevice::PageSize::A5Extra,
//        QPagedPaintDevice::PageSize::B5Extra,
//        QPagedPaintDevice::PageSize::JisB0,
//        QPagedPaintDevice::PageSize::JisB1,
//        QPagedPaintDevice::PageSize::JisB2,
//        QPagedPaintDevice::PageSize::JisB3,
//        QPagedPaintDevice::PageSize::JisB4,
//        QPagedPaintDevice::PageSize::JisB5,
//        QPagedPaintDevice::PageSize::JisB6,
//        QPagedPaintDevice::PageSize::JisB7,
//        QPagedPaintDevice::PageSize::JisB8,
//        QPagedPaintDevice::PageSize::JisB9,
//        QPagedPaintDevice::PageSize::JisB10,
//        QPagedPaintDevice::PageSize::AnsiC,
//        QPagedPaintDevice::PageSize::AnsiD,
//        QPagedPaintDevice::PageSize::AnsiE,
//        QPagedPaintDevice::PageSize::LegalExtra,
//        QPagedPaintDevice::PageSize::LetterExtra,
//        QPagedPaintDevice::PageSize::LetterPlus,
//        QPagedPaintDevice::PageSize::LetterSmall,
//        QPagedPaintDevice::PageSize::TabloidExtra,
//        QPagedPaintDevice::PageSize::ArchA,
//        QPagedPaintDevice::PageSize::ArchB,
//        QPagedPaintDevice::PageSize::ArchC,
//        QPagedPaintDevice::PageSize::ArchD,
//        QPagedPaintDevice::PageSize::ArchE,
//        QPagedPaintDevice::PageSize::Imperial7x9,
//        QPagedPaintDevice::PageSize::Imperial8x10,
//        QPagedPaintDevice::PageSize::Imperial9x11,
//        QPagedPaintDevice::PageSize::Imperial9x12,
//        QPagedPaintDevice::PageSize::Imperial10x11,
//        QPagedPaintDevice::PageSize::Imperial10x13,
//        QPagedPaintDevice::PageSize::Imperial10x14,
//        QPagedPaintDevice::PageSize::Imperial12x11,
//        QPagedPaintDevice::PageSize::Imperial15x11,
//        QPagedPaintDevice::PageSize::ExecutiveStandard,
//        QPagedPaintDevice::PageSize::Note,
//        QPagedPaintDevice::PageSize::Quarto,
//        QPagedPaintDevice::PageSize::Statement,
//        QPagedPaintDevice::PageSize::SuperA,
//        QPagedPaintDevice::PageSize::SuperB,
//        QPagedPaintDevice::PageSize::Postcard,
//        QPagedPaintDevice::PageSize::DoublePostcard,
//        QPagedPaintDevice::PageSize::Prc16K,
//        QPagedPaintDevice::PageSize::Prc32K,
//        QPagedPaintDevice::PageSize::Prc32KBig,
//        QPagedPaintDevice::PageSize::FanFoldUS,
//        QPagedPaintDevice::PageSize::FanFoldGerman,
//        QPagedPaintDevice::PageSize::FanFoldGermanLegal,
//        QPagedPaintDevice::PageSize::EnvelopeB4,
//        QPagedPaintDevice::PageSize::EnvelopeB5,
//        QPagedPaintDevice::PageSize::EnvelopeB6,
//        QPagedPaintDevice::PageSize::EnvelopeC0,
//        QPagedPaintDevice::PageSize::EnvelopeC1,
//        QPagedPaintDevice::PageSize::EnvelopeC2,
//        QPagedPaintDevice::PageSize::EnvelopeC3,
//        QPagedPaintDevice::PageSize::EnvelopeC4,
//        QPagedPaintDevice::PageSize::EnvelopeC6,
//        QPagedPaintDevice::PageSize::EnvelopeC65,
//        QPagedPaintDevice::PageSize::EnvelopeC7,
//        QPagedPaintDevice::PageSize::Envelope9,
//        QPagedPaintDevice::PageSize::Envelope11,
//        QPagedPaintDevice::PageSize::Envelope12,
//        QPagedPaintDevice::PageSize::Envelope14,
//        QPagedPaintDevice::PageSize::EnvelopeMonarch,
//        QPagedPaintDevice::PageSize::EnvelopePersonal,
//        QPagedPaintDevice::PageSize::EnvelopeChou3,
//        QPagedPaintDevice::PageSize::EnvelopeChou4,
//        QPagedPaintDevice::PageSize::EnvelopeInvite,
//        QPagedPaintDevice::PageSize::EnvelopeItalian,
//        QPagedPaintDevice::PageSize::EnvelopeKaku2,
//        QPagedPaintDevice::PageSize::EnvelopeKaku3,
//        QPagedPaintDevice::PageSize::EnvelopePrc1,
//        QPagedPaintDevice::PageSize::EnvelopePrc2,
//        QPagedPaintDevice::PageSize::EnvelopePrc3,
//        QPagedPaintDevice::PageSize::EnvelopePrc4,
//        QPagedPaintDevice::PageSize::EnvelopePrc5,
//        QPagedPaintDevice::PageSize::EnvelopePrc6,
//        QPagedPaintDevice::PageSize::EnvelopePrc7,
//        QPagedPaintDevice::PageSize::EnvelopePrc8,
//        QPagedPaintDevice::PageSize::EnvelopePrc9,
//        QPagedPaintDevice::PageSize::EnvelopePrc10,
//        QPagedPaintDevice::PageSize::EnvelopeYou4,
//        QPagedPaintDevice::PageSize::LastPageSize,
//        QPagedPaintDevice::PageSize::NPageSize,
//        QPagedPaintDevice::PageSize::NPaperSize,
//        QPagedPaintDevice::PageSize::AnsiA,
//        QPagedPaintDevice::PageSize::AnsiB,
//        QPagedPaintDevice::PageSize::EnvelopeC5,
//        QPagedPaintDevice::PageSize::EnvelopeDL,
//        QPagedPaintDevice::PageSize::Envelope10
//    };

//    QVector<QString> sizesPagesStr {
//        // Existing Qt sizes
//        "A4"
//        ,"B5"
//        ,"Letter"
//        ,"Legal"
//        ,"Executive"
//        ,"A0"
//        ,"A1"
//        ,"A2"
//        ,"A3"
//        ,"A5"
//        ,"A6"
//        ,"A7"
//        ,"A8"
//        ,"A9"
//        ,"B0"
//        ,"B1"
//        ,"B10"
//        ,"B2"
//        ,"B3"
//        ,"B4"
//        ,"B6"
//        ,"B7"
//        ,"B8"
//        ,"B9"
//        ,"C5E"
//        ,"Comm10E"
//        ,"DLE"
//        ,"Folio"
//        ,"Ledger"
//        ,"Tabloid"
//        ,"Custom"
//        ,"A10"
//        ,"A3Extra"
//        ,"A4Extra"
//        ,"A4Plus"
//        ,"A4Small"
//        ,"A5Extra"
//        ,"B5Extra"
//        ,"JisB0"
//        ,"JisB1"
//        ,"JisB2"
//        ,"JisB3"
//        ,"JisB4"
//        ,"JisB5"
//        ,"JisB6"
//        ,"JisB7"
//        ,"JisB8"
//        ,"JisB9"
//        ,"JisB10"
//        ,"AnsiC"
//        ,"AnsiD"
//        ,"AnsiE"
//        ,"LegalExtra"
//        ,"LetterExtra"
//        ,"LetterPlus"
//        ,"LetterSmall"
//        ,"TabloidExtra"
//        ,"ArchA"
//        ,"ArchB"
//        ,"ArchC"
//        ,"ArchD"
//        ,"ArchE"
//        ,"Imperial7x9"
//        ,"Imperial8x10"
//        ,"Imperial9x11"
//        ,"Imperial9x12"
//        ,"Imperial10x11"
//        ,"Imperial10x13"
//        ,"Imperial10x14"
//        ,"Imperial12x11"
//        ,"Imperial15x11"
//        ,"ExecutiveStandard"
//        ,"Note"
//        ,"Quarto"
//        ,"Statement"
//        ,"SuperA"
//        ,"SuperB"
//        ,"Postcard"
//        ,"DoublePostcard"
//        ,"Prc16K"
//        ,"Prc32K"
//        ,"Prc32KBig"
//        ,"FanFoldUS"
//        ,"FanFoldGerman"
//        ,"FanFoldGermanLegal"
//        ,"EnvelopeB4"
//        ,"EnvelopeB5"
//        ,"EnvelopeB6"
//        ,"EnvelopeC0"
//        ,"EnvelopeC1"
//        ,"EnvelopeC2"
//        ,"EnvelopeC3"
//        ,"EnvelopeC4"
//        ,"EnvelopeC6"
//        ,"EnvelopeC65"
//        ,"EnvelopeC7"
//        ,"Envelope9"
//        ,"Envelope11"
//        ,"Envelope12"
//        ,"Envelope14"
//        ,"EnvelopeMonarch"
//        ,"EnvelopePersonal"
//        ,"EnvelopeChou3"
//        ,"EnvelopeChou4"
//        ,"EnvelopeInvite"
//        ,"EnvelopeItalian"
//        ,"EnvelopeKaku2"
//        ,"EnvelopeKaku3"
//        ,"EnvelopePrc1"
//        ,"EnvelopePrc2"
//        ,"EnvelopePrc3"
//        ,"EnvelopePrc4"
//        ,"EnvelopePrc5"
//        ,"EnvelopePrc6"
//        ,"EnvelopePrc7"
//        ,"EnvelopePrc8"
//        ,"EnvelopePrc9"
//        ,"EnvelopePrc10"
//        ,"EnvelopeYou4"
//        ,"LastPageSize"
//        ,"NPageSize"
//        ,"NPaperSize"
//        ,"AnsiA"
//        ,"AnsiB"
//        ,"EnvelopeC5"
//        ,"EnvelopeDL"
//        ,"Envelope10"
//    };


//    QPagedPaintDevice::Margins margins;
//    margins.bottom = 0.;
//    margins.left = 0.;
//    margins.right = 0.;
//    margins.top = 0.;
//    pdfW.setMargins(margins);

//    pdfW.setResolution(300);
//    for(int ii = 0; ii < sizesPages.size(); ++ii){

//        pdfW.setPageSize(sizesPages[ii]);
//        std::string line;
//        if(ii == 0){
//            line += "if(formatStr == ";
//        }else if (ii < sizesPages.size()-1){
//            line += "}else if(formatStr == \"";
//        }else{
//            line += "else if(formatStr == \"";
//        }

//         QString pageSize = "  format = QPagedPaintDevice::PageSize::" + sizesPagesStr[ii] + ";\n   sizeMM = QSizeF(" + QString::number(pdfW.widthMM(), 'f', 2).toStdString().c_str() +"," + QString::number(pdfW.heightMM(), 'f', 2).toStdString().c_str() + ");";

//         if(ii == sizesPages.size()-1){
//             pageSize += "\n}";
//         }

//        line += sizesPagesStr[ii].toStdString() + "\"){";
//        qDebug() << line.c_str() << "\n   widthRatio = " << QString::number(1.0*pdfW.width()/pdfW.resolution(),'f', 4).toStdString().c_str() << ";";
//        qDebug() << "   heightRatio = " << QString::number(1.0*pdfW.height()/pdfW.resolution(),'f', 4).toStdString().c_str() << ";\n" << pageSize.toStdString().c_str();
//    }





    pc::PCMainUI w(&app);

    QDesktopWidget dw;
    QSize sizeWindow(1500,900);
    QRect rectScreen = dw.availableGeometry();
    if(rectScreen.width() > 0){
        if(rectScreen.width() < sizeWindow.width()){
            sizeWindow.setWidth(rectScreen.width());
        }
        if(rectScreen.height() < sizeWindow.height()){
            sizeWindow.setHeight(rectScreen.height());
        }
    }

    w.resize(sizeWindow);
    w.show();

    return app.exec();
}

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

// Photos consigne
//#include "MainInterface.h"
#include "MainUI.h"
#include "PCMainUI.hpp"


int main(int argc,char** argv)
{
//    QPdfWriter writer("G:/test.pdf");
//    QVector<QString> formatsStr;
//    QVector<int> formats;

//    formatsStr.push_back("A4"); formats.push_back(QPagedPaintDevice::A4);
//    formatsStr.push_back("B5"); formats.push_back(QPagedPaintDevice::B5);
//    formatsStr.push_back("Letter"); formats.push_back(QPagedPaintDevice::Letter);
//    formatsStr.push_back("Legal"); formats.push_back(QPagedPaintDevice::Legal);
//    formatsStr.push_back("Executive"); formats.push_back(QPagedPaintDevice::Executive);
//    formatsStr.push_back("A0"); formats.push_back(QPagedPaintDevice::A0);
//    formatsStr.push_back("A1"); formats.push_back(QPagedPaintDevice::A1);
//    formatsStr.push_back("A2"); formats.push_back(QPagedPaintDevice::A2);
//    formatsStr.push_back("A3"); formats.push_back(QPagedPaintDevice::A3);
//    formatsStr.push_back("A5"); formats.push_back(QPagedPaintDevice::A5);
//    formatsStr.push_back("A6"); formats.push_back(QPagedPaintDevice::A6);
//    formatsStr.push_back("A7"); formats.push_back(QPagedPaintDevice::A7);
//    formatsStr.push_back("A8"); formats.push_back(QPagedPaintDevice::A8);
//    formatsStr.push_back("A9"); formats.push_back(QPagedPaintDevice::A9);
//    formatsStr.push_back("B0"); formats.push_back(QPagedPaintDevice::B0);
//    formatsStr.push_back("B1"); formats.push_back(QPagedPaintDevice::B1);
//    formatsStr.push_back("B10"); formats.push_back(QPagedPaintDevice::B10);
//    formatsStr.push_back("B2"); formats.push_back(QPagedPaintDevice::B2);
//    formatsStr.push_back("B3"); formats.push_back(QPagedPaintDevice::B3);
//    formatsStr.push_back("B4"); formats.push_back(QPagedPaintDevice::B4);
//    formatsStr.push_back("B6"); formats.push_back(QPagedPaintDevice::B6);
//    formatsStr.push_back("B7"); formats.push_back(QPagedPaintDevice::B7);
//    formatsStr.push_back("B8"); formats.push_back(QPagedPaintDevice::B8);
//    formatsStr.push_back("B9"); formats.push_back(QPagedPaintDevice::B9);

//    formatsStr.push_back("C5E"); formats.push_back(QPagedPaintDevice::C5E);
//    formatsStr.push_back("Comm10E"); formats.push_back(QPagedPaintDevice::Comm10E);
//    formatsStr.push_back("DLE"); formats.push_back(QPagedPaintDevice::DLE);
//    formatsStr.push_back("Folio"); formats.push_back(QPagedPaintDevice::Folio);
//    formatsStr.push_back("Ledger"); formats.push_back(QPagedPaintDevice::Ledger);
//    formatsStr.push_back("Tabloid"); formats.push_back(QPagedPaintDevice::Tabloid);
//    formatsStr.push_back("Custom"); formats.push_back(QPagedPaintDevice::Custom);

//    formatsStr.push_back("A10"); formats.push_back(QPagedPaintDevice::A10);
//    formatsStr.push_back("A3Extra"); formats.push_back(QPagedPaintDevice::A3Extra);
//    formatsStr.push_back("A4Extra"); formats.push_back(QPagedPaintDevice::A4Extra);
//    formatsStr.push_back("A4Plus"); formats.push_back(QPagedPaintDevice::A4Plus);
//    formatsStr.push_back("A4Small"); formats.push_back(QPagedPaintDevice::A4Small);
//    formatsStr.push_back("A5Extra"); formats.push_back(QPagedPaintDevice::A5Extra);
//    formatsStr.push_back("B5Extra"); formats.push_back(QPagedPaintDevice::B5Extra);

//    formatsStr.push_back("JisB0"); formats.push_back(QPagedPaintDevice::JisB0);
//    formatsStr.push_back("JisB1"); formats.push_back(QPagedPaintDevice::JisB1);
//    formatsStr.push_back("JisB2"); formats.push_back(QPagedPaintDevice::JisB2);
//    formatsStr.push_back("JisB3"); formats.push_back(QPagedPaintDevice::JisB3);
//    formatsStr.push_back("JisB4"); formats.push_back(QPagedPaintDevice::JisB4);
//    formatsStr.push_back("JisB5"); formats.push_back(QPagedPaintDevice::JisB5);
//    formatsStr.push_back("JisB6"); formats.push_back(QPagedPaintDevice::JisB6);
//    formatsStr.push_back("JisB7"); formats.push_back(QPagedPaintDevice::JisB7);
//    formatsStr.push_back("JisB8"); formats.push_back(QPagedPaintDevice::JisB8);
//    formatsStr.push_back("JisB9"); formats.push_back(QPagedPaintDevice::JisB9);
//    formatsStr.push_back("JisB10"); formats.push_back(QPagedPaintDevice::JisB10);

//    formatsStr.push_back("AnsiC"); formats.push_back(QPagedPaintDevice::AnsiC);
//    formatsStr.push_back("AnsiD"); formats.push_back(QPagedPaintDevice::AnsiD);
//    formatsStr.push_back("AnsiE"); formats.push_back(QPagedPaintDevice::AnsiE);
//    formatsStr.push_back("LegalExtra"); formats.push_back(QPagedPaintDevice::LegalExtra);
//    formatsStr.push_back("LetterExtra"); formats.push_back(QPagedPaintDevice::LetterExtra);
//    formatsStr.push_back("LetterPlus"); formats.push_back(QPagedPaintDevice::LetterPlus);
//    formatsStr.push_back("LetterSmall"); formats.push_back(QPagedPaintDevice::LetterSmall);
//    formatsStr.push_back("TabloidExtra"); formats.push_back(QPagedPaintDevice::TabloidExtra);

//    formatsStr.push_back("ArchA"); formats.push_back(QPagedPaintDevice::ArchA);
//    formatsStr.push_back("ArchB"); formats.push_back(QPagedPaintDevice::ArchB);
//    formatsStr.push_back("ArchC"); formats.push_back(QPagedPaintDevice::ArchC);
//    formatsStr.push_back("ArchD"); formats.push_back(QPagedPaintDevice::ArchD);
//    formatsStr.push_back("ArchE"); formats.push_back(QPagedPaintDevice::ArchE);

//    formatsStr.push_back("Imperial7x9"); formats.push_back(QPagedPaintDevice::Imperial7x9);
//    formatsStr.push_back("Imperial8x10"); formats.push_back(QPagedPaintDevice::Imperial8x10);
//    formatsStr.push_back("Imperial9x11"); formats.push_back(QPagedPaintDevice::Imperial9x11);
//    formatsStr.push_back("Imperial9x12"); formats.push_back(QPagedPaintDevice::Imperial9x12);
//    formatsStr.push_back("Imperial10x11"); formats.push_back(QPagedPaintDevice::Imperial10x11);
//    formatsStr.push_back("Imperial10x13"); formats.push_back(QPagedPaintDevice::Imperial10x13);
//    formatsStr.push_back("Imperial10x14"); formats.push_back(QPagedPaintDevice::Imperial10x14);
//    formatsStr.push_back("Imperial12x11"); formats.push_back(QPagedPaintDevice::Imperial12x11);
//    formatsStr.push_back("Imperial15x11"); formats.push_back(QPagedPaintDevice::Imperial15x11);

//    formatsStr.push_back("ExecutiveStandard"); formats.push_back(QPagedPaintDevice::ExecutiveStandard);
//    formatsStr.push_back("Note"); formats.push_back(QPagedPaintDevice::Note);
//    formatsStr.push_back("Quarto"); formats.push_back(QPagedPaintDevice::Quarto);
//    formatsStr.push_back("Statement"); formats.push_back(QPagedPaintDevice::Statement);
//    formatsStr.push_back("SuperA"); formats.push_back(QPagedPaintDevice::SuperA);
//    formatsStr.push_back("SuperB"); formats.push_back(QPagedPaintDevice::SuperB);
//    formatsStr.push_back("Postcard"); formats.push_back(QPagedPaintDevice::Postcard);
//    formatsStr.push_back("DoublePostcard"); formats.push_back(QPagedPaintDevice::DoublePostcard);
//    formatsStr.push_back("Prc16K"); formats.push_back(QPagedPaintDevice::Prc16K);
//    formatsStr.push_back("Prc32K"); formats.push_back(QPagedPaintDevice::Prc32K);
//    formatsStr.push_back("Prc32KBig"); formats.push_back(QPagedPaintDevice::Prc32KBig);

//    formatsStr.push_back("FanFoldUS"); formats.push_back(QPagedPaintDevice::FanFoldUS);
//    formatsStr.push_back("FanFoldGerman"); formats.push_back(QPagedPaintDevice::FanFoldGerman);
//    formatsStr.push_back("FanFoldGermanLegal"); formats.push_back(QPagedPaintDevice::FanFoldGermanLegal);

//    formatsStr.push_back("EnvelopeB4"); formats.push_back(QPagedPaintDevice::EnvelopeB4);
//    formatsStr.push_back("EnvelopeB5"); formats.push_back(QPagedPaintDevice::EnvelopeB5);
//    formatsStr.push_back("EnvelopeB6"); formats.push_back(QPagedPaintDevice::EnvelopeB6);
//    formatsStr.push_back("EnvelopeC0"); formats.push_back(QPagedPaintDevice::EnvelopeC0);
//    formatsStr.push_back("EnvelopeC1"); formats.push_back(QPagedPaintDevice::EnvelopeC1);
//    formatsStr.push_back("EnvelopeC2"); formats.push_back(QPagedPaintDevice::EnvelopeC2);
//    formatsStr.push_back("EnvelopeC3"); formats.push_back(QPagedPaintDevice::EnvelopeC3);
//    formatsStr.push_back("EnvelopeC4"); formats.push_back(QPagedPaintDevice::EnvelopeC4);

//    formatsStr.push_back("EnvelopeC6"); formats.push_back(QPagedPaintDevice::EnvelopeC6);
//    formatsStr.push_back("EnvelopeC65"); formats.push_back(QPagedPaintDevice::EnvelopeC65);
//    formatsStr.push_back("EnvelopeC7"); formats.push_back(QPagedPaintDevice::EnvelopeC7);

//    formatsStr.push_back("Envelope9"); formats.push_back(QPagedPaintDevice::Envelope9);
//    formatsStr.push_back("Envelope11"); formats.push_back(QPagedPaintDevice::Envelope11);
//    formatsStr.push_back("Envelope12"); formats.push_back(QPagedPaintDevice::Envelope12);
//    formatsStr.push_back("Envelope14"); formats.push_back(QPagedPaintDevice::Envelope14);
//    formatsStr.push_back("EnvelopeMonarch"); formats.push_back(QPagedPaintDevice::EnvelopeMonarch);
//    formatsStr.push_back("EnvelopePersonal"); formats.push_back(QPagedPaintDevice::EnvelopePersonal);

//    formatsStr.push_back("EnvelopeChou3"); formats.push_back(QPagedPaintDevice::EnvelopeChou3);
//    formatsStr.push_back("EnvelopeChou4"); formats.push_back(QPagedPaintDevice::EnvelopeChou4);
//    formatsStr.push_back("EnvelopeInvite"); formats.push_back(QPagedPaintDevice::EnvelopeInvite);
//    formatsStr.push_back("EnvelopeItalian"); formats.push_back(QPagedPaintDevice::EnvelopeItalian);
//    formatsStr.push_back("EnvelopeKaku2"); formats.push_back(QPagedPaintDevice::EnvelopeKaku2);
//    formatsStr.push_back("EnvelopeKaku3"); formats.push_back(QPagedPaintDevice::EnvelopeKaku3);
//    formatsStr.push_back("EnvelopePrc1"); formats.push_back(QPagedPaintDevice::EnvelopePrc1);
//    formatsStr.push_back("EnvelopePrc2"); formats.push_back(QPagedPaintDevice::EnvelopePrc2);
//    formatsStr.push_back("EnvelopePrc3"); formats.push_back(QPagedPaintDevice::EnvelopePrc3);
//    formatsStr.push_back("EnvelopePrc4"); formats.push_back(QPagedPaintDevice::EnvelopePrc4);
//    formatsStr.push_back("EnvelopePrc5"); formats.push_back(QPagedPaintDevice::EnvelopePrc5);
//    formatsStr.push_back("EnvelopePrc6"); formats.push_back(QPagedPaintDevice::EnvelopePrc6);
//    formatsStr.push_back("EnvelopePrc7"); formats.push_back(QPagedPaintDevice::EnvelopePrc7);
//    formatsStr.push_back("EnvelopePrc8"); formats.push_back(QPagedPaintDevice::EnvelopePrc8);
//    formatsStr.push_back("EnvelopePrc9"); formats.push_back(QPagedPaintDevice::EnvelopePrc9);
//    formatsStr.push_back("EnvelopePrc10"); formats.push_back(QPagedPaintDevice::EnvelopePrc10);
//    formatsStr.push_back("EnvelopeYou4"); formats.push_back(QPagedPaintDevice::EnvelopeYou4);


//    #define myqDebug() qDebug() << fixed << qSetRealNumberPrecision(3)

//    writer.setResolution(300);
//    QVector<int> dpis = {72,96,150, 300, 600, 900, 1200, 2400};
//    bool landScape = false;
//    for(int ii = 0; ii < formatsStr.size(); ++ii){

//        QPageLayout pageLayout(QPageSize(static_cast<QPageSize::PageSizeId>(formats[ii])),
//                               landScape ? QPageLayout::Landscape : QPageLayout::Portrait, QMarginsF(0.,0.,0.,0.));
//        writer.setPageLayout(pageLayout);


////        writer.setPageSize(static_cast<QPagedPaintDevice::PageSize>(formats[ii]));
//        qreal widthRatio = (1.*writer.width()/300);
//        qreal heightRatio = (1.*writer.height()/300);

////        QString start = (ii == 0) ? "if(formatStr == " : "}else if(formatStr == ";

//        myqDebug() << ((ii == 0) ? "if(formatStr == " : "}else if(formatStr == ") << formatsStr[ii] << "){\n    widthRatio = " << widthRatio << ";\n    heightRatio = " << heightRatio << ";\n    format = QPagedPaintDevice::PageSize::" << formatsStr[ii].toStdString().c_str() <<";";// + formatsStr[ii]  + ";\n";
//        if(ii == formatsStr.size()-1){
//            qDebug() << "}";
//        }

//    }


    //1 : Get the language setting of the system
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);

    //2 : Installation of language setting
    app.installTranslator(&qtTranslator);

//    MainInterface w(&app);
    pc::PCMainUI w(&app);
    w.resize(1500, 900);

    w.show();

    return app.exec();
}

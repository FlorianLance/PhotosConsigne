
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



/**
 * \file PaperFormat.cpp
 * \brief defines PaperFormat
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "PaperFormat.hpp"

#include <QDebug>
using namespace pc;

pc::PaperFormat::PaperFormat(QString dpiStr, QString formatStr, bool landScape, bool doublePage){

    dpi = dpiStr.toInt();
    isLandScape = landScape;

    constexpr qreal factorInchToMM = 25.4;

    if(formatStr == "A4"){
        format = QPagedPaintDevice::PageSize::A4;        
        sizeMM = QSizeF(210.00,297.00);        
    }else if(formatStr == "Petit carré"){
        format = QPagedPaintDevice::PageSize::Custom;
        sizeMM = QSizeF(210.00,210.00);
    }else if(formatStr == "Grand carré"){
        format = QPagedPaintDevice::PageSize::Custom;
        sizeMM = QSizeF(300.00,300.00);
    }else if(formatStr == "B5"){
        format = QPagedPaintDevice::PageSize::B5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "Legal"){
        format = QPagedPaintDevice::PageSize::Legal;
        sizeMM = QSizeF(216.00,356.00);
    }else if(formatStr == "Executive"){
        format = QPagedPaintDevice::PageSize::Executive;
        sizeMM = QSizeF(191.00,254.00);
    }else if(formatStr == "A0"){
        format = QPagedPaintDevice::PageSize::A0;
        sizeMM = QSizeF(841.00,1189.00);
    }else if(formatStr == "A1"){
        format = QPagedPaintDevice::PageSize::A1;
        sizeMM = QSizeF(594.00,841.00);
    }else if(formatStr == "A2"){
        format = QPagedPaintDevice::PageSize::A2;
        sizeMM = QSizeF(420.00,594.00);
    }else if(formatStr == "A3"){
        format = QPagedPaintDevice::PageSize::A3;
        sizeMM = QSizeF(297.00,420.00);
    }else if(formatStr == "A5"){
        format = QPagedPaintDevice::PageSize::A5;
        sizeMM = QSizeF(148.00,210.00);
    }else if(formatStr == "A6"){
        format = QPagedPaintDevice::PageSize::A6;
        sizeMM = QSizeF(105.00,148.00);
    }else if(formatStr == "A7"){
        format = QPagedPaintDevice::PageSize::A7;
        sizeMM = QSizeF(74.00,105.00);
    }else if(formatStr == "A8"){
        format = QPagedPaintDevice::PageSize::A8;
        sizeMM = QSizeF(52.00,74.00);
    }else if(formatStr == "A9"){
        format = QPagedPaintDevice::PageSize::A9;
        sizeMM = QSizeF(37.00,52.00);
    }else if(formatStr == "B0"){
        format = QPagedPaintDevice::PageSize::B0;
        sizeMM = QSizeF(1000.00,1414.00);
    }else if(formatStr == "B1"){
        format = QPagedPaintDevice::PageSize::B1;
        sizeMM = QSizeF(707.00,1000.00);
    }else if(formatStr == "B10"){
        format = QPagedPaintDevice::PageSize::B10;
        sizeMM = QSizeF(31.00,44.00);
    }else if(formatStr == "B2"){
        format = QPagedPaintDevice::PageSize::B2;
        sizeMM = QSizeF(500.00,707.00);
    }else if(formatStr == "B3"){
        format = QPagedPaintDevice::PageSize::B3;
        sizeMM = QSizeF(353.00,500.00);
    }else if(formatStr == "B4"){
        format = QPagedPaintDevice::PageSize::B4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "B6"){
        format = QPagedPaintDevice::PageSize::B6;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "B7"){
        format = QPagedPaintDevice::PageSize::B7;
        sizeMM = QSizeF(88.00,125.00);
    }else if(formatStr == "B8"){
        format = QPagedPaintDevice::PageSize::B8;
        sizeMM = QSizeF(62.00,88.00);
    }else if(formatStr == "B9"){
        format = QPagedPaintDevice::PageSize::B9;
        sizeMM = QSizeF(44.00,62.00);
    }else if(formatStr == "Folio"){
        format = QPagedPaintDevice::PageSize::Folio;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "Tabloid"){
        format = QPagedPaintDevice::PageSize::Tabloid;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "Custom"){
        format = QPagedPaintDevice::PageSize::Custom;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "A10"){
        format = QPagedPaintDevice::PageSize::A10;
        sizeMM = QSizeF(26.00,37.00);
    }else if(formatStr == "A3Extra"){
        format = QPagedPaintDevice::PageSize::A3Extra;
        sizeMM = QSizeF(322.00,445.00);
    }else if(formatStr == "A4Extra"){
        format = QPagedPaintDevice::PageSize::A4Extra;
        sizeMM = QSizeF(236.00,322.00);
    }else if(formatStr == "A4Plus"){
        format = QPagedPaintDevice::PageSize::A4Plus;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "A4Small"){
        format = QPagedPaintDevice::PageSize::A4Small;
        sizeMM = QSizeF(210.00,297.00);
    }else if(formatStr == "A5Extra"){
        format = QPagedPaintDevice::PageSize::A5Extra;
        sizeMM = QSizeF(174.00,235.00);
    }else if(formatStr == "B5Extra"){
        format = QPagedPaintDevice::PageSize::B5Extra;
        sizeMM = QSizeF(201.00,276.00);
    }else if(formatStr == "JisB0"){
        format = QPagedPaintDevice::PageSize::JisB0;
        sizeMM = QSizeF(1030.00,1456.00);
    }else if(formatStr == "JisB1"){
        format = QPagedPaintDevice::PageSize::JisB1;
        sizeMM = QSizeF(728.00,1030.00);
    }else if(formatStr == "JisB2"){
        format = QPagedPaintDevice::PageSize::JisB2;
        sizeMM = QSizeF(515.00,728.00);
    }else if(formatStr == "JisB3"){
        format = QPagedPaintDevice::PageSize::JisB3;
        sizeMM = QSizeF(364.00,515.00);
    }else if(formatStr == "JisB4"){
        format = QPagedPaintDevice::PageSize::JisB4;
        sizeMM = QSizeF(257.00,364.00);
    }else if(formatStr == "JisB5"){
        format = QPagedPaintDevice::PageSize::JisB5;
        sizeMM = QSizeF(182.00,257.00);
    }else if(formatStr == "JisB6"){
        format = QPagedPaintDevice::PageSize::JisB6;
        sizeMM = QSizeF(128.00,182.00);
    }else if(formatStr == "JisB7"){
        format = QPagedPaintDevice::PageSize::JisB7;
        sizeMM = QSizeF(91.00,128.00);
    }else if(formatStr == "JisB8"){
        format = QPagedPaintDevice::PageSize::JisB8;
        sizeMM = QSizeF(64.00,91.00);
    }else if(formatStr == "JisB9"){
        format = QPagedPaintDevice::PageSize::JisB9;
        sizeMM = QSizeF(45.00,64.00);
    }else if(formatStr == "JisB10"){
        format = QPagedPaintDevice::PageSize::JisB10;
        sizeMM = QSizeF(32.00,45.00);
    }else if(formatStr == "AnsiC"){
        format = QPagedPaintDevice::PageSize::AnsiC;
        sizeMM = QSizeF(432.00,559.00);
    }else if(formatStr == "AnsiD"){
        format = QPagedPaintDevice::PageSize::AnsiD;
        sizeMM = QSizeF(559.00,864.00);
    }else if(formatStr == "AnsiE"){
        format = QPagedPaintDevice::PageSize::AnsiE;
        sizeMM = QSizeF(864.00,1118.00);
    }else if(formatStr == "LegalExtra"){
        format = QPagedPaintDevice::PageSize::LegalExtra;
        sizeMM = QSizeF(241.00,381.00);
    }else if(formatStr == "LetterExtra"){
        format = QPagedPaintDevice::PageSize::LetterExtra;
        sizeMM = QSizeF(241.00,305.00);
    }else if(formatStr == "LetterPlus"){
        format = QPagedPaintDevice::PageSize::LetterPlus;
        sizeMM = QSizeF(216.00,322.00);
    }else if(formatStr == "LetterSmall"){
        format = QPagedPaintDevice::PageSize::LetterSmall;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "TabloidExtra"){
        format = QPagedPaintDevice::PageSize::TabloidExtra;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchA"){
        format = QPagedPaintDevice::PageSize::ArchA;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "ArchB"){
        format = QPagedPaintDevice::PageSize::ArchB;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchC"){
        format = QPagedPaintDevice::PageSize::ArchC;
        sizeMM = QSizeF(457.00,610.00);
    }else if(formatStr == "ArchD"){
        format = QPagedPaintDevice::PageSize::ArchD;
        sizeMM = QSizeF(610.00,914.00);
    }else if(formatStr == "ArchE"){
        format = QPagedPaintDevice::PageSize::ArchE;
        sizeMM = QSizeF(914.00,1219.00);
    }else if(formatStr == "Imperial7x9"){
        format = QPagedPaintDevice::PageSize::Imperial7x9;
        sizeMM = QSizeF(178.00,229.00);
    }else if(formatStr == "Imperial8x10"){
        format = QPagedPaintDevice::PageSize::Imperial8x10;
        sizeMM = QSizeF(203.00,254.00);
    }else if(formatStr == "Imperial9x11"){
        format = QPagedPaintDevice::PageSize::Imperial9x11;
        sizeMM = QSizeF(229.00,279.00);
    }else if(formatStr == "Imperial9x12"){
        format = QPagedPaintDevice::PageSize::Imperial9x12;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "Imperial10x11"){
        format = QPagedPaintDevice::PageSize::Imperial10x11;
        sizeMM = QSizeF(254.00,279.00);
    }else if(formatStr == "Imperial10x13"){
        format = QPagedPaintDevice::PageSize::Imperial10x13;
        sizeMM = QSizeF(254.00,330.00);
    }else if(formatStr == "Imperial10x14"){
        format = QPagedPaintDevice::PageSize::Imperial10x14;
        sizeMM = QSizeF(254.00,356.00);
    }else if(formatStr == "Imperial12x11"){
        format = QPagedPaintDevice::PageSize::Imperial12x11;
        sizeMM = QSizeF(305.00,279.00);
    }else if(formatStr == "Imperial15x11"){
        format = QPagedPaintDevice::PageSize::Imperial15x11;
        sizeMM = QSizeF(381.00,279.00);
    }else if(formatStr == "ExecutiveStandard"){
        format = QPagedPaintDevice::PageSize::ExecutiveStandard;
        sizeMM = QSizeF(184.00,267.00);
    }else if(formatStr == "Note"){
        format = QPagedPaintDevice::PageSize::Note;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "Quarto"){
        format = QPagedPaintDevice::PageSize::Quarto;
        sizeMM = QSizeF(216.00,275.00);
    }else if(formatStr == "Statement"){
        format = QPagedPaintDevice::PageSize::Statement;
        sizeMM = QSizeF(140.00,216.00);
    }else if(formatStr == "SuperA"){
        format = QPagedPaintDevice::PageSize::SuperA;
        sizeMM = QSizeF(227.00,356.00);
    }else if(formatStr == "SuperB"){
        format = QPagedPaintDevice::PageSize::SuperB;
        sizeMM = QSizeF(305.00,487.00);
    }else if(formatStr == "Postcard"){
        format = QPagedPaintDevice::PageSize::Postcard;
        sizeMM = QSizeF(100.00,148.00);
    }else if(formatStr == "DoublePostcard"){
        format = QPagedPaintDevice::PageSize::DoublePostcard;
        sizeMM = QSizeF(200.00,148.00);
    }else if(formatStr == "Prc16K"){
        format = QPagedPaintDevice::PageSize::Prc16K;
        sizeMM = QSizeF(146.00,215.00);
    }else if(formatStr == "Prc32K"){
        format = QPagedPaintDevice::PageSize::Prc32K;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "Prc32KBig"){
        format = QPagedPaintDevice::PageSize::Prc32KBig;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "FanFoldUS"){
        format = QPagedPaintDevice::PageSize::FanFoldUS;
        sizeMM = QSizeF(378.00,279.00);
    }else if(formatStr == "FanFoldGerman"){
        format = QPagedPaintDevice::PageSize::FanFoldGerman;
        sizeMM = QSizeF(216.00,305.00);
    }else if(formatStr == "FanFoldGermanLegal"){
        format = QPagedPaintDevice::PageSize::FanFoldGermanLegal;
        sizeMM = QSizeF(216.00,330.00);
    }else if(formatStr == "EnvelopeB4"){
        format = QPagedPaintDevice::PageSize::EnvelopeB4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "EnvelopeB5"){
        format = QPagedPaintDevice::PageSize::EnvelopeB5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "EnvelopeB6"){
        format = QPagedPaintDevice::PageSize::EnvelopeB6;
        sizeMM = QSizeF(176.00,125.00);
    }else if(formatStr == "EnvelopeC0"){
        format = QPagedPaintDevice::PageSize::EnvelopeC0;
        sizeMM = QSizeF(917.00,1297.00);
    }else if(formatStr == "EnvelopeC1"){
        format = QPagedPaintDevice::PageSize::EnvelopeC1;
        sizeMM = QSizeF(648.00,917.00);
    }else if(formatStr == "EnvelopeC2"){
        format = QPagedPaintDevice::PageSize::EnvelopeC2;
        sizeMM = QSizeF(458.00,648.00);
    }else if(formatStr == "EnvelopeC3"){
        format = QPagedPaintDevice::PageSize::EnvelopeC3;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeC4"){
        format = QPagedPaintDevice::PageSize::EnvelopeC4;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopeC6"){
        format = QPagedPaintDevice::PageSize::EnvelopeC6;
        sizeMM = QSizeF(114.00,162.00);
    }else if(formatStr == "EnvelopeC65"){
        format = QPagedPaintDevice::PageSize::EnvelopeC65;
        sizeMM = QSizeF(114.00,229.00);
    }else if(formatStr == "EnvelopeC7"){
        format = QPagedPaintDevice::PageSize::EnvelopeC7;
        sizeMM = QSizeF(81.00,114.00);
    }else if(formatStr == "Envelope9"){
        format = QPagedPaintDevice::PageSize::Envelope9;
        sizeMM = QSizeF(98.00,225.00);
    }else if(formatStr == "Envelope11"){
        format = QPagedPaintDevice::PageSize::Envelope11;
        sizeMM = QSizeF(114.00,264.00);
    }else if(formatStr == "Envelope12"){
        format = QPagedPaintDevice::PageSize::Envelope12;
        sizeMM = QSizeF(121.00,279.00);
    }else if(formatStr == "Envelope14"){
        format = QPagedPaintDevice::PageSize::Envelope14;
        sizeMM = QSizeF(127.00,292.00);
    }else if(formatStr == "EnvelopeMonarch"){
        format = QPagedPaintDevice::PageSize::EnvelopeMonarch;
        sizeMM = QSizeF(98.00,191.00);
    }else if(formatStr == "EnvelopePersonal"){
        format = QPagedPaintDevice::PageSize::EnvelopePersonal;
        sizeMM = QSizeF(92.00,165.00);
    }else if(formatStr == "EnvelopeChou3"){
        format = QPagedPaintDevice::PageSize::EnvelopeChou3;
        sizeMM = QSizeF(120.00,235.00);
    }else if(formatStr == "EnvelopeChou4"){
        format = QPagedPaintDevice::PageSize::EnvelopeChou4;
        sizeMM = QSizeF(90.00,205.00);
    }else if(formatStr == "EnvelopeInvite"){
        format = QPagedPaintDevice::PageSize::EnvelopeInvite;
        sizeMM = QSizeF(220.00,220.00);
    }else if(formatStr == "EnvelopeItalian"){
        format = QPagedPaintDevice::PageSize::EnvelopeItalian;
        sizeMM = QSizeF(110.00,230.00);
    }else if(formatStr == "EnvelopeKaku2"){
        format = QPagedPaintDevice::PageSize::EnvelopeKaku2;
        sizeMM = QSizeF(240.00,332.00);
    }else if(formatStr == "EnvelopeKaku3"){
        format = QPagedPaintDevice::PageSize::EnvelopeKaku3;
        sizeMM = QSizeF(216.00,277.00);
    }else if(formatStr == "EnvelopePrc1"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc1;
        sizeMM = QSizeF(102.00,165.00);
    }else if(formatStr == "EnvelopePrc2"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc2;
        sizeMM = QSizeF(102.00,176.00);
    }else if(formatStr == "EnvelopePrc3"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc3;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "EnvelopePrc4"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc4;
        sizeMM = QSizeF(110.00,208.00);
    }else if(formatStr == "EnvelopePrc5"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc5;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "EnvelopePrc6"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc6;
        sizeMM = QSizeF(120.00,230.00);
    }else if(formatStr == "EnvelopePrc7"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc7;
        sizeMM = QSizeF(160.00,230.00);
    }else if(formatStr == "EnvelopePrc8"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc8;
        sizeMM = QSizeF(120.00,309.00);
    }else if(formatStr == "EnvelopePrc9"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc9;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopePrc10"){
        format = QPagedPaintDevice::PageSize::EnvelopePrc10;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeYou4"){
        format = QPagedPaintDevice::PageSize::EnvelopeYou4;
        sizeMM = QSizeF(105.00,235.00);
    }else if(formatStr == "AnsiA" || formatStr == "Letter"){
        format = QPagedPaintDevice::PageSize::AnsiA;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "AnsiB" || formatStr == "Ledger"){
        format = QPagedPaintDevice::PageSize::AnsiB;
        sizeMM = QSizeF(432.00,279.00);
    }else if(formatStr == "EnvelopeC5" || formatStr == "C5E"){
        format = QPagedPaintDevice::PageSize::EnvelopeC5;
        sizeMM = QSizeF(162.00,229.00);
    }else if(formatStr == "EnvelopeDL" || formatStr == "DLE"){
        format = QPagedPaintDevice::PageSize::EnvelopeDL;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "Envelope10" || formatStr == "Comm10E"){
        format = QPagedPaintDevice::PageSize::Envelope10;
        sizeMM = QSizeF(105.00,241.00);
    }   

    if(landScape){
        sizeMM = QSizeF(sizeMM.height(), sizeMM.width());
    }

    if(doublePage){
        isCustom = true;
        isDoublePage = true;
        sizeMM = QSizeF(sizeMM.width()*2., sizeMM.height());
        format = QPagedPaintDevice::PageSize::Custom;
    }

    ratioMM = sizeMM/factorInchToMM;
}


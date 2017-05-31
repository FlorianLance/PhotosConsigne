

#include "PaperFormat.hpp"


using namespace pc;



pc::PaperFormat::PaperFormat(QString dpiStr, QString formatStr, bool landScape){

    dpi = dpiStr.toInt();

    if(formatStr == "A4"){
        widthRatio =  8.2633 ;
        heightRatio =  11.6933 ;
        format = QPagedPaintDevice::PageSize::A4;
        sizeMM = QSizeF(210.00,297.00);
    }else if(formatStr == "B5"){
        widthRatio =  6.9300 ;
        heightRatio =  9.8467 ;
        format = QPagedPaintDevice::PageSize::B5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "Legal"){
        widthRatio =  8.5000 ;
        heightRatio =  14.0000 ;
        format = QPagedPaintDevice::PageSize::Legal;
        sizeMM = QSizeF(216.00,356.00);
    }else if(formatStr == "Executive"){
        widthRatio =  7.5000 ;
        heightRatio =  10.0000 ;
        format = QPagedPaintDevice::PageSize::Executive;
        sizeMM = QSizeF(191.00,254.00);
    }else if(formatStr == "A0"){
        widthRatio =  33.1100 ;
        heightRatio =  46.8067 ;
        format = QPagedPaintDevice::PageSize::A0;
        sizeMM = QSizeF(841.00,1189.00);
    }else if(formatStr == "A1"){
        widthRatio =  23.3900 ;
        heightRatio =  33.1100 ;
        format = QPagedPaintDevice::PageSize::A1;
        sizeMM = QSizeF(594.00,841.00);
    }else if(formatStr == "A2"){
        widthRatio =  16.5433 ;
        heightRatio =  23.3900 ;
        format = QPagedPaintDevice::PageSize::A2;
        sizeMM = QSizeF(420.00,594.00);
    }else if(formatStr == "A3"){
        widthRatio =  11.6933 ;
        heightRatio =  16.5433 ;
        format = QPagedPaintDevice::PageSize::A3;
        sizeMM = QSizeF(297.00,420.00);
    }else if(formatStr == "A5"){
        widthRatio =  5.8333 ;
        heightRatio =  8.2633 ;
        format = QPagedPaintDevice::PageSize::A5;
        sizeMM = QSizeF(148.00,210.00);
    }else if(formatStr == "A6"){
        widthRatio =  4.1267 ;
        heightRatio =  5.8333 ;
        format = QPagedPaintDevice::PageSize::A6;
        sizeMM = QSizeF(105.00,148.00);
    }else if(formatStr == "A7"){
        widthRatio =  2.9167 ;
        heightRatio =  4.1267 ;
        format = QPagedPaintDevice::PageSize::A7;
        sizeMM = QSizeF(74.00,105.00);
    }else if(formatStr == "A8"){
        widthRatio =  2.0567 ;
        heightRatio =  2.9167 ;
        format = QPagedPaintDevice::PageSize::A8;
        sizeMM = QSizeF(52.00,74.00);
    }else if(formatStr == "A9"){
        widthRatio =  1.4600 ;
        heightRatio =  2.0567 ;
        format = QPagedPaintDevice::PageSize::A9;
        sizeMM = QSizeF(37.00,52.00);
    }else if(formatStr == "B0"){
        widthRatio =  39.3767 ;
        heightRatio =  55.6667 ;
        format = QPagedPaintDevice::PageSize::B0;
        sizeMM = QSizeF(1000.00,1414.00);
    }else if(formatStr == "B1"){
        widthRatio =  27.8333 ;
        heightRatio =  39.3767 ;
        format = QPagedPaintDevice::PageSize::B1;
        sizeMM = QSizeF(707.00,1000.00);
    }else if(formatStr == "B10"){
        widthRatio =  1.2233 ;
        heightRatio =  1.7367 ;
        format = QPagedPaintDevice::PageSize::B10;
        sizeMM = QSizeF(31.00,44.00);
    }else if(formatStr == "B2"){
        widthRatio =  19.6800 ;
        heightRatio =  27.8333 ;
        format = QPagedPaintDevice::PageSize::B2;
        sizeMM = QSizeF(500.00,707.00);
    }else if(formatStr == "B3"){
        widthRatio =  13.9033 ;
        heightRatio =  19.6800 ;
        format = QPagedPaintDevice::PageSize::B3;
        sizeMM = QSizeF(353.00,500.00);
    }else if(formatStr == "B4"){
        widthRatio =  9.8467 ;
        heightRatio =  13.9033 ;
        format = QPagedPaintDevice::PageSize::B4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "B6"){
        widthRatio =  4.9167 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::B6;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "B7"){
        widthRatio =  3.4600 ;
        heightRatio =  4.9167 ;
        format = QPagedPaintDevice::PageSize::B7;
        sizeMM = QSizeF(88.00,125.00);
    }else if(formatStr == "B8"){
        widthRatio =  2.4433 ;
        heightRatio =  3.4600 ;
        format = QPagedPaintDevice::PageSize::B8;
        sizeMM = QSizeF(62.00,88.00);
    }else if(formatStr == "B9"){
        widthRatio =  1.7367 ;
        heightRatio =  2.4433 ;
        format = QPagedPaintDevice::PageSize::B9;
        sizeMM = QSizeF(44.00,62.00);
    }else if(formatStr == "Folio"){
        widthRatio =  8.2633 ;
        heightRatio =  12.9867 ;
        format = QPagedPaintDevice::PageSize::Folio;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "Tabloid"){
        widthRatio =  11.0000 ;
        heightRatio =  17.0000 ;
        format = QPagedPaintDevice::PageSize::Tabloid;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "Custom"){
        widthRatio =  11.0000 ;
        heightRatio =  17.0000 ;
        format = QPagedPaintDevice::PageSize::Custom;
        sizeMM = QSizeF(279.00,432.00);
    }else if(formatStr == "A10"){
        widthRatio =  1.0133 ;
        heightRatio =  1.4600 ;
        format = QPagedPaintDevice::PageSize::A10;
        sizeMM = QSizeF(26.00,37.00);
    }else if(formatStr == "A3Extra"){
        widthRatio =  12.6800 ;
        heightRatio =  17.5267 ;
        format = QPagedPaintDevice::PageSize::A3Extra;
        sizeMM = QSizeF(322.00,445.00);
    }else if(formatStr == "A4Extra"){
        widthRatio =  9.2633 ;
        heightRatio =  12.6933 ;
        format = QPagedPaintDevice::PageSize::A4Extra;
        sizeMM = QSizeF(236.00,322.00);
    }else if(formatStr == "A4Plus"){
        widthRatio =  8.2633 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::A4Plus;
        sizeMM = QSizeF(210.00,330.00);
    }else if(formatStr == "A4Small"){
        widthRatio =  8.2633 ;
        heightRatio =  11.6933 ;
        format = QPagedPaintDevice::PageSize::A4Small;
        sizeMM = QSizeF(210.00,297.00);
    }else if(formatStr == "A5Extra"){
        widthRatio =  6.8333 ;
        heightRatio =  9.2767 ;
        format = QPagedPaintDevice::PageSize::A5Extra;
        sizeMM = QSizeF(174.00,235.00);
    }else if(formatStr == "B5Extra"){
        widthRatio =  7.9167 ;
        heightRatio =  10.8600 ;
        format = QPagedPaintDevice::PageSize::B5Extra;
        sizeMM = QSizeF(201.00,276.00);
    }else if(formatStr == "JisB0"){
        widthRatio =  40.5567 ;
        heightRatio =  57.3200 ;
        format = QPagedPaintDevice::PageSize::JisB0;
        sizeMM = QSizeF(1030.00,1456.00);
    }else if(formatStr == "JisB1"){
        widthRatio =  28.6667 ;
        heightRatio =  40.5567 ;
        format = QPagedPaintDevice::PageSize::JisB1;
        sizeMM = QSizeF(728.00,1030.00);
    }else if(formatStr == "JisB2"){
        widthRatio =  20.2767 ;
        heightRatio =  28.6667 ;
        format = QPagedPaintDevice::PageSize::JisB2;
        sizeMM = QSizeF(515.00,728.00);
    }else if(formatStr == "JisB3"){
        widthRatio =  14.3333 ;
        heightRatio =  20.2767 ;
        format = QPagedPaintDevice::PageSize::JisB3;
        sizeMM = QSizeF(364.00,515.00);
    }else if(formatStr == "JisB4"){
        widthRatio =  10.1267 ;
        heightRatio =  14.3333 ;
        format = QPagedPaintDevice::PageSize::JisB4;
        sizeMM = QSizeF(257.00,364.00);
    }else if(formatStr == "JisB5"){
        widthRatio =  7.1667 ;
        heightRatio =  10.1267 ;
        format = QPagedPaintDevice::PageSize::JisB5;
        sizeMM = QSizeF(182.00,257.00);
    }else if(formatStr == "JisB6"){
        widthRatio =  5.0433 ;
        heightRatio =  7.1667 ;
        format = QPagedPaintDevice::PageSize::JisB6;
        sizeMM = QSizeF(128.00,182.00);
    }else if(formatStr == "JisB7"){
        widthRatio =  3.5833 ;
        heightRatio =  5.0433 ;
        format = QPagedPaintDevice::PageSize::JisB7;
        sizeMM = QSizeF(91.00,128.00);
    }else if(formatStr == "JisB8"){
        widthRatio =  2.5133 ;
        heightRatio =  3.5833 ;
        format = QPagedPaintDevice::PageSize::JisB8;
        sizeMM = QSizeF(64.00,91.00);
    }else if(formatStr == "JisB9"){
        widthRatio =  1.7633 ;
        heightRatio =  2.5133 ;
        format = QPagedPaintDevice::PageSize::JisB9;
        sizeMM = QSizeF(45.00,64.00);
    }else if(formatStr == "JisB10"){
        widthRatio =  1.2633 ;
        heightRatio =  1.7633 ;
        format = QPagedPaintDevice::PageSize::JisB10;
        sizeMM = QSizeF(32.00,45.00);
    }else if(formatStr == "AnsiC"){
        widthRatio =  17.0000 ;
        heightRatio =  22.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiC;
        sizeMM = QSizeF(432.00,559.00);
    }else if(formatStr == "AnsiD"){
        widthRatio =  22.0000 ;
        heightRatio =  34.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiD;
        sizeMM = QSizeF(559.00,864.00);
    }else if(formatStr == "AnsiE"){
        widthRatio =  34.0000 ;
        heightRatio =  44.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiE;
        sizeMM = QSizeF(864.00,1118.00);
    }else if(formatStr == "LegalExtra"){
        widthRatio =  9.5000 ;
        heightRatio =  15.0000 ;
        format = QPagedPaintDevice::PageSize::LegalExtra;
        sizeMM = QSizeF(241.00,381.00);
    }else if(formatStr == "LetterExtra"){
        widthRatio =  9.5000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::LetterExtra;
        sizeMM = QSizeF(241.00,305.00);
    }else if(formatStr == "LetterPlus"){
        widthRatio =  8.5000 ;
        heightRatio =  12.6933 ;
        format = QPagedPaintDevice::PageSize::LetterPlus;
        sizeMM = QSizeF(216.00,322.00);
    }else if(formatStr == "LetterSmall"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::LetterSmall;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "TabloidExtra"){
        widthRatio =  12.0000 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::TabloidExtra;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchA"){
        widthRatio =  9.0000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::ArchA;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "ArchB"){
        widthRatio =  12.0000 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::ArchB;
        sizeMM = QSizeF(305.00,457.00);
    }else if(formatStr == "ArchC"){
        widthRatio =  18.0000 ;
        heightRatio =  24.0000 ;
        format = QPagedPaintDevice::PageSize::ArchC;
        sizeMM = QSizeF(457.00,610.00);
    }else if(formatStr == "ArchD"){
        widthRatio =  24.0000 ;
        heightRatio =  36.0000 ;
        format = QPagedPaintDevice::PageSize::ArchD;
        sizeMM = QSizeF(610.00,914.00);
    }else if(formatStr == "ArchE"){
        widthRatio =  36.0000 ;
        heightRatio =  48.0000 ;
        format = QPagedPaintDevice::PageSize::ArchE;
        sizeMM = QSizeF(914.00,1219.00);
    }else if(formatStr == "Imperial7x9"){
        widthRatio =  7.0000 ;
        heightRatio =  9.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial7x9;
        sizeMM = QSizeF(178.00,229.00);
    }else if(formatStr == "Imperial8x10"){
        widthRatio =  8.0000 ;
        heightRatio =  10.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial8x10;
        sizeMM = QSizeF(203.00,254.00);
    }else if(formatStr == "Imperial9x11"){
        widthRatio =  9.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial9x11;
        sizeMM = QSizeF(229.00,279.00);
    }else if(formatStr == "Imperial9x12"){
        widthRatio =  9.0000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial9x12;
        sizeMM = QSizeF(229.00,305.00);
    }else if(formatStr == "Imperial10x11"){
        widthRatio =  10.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x11;
        sizeMM = QSizeF(254.00,279.00);
    }else if(formatStr == "Imperial10x13"){
        widthRatio =  10.0000 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x13;
        sizeMM = QSizeF(254.00,330.00);
    }else if(formatStr == "Imperial10x14"){
        widthRatio =  10.0000 ;
        heightRatio =  14.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial10x14;
        sizeMM = QSizeF(254.00,356.00);
    }else if(formatStr == "Imperial12x11"){
        widthRatio =  12.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial12x11;
        sizeMM = QSizeF(305.00,279.00);
    }else if(formatStr == "Imperial15x11"){
        widthRatio =  15.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Imperial15x11;
        sizeMM = QSizeF(381.00,279.00);
    }else if(formatStr == "ExecutiveStandard"){
        widthRatio =  7.2500 ;
        heightRatio =  10.5000 ;
        format = QPagedPaintDevice::PageSize::ExecutiveStandard;
        sizeMM = QSizeF(184.00,267.00);
    }else if(formatStr == "Note"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Note;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "Quarto"){
        widthRatio =  8.4733 ;
        heightRatio =  10.8333 ;
        format = QPagedPaintDevice::PageSize::Quarto;
        sizeMM = QSizeF(216.00,275.00);
    }else if(formatStr == "Statement"){
        widthRatio =  5.5000 ;
        heightRatio =  8.5000 ;
        format = QPagedPaintDevice::PageSize::Statement;
        sizeMM = QSizeF(140.00,216.00);
    }else if(formatStr == "SuperA"){
        widthRatio =  8.9300 ;
        heightRatio =  14.0133 ;
        format = QPagedPaintDevice::PageSize::SuperA;
        sizeMM = QSizeF(227.00,356.00);
    }else if(formatStr == "SuperB"){
        widthRatio =  12.0000 ;
        heightRatio =  19.1667 ;
        format = QPagedPaintDevice::PageSize::SuperB;
        sizeMM = QSizeF(305.00,487.00);
    }else if(formatStr == "Postcard"){
        widthRatio =  3.9433 ;
        heightRatio =  5.8200 ;
        format = QPagedPaintDevice::PageSize::Postcard;
        sizeMM = QSizeF(100.00,148.00);
    }else if(formatStr == "DoublePostcard"){
        widthRatio =  7.8767 ;
        heightRatio =  5.8200 ;
        format = QPagedPaintDevice::PageSize::DoublePostcard;
        sizeMM = QSizeF(200.00,148.00);
    }else if(formatStr == "Prc16K"){
        widthRatio =  5.7500 ;
        heightRatio =  8.4733 ;
        format = QPagedPaintDevice::PageSize::Prc16K;
        sizeMM = QSizeF(146.00,215.00);
    }else if(formatStr == "Prc32K"){
        widthRatio =  3.8200 ;
        heightRatio =  5.9433 ;
        format = QPagedPaintDevice::PageSize::Prc32K;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "Prc32KBig"){
        widthRatio =  3.8200 ;
        heightRatio =  5.9433 ;
        format = QPagedPaintDevice::PageSize::Prc32KBig;
        sizeMM = QSizeF(97.00,151.00);
    }else if(formatStr == "FanFoldUS"){
        widthRatio =  14.8767 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldUS;
        sizeMM = QSizeF(378.00,279.00);
    }else if(formatStr == "FanFoldGerman"){
        widthRatio =  8.5000 ;
        heightRatio =  12.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldGerman;
        sizeMM = QSizeF(216.00,305.00);
    }else if(formatStr == "FanFoldGermanLegal"){
        widthRatio =  8.5000 ;
        heightRatio =  13.0000 ;
        format = QPagedPaintDevice::PageSize::FanFoldGermanLegal;
        sizeMM = QSizeF(216.00,330.00);
    }else if(formatStr == "EnvelopeB4"){
        widthRatio =  9.8333 ;
        heightRatio =  13.9033 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB4;
        sizeMM = QSizeF(250.00,353.00);
    }else if(formatStr == "EnvelopeB5"){
        widthRatio =  6.9300 ;
        heightRatio =  9.8467 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB5;
        sizeMM = QSizeF(176.00,250.00);
    }else if(formatStr == "EnvelopeB6"){
        widthRatio =  6.9300 ;
        heightRatio =  4.9167 ;
        format = QPagedPaintDevice::PageSize::EnvelopeB6;
        sizeMM = QSizeF(176.00,125.00);
    }else if(formatStr == "EnvelopeC0"){
        widthRatio =  36.0967 ;
        heightRatio =  51.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC0;
        sizeMM = QSizeF(917.00,1297.00);
    }else if(formatStr == "EnvelopeC1"){
        widthRatio =  25.5133 ;
        heightRatio =  36.0967 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC1;
        sizeMM = QSizeF(648.00,917.00);
    }else if(formatStr == "EnvelopeC2"){
        widthRatio =  18.0267 ;
        heightRatio =  25.5133 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC2;
        sizeMM = QSizeF(458.00,648.00);
    }else if(formatStr == "EnvelopeC3"){
        widthRatio =  12.7500 ;
        heightRatio =  18.0000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC3;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeC4"){
        widthRatio =  9.0133 ;
        heightRatio =  12.7500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC4;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopeC6"){
        widthRatio =  4.4867 ;
        heightRatio =  6.3767 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC6;
        sizeMM = QSizeF(114.00,162.00);
    }else if(formatStr == "EnvelopeC65"){
        widthRatio =  4.5000 ;
        heightRatio =  9.0000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC65;
        sizeMM = QSizeF(114.00,229.00);
    }else if(formatStr == "EnvelopeC7"){
        widthRatio =  3.1933 ;
        heightRatio =  4.4867 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC7;
        sizeMM = QSizeF(81.00,114.00);
    }else if(formatStr == "Envelope9"){
        widthRatio =  3.8767 ;
        heightRatio =  8.8767 ;
        format = QPagedPaintDevice::PageSize::Envelope9;
        sizeMM = QSizeF(98.00,225.00);
    }else if(formatStr == "Envelope11"){
        widthRatio =  4.5000 ;
        heightRatio =  10.3767 ;
        format = QPagedPaintDevice::PageSize::Envelope11;
        sizeMM = QSizeF(114.00,264.00);
    }else if(formatStr == "Envelope12"){
        widthRatio =  4.7500 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::Envelope12;
        sizeMM = QSizeF(121.00,279.00);
    }else if(formatStr == "Envelope14"){
        widthRatio =  5.0000 ;
        heightRatio =  11.5000 ;
        format = QPagedPaintDevice::PageSize::Envelope14;
        sizeMM = QSizeF(127.00,292.00);
    }else if(formatStr == "EnvelopeMonarch"){
        widthRatio =  3.8767 ;
        heightRatio =  7.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopeMonarch;
        sizeMM = QSizeF(98.00,191.00);
    }else if(formatStr == "EnvelopePersonal"){
        widthRatio =  3.6267 ;
        heightRatio =  6.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopePersonal;
        sizeMM = QSizeF(92.00,165.00);
    }else if(formatStr == "EnvelopeChou3"){
        widthRatio =  4.7233 ;
        heightRatio =  9.2500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeChou3;
        sizeMM = QSizeF(120.00,235.00);
    }else if(formatStr == "EnvelopeChou4"){
        widthRatio =  3.5433 ;
        heightRatio =  8.0700 ;
        format = QPagedPaintDevice::PageSize::EnvelopeChou4;
        sizeMM = QSizeF(90.00,205.00);
    }else if(formatStr == "EnvelopeInvite"){
        widthRatio =  8.6667 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopeInvite;
        sizeMM = QSizeF(220.00,220.00);
    }else if(formatStr == "EnvelopeItalian"){
        widthRatio =  4.3333 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopeItalian;
        sizeMM = QSizeF(110.00,230.00);
    }else if(formatStr == "EnvelopeKaku2"){
        widthRatio =  9.4433 ;
        heightRatio =  13.0700 ;
        format = QPagedPaintDevice::PageSize::EnvelopeKaku2;
        sizeMM = QSizeF(240.00,332.00);
    }else if(formatStr == "EnvelopeKaku3"){
        widthRatio =  8.5000 ;
        heightRatio =  10.9033 ;
        format = QPagedPaintDevice::PageSize::EnvelopeKaku3;
        sizeMM = QSizeF(216.00,277.00);
    }else if(formatStr == "EnvelopePrc1"){
        widthRatio =  4.0133 ;
        heightRatio =  6.5000 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc1;
        sizeMM = QSizeF(102.00,165.00);
    }else if(formatStr == "EnvelopePrc2"){
        widthRatio =  4.0133 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc2;
        sizeMM = QSizeF(102.00,176.00);
    }else if(formatStr == "EnvelopePrc3"){
        widthRatio =  4.9167 ;
        heightRatio =  6.9300 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc3;
        sizeMM = QSizeF(125.00,176.00);
    }else if(formatStr == "EnvelopePrc4"){
        widthRatio =  4.3333 ;
        heightRatio =  8.1933 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc4;
        sizeMM = QSizeF(110.00,208.00);
    }else if(formatStr == "EnvelopePrc5"){
        widthRatio =  4.3333 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc5;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "EnvelopePrc6"){
        widthRatio =  4.7233 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc6;
        sizeMM = QSizeF(120.00,230.00);
    }else if(formatStr == "EnvelopePrc7"){
        widthRatio =  6.3067 ;
        heightRatio =  9.0567 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc7;
        sizeMM = QSizeF(160.00,230.00);
    }else if(formatStr == "EnvelopePrc8"){
        widthRatio =  4.7233 ;
        heightRatio =  12.1667 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc8;
        sizeMM = QSizeF(120.00,309.00);
    }else if(formatStr == "EnvelopePrc9"){
        widthRatio =  9.0133 ;
        heightRatio =  12.7500 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc9;
        sizeMM = QSizeF(229.00,324.00);
    }else if(formatStr == "EnvelopePrc10"){
        widthRatio =  12.7500 ;
        heightRatio =  18.0267 ;
        format = QPagedPaintDevice::PageSize::EnvelopePrc10;
        sizeMM = QSizeF(324.00,458.00);
    }else if(formatStr == "EnvelopeYou4"){
        widthRatio =  4.1400 ;
        heightRatio =  9.2500 ;
        format = QPagedPaintDevice::PageSize::EnvelopeYou4;
        sizeMM = QSizeF(105.00,235.00);
    }else if(formatStr == "AnsiA" || formatStr == "Letter"){
        widthRatio =  8.5000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiA;
        sizeMM = QSizeF(216.00,279.00);
    }else if(formatStr == "AnsiB" || formatStr == "Ledger"){
        widthRatio =  17.0000 ;
        heightRatio =  11.0000 ;
        format = QPagedPaintDevice::PageSize::AnsiB;
        sizeMM = QSizeF(432.00,279.00);
    }else if(formatStr == "EnvelopeC5" || formatStr == "C5E"){
        widthRatio =  6.3767 ;
        heightRatio =  9.0133 ;
        format = QPagedPaintDevice::PageSize::EnvelopeC5;
        sizeMM = QSizeF(162.00,229.00);
    }else if(formatStr == "EnvelopeDL" || formatStr == "DLE"){
        widthRatio =  4.3333 ;
        heightRatio =  8.6667 ;
        format = QPagedPaintDevice::PageSize::EnvelopeDL;
        sizeMM = QSizeF(110.00,220.00);
    }else if(formatStr == "Envelope10" || formatStr == "Comm10E"){
        widthRatio =  4.1267 ;
        heightRatio =  9.5000 ;
        format = QPagedPaintDevice::PageSize::Envelope10;
        sizeMM = QSizeF(105.00,241.00);
    }

    if(landScape){
        std::swap(widthRatio,heightRatio);
        sizeMM = QSizeF(sizeMM.height(), sizeMM.width());
    }
}


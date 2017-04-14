

#pragma once


// std
#include <memory>

// debug
#include <QDebug>

// Qt
#include <QImage>
#include <QList>
#include <QFont>
#include <QReadWriteLock>
#include <QPagedPaintDevice>
#include <QTextDocument>
#include <QPainter>
#include <QDate>

#include "RichTextUI.hpp"

namespace pc
{
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};

    struct RatioMargins
    {
        bool exteriorMarginsEnabled;
        bool interiorMarginsEnabled;
        qreal left;
        qreal right;
        qreal top;
        qreal bottom;
        qreal interWidth;
        qreal interHeight;
    };

    struct PreviewZonesDisplay
    {
        bool externMargins = false;
        bool InterMargins = false;
        bool titles = false;
        bool photos = false;
        bool consigns = false;
    };

    struct PaperFormat
    {        
        int dpi = 300;
        qreal widthRatio = 8.263;
        qreal heightRatio = 11.693;
        QPagedPaintDevice::PageSize format = QPagedPaintDevice::PageSize::A4;

        PaperFormat(){}

        PaperFormat(QString dpiStr, QString formatStr){

            dpi = dpiStr.toInt();

            if(formatStr ==  "A4" ){
                widthRatio =  8.263 ;
                heightRatio =  11.693 ;
                format = QPagedPaintDevice::PageSize:: A4 ;
            }else if(formatStr ==  "B5" ){
                widthRatio =  6.930 ;
                heightRatio =  9.847 ;
                format = QPagedPaintDevice::PageSize:: B5 ;
            }else if(formatStr ==  "Letter" ){
                widthRatio =  8.500 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Letter ;
            }else if(formatStr ==  "Legal" ){
                widthRatio =  8.500 ;
                heightRatio =  14.000 ;
                format = QPagedPaintDevice::PageSize:: Legal ;
            }else if(formatStr ==  "Executive" ){
                widthRatio =  7.500 ;
                heightRatio =  10.000 ;
                format = QPagedPaintDevice::PageSize:: Executive ;
            }else if(formatStr ==  "A0" ){
                widthRatio =  33.110 ;
                heightRatio =  46.807 ;
                format = QPagedPaintDevice::PageSize:: A0 ;
            }else if(formatStr ==  "A1" ){
                widthRatio =  23.390 ;
                heightRatio =  33.110 ;
                format = QPagedPaintDevice::PageSize:: A1 ;
            }else if(formatStr ==  "A2" ){
                widthRatio =  16.543 ;
                heightRatio =  23.390 ;
                format = QPagedPaintDevice::PageSize:: A2 ;
            }else if(formatStr ==  "A3" ){
                widthRatio =  11.693 ;
                heightRatio =  16.543 ;
                format = QPagedPaintDevice::PageSize:: A3 ;
            }else if(formatStr ==  "A5" ){
                widthRatio =  5.833 ;
                heightRatio =  8.263 ;
                format = QPagedPaintDevice::PageSize:: A5 ;
            }else if(formatStr ==  "A6" ){
                widthRatio =  4.127 ;
                heightRatio =  5.833 ;
                format = QPagedPaintDevice::PageSize:: A6 ;
            }else if(formatStr ==  "A7" ){
                widthRatio =  2.917 ;
                heightRatio =  4.127 ;
                format = QPagedPaintDevice::PageSize:: A7 ;
            }else if(formatStr ==  "A8" ){
                widthRatio =  2.057 ;
                heightRatio =  2.917 ;
                format = QPagedPaintDevice::PageSize:: A8 ;
            }else if(formatStr ==  "A9" ){
                widthRatio =  1.460 ;
                heightRatio =  2.057 ;
                format = QPagedPaintDevice::PageSize:: A9 ;
            }else if(formatStr ==  "B0" ){
                widthRatio =  39.377 ;
                heightRatio =  55.667 ;
                format = QPagedPaintDevice::PageSize:: B0 ;
            }else if(formatStr ==  "B1" ){
                widthRatio =  27.833 ;
                heightRatio =  39.377 ;
                format = QPagedPaintDevice::PageSize:: B1 ;
            }else if(formatStr ==  "B10" ){
                widthRatio =  1.223 ;
                heightRatio =  1.737 ;
                format = QPagedPaintDevice::PageSize:: B10 ;
            }else if(formatStr ==  "B2" ){
                widthRatio =  19.680 ;
                heightRatio =  27.833 ;
                format = QPagedPaintDevice::PageSize:: B2 ;
            }else if(formatStr ==  "B3" ){
                widthRatio =  13.903 ;
                heightRatio =  19.680 ;
                format = QPagedPaintDevice::PageSize:: B3 ;
            }else if(formatStr ==  "B4" ){
                widthRatio =  9.847 ;
                heightRatio =  13.903 ;
                format = QPagedPaintDevice::PageSize:: B4 ;
            }else if(formatStr ==  "B6" ){
                widthRatio =  4.917 ;
                heightRatio =  6.930 ;
                format = QPagedPaintDevice::PageSize:: B6 ;
            }else if(formatStr ==  "B7" ){
                widthRatio =  3.460 ;
                heightRatio =  4.917 ;
                format = QPagedPaintDevice::PageSize:: B7 ;
            }else if(formatStr ==  "B8" ){
                widthRatio =  2.443 ;
                heightRatio =  3.460 ;
                format = QPagedPaintDevice::PageSize:: B8 ;
            }else if(formatStr ==  "B9" ){
                widthRatio =  1.737 ;
                heightRatio =  2.443 ;
                format = QPagedPaintDevice::PageSize:: B9 ;
            }else if(formatStr ==  "C5E" ){
                widthRatio =  6.377 ;
                heightRatio =  9.013 ;
                format = QPagedPaintDevice::PageSize:: C5E ;
            }else if(formatStr ==  "Comm10E" ){
                widthRatio =  4.127 ;
                heightRatio =  9.500 ;
                format = QPagedPaintDevice::PageSize:: Comm10E ;
            }else if(formatStr ==  "DLE" ){
                widthRatio =  4.333 ;
                heightRatio =  8.667 ;
                format = QPagedPaintDevice::PageSize:: DLE ;
            }else if(formatStr ==  "Folio" ){
                widthRatio =  8.263 ;
                heightRatio =  12.987 ;
                format = QPagedPaintDevice::PageSize:: Folio ;
            }else if(formatStr ==  "Ledger" ){
                widthRatio =  17.000 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Ledger ;
            }else if(formatStr ==  "Tabloid" ){
                widthRatio =  11.000 ;
                heightRatio =  17.000 ;
                format = QPagedPaintDevice::PageSize:: Tabloid ;
            }else if(formatStr ==  "Custom" ){
                widthRatio =  0.000 ;
                heightRatio =  0.000 ;
                format = QPagedPaintDevice::PageSize:: Custom ;
            }else if(formatStr ==  "A10" ){
                widthRatio =  1.013 ;
                heightRatio =  1.460 ;
                format = QPagedPaintDevice::PageSize:: A10 ;
            }else if(formatStr ==  "A3Extra" ){
                widthRatio =  12.680 ;
                heightRatio =  17.527 ;
                format = QPagedPaintDevice::PageSize:: A3Extra ;
            }else if(formatStr ==  "A4Extra" ){
                widthRatio =  9.263 ;
                heightRatio =  12.693 ;
                format = QPagedPaintDevice::PageSize:: A4Extra ;
            }else if(formatStr ==  "A4Plus" ){
                widthRatio =  8.263 ;
                heightRatio =  13.000 ;
                format = QPagedPaintDevice::PageSize:: A4Plus ;
            }else if(formatStr ==  "A4Small" ){
                widthRatio =  8.263 ;
                heightRatio =  11.693 ;
                format = QPagedPaintDevice::PageSize:: A4Small ;
            }else if(formatStr ==  "A5Extra" ){
                widthRatio =  6.833 ;
                heightRatio =  9.277 ;
                format = QPagedPaintDevice::PageSize:: A5Extra ;
            }else if(formatStr ==  "B5Extra" ){
                widthRatio =  7.917 ;
                heightRatio =  10.860 ;
                format = QPagedPaintDevice::PageSize:: B5Extra ;
            }else if(formatStr ==  "JisB0" ){
                widthRatio =  40.557 ;
                heightRatio =  57.320 ;
                format = QPagedPaintDevice::PageSize:: JisB0 ;
            }else if(formatStr ==  "JisB1" ){
                widthRatio =  28.667 ;
                heightRatio =  40.557 ;
                format = QPagedPaintDevice::PageSize:: JisB1 ;
            }else if(formatStr ==  "JisB2" ){
                widthRatio =  20.277 ;
                heightRatio =  28.667 ;
                format = QPagedPaintDevice::PageSize:: JisB2 ;
            }else if(formatStr ==  "JisB3" ){
                widthRatio =  14.333 ;
                heightRatio =  20.277 ;
                format = QPagedPaintDevice::PageSize:: JisB3 ;
            }else if(formatStr ==  "JisB4" ){
                widthRatio =  10.127 ;
                heightRatio =  14.333 ;
                format = QPagedPaintDevice::PageSize:: JisB4 ;
            }else if(formatStr ==  "JisB5" ){
                widthRatio =  7.167 ;
                heightRatio =  10.127 ;
                format = QPagedPaintDevice::PageSize:: JisB5 ;
            }else if(formatStr ==  "JisB6" ){
                widthRatio =  5.043 ;
                heightRatio =  7.167 ;
                format = QPagedPaintDevice::PageSize:: JisB6 ;
            }else if(formatStr ==  "JisB7" ){
                widthRatio =  3.583 ;
                heightRatio =  5.043 ;
                format = QPagedPaintDevice::PageSize:: JisB7 ;
            }else if(formatStr ==  "JisB8" ){
                widthRatio =  2.513 ;
                heightRatio =  3.583 ;
                format = QPagedPaintDevice::PageSize:: JisB8 ;
            }else if(formatStr ==  "JisB9" ){
                widthRatio =  1.763 ;
                heightRatio =  2.513 ;
                format = QPagedPaintDevice::PageSize:: JisB9 ;
            }else if(formatStr ==  "JisB10" ){
                widthRatio =  1.263 ;
                heightRatio =  1.763 ;
                format = QPagedPaintDevice::PageSize:: JisB10 ;
            }else if(formatStr ==  "AnsiC" ){
                widthRatio =  17.000 ;
                heightRatio =  22.000 ;
                format = QPagedPaintDevice::PageSize:: AnsiC ;
            }else if(formatStr ==  "AnsiD" ){
                widthRatio =  22.000 ;
                heightRatio =  34.000 ;
                format = QPagedPaintDevice::PageSize:: AnsiD ;
            }else if(formatStr ==  "AnsiE" ){
                widthRatio =  34.000 ;
                heightRatio =  44.000 ;
                format = QPagedPaintDevice::PageSize:: AnsiE ;
            }else if(formatStr ==  "LegalExtra" ){
                widthRatio =  9.500 ;
                heightRatio =  15.000 ;
                format = QPagedPaintDevice::PageSize:: LegalExtra ;
            }else if(formatStr ==  "LetterExtra" ){
                widthRatio =  9.500 ;
                heightRatio =  12.000 ;
                format = QPagedPaintDevice::PageSize:: LetterExtra ;
            }else if(formatStr ==  "LetterPlus" ){
                widthRatio =  8.500 ;
                heightRatio =  12.693 ;
                format = QPagedPaintDevice::PageSize:: LetterPlus ;
            }else if(formatStr ==  "LetterSmall" ){
                widthRatio =  8.500 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: LetterSmall ;
            }else if(formatStr ==  "TabloidExtra" ){
                widthRatio =  12.000 ;
                heightRatio =  18.000 ;
                format = QPagedPaintDevice::PageSize:: TabloidExtra ;
            }else if(formatStr ==  "ArchA" ){
                widthRatio =  9.000 ;
                heightRatio =  12.000 ;
                format = QPagedPaintDevice::PageSize:: ArchA ;
            }else if(formatStr ==  "ArchB" ){
                widthRatio =  12.000 ;
                heightRatio =  18.000 ;
                format = QPagedPaintDevice::PageSize:: ArchB ;
            }else if(formatStr ==  "ArchC" ){
                widthRatio =  18.000 ;
                heightRatio =  24.000 ;
                format = QPagedPaintDevice::PageSize:: ArchC ;
            }else if(formatStr ==  "ArchD" ){
                widthRatio =  24.000 ;
                heightRatio =  36.000 ;
                format = QPagedPaintDevice::PageSize:: ArchD ;
            }else if(formatStr ==  "ArchE" ){
                widthRatio =  36.000 ;
                heightRatio =  48.000 ;
                format = QPagedPaintDevice::PageSize:: ArchE ;
            }else if(formatStr ==  "Imperial7x9" ){
                widthRatio =  7.000 ;
                heightRatio =  9.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial7x9 ;
            }else if(formatStr ==  "Imperial8x10" ){
                widthRatio =  8.000 ;
                heightRatio =  10.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial8x10 ;
            }else if(formatStr ==  "Imperial9x11" ){
                widthRatio =  9.000 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial9x11 ;
            }else if(formatStr ==  "Imperial9x12" ){
                widthRatio =  9.000 ;
                heightRatio =  12.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial9x12 ;
            }else if(formatStr ==  "Imperial10x11" ){
                widthRatio =  10.000 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial10x11 ;
            }else if(formatStr ==  "Imperial10x13" ){
                widthRatio =  10.000 ;
                heightRatio =  13.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial10x13 ;
            }else if(formatStr ==  "Imperial10x14" ){
                widthRatio =  10.000 ;
                heightRatio =  14.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial10x14 ;
            }else if(formatStr ==  "Imperial12x11" ){
                widthRatio =  12.000 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial12x11 ;
            }else if(formatStr ==  "Imperial15x11" ){
                widthRatio =  15.000 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Imperial15x11 ;
            }else if(formatStr ==  "ExecutiveStandard" ){
                widthRatio =  7.250 ;
                heightRatio =  10.500 ;
                format = QPagedPaintDevice::PageSize:: ExecutiveStandard ;
            }else if(formatStr ==  "Note" ){
                widthRatio =  8.500 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: Note ;
            }else if(formatStr ==  "Quarto" ){
                widthRatio =  8.473 ;
                heightRatio =  10.833 ;
                format = QPagedPaintDevice::PageSize:: Quarto ;
            }else if(formatStr ==  "Statement" ){
                widthRatio =  5.500 ;
                heightRatio =  8.500 ;
                format = QPagedPaintDevice::PageSize:: Statement ;
            }else if(formatStr ==  "SuperA" ){
                widthRatio =  8.930 ;
                heightRatio =  14.013 ;
                format = QPagedPaintDevice::PageSize:: SuperA ;
            }else if(formatStr ==  "SuperB" ){
                widthRatio =  12.000 ;
                heightRatio =  19.167 ;
                format = QPagedPaintDevice::PageSize:: SuperB ;
            }else if(formatStr ==  "Postcard" ){
                widthRatio =  3.943 ;
                heightRatio =  5.820 ;
                format = QPagedPaintDevice::PageSize:: Postcard ;
            }else if(formatStr ==  "DoublePostcard" ){
                widthRatio =  7.877 ;
                heightRatio =  5.820 ;
                format = QPagedPaintDevice::PageSize:: DoublePostcard ;
            }else if(formatStr ==  "Prc16K" ){
                widthRatio =  5.750 ;
                heightRatio =  8.473 ;
                format = QPagedPaintDevice::PageSize:: Prc16K ;
            }else if(formatStr ==  "Prc32K" ){
                widthRatio =  3.820 ;
                heightRatio =  5.943 ;
                format = QPagedPaintDevice::PageSize:: Prc32K ;
            }else if(formatStr ==  "Prc32KBig" ){
                widthRatio =  3.820 ;
                heightRatio =  5.943 ;
                format = QPagedPaintDevice::PageSize:: Prc32KBig ;
            }else if(formatStr ==  "FanFoldUS" ){
                widthRatio =  14.877 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize:: FanFoldUS ;
            }else if(formatStr ==  "FanFoldGerman" ){
                widthRatio =  8.500 ;
                heightRatio =  12.000 ;
                format = QPagedPaintDevice::PageSize:: FanFoldGerman ;
            }else if(formatStr ==  "FanFoldGermanLegal" ){
                widthRatio =  8.500 ;
                heightRatio =  13.000 ;
                format = QPagedPaintDevice::PageSize::FanFoldGermanLegal ;
            }else if(formatStr ==  "EnvelopeB4" ){
                widthRatio =  9.833 ;
                heightRatio =  13.903 ;
                format = QPagedPaintDevice::PageSize::EnvelopeB4 ;
            }else if(formatStr ==  "EnvelopeB5" ){
                widthRatio =  6.930 ;
                heightRatio =  9.847 ;
                format = QPagedPaintDevice::PageSize::EnvelopeB5 ;
            }else if(formatStr ==  "EnvelopeB6" ){
                widthRatio =  6.930 ;
                heightRatio =  4.917 ;
                format = QPagedPaintDevice::PageSize::EnvelopeB6 ;
            }else if(formatStr ==  "EnvelopeC0" ){
                widthRatio =  36.097 ;
                heightRatio =  51.057 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC0 ;
            }else if(formatStr ==  "EnvelopeC1" ){
                widthRatio =  25.513 ;
                heightRatio =  36.097 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC1 ;
            }else if(formatStr ==  "EnvelopeC2" ){
                widthRatio =  18.027 ;
                heightRatio =  25.513 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC2 ;
            }else if(formatStr ==  "EnvelopeC3" ){
                widthRatio =  12.750 ;
                heightRatio =  18.000 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC3 ;
            }else if(formatStr ==  "EnvelopeC4" ){
                widthRatio =  9.013 ;
                heightRatio =  12.750 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC4 ;
            }else if(formatStr ==  "EnvelopeC6" ){
                widthRatio =  4.487 ;
                heightRatio =  6.377 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC6 ;
            }else if(formatStr ==  "EnvelopeC65" ){
                widthRatio =  4.500 ;
                heightRatio =  9.000 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC65 ;
            }else if(formatStr ==  "EnvelopeC7" ){
                widthRatio =  3.193 ;
                heightRatio =  4.487 ;
                format = QPagedPaintDevice::PageSize::EnvelopeC7 ;
            }else if(formatStr ==  "Envelope9" ){
                widthRatio =  3.877 ;
                heightRatio =  8.877 ;
                format = QPagedPaintDevice::PageSize::Envelope9 ;
            }else if(formatStr ==  "Envelope11" ){
                widthRatio =  4.500 ;
                heightRatio =  10.377 ;
                format = QPagedPaintDevice::PageSize::Envelope11 ;
            }else if(formatStr ==  "Envelope12" ){
                widthRatio =  4.750 ;
                heightRatio =  11.000 ;
                format = QPagedPaintDevice::PageSize::Envelope12 ;
            }else if(formatStr ==  "Envelope14" ){
                widthRatio =  5.000 ;
                heightRatio =  11.500 ;
                format = QPagedPaintDevice::PageSize::Envelope14 ;
            }else if(formatStr ==  "EnvelopeMonarch" ){
                widthRatio =  3.877 ;
                heightRatio =  7.500 ;
                format = QPagedPaintDevice::PageSize::EnvelopeMonarch ;
            }else if(formatStr ==  "EnvelopePersonal" ){
                widthRatio =  3.627 ;
                heightRatio =  6.500 ;
                format = QPagedPaintDevice::PageSize::EnvelopePersonal ;
            }else if(formatStr ==  "EnvelopeChou3" ){
                widthRatio =  4.723 ;
                heightRatio =  9.250 ;
                format = QPagedPaintDevice::PageSize::EnvelopeChou3 ;
            }else if(formatStr ==  "EnvelopeChou4" ){
                widthRatio =  3.543 ;
                heightRatio =  8.070 ;
                format = QPagedPaintDevice::PageSize::EnvelopeChou4 ;
            }else if(formatStr ==  "EnvelopeInvite" ){
                widthRatio =  8.667 ;
                heightRatio =  8.667 ;
                format = QPagedPaintDevice::PageSize::EnvelopeInvite ;
            }else if(formatStr ==  "EnvelopeItalian" ){
                widthRatio =  4.333 ;
                heightRatio =  9.057 ;
                format = QPagedPaintDevice::PageSize::EnvelopeItalian ;
            }else if(formatStr ==  "EnvelopeKaku2" ){
                widthRatio =  9.443 ;
                heightRatio =  13.070 ;
                format = QPagedPaintDevice::PageSize::EnvelopeKaku2 ;
            }else if(formatStr ==  "EnvelopeKaku3" ){
                widthRatio =  8.500 ;
                heightRatio =  10.903 ;
                format = QPagedPaintDevice::PageSize::EnvelopeKaku3 ;
            }else if(formatStr ==  "EnvelopePrc1" ){
                widthRatio =  4.013 ;
                heightRatio =  6.500 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc1 ;
            }else if(formatStr ==  "EnvelopePrc2" ){
                widthRatio =  4.013 ;
                heightRatio =  6.930 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc2 ;
            }else if(formatStr ==  "EnvelopePrc3" ){
                widthRatio =  4.917 ;
                heightRatio =  6.930 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc3 ;
            }else if(formatStr ==  "EnvelopePrc4" ){
                widthRatio =  4.333 ;
                heightRatio =  8.193 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc4 ;
            }else if(formatStr ==  "EnvelopePrc5" ){
                widthRatio =  4.333 ;
                heightRatio =  8.667 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc5 ;
            }else if(formatStr ==  "EnvelopePrc6" ){
                widthRatio =  4.723 ;
                heightRatio =  9.057 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc6 ;
            }else if(formatStr ==  "EnvelopePrc7" ){
                widthRatio =  6.307 ;
                heightRatio =  9.057 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc7 ;
            }else if(formatStr ==  "EnvelopePrc8" ){
                widthRatio =  4.723 ;
                heightRatio =  12.167 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc8 ;
            }else if(formatStr ==  "EnvelopePrc9" ){
                widthRatio =  9.013 ;
                heightRatio =  12.750 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc9 ;
            }else if(formatStr ==  "EnvelopePrc10" ){
                widthRatio =  12.750 ;
                heightRatio =  18.027 ;
                format = QPagedPaintDevice::PageSize::EnvelopePrc10 ;
            }else if(formatStr ==  "EnvelopeYou4" ){
                widthRatio =  4.140 ;
                heightRatio =  9.250 ;
                format = QPagedPaintDevice::PageSize::EnvelopeYou4 ;
            }
        }

        int64_t widthPixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(widthRatio*wantedDpi);
        }

        int64_t heightPixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(heightRatio*wantedDpi);
        }
    };

    struct Photo;
    using SPhoto  = std::shared_ptr<Photo>;
    using Photos  = QList<SPhoto>;
    using SPhotos = std::shared_ptr<Photos>;

    struct Consign;
    using SConsign = std::shared_ptr<Consign>;

    struct Title;
    using STitle = std::shared_ptr<Title>;

    struct PCSet;
    using SPCSet = std::shared_ptr<PCSet>;

    struct PCPage;
    using SPCPage = std::shared_ptr<PCPage>;    

    struct PCPages;
    using SPCPages = std::shared_ptr<PCPages>;


    struct RectPageItem{
        QRectF rectOnPage;
        virtual void compute_sizes(QRectF upperRect) = 0;
    };

    struct Photo : public RectPageItem {
        Photo() = delete;
        Photo(const QString &path) : pathPhoto(path)
        {
            constexpr int maxHeight = 800;
            constexpr int maxWidth = 800;

            scaledPhoto = QImage(path);
            originalSize= scaledPhoto.size();
            if(!scaledPhoto.isNull())
            {
                namePhoto = pathPhoto.split('/').last().split('.').first();

                if(scaledPhoto.size().width() > maxWidth)
                    scaledPhoto = scaledPhoto.scaledToWidth(maxWidth);
                if(scaledPhoto.size().height() > maxHeight)
                    scaledPhoto = scaledPhoto.scaledToHeight(maxHeight);
            }else{
                namePhoto = "Erreur";
                qWarning() << "-Error: photo not loaded: " << pathPhoto;
            }
        }

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }

        QSize size() const noexcept {return originalSize;}
        QSize scaled_size() const noexcept {return scaledPhoto.size();}

        void draw(QPainter &painter, const QRectF &rectPhoto, bool preview){

            if(scaledPhoto.isNull()){
                qWarning() << "-Error: photo is null, can't be drawn";
                return;
            }

            if(preview){
                draw(painter, rectPhoto, scaledPhoto, true);
            }else{
                if(rectPhoto.width() > 5000 || rectPhoto.height() > 5000){
                    draw_huge(painter, rectPhoto);
                }else{
                    draw(painter, rectPhoto, QImage(pathPhoto), false);
                }
            }

        }

    private:

        void draw(QPainter &painter, QRectF rectPhoto, QImage photo, bool preview){

            QImage rotatedPhoto = preview ? photo.scaled(rectPhoto.width(),rectPhoto.height(), Qt::KeepAspectRatio) :
                                           (photo.transformed(QTransform().rotate(rotation))).scaled(rectPhoto.width(),rectPhoto.height(), Qt::KeepAspectRatio);

            qreal newX     = rectPhoto.x(), newY = rectPhoto.y();
            qreal newWidth = rotatedPhoto.width(), newHeight = rotatedPhoto.height();
            if(rotatedPhoto.width() < rectPhoto.width()){
                if(alignment & Qt::AlignRight){
                    newX     += rectPhoto.width()-rotatedPhoto.width();
                }
                else if(alignment & Qt::AlignHCenter){
                    newX     += rectPhoto.width()*0.5  - rotatedPhoto.width()*0.5;
                }
            }
            if(rotatedPhoto.height() < rectPhoto.height()){
                if(alignment & Qt::AlignBottom){
                    newY      += rectPhoto.height()-rotatedPhoto.height();
                }
                else if(alignment & Qt::AlignVCenter){
                    newY      += rectPhoto.height()*0.5-rotatedPhoto.height()*0.5;
                }
            }

            if((rectPhoto.width()-rotatedPhoto.width()) > (rectPhoto.height()-rotatedPhoto.height())){
                newHeight = rectPhoto.height();
                newY      = rectPhoto.y();
            }else{
                newWidth = rectPhoto.width();
                newX     = rectPhoto.x();
            }

            // draw image
            painter.drawImage(QRectF(newX, newY, newWidth, newHeight), rotatedPhoto, QRectF(0.,0.,rotatedPhoto.width(),rotatedPhoto.height()));
        }

        void draw_huge(QPainter &painter, QRectF rectPhoto){

            QImage rotatedPhoto = QImage(pathPhoto).transformed(QTransform().rotate(rotation));
            QSizeF upscaledSize(rectPhoto.width(),rectPhoto.height());
            QSizeF size = rotatedPhoto.size();

            qreal ratioW = 1.*size.width() / upscaledSize.width();
            qreal ratioH = 1.*size.height() / upscaledSize.height();

            if(ratioW > ratioH){
                upscaledSize = QSizeF(upscaledSize.width(), size.height()/ratioW);
            }else{
                upscaledSize = QSizeF(size.width()/ratioH, upscaledSize.height());
            }


            // alignment
            qreal newX = rectPhoto.x(), newY = rectPhoto.y();
            if(upscaledSize.width() < rectPhoto.width()){
                if(alignment & Qt::AlignRight){
                    newX = rectPhoto.x() + rectPhoto.width()-upscaledSize.width();
                }
                if(alignment & Qt::AlignHCenter){
                    newX = rectPhoto.x() + rectPhoto.width()*0.5 - upscaledSize.width()*0.5;
                }
            }
            if(upscaledSize.height() < rectPhoto.height()){
                if(alignment & Qt::AlignBottom){
                    newY = rectPhoto.y() + rectPhoto.height()-upscaledSize.height();
                }
                if(alignment & Qt::AlignVCenter){
                    newY = rectPhoto.y() + rectPhoto.height()*0.5-upscaledSize.height()*0.5;
                }
            }
            rectPhoto = QRectF(newX,newY,rectPhoto.width(),rectPhoto.height());

//            qreal newX     = rectPhoto.x(), newY = rectPhoto.y();
//            qreal newWidth = rotatedPhoto.width(), newHeight = rotatedPhoto.height();
//            if(rotatedPhoto.width() < rectPhoto.width()){
//                if(alignment & Qt::AlignRight){
//                    newX     += rectPhoto.width()-rotatedPhoto.width();
//                }
//                else if(alignment & Qt::AlignHCenter){
//                    newX     += rectPhoto.width()*0.5  - rotatedPhoto.width()*0.5;
//                }
//            }
//            if(rotatedPhoto.height() < rectPhoto.height()){
//                if(alignment & Qt::AlignBottom){
//                    newY      += rectPhoto.height()-rotatedPhoto.height();
//                }
//                else if(alignment & Qt::AlignVCenter){
//                    newY      += rectPhoto.height()*0.5-rotatedPhoto.height()*0.5;
//                }
//            }

//            if((rectPhoto.width()-rotatedPhoto.width()) > (rectPhoto.height()-rotatedPhoto.height())){
//                newHeight = rectPhoto.height();
//                newY      = rectPhoto.y();
//            }else{
//                newWidth = rectPhoto.width();
//                newX     = rectPhoto.x();
//            }
//            QRectF target(newX, newY, newWidth, newHeight);


            // draw tiles
            int widthUpPerTile  = (upscaledSize.width() /10);
            int heightUpPerTile = (upscaledSize.height()/10);

            int widthUpTileRemainder  = static_cast<int>(upscaledSize.width())%widthUpPerTile;
            int heightUpTileRemainder = static_cast<int>(upscaledSize.height())%heightUpPerTile;

            int nbTilesH            = upscaledSize.width()/widthUpPerTile;
            int widthPerTile        = size.width()/nbTilesH;
            int widthTileRemainder  = static_cast<int>(size.width())%widthPerTile;

            int nbTilesV            = upscaledSize.height()/heightUpPerTile;
            int heightPerTile       = size.height()/nbTilesV;
            int heightTileRemainder = static_cast<int>(size.height())%heightPerTile;

            int nbTotalTilesH = nbTilesH + (widthTileRemainder  > 0 ? 1:0);
            int nbTotalTilesV = nbTilesV + (heightTileRemainder > 0 ? 1:0);

            for(int ii = 0; ii < nbTotalTilesH; ++ii){

                bool lastTileH   = (widthTileRemainder > 0) && (ii == nbTotalTilesH-1);
                bool lastTileUpH = (widthUpTileRemainder > 0) && (ii == nbTotalTilesH-1);

                for (int jj = 0; jj < nbTotalTilesV; ++jj){

                    bool lastTileV = (heightTileRemainder > 0) && (jj == nbTotalTilesV-1);
                    bool lastTileUpV = (heightUpTileRemainder > 0) && (jj == nbTotalTilesV-1);
                    QRectF itRect(ii* widthPerTile, jj * heightPerTile, lastTileH ? widthTileRemainder : widthPerTile, lastTileV ?  heightTileRemainder : heightPerTile);
                    QRectF itUpRect(ii* widthUpPerTile, jj * heightUpPerTile, lastTileUpH ? widthUpTileRemainder : widthUpPerTile, lastTileUpV ? heightUpTileRemainder : heightUpPerTile);
                    size_t offset = itRect.x() * rotatedPhoto.depth() / 8 + itRect.y() * rotatedPhoto.bytesPerLine();

//                    painter.drawImage(target, QImage(rotatedPhoto.bits() + offset, itRect.width(),itRect.height(),
//                                                    rotatedPhoto.bytesPerLine(), rotatedPhoto.format()).scaled(itUpRect.width(),itUpRect.height()));
                    painter.drawImage(rectPhoto.x()+itUpRect.x(),rectPhoto.y()+itUpRect.y(), QImage(rotatedPhoto.bits() + offset, itRect.width(),itRect.height(),
                                      rotatedPhoto.bytesPerLine(), rotatedPhoto.format()).scaled(itUpRect.width(),itUpRect.height()));
                }
            }
        }

    public:
        bool isRemoved = false;
        int alignment;
        int rotation = 0;

        QSize originalSize;
        QImage scaledPhoto;
        QString pathPhoto;
        QString namePhoto;
    };

    struct Consign : public RectPageItem {
        QTextDocument *doc = nullptr;

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Title : public RectPageItem {
        QTextDocument *doc = nullptr;

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct PCSet : public RectPageItem{

        SPhoto photo = nullptr;
        SConsign consign = nullptr;

        bool consignOnPhoto;
        qreal ratio;
        Position consignPositionFromPhoto;

        int id;
        int totalId;

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);

            //  consign
            qreal heightConsigneV = rectOnPage.height() * (1. - ratio);
            qreal widthConsigneV  = rectOnPage.width();
            qreal heightConsigneH = rectOnPage.height();
            qreal widthConsigneH  = rectOnPage.width() * (1. - ratio);
            //  photo
            qreal heightPhotoV    = ratio * rectOnPage.height();
            qreal widthPhotoV     = rectOnPage.width();
            qreal heightPhotoH    = rectOnPage.height();
            qreal widthPhotoH     = ratio * rectOnPage.width();

            QRectF consignRect, photoRect;
            if(consignOnPhoto){
                photoRect = consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), rectOnPage.width(), rectOnPage.height());
            }else{
                switch (consignPositionFromPhoto) {
                case Position::top:
                    consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), widthConsigneV, heightConsigneV);
                    photoRect   = QRectF(rectOnPage.x(), rectOnPage.y() + heightConsigneV, widthPhotoV, heightPhotoV);
                    break;
                case Position::left:
                    consignRect = QRectF(rectOnPage.x(), rectOnPage.y(), widthConsigneH, heightConsigneH);
                    photoRect   = QRectF(rectOnPage.x() + widthConsigneH, rectOnPage.y(), widthPhotoH, heightPhotoH);
                    break;
                case Position::right:
                    consignRect = QRectF(rectOnPage.x() + widthPhotoH, rectOnPage.y(), widthConsigneH, heightConsigneH);
                    photoRect   = QRectF(rectOnPage.x(), rectOnPage.y(), widthPhotoH, heightPhotoH);
                    break;
                case Position::bottom:
                    consignRect = QRectF(rectOnPage.x(), rectOnPage.y() + heightPhotoV, widthConsigneV, heightConsigneV);
                    photoRect   = QRectF(rectOnPage.x(), rectOnPage.y(), widthPhotoV, heightPhotoV);
                    break;
                default:
                    break;
                }
            }

            consign->compute_sizes(consignRect);
            photo->compute_sizes(photoRect);
        }
    };

    struct PCPage : public RectPageItem{

        qreal ratioWithTitle;
        STitle title = nullptr;
        QVector<SPCSet> sets;
        QVector<QRectF> interMarginsRects;
        QVector<QLineF>  horizontalCutLines;
        QVector<QLineF>  verticalCutLines;

        bool displayCutLines;
        int nbPhotosV;
        int nbPhotosH;
        int id;
        Position titlePositionFromPC;
//        PageOrientation orientation;
        RatioMargins margins;

        QRectF setsAndMarginsRect;
        QRectF setsRect;


        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);

            // title rect and sets with margins rect
            QRectF titleRect(0.,0.,0.,0.);
            if(title != nullptr){
                if(titlePositionFromPC == Position::on){                    
                    titleRect = rectOnPage;
                    setsAndMarginsRect = rectOnPage;
                }else{
                    qreal heightTitle = ratioWithTitle * rectOnPage.height();
                    titleRect = QRectF(0, (titlePositionFromPC == Position::top) ? 0 : rectOnPage.height() - heightTitle,
                                       rectOnPage.width(), heightTitle);
                    setsAndMarginsRect = QRectF(0., (titlePositionFromPC == Position::top) ? titleRect.height() : 0.,
                                                rectOnPage.width(), rectOnPage.height()- titleRect.height());
                }

                title->compute_sizes(QRectF(titleRect));
            }else{
                setsAndMarginsRect = rectOnPage;
            }

            // extern margins
            qreal heightTopMargin = 0., heightBottomMargin = 0., widthLeftMargin = 0., widthRightMargin = 0.;
            if(margins.exteriorMarginsEnabled){
                heightTopMargin     = setsAndMarginsRect.height() * margins.top;
                heightBottomMargin  = setsAndMarginsRect.height() * margins.bottom;
                widthLeftMargin     = setsAndMarginsRect.width()  * margins.left;
                widthRightMargin    = setsAndMarginsRect.width()  * margins.right;
            }

            // sets
            setsRect = QRectF(setsAndMarginsRect.x() + widthLeftMargin, setsAndMarginsRect.y() + heightTopMargin,
                              setsAndMarginsRect.width() - widthLeftMargin - widthRightMargin, setsAndMarginsRect.height() - heightTopMargin - heightBottomMargin);

            int nbInterVMargins     = nbPhotosV - 1;
            int nbInterHMargins     = nbPhotosH - 1;
            qreal heightInterMargin = 0., widthInterMargin = 0.;
            if(margins.interiorMarginsEnabled){
                if(nbInterVMargins > 0){
                    heightInterMargin = setsRect.height()/nbInterVMargins * margins.interHeight;
                }
                if(nbInterHMargins > 0){
                    widthInterMargin  = setsRect.width()/nbInterHMargins  * margins.interWidth;
                }
            }

            // PC
            qreal widthPhotoAndConsign   = (setsRect.width()  - nbInterHMargins * widthInterMargin)  / nbPhotosH;
            qreal heightPhotoAndConsign  = (setsRect.height() - nbInterVMargins * heightInterMargin) / nbPhotosV;

            int currPC = 0;
            interMarginsRects.clear();
            interMarginsRects.reserve(sets.size());
            qreal offsetV = setsRect.y();
            for(int ii = 0; ii < nbPhotosV; ++ii){
                qreal offsetH = setsRect.x();
                for(int jj = 0; jj < nbPhotosH; ++jj){
                    if(currPC >= sets.size())
                        break;

                    sets[currPC]->compute_sizes(QRectF(offsetH, offsetV, widthPhotoAndConsign, heightPhotoAndConsign));                    
                    interMarginsRects.push_back(QRectF(offsetH, offsetV,
                                                      (jj < nbPhotosH-1) ? widthPhotoAndConsign  + widthInterMargin  : widthPhotoAndConsign,
                                                      (ii < nbPhotosV-1) ? heightPhotoAndConsign + heightInterMargin : heightPhotoAndConsign));

                    offsetH += widthPhotoAndConsign + widthInterMargin;
                    ++currPC;
                }

                offsetV += heightPhotoAndConsign + heightInterMargin;
            }

            // horizontal cut lines
            horizontalCutLines.clear();
            horizontalCutLines.push_back(QLineF(QPointF(0,setsRect.y()), QPointF(rectOnPage.width(),setsRect.y())));
            horizontalCutLines.push_back(QLineF(QPointF(0,setsRect.y()+setsRect.height()), QPointF(rectOnPage.width(),setsRect.y()+setsRect.height())));

            for(int ii = 1; ii < nbPhotosV; ++ii){
                qreal startV = setsRect.y() + ii*(heightPhotoAndConsign + heightInterMargin) - 0.5*heightInterMargin;
                horizontalCutLines.push_back(QLineF(QPointF(0,startV), QPointF(rectOnPage.width(),startV)));
            }

            // vertical cut lines
            verticalCutLines.clear();
            verticalCutLines.push_back(QLineF(QPointF(widthLeftMargin ,setsRect.y()), QPointF(widthLeftMargin, setsRect.y() + setsRect.height())));
            verticalCutLines.push_back(QLineF(QPointF(rectOnPage.width()-widthLeftMargin,setsRect.y()), QPointF(rectOnPage.width()-widthRightMargin, setsRect.y() + setsRect.height())));
            for(int ii = 1; ii < nbPhotosH; ++ii){
                qreal startH = setsRect.x() + ii*(widthPhotoAndConsign + widthInterMargin) - 0.5*widthInterMargin;
                verticalCutLines.push_back(QLineF(QPointF(startH,setsRect.y()), QPointF(startH, setsRect.y() + setsRect.height())));
            }
        }
    };

    struct PCPages{

        // paths        
        QString pdfFileName = "";

        PaperFormat paperFormat;
        QVector<SPCPage> pages;        

        int page_photos_number(int index) const{
            return pages[index]->sets.size();
        }

        void compute_all_pages_sizes(int width, int height){
            for(auto &&page : pages)
                page->compute_sizes(QRectF(0,0,width,height));
        }
    };

    struct GlobalParameters{

        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;
        bool displayZones = false;

        int nbPhotosPageH;
        int nbPhotosPageV;
        int lastPagePhotosNb;
        int nbPhotosPerPage;

        int currentPageId = 0;
        int currentIdImageDisplayed = 0;
        int currentPCDisplayed = 0;

        // global
        // # misc
        bool globalDisplayCutLines;
        int globalPhotosAlignment;
        qreal globalPCRatio;

        Position globalConsignPositionFromPhotos;
        Position previousGlobalConsignPositionFromPhotos;
        QVector<Position> previousIndividualConsignPositionFromPhotos;

        PageOrientation globalOrientation;
        RatioMargins globalMargins;
        // # misc consign
        bool globalConsignOnPhoto;
        TextEdit *globalConsignDoc = nullptr;
        QVector<TextEdit *> consignsDoc;

        // title
        bool titleAdded;
        bool titleOnAllPages;
        qreal globalRatioTitle;
        Position globalTitlePositionFromPC;
        TextEdit *titleDoc = nullptr;

        QString photosDirectoryPath= "";

        SPhotos photosLoaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
        SPhotos photosValided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    };


    struct ExtraPCInfo{

        ExtraPCInfo(){}

        QString namePCAssociatedPhoto = "";
        int pageNum       = -1;
        int pagesNb       = -1;
        int photoNum      = -1;
        int photoPCNum    = -1;
        int photoTotalNum = -1;
    };

    static void draw_doc_html_with_size_factor(QPainter &painter, QReadWriteLock *docLocker, QTextDocument *doc, QRectF upperRect, QRectF docRect, qreal sizeFactor, ExtraPCInfo infos = ExtraPCInfo()){

        qDebug() << "start draw_doc_html_with_size_factor";
        QImage pix(QSize(docRect.width(),docRect.height()), QImage::Format_ARGB32);
        pix.fill(QColor(255,255,255,0));
        painter.drawImage(docRect, pix);

        QImage pixDoc(QSize(upperRect.width(),upperRect.height()), QImage::Format_ARGB32);
        pixDoc.fill(QColor(255,255,255,0));

        QPainter painterDoc(&pixDoc);
        painterDoc.setPen(QPen());


//        QTextDocument *testDoc = textEdit.document()->clone(nullptr);
//        std::unique_ptr<QTextDocument> testDoc = std::unique_ptr<QTextDocument>(textEdit.document()->clone(nullptr));
        docLocker->lockForRead();
        doc = doc->clone(nullptr);
        docLocker->unlock();

        doc->setPageSize(QSizeF(upperRect.width(), upperRect.height()));
        QString html = doc->toHtml();
        int index = 0;
        html = html.replace("family:'Helvetica'; font-size:", "#B_#B_#B_#B_");
        QVector<qreal> sizes;
        while (index != -1){
            index = html.indexOf(QString("font-size:"));
            if(index == -1)
                break;

            int indexEndImg = html.indexOf("pt;", index)+3;
            QString subString = html.mid(index, indexEndImg - index);
            qreal sizePolice = (subString.mid(10, subString.size()-13)).toDouble();

            qreal newPoliceSize = (sizePolice*sizeFactor);
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

        html = html.replace("#B_#B_#B_#B_", "family:'Helvetica'; font-size:");
        index = 0;
        while(index != -1){
            index = html.indexOf(QString("$name$"));
            if(index == -1)
                break;
            html = html.remove(index, 6);
            html = html.insert(index, infos.namePCAssociatedPhoto);
        }

        index = 0;
        while(index != -1){
            index = html.indexOf(QString("$date$"));
            if(index == -1)
                break;
            html = html.remove(index, 6);
            html = html.insert(index, QDate::currentDate().toString("dd.MM.yyyy"));
        }

        index = 0;
        while(index != -1){
            index = html.indexOf(QString("$num_page$"));
            if(index == -1)
                break;
            html = html.remove(index, 10);
            html = html.insert(index, QString::number(infos.pageNum+1) + "/" + QString::number(infos.pagesNb));
        }

        index = 0;
        while(index != -1){
            index = html.indexOf(QString("$num_photo$"));
            if(index == -1)
                break;
            html = html.remove(index, 11);
            html = html.insert(index, QString::number(infos.photoNum+1) + "/" + QString::number(infos.photoTotalNum));
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
                                + " width=\""+ QString::number(sizeFactor* onlyWidth.toFloat())
                                + "\" height=\"" + QString::number(sizeFactor* onlyHeight.toFloat())+ "\" />");
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

        doc->setHtml(html);
        doc->drawContents(&painterDoc);
        if(pixDoc.width() > 0){
            QImage cropped = pixDoc.copy(0,0, docRect.width(), docRect.height());
            painter.drawImage(QRectF(docRect.x(),docRect.y(),docRect.width(),docRect.height()), cropped);
        }

        delete doc;
        qDebug() << "end draw_doc_html_with_size_factor";
    }
}

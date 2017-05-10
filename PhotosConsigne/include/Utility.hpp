
/**
 * \file Utility.hpp
 * \brief ...
 * \author Florian Lance
 * \date 04/04/2017
 */


#pragma once


// std
#include <memory>

// debug
#include <QDebug>
#include <QElapsedTimer>

// local
// # ui
#include "RichTextUI.hpp"
#include "ImageLabel.hpp"

// Qt
#include <QImage>
#include <QList>
#include <QFont>
#include <QReadWriteLock>
#include <QPagedPaintDevice>
#include <QTextDocument>
#include <QPainter>
#include <QDate>



namespace pc
{
    // define aliases
    struct Photo;
    using SPhoto  = std::shared_ptr<Photo>;

    using Photos  = QList<SPhoto>;
    using SPhotos = std::shared_ptr<Photos>;

    struct Consign;
    using SConsign = std::shared_ptr<Consign>;

    struct Title;                          // ############ TO BE REMOVED
    using STitle = std::shared_ptr<Title>; // ############ TO BE REMOVED

    struct Header;
    using SHeader = std::shared_ptr<Header>;

    struct Footer;
    using SFooter = std::shared_ptr<Footer>;

    struct PCSet;
    using SPCSet = std::shared_ptr<PCSet>;

    struct PCPage;
    using SPCPage = std::shared_ptr<PCPage>;

    struct PCPages;
    using SPCPages = std::shared_ptr<PCPages>;

    // define enums
    enum class PageOrientation { landScape = 0, portrait = 1};
    enum class Position { top = 0, bottom = 1, left = 2, right = 3, on = 4};

    // define classes
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

    struct PaperFormat
    {        
        int dpi = 300;
        qreal widthRatio = 8.263;
        qreal heightRatio = 11.693;
        QSizeF sizeMM;
        QPagedPaintDevice::PageSize format = QPagedPaintDevice::PageSize::A4;        

        PaperFormat(){}

        PaperFormat(QString dpiStr, QString formatStr);

        int64_t widthPixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(widthRatio*wantedDpi);
        }

        int64_t heightPixels(int wantedDpi) const noexcept{
            return static_cast<int64_t>(heightRatio*wantedDpi);
        }
    };


    struct RectPageItem{
        QRectF rectOnPage;
        virtual void compute_sizes(QRectF upperRect) = 0;
    };

    struct Photo : public RectPageItem {

        Photo() = delete;   

        Photo(const Photo &photo) = default;

        Photo(const QString &path, bool isWhiteSpace = false);

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }

        QSize size() const noexcept {return originalSize;}

        QSize scaled_size() const noexcept {return scaledPhoto.size();}

        void draw(QPainter &painter, const QRectF &rectPhoto, bool preview, bool drawImageSize, const PaperFormat &format, const QRectF &pageRect);

    private:

        void draw(QPainter &painter, const QRectF &rectPhoto, const QImage &photo, bool preview, bool drawImageSize, PaperFormat format, const QRectF &pageRect);

        void draw_huge(QPainter &painter, const QRectF &rectPhoto);

    public:

        bool isWhiteSpace = false;
        bool isADuplicate = false;
        bool isRemoved    = false;
        bool isOnDocument = false;
        int alignment;
        int rotation = 0;
        int globalId = 0;

        QSize originalSize;
        QImage scaledPhoto;
        QString pathPhoto;
        QString namePhoto;
        QFileInfo info;
    };



    struct ExtraPCInfo{

        ExtraPCInfo(){}

        QFileInfo fileInfo;
        QString namePCAssociatedPhoto = "";
        int pageNum       = -1;
        int pagesNb       = -1;
        int photoNum      = -1;
        int photoPCNum    = -1;
        int photoTotalNum = -1;
        QColor pageColor = Qt::white;
    };


    struct Consign : public RectPageItem {

        std::shared_ptr<QString> html = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Title : public RectPageItem { // ############ TO BE REMOVED

        std::shared_ptr<QString> html = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Header : public RectPageItem {

        std::shared_ptr<QString> html = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Footer : public RectPageItem {

        std::shared_ptr<QString> html = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct PCSet : public RectPageItem{

        int id; /**< id of the set in its page */
        int totalId; /**< global id of the set */

        SPhoto photo = nullptr;
        SConsign consign = nullptr;

        bool consignOnPhoto;
        qreal ratio;
        Position consignPositionFromPhoto;

        void compute_sizes(QRectF upperRect);
    };

    struct PCPage : public RectPageItem{

        int id; /**< id of the page */

    //        int nbPhotos;
        int nbPhotosV;
        int nbPhotosH;
        qreal ratioWithTitle;
        RatioMargins margins;

        QColor backgroundColor;
        SHeader header = nullptr;
        SFooter footer = nullptr;
        Position titlePositionFromPC; // ############ TO BE REMOVED
        STitle title = nullptr;       // ############ TO BE REMOVED
        QVector<SPCSet> sets;

        QRectF setsRect;
        QRectF setsAndMarginsRect;
        QVector<QRectF> interMarginsRects;

        void compute_sizes(QRectF upperRect);
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



    struct GlobalData{

        bool resetPages          = false;   /**< if true, the number of pages will be reseted and initialized in function of the number of valid photos and H/V parameters */

        bool grayScale           = false;
        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;
        bool displayZones        = false;

        int nbPages;                        /**< number of pages for the document */
        int nbPhotosPageH;                  /**< size of the grid horizontally */
        int nbPhotosPageV;                  /**< size of the grid vertically */
        int nbPhotosPerPage;                /**< number of photos per page (< H*V) */
        int lastPagePhotosNb;               /**< ######################################## TO BE REMOVED*/

        int previousPhotoId     = -1;
        int previousPageId      = -2;
        int currentPageId       = -1;       /**< id of the current selected page (page list widget) */
        int currentPhotoId      = -1;        /**< id of the current selcted photo (photo list widget) */
        int currentPCDisplayed  = -1;        /**< id of the current PC */

        // global
        // # misc
        int photoAlignment;
        qreal PCRation;
        QColor pageColor = Qt::white;        

        Position consignPositionFromPhotos;

        PageOrientation orientation;
        RatioMargins margins;

        // # misc consign
        bool consignOnPhoto;

        // border
        bool displayBorders = false;
        qreal widthBorder = 1.;
        QPen borderPen;

        // title
        bool titleAdded;
        bool titleOnAllPages;
        qreal ratioTitle;
        Position titlePositionFromPC;

        QString lastPhotosDirectoryPath= "";
        SPhotos photosLoaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
        SPhotos photosValided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    };


    // define static functions
    static QString format_html_for_generation(QString html, qreal sizeFactor, ExtraPCInfo infos = ExtraPCInfo()){

//        QElapsedTimer timer;
//        timer.start();

        int index = 0;
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

        html = html.replace("B_#B_#B_#B_","style=\"font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:300; "
                                          " font-style:normal;\"");

        // text-decoration:line-through;  // text-decoration:overline ; oblique // tables

        html = html.replace("$nom$", "$name$");
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
            html = html.insert(index, QString::number(infos.pageNum+1) + "/" + QString::number(infos.pagesNb));
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
                                + " width=\""    + QString::number(sizeFactor * onlyWidth.toDouble())
                                + "\" height=\"" + QString::number(sizeFactor * onlyHeight.toDouble())+ "\" />");
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
//        qDebug() << "Format html took" << timer.elapsed() << "milliseconds";
        return html;
    }
}

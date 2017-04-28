
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

// local
// # generated ui
#include "ui_PhotosConsigneMainW.h"
#include "ui_IndividualConsign.h"
#include "ui_Support.h"
#include "ui_Help.h"
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

namespace Ui {
    class PhotosConsigneMainW;
    class IndividualConsign;
    class SupportW;
    class HelpW;
}

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

        Photo(const QString &path);

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }

        QSize size() const noexcept {return originalSize;}

        QSize scaled_size() const noexcept {return scaledPhoto.size();}

        void draw(QPainter &painter, const QRectF &rectPhoto, bool preview, bool drawImageSize, PaperFormat format, QRectF pageRect);

    private:

        void draw(QPainter &painter, QRectF rectPhoto, QImage photo, bool preview, bool drawImageSize, PaperFormat format, QRectF pageRect);

        void draw_huge(QPainter &painter, QRectF rectPhoto);

    public:

        bool isWhiteSpace = false;
        bool isADuplicate = false;
        bool isRemoved    = false;
        int alignment;
        int rotation = 0;

        QSize originalSize;
        QImage scaledPhoto;
        QString pathPhoto;
        QString namePhoto;
        QFileInfo info;
    };

    struct Consign : public RectPageItem {

        QTextDocument *doc = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Title : public RectPageItem { // ############ TO BE REMOVED

        QTextDocument *doc = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Header : public RectPageItem {

        QTextDocument *doc = nullptr;
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct Footer : public RectPageItem {

        QTextDocument *doc = nullptr;
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

        bool displayCutLines;
        int nbPhotosV;
        int nbPhotosH;
        qreal ratioWithTitle;
        RatioMargins margins;

        SHeader header = nullptr;
        SFooter footer = nullptr;
        Position titlePositionFromPC; // ############ TO BE REMOVED
        STitle title = nullptr;       // ############ TO BE REMOVED
        QVector<SPCSet> sets;

        QRectF setsRect;
        QRectF setsAndMarginsRect;
        QVector<QRectF> interMarginsRects;
        QVector<QLineF> horizontalCutLines;
        QVector<QLineF> verticalCutLines;

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




    struct GlobalData{

        bool grayScale           = false;
        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;
        bool displayZones        = false;

        int nbPhotosPageH;
        int nbPhotosPageV;
        int lastPagePhotosNb;
        int nbPhotosPerPage;

        int currentPageId       = 0;
        int currentPhotoId      = 0;
        int currentPCDisplayed  = 0;

        // global
        // # misc
        bool globalDisplayCutLines;
        int globalPhotosAlignment;
        qreal globalPCRatio;
        QColor pageColor = Qt::white;

        Position globalConsignPositionFromPhotos;        

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


        void reset_photos(){

        }

        void insert_photo(int index){

        }

//        void remove_photo(int index){

//        }

    };


    class UIElements : public QObject{

        Q_OBJECT

    public :

        UIElements(Ui::PhotosConsigneMainW *mainUI) : m_mainUI(mainUI) {}

        ~UIElements(){

            individualConsignsTEditLoaded.clear();
            individualConsignsWLoaded.clear();

            individualConsignsTEditValided.clear();
            individualConsignsWValided.clear();
        }

        // consign related
        void insert_individual_consign(int index, bool whiteSpace = false);
        void remove_individual_consign(int index);
        void reset_individual_consigns(int nbPhotos);

        // utility
        void update_settings_buttons(QVector<QPushButton*> buttons, bool displayZones = false);
        void update_settings_sliders(QVector<QSlider*> sliders, bool displayZones = false);
        void update_settings_spinboxes(QVector<QSpinBox*> spinBoxes, bool displayZones = false);
        void update_settings_double_spinboxes(QVector<QDoubleSpinBox*> dSpinBoxes, bool displayZones = false);
        void update_settings_checkboxes(QVector<QCheckBox*> checkBoxes, bool displayZones = false);
        void update_settings_format_combo_boxes(QVector<QComboBox*> comboBoxes, bool displayZones = false);
        void update_settings_radio_buttons(QVector<QRadioButton*> radioButtons, bool displayZones = false);
        void associate_buttons (QVector<QPushButton*> buttons);
        void associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider);
        void associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2);
        void checkbox_enable_UI(QCheckBox *cb, QVector<QWidget*> widgets, bool inverted = false);

        // members
        QReadWriteLock  docLocker;
        QTimer          zonesTimer;
        Position        previousGlobalConsignPositionFromPhotos;
        QList<Position> previousIndividualConsignPositionFromPhotos;

        std::unique_ptr<QWidget> wSupport  = nullptr;
        std::unique_ptr<QWidget> wHelp     = nullptr;

        ImageLabel   *selectedPhotoW     = nullptr;
        PreviewLabel *previewW           = nullptr;
        RichTextEdit *titleTEdit         = nullptr;
        RichTextEdit *globalConsignTEdit = nullptr;

        QList<std::shared_ptr<RichTextEdit>> individualConsignsTEditLoaded;
        QList<std::shared_ptr<QWidget>>      individualConsignsWLoaded;

        QList<std::shared_ptr<RichTextEdit>> individualConsignsTEditValided;
        QList<std::shared_ptr<QWidget>>      individualConsignsWValided;


    private:

        Ui::PhotosConsigneMainW *m_mainUI = nullptr;

    signals:

        void update_settings_signal();

        void set_progress_bar_state_signal(int state);

        void set_progress_bar_text_signal(QString text);

    };

    // define static functions
    static void draw_doc_html_with_size_factor(QPainter &painter, QReadWriteLock *docLocker, QTextDocument *doc, QRectF upperRect, QRectF docRect, qreal sizeFactor, ExtraPCInfo infos = ExtraPCInfo()){

        QImage pixDoc(QSize(upperRect.width(),upperRect.height()), QImage::Format_ARGB32);
        pixDoc.fill(QColor(255,255,255,0)); // ## to be parametreized
        //        pixDoc.fill(infos.pageColor);

        QPainter painterDoc(&pixDoc);
        painterDoc.setPen(QPen());

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
    }
}


#pragma once


#include "Settings.hpp"

namespace pc {

    struct Consign;
    using SConsign = std::shared_ptr<Consign>;

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



    struct Consign : public RectPageItem {

        virtual ~Consign(){}

        std::shared_ptr<QString> html = nullptr;
        virtual void compute_sizes(QRectF upperRect);
    };

    struct Header : public RectPageItem {

        HeaderSettings settings;

        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);            
        }
    };

    struct Footer : public RectPageItem {

        FooterSettings settings;

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

        qreal ratio;
        Position consignPositionFromPhoto;

        void compute_sizes(QRectF upperRect);
    };



    struct PCPage : public RectPageItem{

        int id; /**< id of the page */

        // generate
    //        int nbPhotos;
        bool drawThisPage;
        PageOrientation orientation;


        // margins
        MarginsSettings margins;

        // border
        BordersSettings bordersSettings;

        // background
        BackGroundSettings backgroundSettings;

        // sets
        PageSetsSettings pageSetsSettings;

        // header
        SHeader header = std::make_shared<Header>();

        // footer
        SFooter footer = std::make_shared<Footer>();

        // sets
        QVector<SPCSet> sets;

        // rects
        QRectF setsRect;
        QRectF pageMinusMarginsRect;
//        QRectF headerRect;
//        QRectF footerRect;
        QRectF marginHeaderRect;
        QRectF marginFooterRect;
        QVector<QRectF> interMarginsRects;

        void compute_sizes(QRectF upperRect);
    };

    struct PCPages{

        bool grayScale;
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
}

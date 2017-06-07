
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

        // sizes
        void compute_sizes(QRectF upperRect){
             rectOnPage = std::move(upperRect);
        }
    };

    struct Header : public RectPageItem {

        // settings
        HeaderSettings settings;

        // sizes
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);            
        }
    };

    struct Footer : public RectPageItem {

        // settings
        FooterSettings settings;

        // sizes
        void compute_sizes(QRectF upperRect){
            rectOnPage = std::move(upperRect);
        }
    };

    struct PCSet : public RectPageItem{

        // settings
        SetSettings settings;

        // current
        int id;         /**< id of the set in its page */
        int totalId;    /**< global id of the set */

        // sub elements
        // # photo
        SPhoto photo = nullptr;
        // # text
        SConsign text = nullptr;

        // sizes
        void compute_sizes(QRectF upperRect);
    };

    struct PCPage : public RectPageItem{

        // settings
        PageSettings settings;

        // current
        int id;
        bool drawThisPage;        

        // sub elements
        // # header
        SHeader header = std::make_shared<Header>();
        // # footer
        SFooter footer = std::make_shared<Footer>();
        // # sets
        QVector<SPCSet> sets;

        // sizes
        QRectF marginHeaderRect;
        QRectF marginFooterRect;
        QRectF setsRect;
        QRectF pageMinusMarginsRect;
        QVector<QRectF> interMarginsRects;
        void compute_sizes(QRectF upperRect);                
    };

    struct PCPages{

        // settings
        DocumentSettings settings;

        // current
        QString pdfFileName = "";

        // sub elemetns
        // # pages
        QVector<SPCPage> pages;

        // sizes
        void compute_all_pages_sizes(int width, int height){
            for(auto &&page : pages)
                page->compute_sizes(QRectF(0,0,width,height));
        }

        // others
        ~PCPages(){
            pages.clear();
        }

        int page_photos_number(int index) const{
            return pages[index]->sets.size();
        }
    };
}

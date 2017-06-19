
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

#pragma once

/**
 * \file DocumentElements.hpp
 * \brief defines Consign/Header/Footer/PCSet/PCPage/PCPages
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
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
        void compute_sizes(QRectF upperRect);
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

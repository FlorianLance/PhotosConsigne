
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
 * \file PageW.hpp
 * \brief defines pc::PageW/Ui::PageUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "MarginsW.hpp"
#include "BackgroundW.hpp"
#include "PageSetsW.hpp"
#include "MiscW.hpp"


// generated ui
#include "ui_Page.h"

namespace Ui {
    class PageUI;
}

namespace pc{

struct PageW;
using UPageW = std::unique_ptr<PageW>;
using SPageW = std::shared_ptr<PageW>;

struct PageW : public SettingsW{

    PageW();

    static void init_ui(pc::PageW &p1, const pc::PageW &p2);

    bool individual() const noexcept;

    void set_enabled(bool enabled) noexcept;

    void write_to_xml(QXmlStreamWriter &xml) const;

    void load_from_xml(QXmlStreamReader &xml);

    void update_settings(Settings &settings) const{

        PageSettings &page = dynamic_cast<PageSettings&>(settings);
        page.name =pageSetsW.ui.leNamePage->text();
        backgroundW.update_settings(page.background);
        marginsW.update_settings(page.margins);
        pageSetsW.update_settings(page.positions);
        miscW.update_settings(page.misc);
    }


public :

    int id = 0;
//    QColor borderColor = QColor(0,0,0,0);
//    QPen borderPen = QPen(QColor(0,0,0,0));

    bool global = false;

    BackgroundW  backgroundW;    
    MarginsW     marginsW;
    PageSetsW    pageSetsW;
    MiscPageW    miscW;

    Ui::PageUI   ui;
};
}


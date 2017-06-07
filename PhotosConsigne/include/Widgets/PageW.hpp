
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


    void update_settings(PageSettings &settings) const{

        backgroundW.update_settings(settings.background);
        marginsW.update_settings(settings.margins);
        setsW.update_settings(settings.positions);
        miscW.update_settings(settings.misc);
    }


public :

    int id = 0;
    QColor borderColor = QColor(0,0,0,0);
    QPen borderPen = QPen(QColor(0,0,0,0));

    BackgroundW  backgroundW;    
    MarginsW     marginsW;
    PageSetsW    setsW;
    MiscPageW    miscW;

    Ui::PageUI   ui;
};
}


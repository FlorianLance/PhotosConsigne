
#pragma once

/**
 * \file PageW.hpp
 * \brief defines pc::PageW/Ui::PageUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "BordersW.hpp"
#include "MarginsW.hpp"
#include "BackgroundW.hpp"
#include "PageSetsW.hpp"


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

    virtual ~PageW(){
//        DebugMessage("~PageW");
    }

    static void init_ui(pc::PageW &p1, const pc::PageW &p2);

    bool individual() const noexcept;

    void set_enabled(bool enabled) noexcept;

    int id;
    QColor borderColor = QColor(0,0,0,0);
    QPen borderPen = QPen(QColor(0,0,0,0));

    BackgroundW  backgroundW;
    BordersW     bordersW;
    MarginsW     marginsW;
    PageSetsW    setsW;

    Ui::PageUI   ui;
};
}


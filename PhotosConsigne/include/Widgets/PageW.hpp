
#pragma once

/**
 * \file PageW.hpp
 * \brief defines pc::PageW/Ui::PageUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "Utility.hpp"
#include "BordersW.hpp"
#include "MarginsW.hpp"
#include "BackgroundW.hpp"

// generated ui
#include "ui_Page.h"

namespace Ui {
    class PageUI;
}

namespace pc{

struct PageW;
using SPageUI = std::shared_ptr<PageW>;

struct PageW : public SettingsW{

    PageW();

    void init_with_global_settings(const GlobalData &globalSettings);

    bool individual() const noexcept {
        return ui.cbEnableIndividualPage->isChecked();
    }

    int id;
    QColor borderColor = QColor(0,0,0,0);
    QPen borderPen = QPen(QColor(0,0,0,0));

    BackgroundW  backgroundW;
    BordersW     bordersW;
    MarginsW     marginsW;
    Ui::PageUI   ui;
};
}


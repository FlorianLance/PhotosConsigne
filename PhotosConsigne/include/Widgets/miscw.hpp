#pragma once

/**
 * \file MiscW.hpp
 * \brief defines ...
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "SettingsW.hpp"

// generated ui
#include "ui_MiscPage.h"
#include "ui_MiscSet.h"

namespace Ui {
    class MiscPageUI;
    class MiscSetUI;
}

namespace pc{

struct MiscPageW : public SettingsW{

    Q_OBJECT

public :

    MiscPageW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        init_checkboxes_connections({ui.cbDoNotShowFooter,ui.cbDoNotShowHeader}, true);
        init_pushbuttons_connections({ui.pbGlobal, ui.pbPerPage, ui.pbReplaceLocalWIthGlobal, ui.pbReplaceGlobalWithLocal});

        connect(ui.pbGlobal, &QPushButton::clicked, this, &MiscPageW::all_global_pages_signal);
        connect(ui.pbPerPage, &QPushButton::clicked, this, &MiscPageW::all_individual_pages_signal);

        connect(ui.pbReplaceLocalWIthGlobal, &QPushButton::clicked, this, &MiscPageW::replace_individual_with_global_signal);
        connect(ui.pbReplaceGlobalWithLocal, &QPushButton::clicked, this, &MiscPageW::replace_global_with_individual_signal);
    }

    static void init_ui(MiscPageW &p1, const MiscPageW &p2){

        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDoNotShowHeader, p2.ui.cbDoNotShowHeader->isChecked());
        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDoNotShowFooter, p2.ui.cbDoNotShowFooter->isChecked());
    }

    void update_settings(MiscSettings &settings) const{

        settings.doNotDisplayHeader = ui.cbDoNotShowHeader->isChecked();
        settings.doNotDisplayFooter = ui.cbDoNotShowFooter->isChecked();
    }

signals:

    void all_individual_pages_signal();
    void all_global_pages_signal();
    void replace_global_with_individual_signal();
    void replace_individual_with_global_signal();

public :

    // ui
    Ui::MiscPageUI ui;
};

struct MiscSetW : public SettingsW{

    Q_OBJECT

public :

    MiscSetW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        init_pushbuttons_connections({ui.pbGlobal, ui.pbPerPage, ui.pbReplaceLocalWIthGlobal, ui.pbReplaceGlobalWithLocal});

        connect(ui.pbGlobal, &QPushButton::clicked, this, &MiscSetW::all_global_pages_signal);
        connect(ui.pbPerPage, &QPushButton::clicked, this, &MiscSetW::all_individual_pages_signal);

        connect(ui.pbReplaceLocalWIthGlobal, &QPushButton::clicked, this, &MiscSetW::replace_individual_with_global_signal);
        connect(ui.pbReplaceGlobalWithLocal, &QPushButton::clicked, this, &MiscSetW::replace_global_with_individual_signal);
    }

signals:

    void all_individual_pages_signal();
    void all_global_pages_signal();
    void replace_global_with_individual_signal();
    void replace_individual_with_global_signal();

public :

    // ui
    Ui::MiscSetUI ui;
};


}



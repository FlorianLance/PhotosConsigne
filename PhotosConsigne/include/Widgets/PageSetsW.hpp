
#pragma once

/**
 * \file PageSetsW.hpp
 * \brief defines pc::PageSetsW/Ui::PageSetsUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "CustomPageW.hpp"
#include "GlobalDocumentSettings.hpp"

// generated ui
#include "ui_PageSets.h"

namespace Ui {
    class PageSetsUI;
}

namespace pc{


struct PageSetsW : public SettingsW{

    PageSetsW() : SettingsW(){

        // init ui
        ui.setupUi(this);

        connect(ui.sbPhotosNbWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

            int nbMaxPhotos = ui.sbPhotosNbWidth->value() * ui.sbPhotosNbHeight->value();
            if(nbMaxPhotos < ui.sbPhotosNbPerPage->value()){
                Utility::safe_init_spinbox_value(ui.sbPhotosNbPerPage, nbMaxPhotos);
            }
            emit settings_updated_signal(true);
        });

        connect(ui.sbPhotosNbHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

            int nbMaxPhotos = ui.sbPhotosNbWidth->value() * ui.sbPhotosNbHeight->value();
            if(nbMaxPhotos < ui.sbPhotosNbPerPage->value()){
                Utility::safe_init_spinbox_value(ui.sbPhotosNbPerPage, nbMaxPhotos);
            }
            emit settings_updated_signal(true);
        });

        connect(ui.sbPhotosNbPerPage, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{

            int nbMaxPhotos = ui.sbPhotosNbWidth->value() * ui.sbPhotosNbHeight->value();
            if(nbMaxPhotos < ui.sbPhotosNbPerPage->value()){
                Utility::safe_init_spinbox_value(ui.sbPhotosNbPerPage, nbMaxPhotos);
            }
            emit settings_updated_signal(true);
        });

        ui.vlCustom->addWidget(&customW);
//        ui.vlCustom->setAlignment(Qt::AlignLeft);

        ui.frameCustom->hide();
        customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
        connect(ui.sbHPerso, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
            emit settings_updated_signal(true);
        });
        connect(ui.sbWPerso, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
            emit settings_updated_signal(true);
        });

        connect(ui.rbGrid, &QRadioButton::clicked, this, [&]{
           ui.frameCustom->setEnabled(false);
           ui.framePhotosNb->setEnabled(true);
           ui.frameCustom->hide();
           ui.framePhotosNb->show();
        });
        connect(ui.rbPersonnalised, &QRadioButton::clicked, this, [&]{
           ui.frameCustom->setEnabled(true);
           ui.framePhotosNb->setEnabled(false);
           ui.frameCustom->show();
           ui.framePhotosNb->hide();
        });
    }

    static void init_ui(PageSetsW &p1, const PageSetsW &p2){

        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbHeight,    p2.ui.sbPhotosNbHeight->value());
        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbWidth,     p2.ui.sbPhotosNbWidth->value());
        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbPerPage,   p2.ui.sbPhotosNbPerPage->value());
    }


    PageSetsSettings settings() const noexcept {

        PageSetsSettings pageSets;
        pageSets.nbPhotosH      = ui.sbPhotosNbWidth->value();
        pageSets.nbPhotosV      = ui.sbPhotosNbHeight->value();
        pageSets.nbPhotos       = ui.sbPhotosNbPerPage->value();
        return pageSets;
    }

    // ui
    Ui::PageSetsUI    ui;
    CustomPageW customW;
};
}



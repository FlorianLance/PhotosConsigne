
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
        ui.pbValidate->setEnabled(false);
        ui.pbRemoveLast->setEnabled(false);
        customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
        connect(ui.sbHPerso, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
            emit settings_updated_signal(true);
        });
        connect(ui.sbWPerso, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            customW.init(ui.sbWPerso->value(),ui.sbHPerso->value());
            emit settings_updated_signal(true);
        });
        connect(ui.rbGrid, &QRadioButton::clicked, this, [=]{
            ui.frameCustom->setEnabled(false);
            ui.framePhotosNb->setEnabled(true);
            ui.frameCustom->hide();
            ui.framePhotosNb->show();
            emit settings_updated_signal(false);
        });
        connect(ui.rbPersonnalised, &QRadioButton::clicked, this, [=]{
            ui.frameCustom->setEnabled(true);
            ui.framePhotosNb->setEnabled(false);
            ui.frameCustom->show();
            ui.framePhotosNb->hide();
            qDebug() << "ui.rbPersonnalised";
            emit settings_updated_signal(false);
            qDebug() << "end ui.rbPersonnalised";
        });
        connect(ui.cbDontDisplayFooter, &QCheckBox::clicked, this, [=]{
            emit settings_updated_signal(true);
        });
        connect(ui.cbDontDisplayHeader, &QCheckBox::clicked, this, [=]{
            emit settings_updated_signal(true);
        });

        connect(&customW, &CustomPageW::current_state_signal, this, [&](QPoint p1, QPoint p2, int validSets){
            Q_UNUSED(p1);
            ui.pbValidate->setEnabled(p2.x() != -1);
            ui.pbRemoveLast->setEnabled(validSets>0 || p2.x() != -1);
            ui.laNbSets->setText("Nb: " + QString::number(validSets + ((p2.x() != -1)?1:0)));
            emit settings_updated_signal(false);
        });
        connect(ui.pbValidate, &QPushButton::clicked, &customW, &CustomPageW::validate_current_set);
        connect(ui.pbRemoveLast, &QPushButton::clicked, &customW, &CustomPageW::remove_last_set);


    }

    virtual ~PageSetsW(){
//        DebugMessage("~PageSetsW");
    }

    static void init_ui(PageSetsW &p1, const PageSetsW &p2){

        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbHeight,    p2.ui.sbPhotosNbHeight->value());
        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbWidth,     p2.ui.sbPhotosNbWidth->value());
        Utility::safe_init_spinbox_value(p1.ui.sbPhotosNbPerPage,   p2.ui.sbPhotosNbPerPage->value());

        Utility::safe_init_spinbox_value(p1.ui.sbHPerso, p2.ui.sbHPerso->value());
        Utility::safe_init_spinbox_value(p1.ui.sbWPerso, p2.ui.sbWPerso->value());
        Utility::safe_init_radio_button_state(p1.ui.rbGrid, p2.ui.rbGrid->isChecked());
        Utility::safe_init_radio_button_state(p1.ui.rbPersonnalised, p2.ui.rbPersonnalised->isChecked());

        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDontDisplayHeader, p2.ui.cbDontDisplayHeader->isChecked());
        Utility::safe_init_checkboxe_checked_state(p1.ui.cbDontDisplayFooter, p2.ui.cbDontDisplayFooter->isChecked());

        if(p2.ui.frameCustom->isEnabled()){
            p1.ui.frameCustom->setEnabled(true);
            p1.ui.framePhotosNb->setEnabled(false);
            p1.ui.frameCustom->show();
            p1.ui.framePhotosNb->hide();
        }else{
            p1.ui.frameCustom->setEnabled(false);
            p1.ui.framePhotosNb->setEnabled(true);
            p1.ui.frameCustom->hide();
            p1.ui.framePhotosNb->show();
        }

        CustomPageW::init_ui(p1.customW, p2.customW);
    }


    PageSetsSettings settings() const noexcept {

        PageSetsSettings pageSets;
        pageSets.nbPhotosH          = ui.sbPhotosNbWidth->value();
        pageSets.nbPhotosV          = ui.sbPhotosNbHeight->value();
        pageSets.nbPhotos           = ui.sbPhotosNbPerPage->value();
        pageSets.customMode         = ui.rbPersonnalised->isChecked();
        pageSets.relativePosCustom  = customW.relativePositions;
        pageSets.doNotDisplayHeader = ui.cbDontDisplayHeader->isChecked();
        pageSets.doNotDisplayFooter = ui.cbDontDisplayFooter->isChecked();

        if(customW.currentSecondPoint.x() > -1){
            pageSets.relativePosCustom.push_back(customW.currRelativePos);
        }

        return pageSets;
    }

    // ui
    Ui::PageSetsUI    ui;
    CustomPageW customW;
};
}



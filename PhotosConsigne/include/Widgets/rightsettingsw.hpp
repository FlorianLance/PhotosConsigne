
#pragma once

/**
 * \file RightSettingsW.hpp
 * \brief defines pc::RightSettingsW/Ui::RightSettingsUI
 * \author Florian Lance
 * \date 17/05/2017
 */

// local
#include "PageW.hpp"
#include "SetW.hpp"
#include "HeaderW.hpp"
#include "FooterW.hpp"

// generated ui
#include "ui_RightSettings.h"

namespace Ui {
    class RightSettingsUI;
}

namespace pc{


    struct RightSettingsW : public SettingsW {

        RightSettingsW() : SettingsW(){

            // init ui
            ui.setupUi(this);

            // header
            ui.vlHeader->addWidget(&headerW);
            connect(&headerW, &HeaderW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            // footer
            ui.vlFooter->addWidget(&footerW);
            connect(&footerW, &FooterW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            // all pages
            ui.tabAllPages->setCurrentIndex(0);
            // # background
            ui.vlGlobalBackground->addWidget(&globalPageW.backgroundW);
            // # borders
            ui.vlGlobalBorders->addWidget(&globalPageW.bordersW);
            // # margins
            ui.vlGlobalMargins->addWidget(&globalPageW.marginsW);
            // # sets
            ui.vlGlobalPageSets->addWidget(&globalPageW.setsW);
            // # misc page
            ui.vlGlobalMisc->addWidget(&globalPageW.miscW);            



            // # whole page
            connect(&globalPageW, &PageW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            connect(ui.tabAllPages, &QTabWidget::currentChanged, this, [&]{

                globalPageW.backgroundW.hide();
                globalPageW.bordersW.hide();
                globalPageW.marginsW.hide();
                globalPageW.setsW.hide();
                globalPageW.miscW.hide();

                switch (ui.tabAllPages->currentIndex()) {
                case 0:
                    globalPageW.setsW.show();
                    break;
                case 1:
                    globalPageW.marginsW.show();
                    break;
                case 2:
                    globalPageW.bordersW.show();
                    break;
                case 3:
                    globalPageW.backgroundW.show();
                    break;                
                case 4:
                    globalPageW.miscW.show();
                    break;
                }
            });

            // misc all pages
            connect(&globalPageW.miscW, &MiscPageW::all_global_pages_signal, this, [&]{

                for(auto &&page : pagesW){
                    page->set_enabled(false);
                }
                emit settings_updated_signal(true);
            });

            connect(&globalPageW.miscW, &MiscPageW::all_individual_pages_signal, this, [&]{

                for(auto &&page : pagesW){
                    page->set_enabled(true);
                }
                emit settings_updated_signal(true);
            });

            connect(&globalPageW.miscW, &MiscPageW::replace_individual_with_global_signal, this, [&]{

                for(auto &&page : pagesW){
                    PageW::init_ui(*page, globalPageW);
                }

                emit settings_updated_signal(true);
            });
            delete globalPageW.miscW.ui.pbReplaceLocalWIthGlobal;

            // misc all sets
            connect(globalSetW.setMiscW.ui.pbGlobal, &QPushButton::clicked, this, [&]{
                for(auto &&set : setsValidedW){
                    Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, false);
                    set->ui.frameSet->setEnabled(false);
                }
                emit settings_updated_signal(true);
            });
            connect(globalSetW.setMiscW.ui.pbPerPage, &QPushButton::clicked, this, [&]{
                for(auto &&set : setsValidedW){
                    Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, true);
                    set->ui.frameSet->setEnabled(true);
                }
                emit settings_updated_signal(true);
            });
            connect(globalSetW.setMiscW.ui.pbReplaceLocalWIthGlobal, &QPushButton::clicked, this, [&]{

                for(auto &&set : setsLoadedW){
                    SetW::init_ui(*set, globalSetW, true);
                }

                emit settings_updated_signal(true);
            });
            delete globalSetW.setMiscW.ui.pbReplaceGlobalWithLocal;


            // all sets
            ui.tabAllSets->setCurrentIndex(0);
            // # misc
            ui.vlAllSetsMisc->addWidget(&globalSetW.setMiscW);
            ui.vlAllSetsMisc->setAlignment(Qt::AlignmentFlag::AlignTop);
            // # style
            ui.vlGlobalSetStyle->addWidget(&globalSetW.setStyleW);
            ui.vlGlobalSetStyle->setAlignment(Qt::AlignmentFlag::AlignTop);
            // # consign
            globalSetW.setTextW.init_style(RichTextType::globalConsign);
            ui.globalConsignLayout->addWidget(&globalSetW.setTextW);
            ui.globalConsignLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
            connect(globalSetW.setTextW.textEdit(), &TextEdit::resource_added_signal, this, &RightSettingsW::resource_added_signal);
            connect(&globalSetW, &SetW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);           

            ui.verticalSpacer1->changeSize(20, 40, QSizePolicy::Ignored);
            connect(ui.tabAllSets, &QTabWidget::currentChanged, this, [&]{

                globalSetW.setTextW.hide();
                globalSetW.setStyleW.hide();
                globalSetW.setMiscW.hide();

                switch (ui.tabAllSets->currentIndex()) {
                case 0:
                    globalSetW.setTextW.show();
                    ui.verticalSpacer1->changeSize(20, 40, QSizePolicy::Ignored);
                    break;
                case 1:
                    globalSetW.setStyleW.show();
                    ui.verticalSpacer1->changeSize(20, 40, QSizePolicy::Expanding);
                    break;
                case 2:
                    globalSetW.setMiscW.show();
                    ui.verticalSpacer1->changeSize(20, 40, QSizePolicy::Expanding);
                    break;
                }
            });

            ui.tbRight->setCurrentIndex(0);
        }

        ~RightSettingsW(){

            setsLoadedW.clear();
            setsValidedW.clear();
            pagesW.clear();
        }


        void update_individual_pages(const GlobalDocumentSettings &settings){

            int diff = pagesW.size() - settings.nbPages;
            if(diff < 0){ // add -diff pages

                for(int ii = 0; ii < -diff;++ ii){

                    pagesW.push_back(std::make_shared<PageW>());

                    PageW *pageW = pagesW.last().get();
                    delete pageW->miscW.ui.frameApply;
                    delete pageW->miscW.ui.line2;
                    delete pageW->miscW.ui.pbReplaceLocalWIthGlobal;

                    PageW::init_ui(*pageW, globalPageW);
                    connect(pageW, &PageW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

                    connect(&pageW->miscW, &MiscPageW::replace_global_with_individual_signal, this, [=]{
                        PageW::init_ui(globalPageW, *pageW);
                    });
                }

            }else if(diff > 0){ // remove diff pages

                for(int ii = 0; ii < -diff;++ ii){
                    pagesW.pop_back();
                }
            }
        }


        void insert_individual_set(int index){

            setsLoadedW.insert(index,std::make_shared<SetW>());
            SetW *setW = setsLoadedW[index].get();
            setW->setEnabled(true);

            delete setW->setMiscW.ui.pbReplaceLocalWIthGlobal;
            delete setW->setMiscW.ui.line1;
            delete setW->setMiscW.ui.frameApply;

            connect(setW->setTextW.textEdit(), &TextEdit::resource_added_signal, this, &RightSettingsW::resource_added_signal);

            // # set style
            connect(setW, &SetW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);
            connect(&setW->setMiscW, &MiscSetW::replace_global_with_individual_signal, this, [=]{
                SetW::init_ui(globalSetW, *setW, true);
            });

            // init new individual ui with global ui
            SetW::init_ui(*setW, globalSetW, false);
        }

        void remove_individual_set(int index){
            setsLoadedW.removeAt(index);
        }

        void reset_individual_sets(int nbPhotos){

            qreal currentState  = 750.;
            qreal offset        = 250. / nbPhotos;

            // clean
            setsLoadedW.clear();

            // insert new consigns
            for(int ii = 0; ii < nbPhotos; ++ii){

                emit set_progress_bar_text_signal("Consigne individuelle n°" + QString::number(ii));
                insert_individual_set(ii);
                currentState += offset;
                emit set_progress_bar_state_signal(static_cast<int>(currentState));
            }
        }

        void display_global_page_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 0);
        }
        void display_current_page_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 1);
        }

        void display_global_set_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 2);
        }

        void display_current_set_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 3);
        }

        void display_header_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 4);
        }

        void display_footer_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 5);
        }



        // ui
        Ui::RightSettingsUI ui;

        // widgets
        HeaderW         headerW;
        FooterW         footerW;

        SetW            globalSetW;
        PageW           globalPageW;

        QList<SPageW>   pagesW;
        QList<SSetW>    setsLoadedW;
        QList<SSetW>    setsValidedW;
    };
}

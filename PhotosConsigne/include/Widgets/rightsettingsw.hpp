
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

            // global page
            // # background
            ui.vlGlobalBackground->addWidget(&globalPageW.backgroundW);
            // # borders
            ui.vlGlobalBorders->addWidget(&globalPageW.bordersW);
            // # margins
            ui.vlGlobalMargins->addWidget(&globalPageW.marginsW);
            // # page sets
            ui.vlGlobalPageSets->addWidget(&globalPageW.setsW);
            // # whole page
            connect(&globalPageW, &PageW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            connect(ui.tabAllPages, &QTabWidget::currentChanged, this, [&]{

                globalPageW.backgroundW.hide();
                globalPageW.bordersW.hide();
                globalPageW.marginsW.hide();
                globalPageW.setsW.hide();

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
                }
            });


            // global set
            // # style
            ui.vlGlobalSetStyle->addWidget(&globalSetW.setStyleW);
            // # consign
            QHBoxLayout *globalConsignLayout = new QHBoxLayout();
            globalConsignLayout->setContentsMargins(3,3,3,3);
            ui.pageAllSetsText->setLayout(globalConsignLayout);
            globalSetW.setTextW.init_style(RichTextType::globalConsign);
            globalConsignLayout->addWidget(&globalSetW.setTextW);
            connect(globalSetW.setTextW.textEdit(), &TextEdit::resource_added_signal, this, &RightSettingsW::resource_added_signal);
            connect(&globalSetW, &SetW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            // # tabs right
            ui.tbRight->setItemEnabled(2, false);
            ui.tbRight->setItemEnabled(5, false);
        }

        ~RightSettingsW(){

//            DebugMessage("~RightSettingsW");
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
                    PageW::init_ui(*pageW, globalPageW);
                    connect(pageW, &PageW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

                    connect(pageW->ui.pbAllGlobalParameters, &QPushButton::clicked, this, [&]{

                        for(auto &&page : pagesW){
                            page->set_enabled(false);
                        }
                        emit settings_updated_signal(true);
                    });

                    connect(pageW->ui.pbAllIndividualParameters, &QPushButton::clicked, this, [&]{

                        for(auto &&page : pagesW){
                            page->set_enabled(true);
                        }
                        emit settings_updated_signal(true);
                    });

                    connect(pageW->ui.pbCopyGlobalToIndividuals, &QPushButton::clicked, this, [&]{

                        for(auto &&page : pagesW){
                            PageW::init_ui(*page, globalPageW);
                        }

                        emit settings_updated_signal(true);
                    });
                    connect(pageW->ui.pbCopyIndividualToGlobal, &QPushButton::clicked, this, [=]{
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
            SSetW &setW = setsLoadedW[index];
            setW->setEnabled(true);
            Ui::SetUI &ui = setW->ui;

            connect(setW->setTextW.textEdit(), &TextEdit::resource_added_signal, this, &RightSettingsW::resource_added_signal);

            // # set style
            connect(setW.get(), &SetW::settings_updated_signal, this, &RightSettingsW::settings_updated_signal);

            // # init/copy settings
            connect(ui.pbAllGlobalParameters, &QPushButton::clicked, this, [&]{
                for(auto &&set : setsValidedW){
                    Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, false);
                    set->ui.frameSet->setEnabled(false);
                }
                emit settings_updated_signal(true);
            });

            connect(ui.pbAllIndividualParameters, &QPushButton::clicked, this, [&]{
                for(auto &&set : setsValidedW){
                    Utility::safe_init_checkboxe_checked_state(set->ui.cbEnableIndividualConsign, true);
                    set->ui.frameSet->setEnabled(true);
                }
                emit settings_updated_signal(true);
            });

            connect(ui.pbCopyGlobalToIndividuals, &QPushButton::clicked, this, [&]{

                for(auto &&set : setsLoadedW){
                    SetW::init_ui(*set, globalSetW, true);
                }

                emit settings_updated_signal(true);
            });
            connect(ui.pbCopyIndividualToGlobal, &QPushButton::clicked, this, [&]{
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
            Utility::safe_init_tool_box_index(ui.tbRight, 3);
        }

        void display_current_set_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 4);
        }

        void display_header_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 6);
        }

        void display_footer_panel(){
            Utility::safe_init_tool_box_index(ui.tbRight, 7);
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

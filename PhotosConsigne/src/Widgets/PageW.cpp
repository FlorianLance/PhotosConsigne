

#include "PageW.hpp"

static int pageUICounter = 0;

pc::PageW::PageW() : SettingsW(){

    ui.setupUi(this);
    ui.vlBackground->addWidget(&backgroundW);
    ui.vlBorders->addWidget(&bordersW);
    ui.vlMargins->addWidget(&marginsW);

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

    init_checkboxes_connections({ui.cbEnableIndividualPage}, true);
    Utility::checkbox_enable_UI(ui.cbEnableIndividualPage, {ui.framePage});

//    // # background
//    connect(&backgroundW, &BackgroundW::settings_updated_signal, this, &PageW::settings_updated_signal);
//    // # borders
//    connect(&bordersW, &BordersW::settings_updated_signal, this, &PageW::settings_updated_signal);
//    // # margins
//    connect(&marginsW, &MarginsW::settings_updated_signal, this, &PageW::settings_updated_signal);

    id = pageUICounter++;
}



void pc::PageW::init_with_global_settings(const pc::GlobalData &globalSettings){

    // init ui
    ui.sbPhotosNbHeight->setValue(globalSettings.nbPhotosPageHeight);
    ui.sbPhotosNbWidth->setValue(globalSettings.nbPhotosPageWidth);
    ui.sbPhotosNbPerPage->setValue(globalSettings.nbPhotosPerPage);
}

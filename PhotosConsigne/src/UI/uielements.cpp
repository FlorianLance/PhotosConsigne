
/**
 * \file UIElements.cpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */


// local
#include "UIElements.hpp"


// Qt
#include <QColorDialog>

using namespace pc;

pc::UIElements::UIElements(QSharedPointer<Ui::PhotosConsigneMainW> mainUI) : m_mainUI(mainUI) {

    // init widgets
    // # photo display widgets
    QHBoxLayout *selectedPhotoLayout = new QHBoxLayout();
    m_mainUI->wSelectedPhotoW->setLayout(selectedPhotoLayout);
    selectedPhotoW = new ImageLabel();
    selectedPhotoLayout->addWidget(selectedPhotoW);
    m_mainUI->twMiddle->setTabEnabled(0, false);
    m_mainUI->pbAdd->hide();

    // # preview image widget
    previewW = new PreviewLabel();
    m_mainUI->vlImagePreview->addWidget(previewW);

    // # text edit widgets
    // ## title
    titleTEdit = new RichTextEdit();
    titleTEdit->set_doc_locker(&docLocker);
    m_mainUI->hlTitleBottom->addWidget(titleTEdit);
    titleTEdit->setEnabled(false);
    titleTEdit->init_as_title();
    titleTEdit->init_colors(qRgba(0,0,0,255), qRgba(255,255,255,0));

    // ## global consign
    globalConsignUI.richTextEdit->set_doc_locker(&docLocker);
    m_mainUI->vlGlobalConsign->addWidget(globalConsignUI.richTextEdit.get());

    // disable ui
    // # tab texte infos
    m_mainUI->twPhotosList->setTabEnabled(1,false); // list photos info tab
    m_mainUI->twPagesList->setTabEnabled(1,false);  // list pages info tab
    // # tabs right
    m_mainUI->tbRight->setItemEnabled(2, false);    // page tab
    m_mainUI->tbRight->setItemEnabled(3, false);    // pc tab

    // init definition
    PaperFormat pf(m_mainUI->cbDPI->currentText(),m_mainUI->cbFormat->currentText());
    int currentDPI = m_mainUI->cbDPI->currentText().toInt();
    m_mainUI->laDefWxH->setText(QString::number(pf.widthPixels(currentDPI)) + "x" + QString::number(pf.heightPixels(currentDPI)));
    m_mainUI->laDefTotal->setText("(" + QString::number(pf.widthPixels(currentDPI)*pf.heightPixels(currentDPI)) + " pixels)");

    // static connections
    // # associate sliders with spin boxes
    associate_double_spinbox_with_slider(m_mainUI->dsbGlobalRatioPC, m_mainUI->hsGlobalRatioPC);
    associate_double_spinbox_with_slider(m_mainUI->dsbRatioTitle, m_mainUI->hsRatioTitle);
    associate_double_spinbox_with_slider(m_mainUI->dsbLeftMargins, m_mainUI->hsLeftMargins, m_mainUI->dsbRightMargins, m_mainUI->hsRightMargins);
    associate_double_spinbox_with_slider(m_mainUI->dsbTopMargins, m_mainUI->hsTopMargins, m_mainUI->dsbBottomMargins, m_mainUI->hsBottomMargins);
    associate_double_spinbox_with_slider(m_mainUI->dsbHorizontalMargins, m_mainUI->hsHorizontalMargins);
    associate_double_spinbox_with_slider(m_mainUI->dsbVerticalMargins, m_mainUI->hsVerticalMargins);

    // # associate buttons
    associate_buttons({m_mainUI->pbGlobalConsignBottom,m_mainUI->pbGlobalConsignLeft,m_mainUI->pbGlobalConsignRight,m_mainUI->pbGlobalConsignTop});
    associate_buttons({m_mainUI->pbGlobalImageAligmentLeft,m_mainUI->pbGlobalImageAligmentRight, m_mainUI->pbGlobalImageAligmentHMiddle});
    associate_buttons({m_mainUI->pbGlobalImageAligmentVMiddle,m_mainUI->pbGlobalImageAligmentTop,m_mainUI->pbGlobalImageAligmentBottom});
    associate_buttons({m_mainUI->pbLandscape,m_mainUI->pbPortrait});

    // # associate checkboxes with UI
    checkbox_enable_UI(m_mainUI->cbAddExteriorMargins, {m_mainUI->hsLeftMargins, m_mainUI->hsRightMargins, m_mainUI->hsTopMargins, m_mainUI->hsBottomMargins,
                       m_mainUI->dsbLeftMargins, m_mainUI->dsbRightMargins, m_mainUI->dsbTopMargins, m_mainUI->dsbBottomMargins});
    checkbox_enable_UI(m_mainUI->cbAddInteriorMargins, {m_mainUI->hsHorizontalMargins,m_mainUI->hsVerticalMargins,m_mainUI->dsbHorizontalMargins,m_mainUI->dsbVerticalMargins});
    checkbox_enable_UI(m_mainUI->cbAddTitle, {m_mainUI->cbAllPagesTitle, m_mainUI->rbBottomTitle, m_mainUI->rbTopTitle,
                                              m_mainUI->rbWriteOnPCTitle, m_mainUI->hsRatioTitle, m_mainUI->dsbRatioTitle, titleTEdit, m_mainUI->pbRatioTitlePage});

    /// # udpate settings
    update_settings_buttons({m_mainUI->pbGlobalConsignBottom,m_mainUI->pbGlobalConsignLeft,m_mainUI->pbGlobalConsignRight,m_mainUI->pbGlobalConsignTop,
                            m_mainUI->pbGlobalImageAligmentLeft,m_mainUI->pbGlobalImageAligmentRight, m_mainUI->pbGlobalImageAligmentHMiddle,
                            m_mainUI->pbGlobalImageAligmentVMiddle,m_mainUI->pbGlobalImageAligmentTop,m_mainUI->pbGlobalImageAligmentBottom,
                            m_mainUI->pbLandscape,m_mainUI->pbPortrait},true);
    update_settings_sliders({m_mainUI->hsGlobalRatioPC, m_mainUI->hsRatioTitle, m_mainUI->hsLeftMargins, m_mainUI->hsRightMargins, m_mainUI->hsTopMargins,
                            m_mainUI->hsBottomMargins, m_mainUI->hsHorizontalMargins, m_mainUI->hsVerticalMargins},true);
    update_settings_double_spinboxes({m_mainUI->dsbGlobalRatioPC, m_mainUI->dsbRatioTitle, m_mainUI->dsbLeftMargins, m_mainUI->dsbRightMargins,
                                     m_mainUI->dsbTopMargins,m_mainUI->dsbBottomMargins,m_mainUI->dsbHorizontalMargins,m_mainUI->dsbVerticalMargins},true);
    update_settings_checkboxes({m_mainUI->cbAddExteriorMargins,m_mainUI->cbAddInteriorMargins,
                               m_mainUI->cbAddTitle, m_mainUI->cbAllPagesTitle},true);
    update_settings_checkboxes({m_mainUI->cbCutLines, m_mainUI->cbSaveOnlyCurrentPage});
}

pc::UIElements::~UIElements(){

    individualConsignsLoadedUI.clear();
    individualConsignsValidedUI.clear();
}


void pc::UIElements::insert_individual_consign(bool whiteSpace){

    individualConsignsLoadedUI.push_back(IndividualConsignUI());

    IndividualConsignUI &consignUI = individualConsignsLoadedUI.last();
    Ui::IndividualConsignW &ui = consignUI.ui;
    consignUI.richTextEdit->set_doc_locker(&docLocker);

    associate_buttons({ui.pbConsignBottom, ui.pbConsignLeft, ui.pbConsignRight, ui.pbConsignTop});
    associate_buttons({ui.pbImageAligmentRight, ui.pbImageAligmentLeft, ui.pbImageAligmentHMiddle});
    associate_buttons({ui.pbImageAligmentBottom, ui.pbImageAligmentVMiddle, ui.pbImageAligmentTop});
    update_settings_buttons({ui.pbConsignBottom, ui.pbConsignLeft, ui.pbConsignRight, ui.pbConsignTop,
                             ui.pbImageAligmentRight, ui.pbImageAligmentLeft, ui.pbImageAligmentHMiddle,
                             ui.pbImageAligmentBottom, ui.pbImageAligmentVMiddle, ui.pbImageAligmentTop}, true);
    update_settings_sliders({ui.hsRatioPC}, true);
    update_settings_double_spinboxes({ui.dsbRatioPC}, true);
    update_settings_checkboxes({ui.cbEnableIndividualConsign});

    checkbox_enable_UI(ui.cbEnableIndividualConsign, {ui.wTop, ui.tbConsigns});
    connect(consignUI.richTextEdit->textEdit(), &TextEdit::textChanged, this, &UIElements::update_settings_signal);
    connect(ui.cbWriteOnPhoto, &QCheckBox::clicked, this, [=](bool checked){

        ui.hsRatioPC->setEnabled(!checked);
        ui.dsbRatioPC->setEnabled(!checked);
        ui.pbConsignBottom->setEnabled(!checked);
        ui.pbConsignLeft->setEnabled(!checked);
        ui.pbConsignRight->setEnabled(!checked);
        ui.pbConsignTop->setEnabled(!checked);

        if(!checked){
            ui.pbConsignBottom->setEnabled(false);
        }

        emit update_settings_signal();
    });

    ui.pbInsertWhiteSpace->setEnabled(!whiteSpace);
    connect(ui.pbInsertWhiteSpace, &QPushButton::clicked, [=]{
        emit insert_white_space_signal();
    });

    associate_double_spinbox_with_slider(ui.dsbRatioPC, ui.hsRatioPC);

}

void pc::UIElements::remove_individual_consign(int index){

    individualConsignsLoadedUI.removeAt(index);
}

void pc::UIElements::reset_individual_consigns(int nbPhotos){

    qreal currentState = 750.;
    qreal offset = 250. / nbPhotos;

    // clean
    individualConsignsLoadedUI.clear();

    // insert new consigns
    for(int ii = 0; ii < nbPhotos; ++ii){

        emit set_progress_bar_text_signal("Consigne individuelle n°" + QString::number(ii));
        insert_individual_consign(ii);
        currentState += offset;
        emit set_progress_bar_state_signal(static_cast<int>(currentState));
    }
}

void pc::UIElements::update_individual_pages(const GlobalData &settings)
{
    int diff = individualPageUI.size() - settings.nbPages;
    if(diff < 0){ // add -diff pages

        for(int ii = 0; ii < -diff;++ ii){

            individualPageUI.push_back(PageUI());

            PageUI &pageUI = individualPageUI.last();
            Ui::IndividualPageW &ui = pageUI.ui;

            // init uit
            ui.sbVSizeGrid->setValue(settings.nbPhotosPageV);
            ui.sbHSizeGrid->setValue(settings.nbPhotosPageH);
            ui.sbNbPhotos->setEnabled(false);

            // define connections...
            // # associate sliders with spin boxes
            associate_double_spinbox_with_slider(ui.dsbLeftMargins, ui.hsLeftMargins, ui.dsbRightMargins, ui.hsRightMargins);
            associate_double_spinbox_with_slider(ui.dsbTopMargins, ui.hsTopMargins, ui.dsbBottomMargins, ui.hsBottomMargins);
            associate_double_spinbox_with_slider(ui.dsbHorizontalMargins, ui.hsHorizontalMargins);
            associate_double_spinbox_with_slider(ui.dsbVerticalMargins, ui.hsVerticalMargins);


            update_settings_checkboxes({ui.cbEnableInvididualPageSettings, ui.cbAddExteriorMargins,ui.cbAddInteriorMargins}, true);
            update_settings_sliders({ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins,ui.hsBottomMargins, ui.hsHorizontalMargins, ui.hsVerticalMargins},true);
            update_settings_spinboxes({ui.sbVSizeGrid,ui.sbHSizeGrid,ui.sbNbPhotos}, true);
            update_settings_double_spinboxes({ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins,ui.dsbBottomMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins},true);
            checkbox_enable_UI(ui.cbAddExteriorMargins, {ui.hsLeftMargins, ui.hsRightMargins, ui.hsTopMargins, ui.hsBottomMargins,
                               ui.dsbLeftMargins, ui.dsbRightMargins, ui.dsbTopMargins, ui.dsbBottomMargins});
            checkbox_enable_UI(ui.cbAddInteriorMargins, {ui.hsHorizontalMargins,ui.hsVerticalMargins,ui.dsbHorizontalMargins,ui.dsbVerticalMargins});

            checkbox_enable_UI(ui.cbEnableInvididualPageSettings, {ui.fInterMargins,ui.fColorPage,ui.fExternMargins,ui.fPhotosNb,ui.laHSize, ui.laNbPhotos, ui.laVSize, ui.dsbLeftMargins, ui.hsLeftMargins, ui.dsbRightMargins, ui.hsRightMargins,
                               ui.dsbTopMargins, ui.hsTopMargins, ui.dsbBottomMargins, ui.hsBottomMargins,ui.dsbHorizontalMargins, ui.hsHorizontalMargins,
                               ui.dsbVerticalMargins, ui.hsVerticalMargins,ui.cbAddExteriorMargins,ui.cbAddInteriorMargins,ui.sbVSizeGrid,ui.sbHSizeGrid, ui.laColorPage, ui.tbColorPage});

            // page color
            QPixmap pixColor(16, 16);
            pixColor.fill(Qt::white);
            QAction *actionPageColor = new QAction(pixColor, tr("Couleur de la page"), this);
            connect(actionPageColor, &QAction::triggered, this, [=, &pageUI]{

                pageUI.currentColorPage = QColorDialog::getColor(pageUI.currentColorPage, nullptr, "Choix de la couleur de page");
                if (!pageUI.currentColorPage.isValid())
                    return;

                QPixmap pix(16, 16);
                pix.fill(pageUI.currentColorPage);
                actionPageColor->setIcon(pix);
                emit update_settings_signal();
            });
            ui.tbColorPage->setDefaultAction(actionPageColor);
        }

    }else if(diff > 0){ // remove diff pages

        for(int ii = 0; ii < -diff;++ ii){
            individualPageUI.pop_back();
        }
    }
}

void pc::UIElements::update_settings_buttons(QVector<QPushButton *> buttons, bool displayZones){
    for(auto &&button : buttons){
        connect(button, &QPushButton::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_sliders(QVector<QSlider *> sliders, bool displayZones){
    for(auto &&slider : sliders){
        connect(slider, &QSlider::valueChanged, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_spinboxes(QVector<QSpinBox *> spinBoxes, bool displayZones){
    for(auto &&spinBox : spinBoxes){
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_double_spinboxes(QVector<QDoubleSpinBox *> dSpinBoxes, bool displayZones){
    for(auto &&dSpinBox : dSpinBoxes){
        connect(dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_checkboxes(QVector<QCheckBox *> checkBoxes, bool displayZones){
    for(auto &&cb : checkBoxes){
        connect(cb, &QCheckBox::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::update_settings_format_combo_boxes(QComboBox *comboDpi, QComboBox *comboFormat, bool displayZones){

    connect(comboDpi, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        if(displayZones){
            zonesTimer.start(1000);
        }
        emit update_settings_signal();
    });
    connect(comboFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{

        if(displayZones){
            zonesTimer.start(1000);
        }
        emit update_settings_signal();
    });
}

void pc::UIElements::update_settings_radio_buttons(QVector<QRadioButton *> radioButtons, bool displayZones){
    for(auto &&rb : radioButtons){
        connect(rb, &QRadioButton::clicked, this, [=]{
            if(displayZones){
                zonesTimer.start(1000);
            }
            emit update_settings_signal();
        });
    }
}

void pc::UIElements::associate_buttons(QVector<QPushButton *> buttons){
    auto nbButtons = buttons.size();
    for(decltype(nbButtons) ii = 0; ii < nbButtons; ++ii){
        connect(buttons[ii], &QPushButton::clicked, this, [=]{
            for(decltype(nbButtons) jj = 0; jj < nbButtons; ++jj){
                buttons[jj]->setEnabled(ii != jj);
            }
        });
    }
}

void pc::UIElements::associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider){
    connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        slider->blockSignals(true);
        slider->setValue(1000*value);
        slider->blockSignals(false);
    });
    // # sliders
    connect(slider, &QSlider::valueChanged, [=](double value){
        sb->blockSignals(true);
        sb->setValue(value*0.001);
        sb->blockSignals(false);
    });
}

void pc::UIElements::associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2){
    connect(sb1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb2->value() > 1){
            value = 1 - sb2->value();
            sb1->setValue(value);
        }
        slider1->blockSignals(true);
        slider1->setValue(1000*value);
        slider1->blockSignals(false);
    });
    connect(sb2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb1->value() > 1){
            value = 1 - sb1->value();
            sb2->setValue(value);
        }
        slider2->blockSignals(true);
        slider2->setValue(1000*value);
        slider2->blockSignals(false);
    });
    // # sliders
    connect(slider1, &QSlider::valueChanged, [=](double value){
        if(value + slider2->value() > 1000){
            value = 1000 - slider2->value();
            slider1->setValue(value);
        }
        sb1->blockSignals(true);
        sb1->setValue(value*0.001);
        sb1->blockSignals(false);
    });
    connect(slider2, &QSlider::valueChanged, [=](double value){
        if(value + slider1->value() > 1000){
            value = 1000 - slider1->value();
            slider2->setValue(value);
        }
        sb2->blockSignals(true);
        sb2->setValue(value*0.001);
        sb2->blockSignals(false);
    });
}

void pc::UIElements::checkbox_enable_UI(QCheckBox *cb, QVector<QWidget *> widgets, bool inverted){
    connect(cb, &QCheckBox::clicked, this, [=](bool checked){
        for(auto &&widget : widgets)
            widget->setEnabled(inverted ? (!checked) : checked);
    });
}



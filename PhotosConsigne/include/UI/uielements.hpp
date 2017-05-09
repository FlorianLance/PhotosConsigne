
#pragma once

/**
 * \file UIElements.hpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Utility.hpp"
// # generated ui
#include "ui_PhotosConsigneMainW.h"
#include "ui_IndividualConsign.h"
#include "ui_Support.h"
#include "ui_Help.h"
#include "ui_IndividualPage.h"


namespace Ui {
    class PhotosConsigneMainW;
    class IndividualConsignW;
    class IndividualPageW;
    class SupportW;
    class HelpW;
}

namespace pc{

struct ImageResource{

    bool hasBeenAdded = false;
    QUrl url;
    QImage image;
};

struct PageUI{

    PageUI(){
        ui.setupUi(widget.get());
    }

    QColor currentColorPage = qRgba(255,255,255,255);
    Ui::IndividualPageW      ui;
    std::shared_ptr<QWidget> widget = std::make_shared<QWidget>();
};

struct TitleUI{

    TitleUI(){

        richTextEdit->init_as_title();
        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
        richTextEdit->setEnabled(false);
        html = std::make_shared<QString>(richTextEdit->textEdit()->document()->toHtml());
    }

    std::shared_ptr<QString>        html = nullptr;
    std::shared_ptr<RichTextEdit>   richTextEdit = std::make_shared<RichTextEdit>();
};

struct GlobalConsignUI{

    GlobalConsignUI(){

        richTextEdit->init_as_consign();
        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
        html = std::make_shared<QString>(richTextEdit->textEdit()->document()->toHtml());
    }

    std::shared_ptr<QString>         html = nullptr;
    std::shared_ptr<RichTextEdit>    richTextEdit = std::make_shared<RichTextEdit>();
};

struct IndividualConsignUI{

    IndividualConsignUI(){

        ui.setupUi(widget.get());
        ui.wTop->setEnabled(false);
        ui.tbConsigns->setEnabled(false);
        ui.vlIndividualConsign->addWidget(richTextEdit.get());

        richTextEdit->init_as_individual_consign();
        richTextEdit->init_colors_icons(qRgba(0,0,0,255), qRgba(225,225,225,0));
        html = std::make_shared<QString>(richTextEdit->textEdit()->document()->toHtml());
    }

    Ui::IndividualConsignW          ui;
    std::shared_ptr<QString>        html = nullptr;
    std::shared_ptr<QWidget>        widget       = std::make_shared<QWidget>();
    std::shared_ptr<RichTextEdit>   richTextEdit = std::make_shared<RichTextEdit>();
};



class UIElements : public QObject{

    Q_OBJECT

public :


    UIElements(std::shared_ptr<Ui::PhotosConsigneMainW> mainUI);

    ~UIElements();

    // consign related
    void insert_individual_consign(bool whiteSpace = false);
    void remove_individual_consign(int index);
    void reset_individual_consigns(int nbPhotos);
    void init_individual_consign_with_global(IndividualConsignUI &consignUI, bool copyHtml = true);

    // pages related
    void update_individual_pages(const GlobalData &settings);

    // utility
    void update_settings_buttons(QVector<QPushButton*> buttons, bool displayZones = false);
    void update_settings_sliders(QVector<QSlider*> sliders, bool displayZones = false);
    void update_settings_spinboxes(QVector<QSpinBox*> spinBoxes, bool displayZones = false);
    void update_settings_double_spinboxes(QVector<QDoubleSpinBox*> dSpinBoxes, bool displayZones = false);
    void update_settings_checkboxes(QVector<QCheckBox*> checkBoxes, bool displayZones = false);
    void update_settings_format_combo_boxes(QComboBox *comboDpi, QComboBox *comboFormat, bool displayZones = false);
    void update_settings_radio_buttons(QVector<QRadioButton*> radioButtons, bool displayZones = false);
    void associate_buttons (QVector<QPushButton*> buttons);
    void associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider);
    void associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2);
    void checkbox_enable_UI(QCheckBox *cb, QVector<QWidget*> widgets, bool inverted = false);

    void safe_init_push_button_enabled_state(QPushButton* button, bool state);
    void safe_init_checkboxe_checked_state(QCheckBox *cb, bool state);
    void safe_init_double_spinbox_value(QDoubleSpinBox *dsb, qreal value);
    void safe_init_slider_value(QSlider *slider, int value);


    // members
    QTimer          zonesTimer;
    Position        previousGlobalConsignPositionFromPhotos;
//        QList<Position> previousIndividualConsignPositionFromPhotos;

    std::unique_ptr<QWidget> wSupport  = nullptr;
    std::unique_ptr<QWidget> wHelp     = nullptr;

    ImageLabel   *selectedPhotoW     = nullptr;
    PreviewLabel *previewW           = nullptr;

    TitleUI                    titleUI;
    GlobalConsignUI            globalConsignUI;
    QList<PageUI>              individualPageUI;
    QList<IndividualConsignUI> individualConsignsLoadedUI;
    QList<IndividualConsignUI> individualConsignsValidedUI;

private:

    std::shared_ptr<Ui::PhotosConsigneMainW> m_mainUI = nullptr;

signals:

    void update_settings_signal();

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void send_page_color_signal(QColor col);

    void resource_added_signal(QUrl url, QImage image);
};
}

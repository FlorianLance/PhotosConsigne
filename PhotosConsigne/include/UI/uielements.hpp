
#pragma once

/**
 * \file UIElements.hpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Utility.h"
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

struct PageUI{

    QColor currentColorPage = Qt::white;

    std::shared_ptr<QWidget> widget = nullptr;
};

class UIElements : public QObject{

    Q_OBJECT

public :


    UIElements(QSharedPointer<Ui::PhotosConsigneMainW> mainUI) : m_mainUI(mainUI) {}

    ~UIElements();

    // consign related
    void insert_individual_consign(int index, bool whiteSpace = false);
    void remove_individual_consign(int index);
    void reset_individual_consigns(int nbPhotos);

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

    // members
    QReadWriteLock  docLocker;
    QTimer          zonesTimer;
    Position        previousGlobalConsignPositionFromPhotos;
//        QList<Position> previousIndividualConsignPositionFromPhotos;

    std::unique_ptr<QWidget> wSupport  = nullptr;
    std::unique_ptr<QWidget> wHelp     = nullptr;

    ImageLabel   *selectedPhotoW     = nullptr;
    PreviewLabel *previewW           = nullptr;
    RichTextEdit *titleTEdit         = nullptr;
    RichTextEdit *globalConsignTEdit = nullptr;

    QList<std::shared_ptr<RichTextEdit>> individualConsignsTEditLoaded;
    QList<std::shared_ptr<QWidget>>      individualConsignsWLoaded;

    QList<std::shared_ptr<RichTextEdit>> individualConsignsTEditValided;
    QList<std::shared_ptr<QWidget>>      individualConsignsWValided;

//    QList<std::shared_ptr<QWidget>>      individualPageW; // -> make a struct -> QList<struct> -> stock color and previous position
    QList<PageUI>      individualPageW;



private:

    QSharedPointer<Ui::PhotosConsigneMainW> m_mainUI = nullptr;

signals:

    void update_settings_signal();

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void insert_white_space_signal();

    void send_page_color_signal(QColor col);
};
}



#include "SettingsW.hpp"

using namespace pc;


int Utility::photo_alignment_from_comboBox(QComboBox *cb){

    int photoAlignment = 0;
    switch(cb->currentIndex()){
        case 0: // center left
        photoAlignment = Qt::AlignVCenter | Qt::AlignLeft;
        break;
        case 1: // center center
        photoAlignment = Qt::AlignVCenter | Qt::AlignHCenter;
        break;
        case 2: // center right
        photoAlignment = Qt::AlignVCenter | Qt::AlignRight;
        break;
        case 3: // top left
        photoAlignment = Qt::AlignTop | Qt::AlignLeft;
        break;
        case 4: // top center
        photoAlignment = Qt::AlignTop | Qt::AlignHCenter;
        break;
        case 5: // top right
        photoAlignment = Qt::AlignTop | Qt::AlignRight;
        break;
        case 6: // bottom left
        photoAlignment = Qt::AlignBottom | Qt::AlignLeft;
        break;
        case 7: // bottom center
        photoAlignment = Qt::AlignBottom | Qt::AlignHCenter;
        break;
        case 8: // bottom right
        photoAlignment = Qt::AlignBottom | Qt::AlignRight;
        break;
    }

    return photoAlignment;
}

PhotoAdjust Utility::photo_adjust_from_comboBox(QComboBox *cb){

    PhotoAdjust adjust = PhotoAdjust::center;
    switch( cb->currentIndex()){
        case 1:
            adjust = PhotoAdjust::extend;
        break;
        case 2:
            adjust = PhotoAdjust::fill;
        break;
        case 3:
            adjust = PhotoAdjust::adjust;
        break;
        case 4:
            adjust = PhotoAdjust::mosaic;
        break;
    }
    return adjust;
}

qreal Utility::borders_line_width_from_comboBox(QComboBox *cb){

    qreal borderWidth = 1.;
    switch(cb->currentIndex()){
        case 1:
            borderWidth = 3.;
        break;
        case 2:
            borderWidth = 5.;
        break;
        case 3:
            borderWidth = 7.;
        break;
        case 4:
            borderWidth = 9.;
        break;
        case 5:
            borderWidth = 15.;
        break;
    }
    return borderWidth;
}

Qt::PenJoinStyle Utility::borders_join_style_from_comboBox(QComboBox *cb){

    Qt::PenJoinStyle style = Qt::BevelJoin;
    switch(cb->currentIndex()){
        case 1:
            style = Qt::MiterJoin;
        break;
        case 2:
            style = Qt::RoundJoin;
        break;
    }

    return style;
}

void Utility::borders_line_style_from_comboBox(QComboBox *cb, QPen &pen){

    QVector<qreal> dashes;
    qreal space;
    switch(cb->currentIndex()){
            case 0:
            pen.setStyle(Qt::SolidLine);
        break;
            case 1:
            pen.setStyle(Qt::CustomDashLine);
            space = 6.;
            dashes << 1 <<space;
            pen.setDashPattern(dashes);
        break;
            case 2:
            pen.setStyle(Qt::CustomDashLine);
            space = 4.;
            dashes << 7 << space;
            pen.setDashPattern(dashes);
        break;
            case 3:
            pen.setStyle(Qt::CustomDashLine);
            space = 4.;
            dashes << 1 <<space << 5 << space;
            pen.setDashPattern(dashes);
        break;
            case 4:
            pen.setStyle(Qt::CustomDashLine);
            space = 4.;
            dashes << 1 <<space << 1 << space << 5 << space;
            pen.setDashPattern(dashes);
        break;
    }
}

Qt::BrushStyle Utility::pattern_style_comboBox(QComboBox *cb){

    Qt::BrushStyle style;
    switch(cb->currentIndex()){
        case 0:
            style = Qt::BDiagPattern;
        break;
        case 1:
            style = Qt::CrossPattern;
        break;
        case 2:
            style = Qt::Dense1Pattern;
        break;
        case 3:
            style = Qt::Dense2Pattern;
        break;
        case 4:
            style = Qt::Dense3Pattern;
        break;
        case 5:
            style = Qt::Dense4Pattern;
        break;
        case 6:
            style = Qt::Dense5Pattern;
        break;
        case 7:
            style = Qt::Dense6Pattern;
        break;
        case 8:
            style = Qt::Dense7Pattern;
        break;
        case 9:
            style = Qt::DiagCrossPattern;
        break;
        case 10:
            style = Qt::FDiagPattern;
        break;
        case 11:
            style = Qt::HorPattern;
        break;
    }
    return style;
}

void pc::Utility::associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider){
    QObject::connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(10000*value));
        slider->blockSignals(false);
    });
    // # sliders
    QObject::connect(slider, &QSlider::valueChanged, [=](double value){
        sb->blockSignals(true);
        sb->setValue(value*0.0001);
        sb->blockSignals(false);
    });
}

void pc::Utility::associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2){
    QObject::connect(sb1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb2->value() > 1){
            value = 1 - sb2->value();
            sb1->setValue(value);
        }
        slider1->blockSignals(true);
        slider1->setValue(static_cast<int>(10000*value));
        slider1->blockSignals(false);
    });
    QObject::connect(sb2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double value){
        if(value + sb1->value() > 1){
            value = 1 - sb1->value();
            sb2->setValue(value);
        }
        slider2->blockSignals(true);
        slider2->setValue(static_cast<int>(10000*value));
        slider2->blockSignals(false);
    });
    // # sliders
    QObject::connect(slider1, &QSlider::valueChanged, [=](double value){
        if(value + slider2->value() > 10000){
            value = 10000 - slider2->value();
            slider1->setValue(static_cast<int>(value));
        }
        sb1->blockSignals(true);
        sb1->setValue(value*0.0001);
        sb1->blockSignals(false);
    });
    QObject::connect(slider2, &QSlider::valueChanged, [=](double value){
        if(value + slider1->value() > 10000){
            value = 10000 - slider1->value();
            slider2->setValue(static_cast<int>(value));
        }
        sb2->blockSignals(true);
        sb2->setValue(value*0.0001);
        sb2->blockSignals(false);
    });
}


void pc::Utility::checkbox_enable_UI(QCheckBox *cb, QVector<QWidget *> widgets, bool inverted){
    QObject::connect(cb, &QCheckBox::clicked, [=](bool checked){
        for(auto &&widget : widgets)
            widget->setEnabled(inverted ? (!checked) : checked);
    });
}

void Utility::safe_init_push_button_enabled_state(QPushButton *button, bool state){
    button->blockSignals(true);
    button->setEnabled(state);
    button->blockSignals(false);
}

void Utility::safe_init_tool_button_enabled_state(QToolButton *button, bool state){
    button->blockSignals(true);
    button->setEnabled(state);
    button->blockSignals(false);
}

void Utility::safe_init_radio_button_state(QRadioButton *rb, bool state){
    rb->blockSignals(true);
    rb->setChecked(state);
    rb->blockSignals(false);
}

void Utility::safe_init_checkboxe_checked_state(QCheckBox *cb, bool state){
    cb->blockSignals(true);
    cb->setChecked(state);
    cb->blockSignals(false);
}

void Utility::safe_init_spinbox_value(QSpinBox *sb, int value){
    sb->blockSignals(true);
    sb->setValue(value);
    sb->blockSignals(false);
}

void Utility::safe_init_double_spinbox_value(QDoubleSpinBox *dsb, qreal value){
    dsb->blockSignals(true);
    dsb->setValue(value);
    dsb->blockSignals(false);
}

void Utility::safe_init_slider_value(QSlider *slider, int value){
    slider->blockSignals(true);
    slider->setValue(value);
    slider->blockSignals(false);
}

void Utility::safe_init_combo_box_index(QComboBox *cb, int index){
    cb->blockSignals(true);
    cb->setCurrentIndex(index);
    cb->blockSignals(false);
}

void Utility::safe_init_tab_widget_index(QTabWidget *tw, int index){
    tw->blockSignals(true);
    tw->setCurrentIndex(index);
    tw->blockSignals(false);
}

void Utility::safe_init_tool_box_index(QToolBox *tb, int index){
    tb->blockSignals(true);
    tb->setCurrentIndex(index);
    tb->blockSignals(false);
}

void SettingsW::init_checkboxes_connections(QVector<QCheckBox *> checkBoxes, bool displayZones){

    for(auto &&cb : checkBoxes){
        connect(cb, &QCheckBox::clicked, this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}

void SettingsW::init_comboboxes_connections(QVector<QComboBox *> comboBox, bool displayZones){

    for(auto &&cb : comboBox){
        connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}

void pc::SettingsW::init_color_dialog_connections(QToolButton *tb, QString actionToolTip, QString dialogColorText, QColor *color, QSize iconSize, bool alpha){

    QPixmap pixColor(iconSize.width(), iconSize.height());
    pixColor.fill(*color);
    QAction *actionColor = new QAction(pixColor, actionToolTip, this);
    connect(actionColor, &QAction::triggered, this, [=]{

        QColor newColor;
        if(alpha){
            newColor = QColorDialog::getColor(*color, nullptr, dialogColorText, QColorDialog::ColorDialogOption::ShowAlphaChannel);
        }else{
            newColor = QColorDialog::getColor(*color, nullptr, dialogColorText);
        }

        if (!newColor.isValid())
            return;

        *color = newColor;

        QPixmap pix(iconSize.width(), iconSize.height());
        pix.fill(*color);
        actionColor->setIcon(pix);
        emit settings_updated_signal(false);
    });
    tb->setDefaultAction(actionColor);
}

void SettingsW::init_image_dialog_connections(QToolButton *tb, QString actionToolTip, QString dialogText, SPhoto &photo, QSize iconeSIze){

    QAction *actionImage = new QAction(tb->icon(), actionToolTip, this);
    connect(actionImage, &QAction::triggered, this, [=,&photo]{

        QString filePath = QFileDialog::getOpenFileName(nullptr, dialogText, QDir::homePath(), "Images (*.png *.jpg *.jpeg *.jfif, *.jpe *.tif *.bmp *.pdm *.ppm *.xdm *.xpm)");
        if(filePath.size() == 0){
            return;
        }

        photo = std::make_shared<Photo>(Photo(filePath));
        actionImage->setIcon(QPixmap::fromImage(photo->scaledPhoto.scaled(iconeSIze.width(), iconeSIze.height(), Qt::KeepAspectRatio)));
        emit settings_updated_signal(false);
    });
    tb->setDefaultAction(actionImage);
}

void SettingsW::init_pushbuttons_connections(QVector<QPushButton *> buttons, bool displayZones){
    for(auto &&button : buttons){
        connect(button, &QPushButton::clicked, this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}

void pc::SettingsW::init_sliders_connections(QVector<QSlider *> sliders, bool displayZones){
    for(auto &&slider : sliders){
        connect(slider, &QSlider::valueChanged, this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}

void pc::SettingsW::init_spinboxes_connections(QVector<QSpinBox *> spinBoxes, bool displayZones){
    for(auto &&spinBox : spinBoxes){
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}

void pc::SettingsW::init_doublespinboxes_connections(QVector<QDoubleSpinBox *> dSpinBoxes, bool displayZones){
    for(auto &&dSpinBox : dSpinBoxes){
        connect(dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=]{
            emit settings_updated_signal(displayZones);;
        });
    }
}

void SettingsW::init_radiobuttons_connections(QVector<QRadioButton *> buttons, bool displayZones){

    for(auto &&cb : buttons){
        connect(cb, &QRadioButton::clicked, this, [=]{
            emit settings_updated_signal(displayZones);
        });
    }
}


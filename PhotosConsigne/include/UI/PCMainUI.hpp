
/**
 * \file PCMainUI.hpp
 * \brief defines PCMainUI
 * \author Florian Lance
 * \date 04/04/2017
 */

#pragma once


// Qt
#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <QTimer>

// local
// # generated ui
#include "ui_PhotosConsigneMainW.h"
#include "ui_IndividualConsign.h"

// # ui
#include "RichTextUI.hpp"
#include "ImageLabel.hpp"

// # workers
#include "PCMainWorker.hpp"

namespace Ui {
class PhotosConsigneMainW;
class IndividualConsign;
}

namespace pc {

/**
 * @brief ...
 */
class PCMainUI : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief PCMainUI constructor
     * @param [in,out] parent
     */
    PCMainUI(QApplication *parent = 0);

    /**
     * @brief PCMainUI destructor
     */
    ~PCMainUI();

    void update_settings_buttons(QVector<QPushButton*> buttons, bool displayZones = false){
        for(auto &&button : buttons){
            connect(button, &QPushButton::clicked, this, [=]{
                if(displayZones){
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }

    void update_settings_sliders(QVector<QSlider*> sliders, bool displayZones = false){
        for(auto &&slider : sliders){
            connect(slider, &QSlider::valueChanged, this, [=]{
                if(displayZones){
                    qDebug() << "update_settings_sliders ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }
    void update_settings_spinboxes(QVector<QSpinBox*> spinBoxes, bool displayZones = false){
        for(auto &&spinBox : spinBoxes){
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{
                if(displayZones){
                    qDebug() << "update_settings_spinboxes ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }
    void update_settings_double_spinboxes(QVector<QDoubleSpinBox*> dSpinBoxes, bool displayZones = false){
        for(auto &&dSpinBox : dSpinBoxes){
            connect(dSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=]{
                if(displayZones){
                    qDebug() << "update_settings_double_spinboxes ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }
    void update_settings_checkboxes(QVector<QCheckBox*> checkBoxes, bool displayZones = false){
        for(auto &&cb : checkBoxes){
            connect(cb, &QCheckBox::clicked, this, [=]{
                if(displayZones){
                    qDebug() << "update_settings_checkboxes ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }
    void update_settings_format_combo_boxes(QVector<QComboBox*> comboBoxes, bool displayZones = false){
        for(auto &&cb : comboBoxes){
            connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
                PaperFormat pf(m_ui->cbDPI->currentText(),m_ui->cbFormat->currentText());
                int currentDPI = m_ui->cbDPI->currentText().toInt();
                m_ui->laDefWxH->setText(QString::number(pf.widthPixels(currentDPI)) + "x" + QString::number(pf.heightPixels(currentDPI)));
                m_ui->laDefTotal->setText("(" + QString::number(pf.widthPixels(currentDPI)*pf.heightPixels(currentDPI)) + " pixels)");
                if(displayZones){
                    qDebug() << "update_settings_format_combo_boxes ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }
    void update_settings_radio_buttons(QVector<QRadioButton*> radioButtons, bool displayZones = false){
        for(auto &&rb : radioButtons){
            connect(rb, &QRadioButton::clicked, this, [=]{
                if(displayZones){
                    qDebug() << "update_settings_radio_buttons ## start timer!";
                    m_zonesTimer.start(1000);
                }
                update_settings();
            });
        }
    }

    void associate_buttons (QVector<QPushButton*> buttons){
        auto nbButtons = buttons.size();
        for(decltype(nbButtons) ii = 0; ii < nbButtons; ++ii){
            connect(buttons[ii], &QPushButton::clicked, this, [=]{
                for(decltype(nbButtons) jj = 0; jj < nbButtons; ++jj){
                    buttons[jj]->setEnabled(ii != jj);
                }
            });
        }
    }

    void associate_double_spinbox_with_slider(QDoubleSpinBox *sb, QSlider *slider){
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

    void associate_double_spinbox_with_slider(QDoubleSpinBox *sb1, QSlider *slider1, QDoubleSpinBox *sb2, QSlider *slider2){
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


public slots:

    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent *event)
    {
        Q_UNUSED(event);
        emit kill_signal();
    }


private slots:

//    /**
//     * @brief openOnlineDocumentation
//     */
//    void openOnlineDocumentation();

//    /**
//     * @brief openDonatePage
//     */
//    void openDonatePage();

//    /**
//     * @brief openAboutWindow
//     */
//    void openAboutWindow();

    void set_photos_directory();

    void update_photos_number(bool reset = false);

    void reset_pc_pages();

    void end_loading_photos(SPhotos images);

    void update_settings_with_no_preview(){
        m_settings.noPreviewGeneration = true;
        update_settings();
        m_settings.noPreviewGeneration = false;
    }
    void update_settings();

    // photo display
    void update_photo_to_display(SPhoto photo);

signals :

    void kill_signal();

    void send_photos_dir_signal(QString photosDir, QStringList photosNames);

    void start_preview_generation_signal(GlobalParameters settings, PCPages pcPages);

    void start_PDF_generation_signal(GlobalParameters settings, PCPages pcPages);

private:

    // parameters
    bool m_isPreviewComputing = false;
    bool m_generatePreviewAgain = false;
    QReadWriteLock m_previewLocker;

    QTimer m_zonesTimer;

    PCPages m_pcPages;
    GlobalParameters m_settings;

    // widgets
    ImageLabel *m_selectedPhotoW = nullptr; /**< selected photo widget image display */
    PreviewLabel *m_previewW = nullptr;
    RichTextEdit *m_titleTEdit = nullptr;
    RichTextEdit *m_globalConsignTEdit = nullptr;
    QVector<std::shared_ptr<RichTextEdit>>  m_individualConsignsTEdit;
    QVector<std::shared_ptr<QWidget>>       m_individualConsignsW;

    // threads
    std::unique_ptr<PhotoDisplayWorker> m_displayPhotoWorker = nullptr;
    std::unique_ptr<PDFGeneratorWorker> m_pdfGeneratorWorker = nullptr;

    // workers
    QThread m_displayPhotoWorkerThread;
    QThread m_pdfGeneratorWorkerThread;

    // ui
    Ui::PhotosConsigneMainW *m_ui = nullptr; /**< PC Main widget */
};
}

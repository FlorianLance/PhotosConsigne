
#pragma once

// Qt
#include <QWidget>
#include <QThread>

// ui
#include "ui_PreviewUI.h"

// lccal
#include "PreviewWorker.h"
#include "ImageLabel.h"

namespace Ui {
class PreviewUI;
}

namespace pc{


/**
 * @brief ...
 */
class PreviewUI : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief PreviewUI constructor
     * @param [in,out] parent
     */
    PreviewUI(QWidget *parent = 0) : ui(new Ui::PreviewUI)
    {
        Q_UNUSED(parent);

        ui->setupUi(this);
        m_worker = new PreviewWorker();

        // pdf generation
        ui->pbGeneration->setIcon(QIcon(":/images/save"));
        ui->pbGeneration->setIconSize(QSize(35,35));
        ui->pbGeneration->setToolTip(tr("Lance la génération du PDF et le sauvegarde"));

        // open pdf
        ui->pbOpenPDF->setIcon(QIcon(":/images/pdf"));
        ui->pbOpenPDF->setIconSize(QSize(35,35));
        ui->pbOpenPDF->setToolTip(tr("Ouvre le dernier PDF généré avec le lecteur par défaut"));

        // legend
        QString legend("<p><font color=white>LEGENDE :</font> &nbsp;&nbsp;&nbsp;<font color=red>MARGES EXTERIEURES</font> &nbsp;&nbsp;&nbsp;<font color=green>MARGES INTERIEURES</font> &nbsp;&nbsp;&nbsp;<font color=cyan>ESPACE CONSIGNE</font> &nbsp;&nbsp;&nbsp;<font color=yellow>ESPACE PHOTO</font></p>");
        ui->laLegend->setText(legend);

        //  preview label
        m_previewLabel = new ImageLabel(this);
        ui->hlPreviewImage->insertWidget(0, m_previewLabel, Qt::AlignHCenter);

        // connections
        //     zones
        connect(ui->cbZoneConsignes, &QCheckBox::clicked, this, [&](bool checked){m_zones.consigns = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPage); });
        connect(ui->cbZoneExternMargins, &QCheckBox::clicked, this, [&](bool checked){m_zones.externMargins = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPage); });
        connect(ui->cbZoneInternMargins, &QCheckBox::clicked, this, [&](bool checked){m_zones.InterMargins = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPage); });
        connect(ui->cbZonePhotos, &QCheckBox::clicked, this, [&](bool checked){m_zones.photos = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPage); });
        //     worker
        connect(this, &PreviewUI::startPreviewGenerationSignal, m_worker, &PreviewWorker::generatePreview);
        connect(m_worker, &PreviewWorker::endPreviewSignal, this, &PreviewUI::displayPreview);

        // init thread
        m_worker->moveToThread(&m_workerThread);
        m_workerThread.start();
    }

    /**
     * @brief PreviewUI destructor
     */
    ~PreviewUI()
    {
        delete ui;

        m_workerThread.quit();
        m_workerThread.wait();
        delete m_worker;
    }

public slots:


    void updateUI(bool state)
    {
        ui->cbZoneConsignes->setEnabled(state);
        ui->cbZoneExternMargins->setEnabled(state);
        ui->cbZoneInternMargins->setEnabled(state);
        ui->cbZonePhotos->setEnabled(state);
    }

    void updatePreview(SImages images, SPDFSettings settings, Page currentPage)
    {
        // update last data
        m_lastImages = images;
        m_lastSettings = settings;
        m_lastPage = currentPage;

        // disable ui
        ui->pbGeneration->setEnabled(false);

        // state preview
        emit startPreviewGenerationSignal(images, settings, currentPage, m_zones);
    }

    void displayPreview(QPixmap preview)
    {
        // update image label
        m_previewLabel->setPixmap(preview);
        m_previewLabel->update();
    }



private slots :


signals :


    void startPreviewGenerationSignal(SImages images, SPDFSettings settings, Page currentPage, PreviewZonesDisplay zones);

private :

    Page m_lastPage;
    SImages m_lastImages = SImages(new QList<SImage>());
    SPDFSettings m_lastSettings = SPDFSettings(new PDFSettings());
    PreviewZonesDisplay m_zones;


    ImageLabel *m_previewLabel = nullptr;   /**< display preview widget */


    Ui::PreviewUI *ui = nullptr; /**< ui left menu */

    QThread  m_workerThread;            /**< worker thread */
    PreviewWorker *m_worker = nullptr;  /**< worker of the menu*/
};
}

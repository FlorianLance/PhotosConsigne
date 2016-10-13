
#pragma once

// Qt
#include <QWidget>
#include <QThread>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

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

        //  preview label
        m_previewLabel = new ImageLabel(this);
        ui->hlPreviewImage->insertWidget(0, m_previewLabel, Qt::AlignHCenter);

        connect(this, &PreviewUI::killSignal, m_worker, &PreviewWorker::kill);
        // connections
        //     zones
        connect(ui->cbZoneConsignes, &QCheckBox::clicked, this, [&](bool checked){m_zones.consigns = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPageId, m_lastPages); });
        connect(ui->cbZoneExternMargins, &QCheckBox::clicked, this, [&](bool checked){m_zones.externMargins = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPageId, m_lastPages); });
        connect(ui->cbZoneInternMargins, &QCheckBox::clicked, this, [&](bool checked){m_zones.InterMargins = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPageId, m_lastPages); });
        connect(ui->cbZonePhotos, &QCheckBox::clicked, this, [&](bool checked){m_zones.photos = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPageId, m_lastPages); });
        connect(ui->cbZoneTitles, &QCheckBox::clicked, this, [&](bool checked){m_zones.titles = checked; updatePreview(m_lastImages,m_lastSettings, m_lastPageId, m_lastPages); });
        //      button
        connect(ui->pbOpenPDF, &QPushButton::clicked, this, &PreviewUI::openPDF);
        //     worker
        connect(this, &PreviewUI::startPreviewGenerationSignal, m_worker, &PreviewWorker::generatePreview);
        connect(this, &PreviewUI::startGenerationSignal, m_worker, &PreviewWorker::generatePDF);        
        connect(ui->pbGeneration, &QPushButton::clicked, this, &PreviewUI::generatePDF);
        connect(m_worker, &PreviewWorker::endPreviewSignal, this, &PreviewUI::displayPreview);
        connect(m_worker, &PreviewWorker::endGenerationSignal, this, &PreviewUI::endGeneration);

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

    /**
     * @brief Worker
     * @return
     */
    PreviewWorker* Worker() const noexcept {return m_worker;}

public slots:

    void kill()
    {
        emit killSignal();
    }

    /**
     * @brief updateUI
     * @param state
     */
    void updateUI(bool state)
    {
        ui->cbZoneConsignes->setEnabled(state);
        ui->cbZoneExternMargins->setEnabled(state);
        ui->cbZoneInternMargins->setEnabled(state);
        ui->cbZonePhotos->setEnabled(state);
        ui->cbZoneTitles->setEnabled(state);        
    }

    /**
     * @brief updatePreview
     * @param images
     * @param settings
     * @param currentPageId
     * @param pages
     */
    void updatePreview(SImages images, SPDFSettings settings,  int currentPageId, QVector<Page> pages)
    {
        // update last data
        m_lastPageId = currentPageId;
        m_lastImages = images;
        m_lastSettings = settings;
        m_lastPages = pages;

        // disable ui
//        ui->pbOpenPDF->setEnabled(false);

        // state preview
        emit startPreviewGenerationSignal(m_lastImages, m_lastSettings, m_lastPages[currentPageId],  m_zones);
    }

    /**
     * @brief displayPreview
     * @param preview
     */
    void displayPreview(QPixmap preview)
    {
        // update image label
        m_previewLabel->setPixmap(preview);
        m_previewLabel->update();
        ui->pbGeneration->setEnabled(true);
    }

    /**
     * @brief generatePDF
     */
    void generatePDF()
    {
        m_pdfFileName = QFileDialog::getSaveFileName(this, "Nom du fichier pdf",  QDir::homePath(), "*.pdf");

        if(m_pdfFileName.size() == 0)
            return;

        if(m_pdfFileName.right(4) != ".pdf")
            m_pdfFileName += ".pdf";

        ui->pbOpenPDF->setEnabled(false);
        ui->pbGeneration->setEnabled(false);
        emit startGenerationSignal(m_pdfFileName, m_lastImages, m_lastSettings, m_lastPages, m_zones);
    }

    /**
     * @brief endGeneration
     * @param finished
     */
    void endGeneration(bool finished)
    {
        ui->pbOpenPDF->setEnabled(finished);
        ui->pbGeneration->setEnabled(true);
    }

    /**
     * @brief openPDF
     */
    void openPDF()
    {
        bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(m_pdfFileName));
        if(!success)
            QMessageBox::warning(this, tr("Avertissement"), tr("Le PDF n'a pu être lancé.\nVeuillez vous assurez que vous disposez d'un logiciel de lecture de PDF (ex : SumatraPDF, AdobeReader...) .\n"),QMessageBox::Ok);
    }


private slots :


signals :

    void setProgressBarStateSignal(int state);

    void setProgressBarTextSignal(QString text);

    /**
     * @brief startPreviewGenerationSignal
     * @param images
     * @param settings
     * @param currentPage
     * @param zones
     */
    void startPreviewGenerationSignal(SImages images, SPDFSettings settings, Page currentPage, PreviewZonesDisplay zones);

    /**
     * @brief startGenerationSignal
     * @param pdfName
     * @param images
     * @param settings
     * @param pages
     * @param zones
     */
    void startGenerationSignal(QString pdfName, SImages images, SPDFSettings settings, QVector<Page> pages, PreviewZonesDisplay zones);

    /**
     * @brief killSignal
     */
    void killSignal();

private :

    int m_lastPageId;                                               /**< ... */
    QString m_pdfFileName;                                          /**< ... */
    QVector<Page> m_lastPages;                                      /**< ... */
    SImages m_lastImages = SImages(new QList<SImage>());            /**< ... */
    SPDFSettings m_lastSettings = SPDFSettings(new PDFSettings());  /**< ... */
    PreviewZonesDisplay m_zones;                                    /**< ... */

    Ui::PreviewUI *ui = nullptr;                                    /**< ui left menu */
    ImageLabel *m_previewLabel = nullptr;                           /**< display preview widget */
    QThread  m_workerThread;                                        /**< worker thread */
    PreviewWorker *m_worker = nullptr;                              /**< worker of the menu*/
};
}

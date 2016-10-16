
#pragma once

// Qt
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCollator>
#include <QReadWriteLock>
#include <QThread>
#include <QBuffer>
#include <QColorDialog>

// ui
#include "ui_LeftMenuUI.h"

// lccal
#include "LeftMenuWorker.h"


namespace Ui {
class LeftMenuUI;
}

namespace pc {


/**
 * @brief ...
 */
class LeftMenuUI : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief LeftMenuUI constructor
     * @param [in,out] parent
     */
    LeftMenuUI(QWidget *parent = 0) : ui(new Ui::LeftMenuUI)
    {
        Q_UNUSED(parent);

        ui->setupUi(this);

        // open photos dir
        ui->pbSelectPhotos->setIcon(QIcon(":/images/dirImages"));
        ui->pbSelectPhotos->setIconSize(QSize(40,40));
        ui->pbSelectPhotos->setToolTip(tr("Définir le dossier contenant les photos à charger dans le PDF"));

        ui->wTitle->setEnabled(false);

        // init worker
        m_worker = new LeftMenuWorker();

        // connections                
        //  ui -> worker
        connect(this, &LeftMenuUI::sendImagesDirSignal, m_worker, &LeftMenuWorker::loadImages);
        connect(this, &LeftMenuUI::killSignal, m_worker, &LeftMenuWorker::kill);
        //  worker -> UI
        connect(m_worker, &LeftMenuWorker::endLoadingImagesSignal, this, &LeftMenuUI::endLoadImages);
        connect(m_worker, &LeftMenuWorker::photoLoadedSignal, this, [&](QString image){
            ui->lwPhotos->addItem(image);

            if(ui->lwPhotos->count() == 1)
                ui->lwPhotos->setCurrentRow(0);
        });
        //  push button
        connect(ui->pbSelectPhotos, &QPushButton::clicked, this, &LeftMenuUI::setPhotosDirectory);
        connect(ui->pbChooseColor, &QPushButton::clicked, this, &LeftMenuUI::setColorText);
        connect(ui->pbChooseColorTitle, &QPushButton::clicked, this, &LeftMenuUI::setColorTextTitle);
        //  radio button
        connect(ui->rbLeftImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbRightImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbHCenterImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbVCenterImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbTopImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbBottomImage, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);

        connect(ui->rbLeftText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbRightText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbHCenterText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbVCenterText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbBottomText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbTopText, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);

        connect(ui->rbLeftTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbRightTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbHCenterTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbVCenterTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbBottomTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbTopTextTitle, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);

        connect(ui->rbLandScape, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbPortrait, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbConsignPositionTop, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbConsignPositionBottom, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);

        connect(ui->rbTitlePositionTop, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        connect(ui->rbTitlePositionBottom, &QRadioButton::clicked, this, &LeftMenuUI::updateSettings);
        // spinbox
        connect(ui->sbNbImagesV, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->sbNbImagesH, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->sbSizeTexte, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->sbSizeTexteTitle, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsbRatio, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsBottomMargins, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsLeftMargins, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsRightMargins, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsTopMargins, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsInterMarginsWidth, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsInterMarginsHeight, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        connect(ui->dsbRatioTitle, static_cast<void(QDoubleSpinBox::*)(const QString &)>(&QDoubleSpinBox::valueChanged), [=]{ updateSettings();});
        // font
        connect(ui->fcbConsignes, &QFontComboBox::currentFontChanged, this, &LeftMenuUI::updateSettings);
        connect(ui->fcbTitle, &QFontComboBox::currentFontChanged, this, &LeftMenuUI::updateSettings);
        // QPlainText
        connect(ui->pteConsigne, &QPlainTextEdit::textChanged, this, &LeftMenuUI::updateSettings);
        connect(ui->pteTitle, &QPlainTextEdit::textChanged, this, &LeftMenuUI::updateSettings);
        // check box
        connect(ui->cbBold, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        connect(ui->cbItalic, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        connect(ui->cbBoldTitle, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        connect(ui->cbItalicTitle, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        connect(ui->cbUseImageNameAsText, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        // combobox
        connect(ui->cbFormat, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), [=]{ updateSettings();});
        connect(ui->cbDefinition, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), [=]{ updateSettings();});

        connect(ui->cbCutLines, &QCheckBox::toggled, this, &LeftMenuUI::updateSettings);
        connect(ui->cbAddTitle, &QCheckBox::toggled, [=]{ ui->wTitle->setEnabled(ui->cbAddTitle->isChecked()); ui->cbAllPagesTitle->setEnabled(ui->cbAddTitle->isChecked()); updateSettings();});
        connect(ui->cbAllPagesTitle, &QCheckBox::toggled, this,  &LeftMenuUI::updateSettings);

        // list widget
        connect(ui->lwPages, &QListWidget::currentRowChanged, this, [&]
        {
           if(ui->lwPages->currentRow() != -1)
               emit currentPageChangedSignal(m_settings, ui->lwPages->currentRow(), m_pages);
        });
        connect(ui->lwPages, &QListWidget::clicked, this, [&]
        {
           if(ui->lwPages->currentRow() != -1)
               emit currentPageChangedSignal(m_settings, ui->lwPages->currentRow(), m_pages);
        });

        // init thread
        m_worker->moveToThread(&m_workerThread);
        m_workerThread.start();

        m_settings->globalConsignFont.setPixelSize(50);
        m_settings->titleFont.setPixelSize(60);
        updateUI(m_settings);
    }




    /**
     * @brief LeftMenuUI destructor
     */
    ~LeftMenuUI()
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
    LeftMenuWorker* Worker() const noexcept {return m_worker;}

    /**
     * @brief ListPhotos
     * @return
     */
    QListWidget* ListPhotos() const noexcept {return ui->lwPhotos;}

    /**
     * @brief ListPages
     * @return
     */
    QListWidget* ListPages() const noexcept {return ui->lwPages;}

    /**
     * @brief currentPage
     * @return
     */
    Page currentPage() const noexcept
    {
        return m_pages[ui->lwPages->currentRow()];
    }


public slots:

    void kill()
    {
        emit killSignal();
    }


    void updateUI(SPDFSettings newSettings)
    {
        m_settings = newSettings;


        // general
        ui->rbPortrait->setChecked(newSettings->pageOrientation == PageOrientation::portrait);
        ui->cbCutLines->setChecked(newSettings->displayCutLines);

        // format
        ui->cbFormat->setCurrentIndex(newSettings->paperFormat.formatId);
        ui->cbDefinition->setCurrentIndex(newSettings->paperFormat.definitionId);

        // titles
        //      font
        QFont fontTitle = ui->fcbTitle->font();
        fontTitle.setFamily(newSettings->titleFont.family());
        ui->fcbTitle->setCurrentFont(fontTitle);
        ui->cbBoldTitle->setChecked(fontTitle.bold());
        ui->cbItalicTitle->setChecked(fontTitle.italic());
        m_colorTextTitle = newSettings->titleColor;
        fontTitle = newSettings->titleFont;
        fontTitle.setPixelSize(13);
        ui->pteTitle->setFont(fontTitle);
        ui->pteTitle->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorTextTitle.red()) +", " + QString::number(m_colorTextTitle.green()) + ", " + QString::number(m_colorTextTitle.blue()) +")};");
        ui->pbChooseColor->setStyleSheet("background-color: rgb("+ QString::number(m_colorTextTitle.red()) + ", " + QString::number(m_colorTextTitle.green()) + ", " + QString::number(m_colorTextTitle.blue()) +");");
        ui->sbSizeTexteTitle->setValue(newSettings->titleFont.pixelSize());
        ui->cbAddTitle->setChecked(newSettings->titleAdded);
        ui->cbAllPagesTitle->setChecked(newSettings->titleOnAllPages);
        ui->rbTitlePositionTop->setChecked(newSettings->titlePositionFromPC == Position::top);
        ui->dsbRatioTitle->setValue(newSettings->ratioTitle);
        //      alignment
        if((newSettings->titleAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
            ui->rbHCenterTextTitle->setChecked(true);
        else if((newSettings->titleAlignment & Qt::AlignRight) == Qt::AlignRight)
            ui->rbRightTextTitle->setChecked(true);
        else
            ui->rbLeftTextTitle->setChecked(true);

        if((newSettings->titleAlignment & Qt::AlignVCenter) == Qt::AlignVCenter)
            ui->rbVCenterTextTitle->setChecked(true);
        else if((newSettings->titleAlignment & Qt::AlignTop) == Qt::AlignTop)
            ui->rbTopTextTitle->setChecked(true);
        else
            ui->rbBottomTextTitle->setChecked(true);

        // PC
        ui->sbNbImagesH->setValue(newSettings->nbImagesPageH);
        ui->sbNbImagesV->setValue(newSettings->nbImagesPageV);
        ui->dsbRatio->setValue(newSettings->ratioPhotosConsign);
        ui->dsBottomMargins->setValue(newSettings->margins.bottom);
        ui->dsTopMargins->setValue(newSettings->margins.top);
        ui->dsRightMargins->setValue(newSettings->margins.right);
        ui->dsLeftMargins->setValue(newSettings->margins.left);
        ui->dsInterMarginsWidth->setValue(newSettings->margins.interWidth);
        ui->dsInterMarginsHeight->setValue(newSettings->margins.interHeight);

        // text
        //      font
        QFont font = ui->fcbConsignes->font();
        font.setFamily(newSettings->globalConsignFont.family());
        ui->fcbConsignes->setCurrentFont(font);
        ui->cbBold->setChecked(font.bold());
        ui->cbItalic->setChecked(font.italic());
        m_colorText = newSettings->globalConsignColor;
        font = newSettings->globalConsignFont;
        font.setPixelSize(13);
        ui->pteConsigne->setFont(font);
        ui->pteConsigne->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorText.red()) +", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +")};");
        ui->pbChooseColor->setStyleSheet("background-color: rgb("+ QString::number(m_colorText.red()) + ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +");");
        ui->sbSizeTexte->setValue(newSettings->globalConsignFont.pixelSize());
        //      alignment
        if((newSettings->consignAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
            ui->rbHCenterText->setChecked(true);
        else if((newSettings->consignAlignment & Qt::AlignRight) == Qt::AlignRight)
            ui->rbRightText->setChecked(true);
        else
            ui->rbLeftText->setChecked(true);
        if((newSettings->consignAlignment & Qt::AlignVCenter) == Qt::AlignVCenter)
            ui->rbVCenterText->setChecked(true);
        else if((newSettings->consignAlignment & Qt::AlignTop) == Qt::AlignTop)
            ui->rbTopText->setChecked(true);
        else
            ui->rbBottomText->setChecked(true);
        //      position
        ui->rbConsignPositionTop->setChecked(newSettings->globalConsignPositionFromPhotos == Position::top);
        ui->cbUseImageNameAsText->setChecked(newSettings->useImageNameAsText);

        // photos
        //      alignment
        if((newSettings->imagesAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
            ui->rbHCenterImage->setChecked(true);
        else if((newSettings->imagesAlignment & Qt::AlignRight) == Qt::AlignRight)
            ui->rbRightImage->setChecked(true);
        else
            ui->rbLeftImage->setChecked(true);

        if((newSettings->imagesAlignment & Qt::AlignVCenter) == Qt::AlignVCenter)
            ui->rbVCenterImage->setChecked(true);
        else if((newSettings->imagesAlignment & Qt::AlignTop) == Qt::AlignTop)
            ui->rbTopImage->setChecked(true);
        else
            ui->rbBottomImage->setChecked(true);

        updateNumberPhotos();
        updateNumberPages();
    }

    void updateSettings()
    {        
        int nbImagesVPage = ui->sbNbImagesV->value();
        int nbImagesHPage = ui->sbNbImagesH->value();
        double ratioPC = ui->dsbRatio->value();
        double ratioTitle = ui->dsbRatioTitle->value();                                

        // text
        int sizeText = ui->sbSizeTexte->value();
        QFont font = ui->fcbConsignes->currentFont();
        font.setPixelSize(13);
        font.setBold(ui->cbBold->isChecked());
        font.setItalic(ui->cbItalic->isChecked());

        QString text = ui->pteConsigne->toPlainText();
        ui->pteConsigne->setFont(font);
        ui->pteConsigne->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorText.red()) +
                                       ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +")};");
        font.setPixelSize(sizeText);

        // title
        int sizeTextTitle = ui->sbSizeTexteTitle->value();
        QFont fontTitle = ui->fcbTitle->currentFont();
        fontTitle.setPixelSize(13);
        fontTitle.setBold(ui->cbBold->isChecked());
        fontTitle.setItalic(ui->cbItalic->isChecked());
        fontTitle.setPixelSize(sizeTextTitle);

        QString textTitle = ui->pteTitle->toPlainText();
        ui->pteTitle->setFont(textTitle);
        ui->pteTitle->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorTextTitle.red()) +
                                    ", " + QString::number(m_colorTextTitle.green()) + ", " + QString::number(m_colorTextTitle.blue()) +")};");


        // image alignment
        int imageAlignment = 0 ;
        if(ui->rbHCenterImage->isChecked()) // horizontal center
            imageAlignment = imageAlignment |  Qt::AlignHCenter;
        else if(ui->rbRightImage->isChecked()) // horizontal right
            imageAlignment = imageAlignment |  Qt::AlignRight;
        else // horizontal left
            imageAlignment =  imageAlignment |  Qt::AlignLeft;

        if(ui->rbVCenterImage->isChecked()) // vertical center
            imageAlignment = imageAlignment | Qt::AlignVCenter;
        else if(ui->rbTopImage->isChecked()) // vertical top
            imageAlignment = imageAlignment | Qt::AlignTop;
        else
            imageAlignment = imageAlignment | Qt::AlignBottom;

        // text alignment
        int textAlignment = 0 ;
        if(ui->rbHCenterText->isChecked()) // horizontal center
            textAlignment = textAlignment |  Qt::AlignHCenter;
        else if(ui->rbRightText->isChecked()) // horizontal right
            textAlignment = textAlignment |  Qt::AlignRight;
        else // horizontal left
            textAlignment =  textAlignment |  Qt::AlignLeft;

        if(ui->rbVCenterText->isChecked()) // vertical center
            textAlignment = textAlignment |Qt::AlignVCenter;
        else if(ui->rbTopText->isChecked()) // vertical top
            textAlignment = textAlignment |Qt::AlignTop;
        else
            textAlignment = textAlignment |Qt::AlignBottom;

        // title alignment
        int textTitleAlignment = 0 ;
        if(ui->rbHCenterTextTitle->isChecked()) // horizontal center
            textTitleAlignment = textTitleAlignment |  Qt::AlignHCenter;
        else if(ui->rbRightTextTitle->isChecked()) // horizontal right
            textTitleAlignment = textTitleAlignment |  Qt::AlignRight;
        else // horizontal left
            textTitleAlignment =  textTitleAlignment |  Qt::AlignLeft;

        if(ui->rbVCenterTextTitle->isChecked()) // vertical center
            textTitleAlignment = textTitleAlignment |Qt::AlignVCenter;
        else if(ui->rbTopTextTitle->isChecked()) // vertical top
            textTitleAlignment = textTitleAlignment |Qt::AlignTop;
        else
            textTitleAlignment = textTitleAlignment |Qt::AlignBottom;


        m_settings = SPDFSettings(new PDFSettings());

        // general
        m_settings->pageOrientation= ui->rbPortrait->isChecked() ? PageOrientation::portrait : PageOrientation::landScape;
        m_settings->displayCutLines = ui->cbCutLines->isChecked();       
        m_settings->paperFormat.updateDefinition(ui->cbDefinition->currentText());
        m_settings->paperFormat.updateFormat(ui->cbFormat->currentText());

        // titles
        m_settings->titleAdded = ui->cbAddTitle->isChecked();
        m_settings->titleOnAllPages = ui->cbAllPagesTitle->isChecked();
        m_settings->ratioTitle = ratioTitle;
        m_settings->titlePositionFromPC = ui->rbTitlePositionTop->isChecked() ? Position::top : Position::bottom;
        m_settings->titleText = textTitle;
        m_settings->titleFont = fontTitle;
        m_settings->titleColor = m_colorTextTitle;
        m_settings->titleAlignment = textTitleAlignment;

        // PC
        m_settings->nbImagesPageH = nbImagesHPage;
        m_settings->nbImagesPageV = nbImagesVPage;
        m_settings->ratioPhotosConsign = ratioPC;
        m_settings->margins.left = ui->dsLeftMargins->value();
        m_settings->margins.right = ui->dsRightMargins->value();
        m_settings->margins.top = ui->dsTopMargins->value();
        m_settings->margins.bottom = ui->dsBottomMargins->value();
        m_settings->margins.interWidth = ui->dsInterMarginsWidth->value();
        m_settings->margins.interHeight = ui->dsInterMarginsHeight->value();

        // text
        m_settings->globalConsignText = text;
        m_settings->globalConsignFont = font;
        m_settings->globalConsignColor = m_colorText;
        m_settings->consignAlignment = textAlignment;
        m_settings->globalConsignPositionFromPhotos = ui->rbConsignPositionTop->isChecked() ? Position::top : Position::bottom;
        m_settings->useImageNameAsText = ui->cbUseImageNameAsText->isChecked();

        // photos
        m_settings->imagesAlignment= imageAlignment;

        updateNumberPhotos();
        updateNumberPages();
        emit sendSettingsSignal(m_settings, ui->lwPages->currentRow(), m_pages, 1);
    }

    /**
     * @brief forceUpdatePreview
     */
    void forceUpdatePreview()
    {
        updateNumberPhotos();
        updateNumberPages();
        emit sendSettingsSignal(m_settings, ui->lwPages->currentRow(), m_pages, 0);
    }

private slots :

    void updateNumberPhotos()
    {
        // display pages number
        int photosRemoved = 0;
        for(int ii = 0; ii < m_images->size();++ii)
        {
            if(m_images->at(ii)->isRemoved)
                photosRemoved++;
        }

        int nbPhotosTotal = m_images->size() - photosRemoved;
        m_nbPhotosPerPage = m_settings->nbImagesPageH * m_settings->nbImagesPageV;
        m_nbPages = nbPhotosTotal / m_nbPhotosPerPage;
        m_lastPagePhotosNb = nbPhotosTotal % m_nbPhotosPerPage;
        if( m_lastPagePhotosNb != 0)
            ++m_nbPages;

        if(m_lastPagePhotosNb == 0)
            m_lastPagePhotosNb = m_nbPhotosPerPage;

        ui->laNbPages->setText(QString::number(m_nbPages) + ((m_nbPages <= 1) ? " page" : " pages"));
    }

    void updateNumberPages()
    {
        int currentPageId = ui->lwPages->currentRow();

        m_pages.clear();
        ui->lwPages->clear();
        int currentPageStartPhotoId = 0;
        for(int ii = 0; ii < m_nbPages; ++ii)
        {
            Page page;
            page.nbImages = (ii < m_nbPages-1) ? m_nbPhotosPerPage : m_lastPagePhotosNb;
            page.startImageId = currentPageStartPhotoId;
            m_pages.push_back(page);

            currentPageStartPhotoId += page.nbImages;

            ui->lwPages->addItem("Page " + QString::number(ii+1) + " -> " + QString::number(page.nbImages) + ((page.nbImages > 1 ) ? " images." : " image."));
        }

        if(currentPageId != -1)
            ui->lwPages->setCurrentRow(currentPageId < m_nbPages ? currentPageId : m_nbPages-1);
        else
            ui->lwPages->setCurrentRow(0);
    }

    /**
     * @brief fillListImages
     */
    void endLoadImages(SImages images)
    {
        ui->lwPhotos->setEnabled(true);
        ui->pbSelectPhotos->setEnabled(true);
        ui->lwPages->setEnabled(true);
        m_images = images;

        updateNumberPhotos();
        updateNumberPages();
    }

    /**
     * @brief Define the photo directory
     */
    void setPhotosDirectory()
    {
        QString previousDirectory = m_photosDirectory;
        m_photosDirectory = QFileDialog::getExistingDirectory(this, "Sélection du répertoire d'images", QDir::homePath());

        // no directory selected
        if(m_photosDirectory.size() == 0 )
        {
            if(previousDirectory.size() != 0)
                m_photosDirectory = previousDirectory;

            return;
        }

        // retrieve dir
        QDir dir(m_photosDirectory);
        dir.setSorting(QDir::NoSort);  // will sort manually with std::sort
        dir.setFilter(QDir::Files);
        dir.setNameFilters(QStringList() << "*.png" << "*.jpg");

        // retrieve photos name list
        QStringList fileList = dir.entryList();

        // sort list with std
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(
            fileList.begin(),
            fileList.end(),
            [&collator](const QString &file1, const QString &file2)
            {
                return collator.compare(file1, file2) < 0;
            });


        // no photo in the directory
        if(fileList.size() == 0)
        {
            if(previousDirectory.size() != 0)
                m_photosDirectory = previousDirectory;

            QMessageBox::warning(this, tr("Avertissement"), tr("Aucune image (jpg,png) n'a pu être trouvée dans ce répertoire, veuillez en selectionner un autre.\n"),QMessageBox::Ok);
            return;
        }
        ui->laPhotosDir->setText(m_photosDirectory);


        if(fileList.size() > 0)
            ui->lwPhotos->setCurrentRow(0);

        ui->lwPages->clear();
        ui->lwPages->setEnabled(false);

        ui->lwPhotos->clear();
        ui->lwPhotos->setEnabled(false);

        ui->pbSelectPhotos->setEnabled(false);
        emit sendImagesDirSignal(m_photosDirectory, fileList);
    }


    /**
     * @brief setColorText
     */
    void setColorText()
    {
        QColorDialog colorDialog(this);
        colorDialog.setCurrentColor(m_colorText);
        colorDialog.exec();
        m_colorText = colorDialog.selectedColor();

        ui->pbChooseColor->setStyleSheet("background-color: rgb("+ QString::number(m_colorText.red()) +
                                        ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +");");

        updateSettings();
    }

    /**
     * @brief setColorTextTitle
     */
    void setColorTextTitle()
    {
        QColorDialog colorDialog(this);
        colorDialog.setCurrentColor(m_colorText);
        colorDialog.exec();
        m_colorTextTitle = colorDialog.selectedColor();

        ui->pbChooseColorTitle->setStyleSheet("background-color: rgb("+ QString::number(m_colorTextTitle.red()) +
                                        ", " + QString::number(m_colorTextTitle.green()) + ", " + QString::number(m_colorTextTitle.blue()) +");");

        updateSettings();
    }



signals :

    /**
     * @brief sendImagesDirSignal
     * @param photosDir
     * @param photosNames
     */
    void sendImagesDirSignal(QString photosDir, QStringList photosNames);

    /**
     * @brief sendSettingsSignal
     * @param settings
     * @param currentPageId
     * @param pages
     * @param index
     */
    void sendSettingsSignal(SPDFSettings settings, int currentPageId, QVector<Page> pages, int index);

    /**
     * @brief currentPageChangedSignal
     * @param settings
     * @param currentPageId
     * @param pages
     */
    void currentPageChangedSignal(SPDFSettings settings, int currentPageId, QVector<Page> pages);

    /**
     * @brief killSignal
     */
    void killSignal();

private :

    int m_nbPages = 0;
    int m_lastPagePhotosNb = 0;
    int m_nbPhotosPerPage = 0;
    QColor m_colorText = Qt::black;
    QColor m_colorTextTitle = Qt::black;
    QString m_photosDirectory;/**< the photos directory */
    QVector<Page> m_pages;

    SImages m_images = SImages(new QList<SImage>);
    SPDFSettings m_settings = SPDFSettings(new PDFSettings());

    Ui::LeftMenuUI *ui = nullptr; /**< ui left menu */
    QThread  m_workerThread;            /**< worker thread */
    LeftMenuWorker *m_worker= nullptr;   /**< worker of the menu*/
};
}

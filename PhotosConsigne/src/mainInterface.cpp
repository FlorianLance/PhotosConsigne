/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file mainInterface.cpp
 * \brief defines MainInterface
 * \author Florian Lance
 * \date 01/11/15
 */


#include "mainInterface.h"


// qt
#include <QFileDialog>
#include <QStandardItemModel>
#include <QColorDialog>


// std
#include <iostream>


// debug
#include <qdebug.h>



MainInterface::MainInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    // set icon/title
    this->setWindowTitle(QString("PhotosConsigne"));
    this->setWindowIcon(QIcon("../PhotosConsigne/images/icon.png"));

//    QString l_text("<p>An interface for language learning with neuron computing using GPU acceleration.<br /><br />");
//     l_text += "Developped in the Robotic Cognition Laboratory of the <a href=\"http://www.sbri.fr/\"> SBRI</a> under the direction of  <b>Peter Ford Dominey. </b>";
//     l_text += "<br /><br /> Author : <b>Lance Florian </b> <a href=\"https://github.com/FlorianLance\"> Github profile</a>";
//     l_text += "<br /><br /> Language model by <b>Xavier Hinaut</b> and <b>Colas Droin</b>. <br /></p>";
//     l_text += "<a href=\"https://github.com/FlorianLance/neuron-computing-cuda\">Repository</a>";

    QString legend("<p><font color=white>LEGENDE :</font> &nbsp;&nbsp;&nbsp;<font color=red>MARGES EXTERIEURES</font> &nbsp;&nbsp;&nbsp;<font color=green>MARGES INTERIEURES</font> &nbsp;&nbsp;&nbsp;<font color=cyan>ESPACE CONSIGNE</font> &nbsp;&nbsp;&nbsp;<font color=yellow>ESPACE PHOTO</font></p>");
    ui->laLegend->setText(legend);


    // stylesheet
//    ui->gbParametres->setStyleSheet("* { font-weight: bold }");
    setStyleSheet("QGroupBox { padding: 10 0px 0 0px; color: blue; border: 1px solid gray; border-radius: 5px; margin-top: 1ex; /* leave space at the top for the title */}");

//    ui->hlDisplayElements->

    // set styles
    ui->tabPreview->setStyleSheet("background-color: rgb(122,122,122);");

    ui->wPreviewLegend->setStyleSheet("background-color: rgb(0,0,0);");
    ui->wDisplayElements->setStyleSheet("background-color: rgb(255,255,255);");

    // color
    m_colorText = Qt::GlobalColor::black;
    ui->pbChoosColor->setStyleSheet("background-color: rgb(0,0,0);");


    // init interface widgets
    //  image labels
    m_imageLabel = new ImageLabel(this);
    ui->vlImage->insertWidget(1, m_imageLabel);

    m_previewLabel = new ImageLabel(this);
    ui->hlPreviewImage->insertWidget(0, m_previewLabel);



    // init worker
    m_interfaceWorker = new InterfaceWorker(m_previewLabel);

    // connections
    //  push button
    QObject::connect(ui->pbSelectPhotos,SIGNAL(clicked()), this, SLOT(setPhotosDirectory()));
    QObject::connect(ui->pbGeneration,SIGNAL(clicked()), this, SLOT(generatePDF()));
    QObject::connect(ui->pbPreview,SIGNAL(clicked()), this, SLOT(generatePreview()));
    QObject::connect(ui->pbChoosColor,SIGNAL(clicked()), this, SLOT(setColorText()));
    QObject::connect(ui->pbRotationLeft, SIGNAL(clicked()), this, SLOT(setImageLeftRotation()));
    QObject::connect(ui->pbRotationRight, SIGNAL(clicked()), this, SLOT(setImageRightRotation()));
    QObject::connect(ui->pbRemovePhoto, SIGNAL(clicked()), this, SLOT(removeCurrentPhotoFromList()));

    //  list widget
    QObject::connect(ui->lwPhotos,SIGNAL(currentRowChanged(int)), this, SLOT(updatePhotoIndex(int)));
    QObject::connect(ui->lwPhotos, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePhotoIndex(QModelIndex)));

    // radio button
    QObject::connect(ui->rbLeftImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbRightImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbHCenterImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbVCenterImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbTopImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbBottomImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbLeftText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbRightText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbHCenterText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbVCenterText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbBottomText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbTopText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbLandScape, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->rbPortrait, SIGNAL(clicked(bool)), this, SLOT(updateUIParameters(bool)));
    // spinbox
    QObject::connect(ui->sbNbImagesV, SIGNAL(valueChanged(int)), this, SLOT(updateUIParameters(int)));
    QObject::connect(ui->sbNbImagesH, SIGNAL(valueChanged(int)), this, SLOT(updateUIParameters(int)));
    QObject::connect(ui->sbSizeTexte, SIGNAL(valueChanged(int)), this, SLOT(updateUIParameters(int)));
    QObject::connect(ui->dsbRatio, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsBottomMargins, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsLeftMargins, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsRightMargins, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsTopMargins, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsBetween, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    // font
    QObject::connect(ui->fcbConsignes, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateUIParameters(QFont)));
    // QPlainText
    QObject::connect(ui->pteConsigne, SIGNAL(textChanged()), this, SLOT(updateUIParameters()));
    // check box
    QObject::connect(ui->cbBold, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbItalic, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbCutLines, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneExternMargins, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneInternMargins, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneConsignes, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZonePhotos, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    //  interface -> worker
    QObject::connect(this, SIGNAL(sendImagesDir(QString, QStringList)), m_interfaceWorker, SLOT(loadImages(QString, QStringList)));
    QObject::connect(this, SIGNAL(generatePDFSignal(QString)), m_interfaceWorker, SLOT(generatePDF(QString)));
    QObject::connect(this, SIGNAL(generatePreview(int)), m_interfaceWorker, SLOT(generatePreview(int)));

    QObject::connect(this, SIGNAL(sentParameters(UIParameters)),m_interfaceWorker, SLOT(updateParameters(UIParameters)));
//    QObject::connect(this, SIGNAL(sentParameters(QVector<bool>,int, int,double,QFont,QString, QColor, int, int, bool, double, double, double, double, double, bool, bool, bool, bool, bool)),m_interfaceWorker,
//                     SLOT(updateParameters(QVector<bool>,int,int, double,QFont,QString, QColor,int, int, bool, double, double, double, double, double, bool, bool, bool, bool, bool )));
    QObject::connect(this, SIGNAL(updateRotation(int,bool)), m_interfaceWorker, SLOT(updateRotationImage(int,bool)));
    QObject::connect(this, SIGNAL(askForPhoto(int)), m_interfaceWorker, SLOT(sendPhoto(int)));
    // worker -> interface
    QObject::connect(m_interfaceWorker, SIGNAL(unlockSignal()), this, SLOT(unlockUI()));
    QObject::connect(m_interfaceWorker, SIGNAL(setProgressBarState(int)), this, SLOT(setProgressBatState(int)));
    QObject::connect(m_interfaceWorker, SIGNAL(displayPhoto(QImage)), this, SLOT(updatePhotoDisplay(QImage)));


    // init thread
    m_interfaceWorker->moveToThread(&m_workerThread);
    m_workerThread.start();

    // init parameters with default ui values
    updateUIParameters();
}

MainInterface::~MainInterface()
{
    delete ui;

    m_workerThread.quit();
    m_workerThread.wait();
    delete m_interfaceWorker;
}

void MainInterface::unlockUI()
{
    ui->pbGeneration->setEnabled(true);
    ui->pbPreview->setEnabled(true);
    ui->pbSelectPhotos->setEnabled(true);
    ui->pbRotationLeft->setEnabled(true);
    ui->pbRotationRight->setEnabled(true);
    ui->pbRemovePhoto->setEnabled(true);
}

void MainInterface::setPhotosDirectory()
{
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->pbSelectPhotos->setEnabled(false);    
    ui->pbRotationLeft->setEnabled(false);
    ui->pbRotationRight->setEnabled(false);
    ui->pbRemovePhoto->setEnabled(false);

    m_photosDirectory = QFileDialog::getExistingDirectory(this, "Sélection du répertoire de photos");

    // no directory selected
    if(m_photosDirectory.size() == 0)
    {
        ui->pbSelectPhotos->setEnabled(true);
        return;
    }

    ui->laPhotosDir->setText(m_photosDirectory);

    QDir dir(m_photosDirectory);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.png" << "*.jpg");

    // no photo in the directory
    QStringList fileList = dir.entryList();
    if(fileList.size() == 0)
    {
        ui->pbSelectPhotos->setEnabled(true);
        return;
    }

    ui->lwPhotos->clear();
    m_photoRemovedList.clear();
    for(int ii = 0; ii < fileList.size(); ++ii)
    {
        ui->lwPhotos->addItem(fileList[ii]);
        m_photoRemovedList.push_back(false);
    }    

    if(fileList.size() > 0)
    {
        ui->lwPhotos->setCurrentRow(0);
    }

    emit sendImagesDir(m_photosDirectory, fileList);

    updateUIParameters();
}



void MainInterface::updatePhotoDisplay(QImage image)
{
    m_imageLabel->setPixmap(QPixmap::fromImage(image));
    m_imageLabel->update();
}

void MainInterface::updatePhotoIndex(QModelIndex index)
{
    updatePhotoIndex(index.row());
}


void MainInterface::updatePhotoIndex(int index)
{
    ui->tabDisplay->setCurrentIndex(0);

    if(index >= 0  && index < ui->lwPhotos->count())
    {
        emit askForPhoto(index);
    }

    if(!m_photoRemovedList[ui->lwPhotos->currentRow()])
    {
        ui->pbRemovePhoto->setText("Retirer photo");
    }
    else
    {
        ui->pbRemovePhoto->setText("Ajouter photo");
    }
}

void MainInterface::removeCurrentPhotoFromList()
{
    QBrush brush = ui->lwPhotos->currentItem()->foreground();

    if(!m_photoRemovedList[ui->lwPhotos->currentRow()])
    {
        brush.setColor(Qt::red);
        m_photoRemovedList[ui->lwPhotos->currentRow()] = true;
        ui->pbRemovePhoto->setText("Ajouter photo");
    }
    else
    {
        brush.setColor(Qt::black);
        m_photoRemovedList[ui->lwPhotos->currentRow()] = false;
        ui->pbRemovePhoto->setText("Retirer photo");
    }

    ui->lwPhotos->currentItem()->setForeground(brush);
    updateUIParameters();
}


void MainInterface::generatePDF()
{
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->pbSelectPhotos->setEnabled(false);

    QString pdfFileName = QFileDialog::getSaveFileName(this, "Nom du fichier pdf", QString(), "*.pdf");

    if(pdfFileName.size() == 0)
        return;

    if(pdfFileName.right(4) != ".pdf")
    {
        pdfFileName += ".pdf";
    }

    emit generatePDFSignal(pdfFileName);
}


void MainInterface::generatePreview()
{    
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->tabDisplay->setCurrentIndex(1);

    emit generatePreview(ui->lwPhotos->currentIndex().row());
}



void MainInterface::updateUIParameters(QFont notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters();
}

void MainInterface::updateUIParameters(bool notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters();
}


void MainInterface::updateUIParameters(int notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters();
}

void MainInterface::updateUIParameters(double notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters();
}

void MainInterface::updateUIParameters()
{
    int nbImagesVPage = ui->sbNbImagesV->value();
    int nbImagesHPage = ui->sbNbImagesH->value();
    int sizeText = ui->sbSizeTexte->value();
    double ratio = ui->dsbRatio->value();

    QFont font = ui->fcbConsignes->currentFont();
    font.setPixelSize(13);
    font.setBold(ui->cbBold->isChecked());
    font.setItalic(ui->cbItalic->isChecked());

    QString text = ui->pteConsigne->toPlainText();
    ui->pteConsigne->setFont(font);    
    ui->pteConsigne->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorText.red()) +
                                   ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +")};");

    font.setPixelSize(sizeText);

    // image alignment
    int imageAlignment = 0 ;
    if(ui->rbVCenterImage->isChecked()) // vertical center
    {
        imageAlignment = Qt::AlignVCenter;
        if(ui->rbHCenterImage->isChecked()) // horizontal center
        {
            imageAlignment = imageAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightImage->isChecked()) // horizontal right
        {
            imageAlignment = imageAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            imageAlignment =  imageAlignment |  Qt::AlignLeft;
        }
    }
    else if(ui->rbTopImage->isChecked()) // vertical top
    {
        imageAlignment = Qt::AlignTop;
        if(ui->rbHCenterImage->isChecked()) // horizontal center
        {
            imageAlignment = imageAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightImage->isChecked()) // horizontal right
        {
            imageAlignment = imageAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            imageAlignment = imageAlignment |  Qt::AlignLeft;
        }
    }
    else
    {
        imageAlignment = Qt::AlignBottom;
        if(ui->rbHCenterImage->isChecked()) // horizontal center
        {
            imageAlignment = imageAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightImage->isChecked()) // horizontal right
        {
            imageAlignment = imageAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            imageAlignment = imageAlignment |  Qt::AlignLeft;
        }
    }

    // text alignment
    int textAlignment = 0 ;
    if(ui->rbVCenterText->isChecked()) // vertical center
    {
        textAlignment = Qt::AlignVCenter;
        if(ui->rbHCenterText->isChecked()) // horizontal center
        {
            textAlignment = textAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightText->isChecked()) // horizontal right
        {
            textAlignment = textAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            textAlignment =  textAlignment |  Qt::AlignLeft;
        }
    }
    else if(ui->rbTopText->isChecked()) // vertical top
    {
        textAlignment = Qt::AlignTop;
        if(ui->rbHCenterText->isChecked()) // horizontal center
        {
            textAlignment = textAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightText->isChecked()) // horizontal right
        {
            textAlignment = textAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            textAlignment = textAlignment |  Qt::AlignLeft;
        }
    }
    else
    {
        textAlignment = Qt::AlignBottom;
        if(ui->rbHCenterText->isChecked()) // horizontal center
        {
            textAlignment = textAlignment |  Qt::AlignHCenter;
        }
        else if(ui->rbRightText->isChecked()) // horizontal right
        {
            textAlignment = textAlignment |  Qt::AlignRight;
        }
        else // horizontal left
        {
            textAlignment = textAlignment |  Qt::AlignLeft;
        }
    }

    UIParameters params;
    params.removePhotoList = m_photoRemovedList;
    params.nbImagesPageH = nbImagesHPage;
    params.nbImagesPageV = nbImagesVPage;
    params.ratio = ratio;
    params.font = font;
    params.consignText = text;
    params.consignColor = m_colorText;
    params.imageAlignment = imageAlignment;
    params.consignAlignment = textAlignment;
    params.orientation = ui->rbPortrait->isChecked();
    params.leftMargin = ui->dsLeftMargins->value();
    params.rightMargin = ui->dsRightMargins->value();
    params.topMargin = ui->dsTopMargins->value();
    params.bottomMargin = ui->dsBottomMargins->value();
    params.betweenMargin = ui->dsBetween->value();
    params.cutLines = ui->cbCutLines->isChecked();
    params.zExternMargins = ui->cbZoneExternMargins->isChecked();
    params.zInterMargins = ui->cbZoneInternMargins->isChecked();
    params.zPhotos = ui->cbZonePhotos->isChecked();
    params.orientation = ui->rbPortrait->isChecked();
    params.zConsigns = ui->cbZoneConsignes->isChecked();

    emit sentParameters(params);

//    emit sentParameters(m_photoRemovedList, nbImagesVPage,nbImagesHPage, ratio, font, text, m_colorText, imageAlignment, textAlignment, ui->rbPortrait->isChecked(),
//                        ui->dsLeftMargins->value(),ui->dsRightMargins->value(),ui->dsTopMargins->value(),ui->dsBottomMargins->value(),ui->dsBetween->value(),
//                        ui->cbCutLines->isChecked() ,ui->cbZoneExternMargins->isChecked(), ui->cbZoneInternMargins->isChecked(), ui->cbZonePhotos->isChecked(), ui->cbZoneConsignes->isChecked());

    if(ui->lwPhotos->count() > 0)
    {
        generatePreview();
    }
}

void MainInterface::setColorText()
{
    QColorDialog colorDialog(this);
    colorDialog.setCurrentColor(m_colorText);
    colorDialog.exec();
    m_colorText = colorDialog.selectedColor();
    ui->pbChoosColor->setStyleSheet("background-color: rgb("+ QString::number(m_colorText.red()) +
                                    ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +");");

    updateUIParameters();
}

void MainInterface::setProgressBatState(int state)
{
    ui->pbLoading->setValue(state);
}

void MainInterface::setImageLeftRotation()
{
    ui->tabDisplay->setCurrentIndex(0);
    emit updateRotation(ui->lwPhotos->currentIndex().row(), false);
}

void MainInterface::setImageRightRotation()
{
    ui->tabDisplay->setCurrentIndex(0);
    emit updateRotation(ui->lwPhotos->currentIndex().row(), true);
}




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
 * \file MainInterface.cpp
 * \brief defines MainInterface
 * \author Florian Lance
 * \date 01/11/15
 */


#include "MainInterface.h"


// qt
#include <QFileDialog>
#include <QStandardItemModel>
#include <QColorDialog>
#include <QDesktopServices>
#include <QMessageBox>

// std
#include <iostream>


// debug
#include <qdebug.h>



MainInterface::MainInterface(QApplication *parent) : ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    // set icon/title
    this->setWindowTitle(QString("PhotosConsigne"));
    this->setWindowIcon(QIcon(":/images/icon"));

    // load images
    // left image
    ui->pbLeftImage->setIcon(QIcon(":/images/leftArrow"));
    ui->pbLeftImage->setIconSize(QSize(100,45));
    ui->pbLeftImage->setToolTip(tr("Image précédente"));
    // right image
    ui->pbRightImage->setIcon(QIcon(":/images/rightArrow"));
    ui->pbRightImage->setIconSize(QSize(100,45));
    ui->pbRightImage->setToolTip(tr("Image suivante."));
    // left rotation
    ui->pbRotationLeft->setIcon(QIcon(":/images/rotateLeft"));
    ui->pbRotationLeft->setIconSize(QSize(50,45));
    ui->pbRotationLeft->setToolTip(tr("Rotation de l'image à gauche de 90°"));
    // right rotation
    ui->pbRotationRight->setIcon(QIcon(":/images/rotateRight"));
    ui->pbRotationRight->setIconSize(QSize(50,45));
    ui->pbRotationRight->setToolTip(tr("Rotation de l'image à droite de 90°"));
    // preview
    ui->pbPreview->setIcon(QIcon(":/images/preview"));
    ui->pbPreview->setIconSize(QSize(100,50));
    ui->pbPreview->setToolTip(tr("Affiche la prévisualisation du PDF"));
    // add consign
    ui->pbAddTextPhoto->setIcon(QIcon(":/images/addConsign"));
    ui->pbAddTextPhoto->setIconSize(QSize(100,50));
    ui->pbAddTextPhoto->setToolTip(tr("Ajouter un texte unique pour cette photo"));

    // remove photo
    m_addPhotoIcon = QIcon(":/images/add");
    m_removePhotoIcon = QIcon(":/images/remove");
    ui->pbRemovePhoto->setIcon(m_removePhotoIcon);
    ui->pbRemovePhoto->setIconSize(QSize(50,50));
    ui->pbRemovePhoto->setToolTip(tr("Retirer l'image de la liste"));

    // open photos dir
    ui->pbSelectPhotos->setIcon(QIcon(":/images/dirImages"));
    ui->pbSelectPhotos->setIconSize(QSize(25,25));
    ui->pbSelectPhotos->setToolTip(tr("Définir le dossier contenant les photos à charger dans le PDF"));

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

    // set styles
    setStyleSheet("QGroupBox { padding: 10 0px 0 0px; color: blue; border: 1px solid gray; border-radius: 5px; margin-top: 1ex; /* leave space at the top for the title */}");        

    // individual consign
    ui->laPhotoIndividualConsign->hide();
    ui->tePhotoIndividualConsign->hide();

    // color
    m_colorText = Qt::GlobalColor::black;

    // init interface widgets
    //  image label
    m_imageLabel = new ImageLabel(this);
    ui->vlImage->insertWidget(0, m_imageLabel);

    //  preview label
    m_previewLabel = new ImageLabel(this);
    ui->hlPreviewImage->insertWidget(0, m_previewLabel, Qt::AlignHCenter);

    // init worker
    m_interfaceWorker = new InterfaceWorker(m_previewLabel);

    // connections
    //  menu
    QObject::connect(ui->actionExit, SIGNAL(triggered()), parent, SLOT(quit()));
    QObject::connect(ui->actionOnlineDocumentation, SIGNAL(triggered()), this, SLOT(openOnlineDocumentation()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
    QObject::connect(ui->actionDonate, SIGNAL(triggered()), this, SLOT(openDonatePage()));
    QObject::connect(ui->actionSaveProfileTo, SIGNAL(triggered()), this, SLOT(saveProfileTo()));
    QObject::connect(ui->actionSaveProfile, SIGNAL(triggered()), m_interfaceWorker, SLOT(saveProfile()));
    QObject::connect(ui->actionLoadProfile, SIGNAL(triggered()), this, SLOT(loadProfile()));
    QObject::connect(ui->actionSaveDefault, SIGNAL(triggered()), m_interfaceWorker, SLOT(saveDefaultProfile()));
    //  push button
    QObject::connect(ui->pbSelectPhotos,SIGNAL(clicked()), this, SLOT(setPhotosDirectory()));
    QObject::connect(ui->pbGeneration,SIGNAL(clicked()), this, SLOT(generatePDF()));
    QObject::connect(ui->pbPreview,SIGNAL(clicked()), this, SLOT(generatePreview()));
    QObject::connect(ui->pbChooseColor,SIGNAL(clicked()), this, SLOT(setColorText()));
    QObject::connect(ui->pbRotationLeft, SIGNAL(clicked()), this, SLOT(setImageLeftRotation()));
    QObject::connect(ui->pbRotationRight, SIGNAL(clicked()), this, SLOT(setImageRightRotation()));
    QObject::connect(ui->pbRemovePhoto, SIGNAL(clicked()), this, SLOT(removeCurrentPhotoFromList()));
    QObject::connect(ui->pbLeftImage, SIGNAL(clicked()), this, SLOT(leftPhoto()));
    QObject::connect(ui->pbRightImage, SIGNAL(clicked()), this, SLOT(rightPhoto()));
    QObject::connect(ui->pbOpenPDF, SIGNAL(clicked()), this, SLOT(openPDF()));
    QObject::connect(ui->pbAddTextPhoto, SIGNAL(clicked()), this, SLOT(switchTextPhoto()));
    //  list widget
    QObject::connect(ui->lwPhotos,SIGNAL(currentRowChanged(int)), this, SLOT(updatePhotoIndex(int)));
    QObject::connect(ui->lwPhotos, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePhotoIndex(QModelIndex)));
    //  radio button
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
    QObject::connect(ui->dsInterMarginsWidth, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    QObject::connect(ui->dsInterMarginsHeight, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameters(double)));
    // font
    QObject::connect(ui->fcbConsignes, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateUIParameters(QFont)));
    // QPlainText
    QObject::connect(ui->pteConsigne, SIGNAL(textChanged()), this, SLOT(updateUIParameters()));
    // text edit
    QObject::connect(ui->tePhotoIndividualConsign, SIGNAL(textChanged()), this, SLOT(updateIndividualText()));
    // check box
    QObject::connect(ui->cbBold, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbItalic, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbCutLines, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneExternMargins, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneInternMargins, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZoneConsignes, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    QObject::connect(ui->cbZonePhotos, SIGNAL(toggled(bool)), this, SLOT(updateUIParameters(bool)));
    //  interface -> worker
    QObject::connect(this, SIGNAL(sendImagesDirSignal(QString, QStringList)), m_interfaceWorker, SLOT(loadImages(QString, QStringList)));
    QObject::connect(this, SIGNAL(generatePDFSignal(QString)), m_interfaceWorker, SLOT(generatePDF(QString)));
    QObject::connect(this, SIGNAL(generatePreviewSignal(int)), m_interfaceWorker, SLOT(generatePreview(int)));
    QObject::connect(this, SIGNAL(sentParametersSignal(UIParameters)),m_interfaceWorker, SLOT(updateParameters(UIParameters)));
    QObject::connect(this, SIGNAL(updateRotationSignal(int,bool)), m_interfaceWorker, SLOT(updateRotationImage(int,bool)));
    QObject::connect(this, SIGNAL(askForPhotoSignal(int)), m_interfaceWorker, SLOT(sendPhoto(int)));
    QObject::connect(this, SIGNAL(saveProfileToSignal(QString)), m_interfaceWorker, SLOT(saveProfileTo(QString)));
    QObject::connect(this, SIGNAL(loadProfileSignal(QString)), m_interfaceWorker, SLOT(loadProfile(QString)));
    QObject::connect(this, SIGNAL(loadDefautProfileSignal(QString)), m_interfaceWorker, SLOT(loadDefaultProfile(QString)));
    // worker -> interface
    QObject::connect(m_interfaceWorker, SIGNAL(unlockSignal()), this, SLOT(unlockUI()));
    QObject::connect(m_interfaceWorker, SIGNAL(setProgressBarStateSignal(int)), this, SLOT(setProgressBatState(int)));
    QObject::connect(m_interfaceWorker, SIGNAL(displayPhotoSignal(QImage)), this, SLOT(updatePhotoDisplay(QImage)));
    QObject::connect(m_interfaceWorker, SIGNAL(pdfGeneratedSignal()), this, SLOT(unlockOpenPDF()));
    QObject::connect(m_interfaceWorker, SIGNAL(endSaveProfileSignal(QString)), this, SLOT(unlockSaveProfile(QString)));
    QObject::connect(m_interfaceWorker, SIGNAL(endLoadProfileSignal(QString, UIParameters)), this, SLOT(profileLoaded(QString, UIParameters)));


    // init thread
    m_interfaceWorker->moveToThread(&m_workerThread);
    m_workerThread.start();

    // init parameters with default ui values
    updateUIParameters(false, false, true);

    QImage nullImage(QSize(500,500), QImage::Format_ARGB32);
    nullImage.fill(Qt::GlobalColor::white);
    m_interfaceWorker->sendPhoto(QImage(":/images/null"));


    QString defaultProfilPath = QDir::currentPath();
    if(QDir(defaultProfilPath + "/resources").exists())
    {
        defaultProfilPath += "/resources/profils/default.profil";
    }
    else
    {
        defaultProfilPath += "/../PhotosConsigne/resources/profils/default.profil";
    }

    // load default profile
    if(QFile(defaultProfilPath).exists())
    {
        emit loadDefautProfileSignal(defaultProfilPath);
    }
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
    ui->pbLeftImage->setEnabled(true);
    ui->pbRightImage->setEnabled(true);
    ui->pbAddTextPhoto->setEnabled(true);
}

void MainInterface::setPhotosDirectory()
{
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->pbSelectPhotos->setEnabled(false);    
    ui->pbRotationLeft->setEnabled(false);
    ui->pbRotationRight->setEnabled(false);
    ui->pbRemovePhoto->setEnabled(false);
    ui->pbLeftImage->setEnabled(false);
    ui->pbRightImage->setEnabled(false);
    ui->pbAddTextPhoto->setEnabled(false);

    QString previousDirectory = m_photosDirectory;
    m_photosDirectory = QFileDialog::getExistingDirectory(this, "Sélection du répertoire de photos", QDir::homePath());

    // no directory selected
    if(m_photosDirectory.size() == 0 )
    {
        ui->pbSelectPhotos->setEnabled(true);

        if(previousDirectory.size() != 0)
        {
            ui->pbPreview->setEnabled(true);
            ui->pbRotationLeft->setEnabled(true);
            ui->pbRotationRight->setEnabled(true);
            ui->pbRemovePhoto->setEnabled(true);
            ui->pbGeneration->setEnabled(true);
            ui->pbAddTextPhoto->setEnabled(true);
            ui->pbLeftImage->setEnabled(true);
            ui->pbRightImage->setEnabled(true);
            m_photosDirectory = previousDirectory;
        }

        return;
    }   

    QDir dir(m_photosDirectory);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.png" << "*.jpg");

    // no photo in the directory
    QStringList fileList = dir.entryList();
    if(fileList.size() == 0)
    {
        ui->pbSelectPhotos->setEnabled(true);

        if(previousDirectory.size() != 0)
        {
            ui->pbPreview->setEnabled(true);
            ui->pbRotationLeft->setEnabled(true);
            ui->pbRotationRight->setEnabled(true);
            ui->pbRemovePhoto->setEnabled(true);
            ui->pbGeneration->setEnabled(true);
            ui->pbAddTextPhoto->setEnabled(true);
            ui->pbLeftImage->setEnabled(true);
            ui->pbRightImage->setEnabled(true);
            m_photosDirectory = previousDirectory;
        }

        QMessageBox::warning(this, tr("Avertissement"), tr("Aucune photo n'a pu être trouvée dans ce répertoire, veuillez en selectionner un autre.\n"),QMessageBox::Ok);
        return;
    }

    ui->laLoading->setText("Chargement des photos...");

    ui->laPhotosDir->setText(m_photosDirectory);

    ui->lwPhotos->clear();
    m_photoRemovedList.clear();
    m_addTextPhotoEnabled.clear();
    m_photosText.clear();
    for(int ii = 0; ii < fileList.size(); ++ii)
    {
        ui->lwPhotos->addItem(fileList[ii]);
        m_photoRemovedList.push_back(false);
        m_addTextPhotoEnabled.push_back(false);
        m_photosText.push_back("");
    }    

    if(fileList.size() > 0)
    {
        ui->lwPhotos->setCurrentRow(0);
    }

    emit sendImagesDirSignal(m_photosDirectory, fileList);

    updateUIParameters(false, false, false);
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

void MainInterface::leftPhoto()
{
    if(ui->lwPhotos->currentRow() == 0)
    {
        ui->lwPhotos->setCurrentRow(m_photoRemovedList.size()-1);
    }
    else
    {
        ui->lwPhotos->setCurrentRow(ui->lwPhotos->currentRow()-1);
    }
    updatePhotoIndex(ui->lwPhotos->currentRow());
}

void MainInterface::rightPhoto()
{
    if(ui->lwPhotos->currentRow() == m_photoRemovedList.size()-1)
    {
        ui->lwPhotos->setCurrentRow(0);
    }
    else
    {
        ui->lwPhotos->setCurrentRow(ui->lwPhotos->currentRow() + 1);
    }
    updatePhotoIndex(ui->lwPhotos->currentRow());
}


void MainInterface::updatePhotoIndex(int index)
{
    ui->tabDisplay->setCurrentIndex(0);

    if(index >= 0  && index < ui->lwPhotos->count())
    {
        emit askForPhotoSignal(index);
    }

    int idPhoto = ui->lwPhotos->currentRow();
    if(m_photoRemovedList[idPhoto])
    {
        ui->pbRemovePhoto->setIcon(m_addPhotoIcon);
        ui->pbRemovePhoto->setIconSize(QSize(50,50));
        ui->pbRemovePhoto->setToolTip(tr("Rajouter l'image dans la liste"));
    }
    else
    {
        ui->pbRemovePhoto->setIcon(m_removePhotoIcon);
        ui->pbRemovePhoto->setIconSize(QSize(50,50));
        ui->pbRemovePhoto->setToolTip(tr("Retirer l'image de la liste"));
    }

    updateAddTextPhotoUI(idPhoto);
}

void MainInterface::removeCurrentPhotoFromList()
{
    QBrush brush = ui->lwPhotos->currentItem()->foreground();

    int id = ui->lwPhotos->currentRow();
    if(!m_photoRemovedList[ui->lwPhotos->currentRow()])
    {
        brush.setColor(Qt::red);
        m_photoRemovedList[id] = true;
        ui->pbRemovePhoto->setIcon(m_addPhotoIcon);
        ui->pbRemovePhoto->setIconSize(QSize(50,50));
        ui->pbRemovePhoto->setToolTip(tr("Rajouter l'image dans la liste"));
    }
    else
    {
        if(m_addTextPhotoEnabled[id])
            brush.setColor(Qt::blue);
        else
            brush.setColor(Qt::black);

        m_photoRemovedList[id] = false;
        ui->pbRemovePhoto->setIcon(m_removePhotoIcon);
        ui->pbRemovePhoto->setIconSize(QSize(50,50));
        ui->pbRemovePhoto->setToolTip(tr("Retirer l'image de la liste"));
    }

    ui->lwPhotos->currentItem()->setForeground(brush);
    updateUIParameters(false, false, false);
}

void MainInterface::unlockOpenPDF()
{
    ui->pbOpenPDF->setEnabled(true);
}

void MainInterface::openPDF()
{
    bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(m_pdfFileName));
    if(!success)
    {
        QMessageBox::warning(this, tr("Avertissement"), tr("Le PDF n'a pu être lancé.\nVeuillez vous assurez que vous disposez d'un logiciel de lecture de PDF (ex : SumatraPDF, AdobeReader...) .\n"),QMessageBox::Ok);
    }
}

void MainInterface::openAboutWindow()
{
    QString text("<p>PhotosConsigne est un logiciel léger et rapide servant à générer des PDF contenant des images associées à un texte commun.<br />");
    text += "Tous les paramètres sont facilement modifiables (quantités d'images par page, orientation, alignements, police du texte...). <br /><br /></b>";
    text += "Ce logiciel est principalement destiné aux professeurs des écoles, mais si d'autres lui trouve une autre utilité j'en serai ravi.<br /><br /></b>";
    text += "N'hésitez à pas me faire partager vos retours et vos idées d'amélioration, selon mon temps il est possible que j'implémente les plus justifitées.<br /> Contact : <a href=\"mailto:lance.florian@gmail.com?subject=PhotosConsigne&body=...\">email</a> ";
    text += "<br /><br /> Auteur : <b>Lance Florian </b> <a href=\"https://github.com/FlorianLance\"> Github du logiciel </a>";
    QMessageBox::about(this, "A propos du logiciel", text);
}

void MainInterface::openDonatePage()
{
    bool success = QDesktopServices::openUrl(QUrl("https://pledgie.com/campaigns/31286#donors", QUrl::TolerantMode));
    if(!success)
    {
        QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet de don n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
    }
}

void MainInterface::saveProfileTo()
{
    QString defaultPath = QDir::currentPath();
    if(QDir(defaultPath + "/resources").exists())
    {
        defaultPath += "/resources/profils";
    }
    else
    {
        defaultPath += "/../PhotosConsigne/resources/profils";
    }

    QString pathProFile = QFileDialog::getSaveFileName(this, "Sélection du fichier de profil :", defaultPath , "Profils (*.profil)");
    if(pathProFile.size() > 0)
    {
        emit saveProfileToSignal(pathProFile);
    }
}



void MainInterface::unlockSaveProfile(QString profileName)
{
    ui->actionSaveProfile->setEnabled(true);

    if(profileName != "")
    {
        ui->laProfileName->setText(profileName);
    }
}

void MainInterface::loadProfile()
{
    QString defaultPath = QDir::currentPath();
    if(QDir(defaultPath + "/resources").exists())
    {
        defaultPath += "/resources/profils";
    }
    else
    {
        defaultPath += "/../PhotosConsigne/resources/profils";
    }

    QString pathProFile = QFileDialog::getOpenFileName(this, "Fichier de profile", defaultPath, "*.profil");
    if(pathProFile.size() > 0)
    {
        emit loadProfileSignal(pathProFile);
    }
}

void MainInterface::profileLoaded(QString profileName, UIParameters params)
{
    ui->laProfileName->setText(profileName);

    // image alignment
    if((params.imageAlignment & Qt::AlignVCenter) == Qt::AlignVCenter)
    {
        ui->rbVCenterImage->setChecked(true);

        if((params.imageAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterImage->setChecked(true);
        }
        else if((params.imageAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightImage->setChecked(true);
        }
        else
        {
            ui->rbLeftImage->setChecked(true);
        }
    }
    else if((params.imageAlignment & Qt::AlignTop) == Qt::AlignTop)
    {
        ui->rbTopImage->setChecked(true);

        if((params.imageAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterImage->setChecked(true);
        }
        else if((params.imageAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightImage->setChecked(true);
        }
        else
        {
            ui->rbLeftImage->setChecked(true);
        }
    }
    else
    {
        ui->rbBottomImage->setChecked(true);

        if((params.imageAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterImage->setChecked(true);
        }
        else if((params.imageAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightImage->setChecked(true);
        }
        else
        {
            ui->rbLeftImage->setChecked(true);
        }

    }

    // consign alignment
    if((params.consignAlignment & Qt::AlignVCenter) == Qt::AlignVCenter)
    {
        ui->rbVCenterText->setChecked(true);

        if((params.consignAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterText->setChecked(true);
        }
        else if((params.consignAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightText->setChecked(true);
        }
        else
        {
            ui->rbLeftText->setChecked(true);
        }
    }
    else if((params.consignAlignment & Qt::AlignTop) == Qt::AlignTop)
    {
        ui->rbTopText->setChecked(true);

        if((params.consignAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterText->setChecked(true);
        }
        else if((params.consignAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightText->setChecked(true);
        }
        else
        {
            ui->rbLeftText->setChecked(true);
        }
    }
    else
    {
        ui->rbBottomText->setChecked(true);

        if((params.consignAlignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        {
            ui->rbHCenterText->setChecked(true);
        }
        else if((params.consignAlignment & Qt::AlignRight) == Qt::AlignRight)
        {
            ui->rbRightText->setChecked(true);
        }
        else
        {
            ui->rbLeftText->setChecked(true);
        }
    }


    // nb pages
    ui->sbNbImagesH->setValue(params.nbImagesPageH);
    ui->sbNbImagesV->setValue(params.nbImagesPageV);

    // ratio
    ui->dsbRatio->setValue(params.ratio);

    // margins
    ui->dsBottomMargins->setValue(params.bottomMargin);
    ui->dsTopMargins->setValue(params.topMargin);
    ui->dsRightMargins->setValue(params.rightMargin);
    ui->dsLeftMargins->setValue(params.leftMargin);
    ui->dsInterMarginsWidth->setValue(params.interMarginWidth);
    ui->dsInterMarginsHeight->setValue(params.interMarginHeight);

    // orientation
    ui->rbPortrait->setChecked(params.orientation);

    // cut lines
    ui->cbCutLines->setChecked(params.cutLines);

    // zones
    ui->cbZoneExternMargins->setChecked(params.zExternMargins);
    ui->cbZoneInternMargins->setChecked(params.zInterMargins);
    ui->cbZonePhotos->setChecked(params.zPhotos);
    ui->cbZoneConsignes->setChecked(params.zConsigns);

    // update font
    QFont font = ui->fcbConsignes->font();
    font.setFamily(params.font.family());
    ui->fcbConsignes->setCurrentFont(font);
    ui->cbBold->setChecked(font.bold());
    ui->cbItalic->setChecked(font.italic());

    font = params.font;
    font.setPixelSize(13);
    ui->pteConsigne->setFont(font);
    ui->pteConsigne->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(params.consignColor.red()) +
                                   ", " + QString::number(params.consignColor.green()) + ", " + QString::number(params.consignColor.blue()) +")};");

    ui->pbChooseColor->setStyleSheet("background-color: rgb("+ QString::number(params.consignColor.red()) +
                                    ", " + QString::number(params.consignColor.green()) + ", " + QString::number(params.consignColor.blue()) +");");
    ui->sbSizeTexte->setValue(params.font.pixelSize());

    // display pages number
    int photosRemoved = 0;
    for(int ii = 0; ii < m_photoRemovedList.count();++ii)
    {
        if(m_photoRemovedList[ii])
            photosRemoved++;
    }

    int nbPhotosTotal = m_photoRemovedList.size()-photosRemoved;
    int nbPhotosPerPage = params.nbImagesPageV*params.nbImagesPageH;
    int nbPages = nbPhotosTotal / nbPhotosPerPage;
    int rest = nbPhotosTotal % nbPhotosPerPage;
    if( rest != 0)
        ++nbPages;

    if(nbPages <= 1)
    {
        ui->laNbPages->setText(QString::number(nbPages) + " page");
    }
    else
    {
        ui->laNbPages->setText(QString::number(nbPages) + " pages");
    }


    emit sentParametersSignal(params);

    if(ui->lwPhotos->count() > 0)
    {
        generatePreview();
    }

    unlockSaveProfile("");
}

void MainInterface::switchTextPhoto()
{
    int currentId = ui->lwPhotos->currentIndex().row();
    m_addTextPhotoEnabled[currentId] = !m_addTextPhotoEnabled[currentId];
    updateAddTextPhotoUI(currentId);
}

void MainInterface::updateAddTextPhotoUI(const int id)
{
    QBrush brush = ui->lwPhotos->currentItem()->foreground();


    if(m_addTextPhotoEnabled[id])
    {
        if(m_photoRemovedList[id])
            brush.setColor(Qt::red);
        else
            brush.setColor(Qt::blue);

        ui->laPhotoIndividualConsign->show();
        ui->tePhotoIndividualConsign->show();
        ui->tePhotoIndividualConsign->setText(m_photosText[id]);
        ui->pbAddTextPhoto->setIcon(QIcon(":/images/removeConsign"));
        ui->pbAddTextPhoto->setIconSize(QSize(100,50));
        ui->pbAddTextPhoto->setToolTip(tr("Retirer le texte unique pour cette photo"));
        ui->tePhotoIndividualConsign->setText(m_photosText[id]);
    }
    else
    {
        if(m_photoRemovedList[id])
            brush.setColor(Qt::red);
        else
            brush.setColor(Qt::black);

        ui->laPhotoIndividualConsign->hide();
        ui->tePhotoIndividualConsign->hide();
        ui->pbAddTextPhoto->setIcon(QIcon(":/images/addConsign"));
        ui->pbAddTextPhoto->setIconSize(QSize(100,50));
        ui->pbAddTextPhoto->setToolTip(tr("Ajouter un texte unique pour cette photo"));
        m_photosText[id] = "";
    }

    ui->lwPhotos->currentItem()->setForeground(brush);
}

void MainInterface::updateIndividualText()
{
    int id = ui->lwPhotos->currentIndex().row();
    m_photosText[id] = ui->tePhotoIndividualConsign->toPlainText();
}

void MainInterface::openOnlineDocumentation()
{
    bool success = QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=jazpY9XrCuc", QUrl::TolerantMode));
    if(!success)
    {
        QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
    }
}

void MainInterface::generatePDF()
{
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->pbSelectPhotos->setEnabled(false);

    m_pdfFileName = QFileDialog::getSaveFileName(this, "Nom du fichier pdf",  QDir::homePath(), "*.pdf");

    if(m_pdfFileName.size() == 0)
    {
        ui->pbGeneration->setEnabled(true);
        ui->pbPreview->setEnabled(true);
        ui->pbSelectPhotos->setEnabled(true);
        return;
    }

    if(m_pdfFileName.right(4) != ".pdf")
    {
        m_pdfFileName += ".pdf";
    }


    ui->laLoading->setText("Génération du pdf...");
    emit generatePDFSignal(m_pdfFileName);
}


void MainInterface::generatePreview()
{    
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->tabDisplay->setCurrentIndex(1);

    emit generatePreviewSignal(ui->lwPhotos->currentIndex().row());
}

void MainInterface::updateUIParameters()
{
    updateUIParameters(false, false, true);
}


void MainInterface::updateUIParameters(QFont notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters(false, false, true);
}

void MainInterface::updateUIParameters(bool notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters(false, false, true);
}

void MainInterface::updateUIParameters(int notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters(false, false, true);
}

void MainInterface::updateUIParameters(double notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameters(false, false, true);
}

void MainInterface::updateUIParameters(bool notUsedValue1, bool notUsedValue2, bool updatePreview)
{
    Q_UNUSED(notUsedValue1);
    Q_UNUSED(notUsedValue2);

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


    // display pages number
    int photosRemoved = 0;
    for(int ii = 0; ii < m_photoRemovedList.count();++ii)
    {
        if(m_photoRemovedList[ii])
            photosRemoved++;
    }

    int nbPhotosTotal = m_photoRemovedList.size()-photosRemoved;
    int nbPhotosPerPage = nbImagesVPage*nbImagesHPage;
    int nbPages = nbPhotosTotal / nbPhotosPerPage;
    int rest = nbPhotosTotal % nbPhotosPerPage;
    if( rest != 0)
        ++nbPages;

    if(nbPages <= 1)
    {
        ui->laNbPages->setText(QString::number(nbPages) + " page");
    }
    else
    {
        ui->laNbPages->setText(QString::number(nbPages) + " pages");
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
    params.interMarginWidth = ui->dsInterMarginsWidth->value();
    params.interMarginHeight = ui->dsInterMarginsHeight->value();

    params.cutLines = ui->cbCutLines->isChecked();
    params.zExternMargins = ui->cbZoneExternMargins->isChecked();
    params.zInterMargins = ui->cbZoneInternMargins->isChecked();
    params.zPhotos = ui->cbZonePhotos->isChecked();
    params.orientation = ui->rbPortrait->isChecked();
    params.zConsigns = ui->cbZoneConsignes->isChecked();

    emit sentParametersSignal(params);


    if(ui->lwPhotos->count() > 0 && updatePreview)
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
    ui->pbChooseColor->setStyleSheet("background-color: rgb("+ QString::number(m_colorText.red()) +
                                    ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +");");

    updateUIParameters(false, false, true);
}

void MainInterface::setProgressBatState(int state)
{
    ui->pbLoading->setValue(state);

    if(state == 100)
        ui->laLoading->setText("Prêt");
}

void MainInterface::setImageLeftRotation()
{
    ui->tabDisplay->setCurrentIndex(0);
    emit updateRotationSignal(ui->lwPhotos->currentIndex().row(), false);
}

void MainInterface::setImageRightRotation()
{
    ui->tabDisplay->setCurrentIndex(0);
    emit updateRotationSignal(ui->lwPhotos->currentIndex().row(), true);
}




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


#include "MainUI.h"


// qt
//#include <QFileDialog>
//#include <QStandardItemModel>
//#include <QColorDialog>
#include <QDesktopServices>
#include <QMessageBox>

// debug
#include <qdebug.h>


using namespace pc;

MainUI::MainUI(QApplication *parent) : m_ui(new Ui::MainUI)
{
    QString version = "1.10";

    m_ui->setupUi(this);

    // set icon/title
    this->setWindowTitle(QString("PhotosConsigne ") + version);
    this->setWindowIcon(QIcon(":/images/icon"));

    // init left menu UI
    m_leftMenuUI = new LeftMenuUI(this);
    m_ui->hlCentral->insertWidget(0, m_leftMenuUI);

    m_ui->hlCentral->setStretch(0,2);
    m_ui->hlCentral->setStretch(1,10);


    // init photos explorer UI
    m_photosExplorerUI = new PhotosExplorerUI();
    m_ui->vlImage->addWidget(m_photosExplorerUI);

    // init preview UI
    m_previewUI = new PreviewUI();
    m_ui->vlPreview->addWidget(m_previewUI);


    // connections
    //  menu
    QObject::connect(m_ui->actionExit, &QAction::triggered, parent, &QApplication::quit);
    QObject::connect(m_ui->actionOnlineDocumentation,  &QAction::triggered, this, &MainUI::openOnlineDocumentation);
    QObject::connect(m_ui->actionAbout, &QAction::triggered, this, &MainUI::openAboutWindow);
    QObject::connect(m_ui->actionDonate, &QAction::triggered, this, &MainUI::openDonatePage);
    //    QObject::connect(ui->actionSaveProfileTo, SIGNAL(triggered()), this, SLOT(saveProfileTo()));
    //    QObject::connect(ui->actionSaveProfile, SIGNAL(triggered()), m_interfaceWorker, SLOT(saveProfile()));
    //    QObject::connect(ui->actionLoadProfile, SIGNAL(triggered()), this, SLOT(loadProfile()));
    //    QObject::connect(ui->actionSaveDefault, SIGNAL(triggered()), m_interfaceWorker, SLOT(saveDefaultProfile()));
    //  photos explorer
    connect(m_photosExplorerUI->LeftImageButton(), &QPushButton::clicked, this, [&]{ // preivous image
       if(m_currentImageDisplayed == 0)
           m_currentImageDisplayed = m_images->size()-1;
       else
           m_currentImageDisplayed--;

       m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
       m_leftMenuUI->ListPhotos()->setCurrentRow(m_currentImageDisplayed);
    });
    connect(m_photosExplorerUI->RightImageButton(), &QPushButton::clicked, this, [&]{ // next image
       if(m_currentImageDisplayed == m_images->size()-1)
           m_currentImageDisplayed = 0;
       else
           m_currentImageDisplayed++;

       m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
       m_leftMenuUI->ListPhotos()->setCurrentRow(m_currentImageDisplayed);
    });
    connect(m_photosExplorerUI->RotateLeftImageButton(), &QPushButton::clicked, this, [&] // rotate left photo
    {
        m_images->at(m_currentImageDisplayed)->scaledImage = m_images->at(m_currentImageDisplayed)->scaledImage.transformed(QTransform().rotate(-90));
        m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
    });
    connect(m_photosExplorerUI->RotateRightImageButton(), &QPushButton::clicked, this, [&] // rotate right photo
    {
        m_images->at(m_currentImageDisplayed)->scaledImage = m_images->at(m_currentImageDisplayed)->scaledImage.transformed(QTransform().rotate(90));
        m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
    });
    connect(m_photosExplorerUI->AddTextPhotoButton(), &QPushButton::clicked, this, [&] // add/remove consign
    {
        SImage image = m_images->at(m_currentImageDisplayed);
        image->addedConsign = !image->addedConsign;
        m_photosExplorerUI->updateUI(image);

        QBrush brush = m_leftMenuUI->ListPhotos()->currentItem()->foreground();
        if(image->addedConsign)
            brush.setColor(image->isRemoved ? Qt::red : Qt::blue);
        else
            brush.setColor(image->isRemoved ? Qt::red : Qt::black);
        m_leftMenuUI->ListPhotos()->currentItem()->setForeground(brush);
    });
    connect(m_photosExplorerUI->IndividualTextPhotoEdit(), &QTextEdit::textChanged, this, [&]
    {
        if(m_images != nullptr)
            m_images->at(m_currentImageDisplayed)->consign = m_photosExplorerUI->IndividualTextPhotoEdit()->toPlainText();
    });
    connect(m_photosExplorerUI->RemovePhotoButton(), &QPushButton::clicked, this, [&] // add/remove photo from list
    {
        SImage image = m_images->at(m_currentImageDisplayed);
        image->isRemoved = !image->isRemoved;
        m_photosExplorerUI->updateUI(image);

        QBrush brush = m_leftMenuUI->ListPhotos()->currentItem()->foreground();
        if(image->addedConsign)
            brush.setColor(image->isRemoved ? Qt::red : Qt::blue);
        else
            brush.setColor(image->isRemoved ? Qt::red : Qt::black);
        m_leftMenuUI->ListPhotos()->currentItem()->setForeground(brush);
    });


    //  left menu
    //      worker
    connect(m_leftMenuUI->Worker(), &LeftMenuWorker::setProgressBarStateSignal, m_ui->pbLoading, &QProgressBar::setValue);
    connect(m_leftMenuUI->Worker(), &LeftMenuWorker::setProgressBarTextSignal, m_ui->laLoading, &QLabel::setText);
    connect(m_leftMenuUI->Worker(), &LeftMenuWorker::endLoadingImagesSignal, [&](SImages images)
    {
        m_images = images;
        m_ui->tabDisplay->setCurrentIndex(0);

        bool valid = m_images->size() > 0;

        m_ui->tabDisplay->setEnabled(valid);
        if(valid)
        {
            m_currentImageDisplayed = 0;
            m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
        }

        m_photosExplorerUI->enableUI(valid);

    });
    //      UI
    //          list photos
    connect(m_leftMenuUI, &LeftMenuUI::currentPageChangedSignal, this, [&](SPDFSettings settings, Page currentPage)
    {
        m_settings = settings;

        m_previewUI->updatePreview(m_images, m_settings, currentPage);
        m_previewUI->updateUI(true);
        m_ui->tabDisplay->setCurrentIndex(1);

    });
    connect(m_leftMenuUI->ListPhotos(), &QListWidget::currentRowChanged, this, [&](int row)
    {
        if(row == -1 || m_images->size() == 0)
            return;

        m_currentImageDisplayed = row;
        m_photosExplorerUI->updatePhotoToDisplay(m_images->at(m_currentImageDisplayed));
        m_ui->tabDisplay->setCurrentIndex(0);
    });
    connect(m_leftMenuUI, &LeftMenuUI::sendSettingsSignal, this, [=](SPDFSettings settings, Page page)
    {
        m_settings = settings;

        if(m_images->size() == 0)
            return;

        m_previewUI->updatePreview(m_images, m_settings, page);
        m_previewUI->updateUI(true);
        m_ui->tabDisplay->setCurrentIndex(1);
    });

    //    QObject::connect(ui->lwPhotos,SIGNAL(currentRowChanged(int)), this, SLOT(updatePhotoIndex(int)));
    //    QObject::connect(ui->lwPhotos, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePhotoIndex(QModelIndex)));)




}

MainUI::~MainUI()
{
    delete m_ui;
}



void MainUI::openOnlineDocumentation()
{
    if(!QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=jazpY9XrCuc", QUrl::TolerantMode)))
        QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet du tutoriel n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
}


void MainUI::openDonatePage()
{
    if(!QDesktopServices::openUrl(QUrl("https://pledgie.com/campaigns/31286#donors", QUrl::TolerantMode)))
        QMessageBox::warning(this, tr("Avertissement"), tr("Le site internet de don n'a pu être lancé, aucun logiciel de navigation web n'a été trouvé.' .\n"),QMessageBox::Ok);
}

void MainUI::openAboutWindow()
{
    QString text("<p>PhotosConsigne est un logiciel léger et rapide servant à générer des PDF contenant des images associées à un texte commun.<br />");
    text += "Tous les paramètres sont facilement modifiables (quantités d'images par page, orientation, alignements, police du texte...). <br /><br /></b>";
    text += "N'hésitez à pas me faire partager vos retours et vos idées d'amélioration, selon mon temps il est possible que j'implémente les plus justifitées.<br /> Contact : <a href=\"mailto:lance.florian@gmail.com?subject=PhotosConsigne&body=...\">email</a> ";
    text += "<br /><br /> Auteur : <b>Lance Florian </b> <a href=\"https://github.com/FlorianLance\"> Github du logiciel </a>";
    QMessageBox::about(this, "A propos du logiciel", text);
}



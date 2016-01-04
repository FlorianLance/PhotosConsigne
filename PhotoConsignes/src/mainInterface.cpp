

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
    this->setWindowTitle(QString("Générateur de pdf : consigne + photos"));
//    this->setWindowIcon(QIcon("../../PhotoConsignes/images/icone.png"));

    // init interface widgets
    //  image labels
    m_imageLabel = new ImageLabel(this);
    ui->vlImage->insertWidget(0, m_imageLabel);

    m_previewLabel = new ImageLabel(this);
    ui->vlPreview->insertWidget(0, m_previewLabel);

    // set styles
    ui->tabPreview->setStyleSheet("background-color: rgb(122,122,122);");

    // color
    m_colorText = Qt::GlobalColor::black;
    ui->pbChoosColor->setStyleSheet("background-color: rgb(0,0,0);");

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

    //  list widget
    QObject::connect(ui->lwPhotos,SIGNAL(currentRowChanged(int)), this, SLOT(updatePhotoIndex(int)));
    QObject::connect(ui->lwPhotos, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePhotoIndex(QModelIndex)));

    // radio button
    QObject::connect(ui->rbLeftText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->rbLeftImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->rbRightImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->rbRightText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->rbCenterImage, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->rbCenterText, SIGNAL(clicked(bool)), this, SLOT(updateUIParameter(bool)));
    // spinbox
    QObject::connect(ui->sbNbImagesPage, SIGNAL(valueChanged(int)), this, SLOT(updateUIParameter(int)));
    QObject::connect(ui->sbSizeTexte, SIGNAL(valueChanged(int)), this, SLOT(updateUIParameter(int)));
    QObject::connect(ui->dsbRatio, SIGNAL(valueChanged(double)), this, SLOT(updateUIParameter(double)));
    // font
    QObject::connect(ui->fcbConsignes, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateUIParameter(QFont)));
    // QPlainText
    QObject::connect(ui->pteConsigne, SIGNAL(textChanged()), this, SLOT(updateUIParameter()));
    // check box
    QObject::connect(ui->cbBold, SIGNAL(toggled(bool)), this, SLOT(updateUIParameter(bool)));
    QObject::connect(ui->cbItalic, SIGNAL(toggled(bool)), this, SLOT(updateUIParameter(bool)));
    //  interface -> worker
    QObject::connect(this, SIGNAL(sendImagesDir(QString, QStringList)), m_interfaceWorker, SLOT(loadImages(QString, QStringList)));
    QObject::connect(this, SIGNAL(generatePDFSignal(QString)), m_interfaceWorker, SLOT(generatePDF(QString)));
    QObject::connect(this, SIGNAL(generatePreview(int)), m_interfaceWorker, SLOT(generatePreview(int)));
    QObject::connect(this, SIGNAL(sentParameters(int,double,QFont,QString, QColor, int, int )),m_interfaceWorker, SLOT(updateParameters(int,double,QFont,QString, QColor,int, int )));
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
    updateUIParameter();
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
}

void MainInterface::setPhotosDirectory()
{
    ui->pbGeneration->setEnabled(false);
    ui->pbPreview->setEnabled(false);
    ui->pbSelectPhotos->setEnabled(false);

    m_photosDirectory = QFileDialog::getExistingDirectory(this, "Sélection du répertoire de photos");

    if(m_photosDirectory.size() == 0)
    {
        ui->pbSelectPhotos->setEnabled(true);
        return;
    }

    ui->laPhotosDir->setText(m_photosDirectory);

    QDir dir(m_photosDirectory);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << "*.png" << "*.jpg");

    QStringList fileList = dir.entryList();
    ui->lwPhotos->clear();
    for(int ii = 0; ii < fileList.size(); ++ii)
    {
        ui->lwPhotos->addItem(fileList[ii]);
    }

    if(fileList.size() > 0)
    {
        ui->lwPhotos->setCurrentRow(0);
    }

    emit sendImagesDir(m_photosDirectory, fileList);
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



void MainInterface::updateUIParameter(QFont notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameter();
}

void MainInterface::updateUIParameter(bool notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameter();
}


void MainInterface::updateUIParameter(int notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameter();
}

void MainInterface::updateUIParameter(double notUsedValue)
{
    Q_UNUSED(notUsedValue);
    updateUIParameter();
}

void MainInterface::updateUIParameter()
{
    int nbImagesPage = ui->sbNbImagesPage->value();
    int sizeText = ui->sbSizeTexte->value();
    double ratio = ui->dsbRatio->value();

    QFont font = ui->fcbConsignes->currentFont();
    font.setPixelSize(sizeText);
    font.setBold(ui->cbBold->isChecked());
    font.setItalic(ui->cbItalic->isChecked());

    QString text = ui->pteConsigne->toPlainText();
    ui->pteConsigne->setFont(font);    
    ui->pteConsigne->setStyleSheet("QPlainTextEdit{color: rgb("+ QString::number(m_colorText.red()) +
                                   ", " + QString::number(m_colorText.green()) + ", " + QString::number(m_colorText.blue()) +")};");

    Alignment imageAlignment = Left;
    if(ui->rbCenterImage->isChecked())
    {
        imageAlignment = Center;
    }
    else if(ui->rbRightImage->isChecked())
    {
        imageAlignment = Right;
    }

    Alignment textAlignment = Left;
    if(ui->rbCenterText->isChecked())
    {
        textAlignment = Center;
    }
    else if(ui->rbRightText->isChecked())
    {
        textAlignment = Right;
    }

    emit sentParameters(nbImagesPage, ratio, font, text, m_colorText,(int)imageAlignment, (int)textAlignment);    

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

    updateUIParameter();
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


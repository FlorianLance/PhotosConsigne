
#pragma once

// Qt
#include <QWidget>

// ui
#include "ui_PhotosExplorerUI.h"

// lccal
#include "ImageLabel.h"
#include "Utility.h"


namespace Ui {
class PhotosExplorerUI;
}

namespace pc{

/**
 * @brief ...
 */
class PhotosExplorerUI : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief PhotosExplorerUI constructor
     * @param [in,out] parent
     */
    PhotosExplorerUI(QWidget *parent = 0) : ui(new Ui::PhotosExplorerUI)
    {
        Q_UNUSED(parent);

        ui->setupUi(this);
        ui->tePhotoIndividualConsign->hide();
        ui->pbRemovePhoto->hide();
        ui->pbLeftImage->hide();
        ui->pbRightImage->hide();
        ui->pbRotationLeft->hide();
        ui->pbRotationRight->hide();
        ui->pbAddTextPhoto->hide();

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

        // init interface widgets
        //  image label
        m_imageLabel = new ImageLabel(this);
        ui->vlPhotosExplorer->insertWidget(0, m_imageLabel);
        m_imageLabel->setPixmap(QPixmap::fromImage(QImage(":/images/null")));
    }

    /**
     * @brief PhotosExplorerUI destructor
     */
    ~PhotosExplorerUI(){delete ui;}


    QPushButton *LeftImageButton() const noexcept {return ui->pbLeftImage;}
    QPushButton *RightImageButton() const noexcept {return ui->pbRightImage;}
    QPushButton *RotateLeftImageButton() const noexcept {return ui->pbRotationLeft;}
    QPushButton *RotateRightImageButton() const noexcept {return ui->pbRotationRight;}
    QPushButton *AddTextPhotoButton() const noexcept {return ui->pbAddTextPhoto;}
    QPushButton *RemovePhotoButton() const noexcept {return ui->pbRemovePhoto;}
    QTextEdit *IndividualTextPhotoEdit() const noexcept {return ui->tePhotoIndividualConsign;}


public slots:


    void enableUI( bool state)
    {
        LeftImageButton()->setEnabled(state);
        RightImageButton()->setEnabled(state);
        RotateLeftImageButton()->setEnabled(state);
        RotateRightImageButton()->setEnabled(state);
        AddTextPhotoButton()->setEnabled(state);
        RemovePhotoButton()->setEnabled(state);
    }

    void updatePhotoToDisplay(SImage image)
    {
//        ui->tePhotoIndividualConsign->show();
        ui->pbRemovePhoto->show();
        ui->pbLeftImage->show();
        ui->pbRightImage->show();
        ui->pbRotationLeft->show();
        ui->pbRotationRight->show();
        ui->pbAddTextPhoto->show();

        m_currentImageDisplayed = image;
        m_imageLabel->setPixmap(QPixmap::fromImage(QImage(image->pathImage).transformed(QTransform().rotate(image->rotation))));


//        m_imageLabel->setPixmap(QPixmap::fromImage(m_currentImageDisplayed->scaledImage));
        m_imageLabel->update();

        updateUI(image);
    }

    void updateUI(SImage image) noexcept
    {
        if(image->addedText)
        {
            ui->tePhotoIndividualConsign->show();
            ui->tePhotoIndividualConsign->setEnabled(true);
            ui->tePhotoIndividualConsign->setText(image->text);
            ui->pbAddTextPhoto->setIcon(QIcon(":/images/removeConsign"));
            ui->pbAddTextPhoto->setIconSize(QSize(100,50));
            ui->pbAddTextPhoto->setToolTip(tr("Retirer le texte unique pour cette photo"));
        }
        else
        {
            ui->tePhotoIndividualConsign->hide();
            ui->tePhotoIndividualConsign->setEnabled(false);
            ui->pbAddTextPhoto->setIcon(QIcon(":/images/addConsign"));
            ui->pbAddTextPhoto->setIconSize(QSize(100,50));
            ui->pbAddTextPhoto->setToolTip(tr("Ajouter un texte unique pour cette photo"));
        }

        if(image->isRemoved)
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
    }





private slots :


signals :


private :

    QIcon m_addPhotoIcon;
    QIcon m_removePhotoIcon;

    SImage m_currentImageDisplayed = nullptr;


    ImageLabel *m_imageLabel = nullptr; /**< display photos widget */
    Ui::PhotosExplorerUI *ui = nullptr; /**< ui PhotosExplorerUI */
};
}

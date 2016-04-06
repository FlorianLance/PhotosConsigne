
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
 * \file MainInterface.h
 * \brief defines MainInterface
 * \author Florian Lance
 * \date 01/11/15
 */


#pragma once

// Qt
#include <QMainWindow>
#include <QMutex>

// ui
#include "ui_mainUI.h"

// Photos consigne
#include "InterfaceWorker.h"
#include "ImageLabel.h"

namespace Ui {
class MainInterface;
}

/**
 * @brief MainInterface class, manage the UI and the dialogs with the worker
 */
class MainInterface : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief MainInterface constructor
     * @param [in,out] parent
     */
    MainInterface(QApplication *parent = 0);

    /**
     * @brief MainInterface destructor
     */
    ~MainInterface();

public slots:


    /**
     * @brief Ask the worker for an update of the image widget with the left photo of the current index
     */
    void leftPhoto();

    /**
     * @brief Ask the worker for an update of the image widget with the right photo of the current index
     */
    void rightPhoto();

    /**
     * @brief Define the photos directory
     */
    void setPhotosDirectory();

    /**
     * @brief Ask the worker for an update of the image widget with the photo corresponding to the input index
     * @param [in] index
     */
    void updatePhotoIndex(int index);

    /**
     * @brief Ask the worker for an update of the image widget with the photo corresponding to the input index
     * @param [in] index
     */
    void updatePhotoIndex(QModelIndex index);

    /**
     * @brief Update the photo displayed in the image widget
     * @param [in] image
     */
    void updatePhotoDisplay(QImage image);

    /**
     * @brief Ask worker to start PDF generation
     */
    void generatePDF();

    /**
     * @brief Ask worker to generate the preview
     */
    void generatePreview();

    /**
     * @brief Unlock the UI
     */
    void unlockUI();

    /**
     * @brief Send the UI parameters to the worker
     */
    void updateUIParameters();

    /**
     * @brief Send the UI parameters to the worker
     * @param [in] notUsedValue
     */
    void updateUIParameters(QFont notUsedValue);

    /**
     * @brief Send the UI parameters to the worker
     * @param [in] notUsedValue
     */
    void updateUIParameters(bool notUsedValue);

    /**
     * @brief Send the UI parameters to the worker
     * @param [in] notUsedValue
     */
    void updateUIParameters(int notUsedValue);

    /**
     * @brief Send the UI parameters to the worker
     * @param [in] notUsedValue
     */
    void updateUIParameters(double notUsedValue);

    /**
     * @brief Send the UI parameters to the worker
     * @param [in] notUsedValue1
     * @param [in] notUsedValue2
     * @param [in] updatePreview
     */
    void updateUIParameters(bool notUsedValue1, bool notUsedValue2, bool updatePreview = true);

    /**
     * @brief Set the text color (dialog) and call updateUIParameters
     */
    void setColorText();

    /**
     * @brief Set the state of the progress bar
     * @param [in] state
     */
    void setProgressBatState(int state);

    /**
     * @brief Ask the worker for a left rotation of the current photo
     */
    void setImageLeftRotation();

    /**
     * @brief Ask the worker for a right rotation of the current photo
     */
    void setImageRightRotation();

    /**
     * @brief Ask the worker to remove the current photo from the list
     */
    void removeCurrentPhotoFromList();

    /**
     * @brief unlock the open PDF button
     */
    void unlockOpenPDF();

    /**
     * @brief Open the generated PDF file with the default extern program
     */
    void openPDF();

    /**
     * @brief Open the online documentation
     */
    void openOnlineDocumentation();

    /**
     * @brief Open the about window
     */
    void openAboutWindow();

    /**
     * @brief Open the donate web page
     */
    void openDonatePage();

    /**
     * @brief Save the current profile to a pro file
     */
    void saveProfileTo();

    /**
     * @brief Unlock the save profile menu item
     * @param [in] profileName
     */
    void unlockSaveProfile(QString profileName);

    /**
     * @brief Load a profile
     */
    void loadProfile();

    /**
     * @brief Update the UI with the loaded profile and update worker
     * @param [in] profileName
     * @param [in] params
     */
    void profileLoaded(QString profileName, UIParameters params);

    /**
     * @brief Switch between the two state of the add photo text button
     */
    void switchTextPhoto();

    /**
     * @brief Update the add photo text UI depending the current photo
     * @param [in] id
     */
    void updateAddTextPhotoUI(const int id);

    /**
     * @brief Save individual text modifications
     */
    void updateIndividualText();


signals :

    void sendImagesDirSignal(QString, QStringList);

    void generatePDFSignal(QString);

    void generatePreviewSignal(int);

    void sentParametersSignal(UIParameters);

    void updateRotationSignal(int, bool);

    void askForPhotoSignal(int);

    void saveProfileToSignal(QString);

    void loadProfileSignal(QString);

    void loadDefautProfileSignal(QString);

private:

    QIcon m_addPhotoIcon;
    QIcon m_removePhotoIcon;

    QColor m_colorText;                 /**< color of the text */
    QString m_photosDirectory;          /**< the photos directory */
    QString m_pdfFileName;              /**< path of the generated PDF file */
    QVector<bool> m_photoRemovedList;   /**< indicates if a photo is removed from the list */
    QVector<bool> m_addTextPhotoEnabled;/**< indicates if photo has a custom individual text */
    QVector<QString> m_photosText;      /**< individual text for each photo */

    bool lockLWPhotos = false;

    QThread  m_workerThread;                /**< worker thread */
    ImageLabel *m_imageLabel;               /**< display photos widget */
    ImageLabel *m_previewLabel;             /**< display preview widget */
    InterfaceWorker  *m_interfaceWorker;    /**< worker */
    Ui::MainInterface *ui;                  /**< ui interface */
};

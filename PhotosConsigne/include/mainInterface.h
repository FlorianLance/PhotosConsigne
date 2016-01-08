
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
 * \file mainInterface.h
 * \brief defines MainInterface
 * \author Florian Lance
 * \date 01/11/15
 */


#pragma once

// Qt
#include <QMainWindow>

// ui
#include "ui_mainUI.h"

// Photos consigne
#include "interfaceworker.h"
#include "imagelabel.h"

namespace Ui {
class MainInterface;
}

class MainInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainInterface(QWidget *parent = 0);

    ~MainInterface();

public slots:

    void setPhotosDirectory();

    void updatePhotoIndex(int index);

    void updatePhotoIndex(QModelIndex index);

    void updatePhotoDisplay(QImage image);

    void generatePDF();

    void generatePreview();

    void unlockUI();

    void updateUIParameters(QFont notUsedValue);

    void updateUIParameters(bool notUsedValue);

    void updateUIParameters(int notUsedValue);

    void updateUIParameters(double notUsedValue);

    void updateUIParameters();

    void setColorText();

    void setProgressBatState(int state);

    void setImageLeftRotation();

    void setImageRightRotation();

    void removeCurrentPhotoFromList();

signals :

    void sendImagesDir(QString, QStringList);

    void generatePDFSignal(QString);

    void generatePreview(int);

    void sentParameters(QVector<bool>, int,int,double,QFont, QString, QColor,int, int, bool, double, double, double, double, double, bool, bool, bool, bool, bool);

    void updateRotation(int, bool);

    void askForPhoto(int);

private:
    Ui::MainInterface *ui;

    QString m_photosDirectory;

    QColor m_colorText;

    QVector<bool> m_photoRemovedList; /**< indicates if a photo is removed from the list */

//    //threads / workers
    InterfaceWorker  *m_interfaceWorker;
    QThread         m_workerThread;

    ImageLabel *m_imageLabel, *m_previewLabel;
};

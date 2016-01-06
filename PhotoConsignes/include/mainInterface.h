
#pragma once

#include <QMainWindow>

#include "interfaceworker.h"

#include "imagelabel.h"

#include "ui_mainUI.h"

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

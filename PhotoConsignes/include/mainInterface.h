
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

    void updateUIParameter(QFont notUsedValue);

    void updateUIParameter(bool notUsedValue);

    void updateUIParameter(int notUsedValue);

    void updateUIParameter(double notUsedValue);

    void updateUIParameter();

    void setColorText();

    void setProgressBatState(int state);

    void setImageLeftRotation();

    void setImageRightRotation();


signals :

    void sendImagesDir(QString, QStringList);

    void generatePDFSignal(QString);

    void generatePreview(int);

    void sentParameters(int,double,QFont, QString, QColor,int, int);

    void updateRotation(int, bool);

    void askForPhoto(int);

private:
    Ui::MainInterface *ui;

    QString m_photosDirectory;

    QColor m_colorText;

//    //threads / workers
    InterfaceWorker  *m_interfaceWorker;
    QThread         m_workerThread;

    ImageLabel *m_imageLabel, *m_previewLabel;
};

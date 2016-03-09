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
 * \file InterfaceWorker.h
 * \brief defines InterfaceWorker
 * \author Florian Lance
 * \date 01/11/15
 */

#pragma once


// qt
#include <QMainWindow>
#include <QThread>
#include <QReadWriteLock>
#include <QModelIndex>
#include <QCoreApplication>
#include <QTime>

// photos consigne
#include "ImageLabel.h"

/**
 * @brief UI parameters of the pdf generation.
 */
struct UIParameters
{
    bool orientation, cutLines, zExternMargins, zInterMargins, zPhotos, zConsigns;
    int nbImagesPageV, nbImagesPageH, imageAlignment, consignAlignment;
    double ratio,leftMargin,rightMargin, topMargin, bottomMargin, betweenMargin;
    QFont font;
    QString consignText;
    QColor consignColor;
    QVector<bool> removePhotoList;
};


/**
 * @brief Qt Worker used by the interface, a dedicated thread for doing the computing work
 */
class InterfaceWorker : public QObject
{
    Q_OBJECT

    public :

        /**
         * @brief constructor of InterfaceWorker
         * @param [in] preview : imagelabel used for displaying preview pdf
         */
        InterfaceWorker(ImageLabel *preview);

    public slots :

        /**
         * @brief Update the image list with a new directory
         * @param [in] path : directory path
         * @param [in] imagesList : list of images files
         */
        void loadImages(QString path, QStringList imagesList);

        /**
         * @brief Generate the pdf with current parameters
         * @param [in] pdfFileName
         */
        void generatePDF(QString pdfFileName);

        /**
        * @brief Generate the preview image with the current parameters and display it in the imageLabel widget
        * @param [in] currentRowPhoto : start id photo for the preview
        */
        void generatePreview(int currentRowPhoto);

        /**
         * @brief Update pdf generation parameters.
         * @param [in] params
         */
        void updateParameters(UIParameters params);

        /**
         * @brief Update the rotation of a photo
         * @param [in] index : id of the photo in the list
         * @param [in] rightRotation : if true, it's a 90° right rotation, else it's 90° left
         */
        void updateRotationImage(int index, bool rightRotation);

        /**
         * @brief Send a photo to be displayed in the image label in the main interface.
         * @param [in] index : index of the photo
         */
        void sendPhoto(int index);

        /**
         * @brief Send a photo to be displayed in the image label in the main interface.
         * @param [in] image
         */
        void sendPhoto(QImage image);


    signals :

        /**
         * @brief signal for unlocking the main ui
         */
        void unlockSignal();

        /**
         * @brief signal for defining the current state of the progress bar
         */
        void setProgressBarStateSignal(int);

        /**
         * @brief Send a photo to be displayed in the image label in the main interface.
         */
        void displayPhotoSignal(QImage);

        /**
         * @brief pdfGeneratedSignal
         */
        void pdfGeneratedSignal();

    private :

        /**
         * @brief Draw the contour of the preview image
         * @param [in] image : current preview image
         */
        void addContourPreview(QImage &image);

        bool m_landScapeOrientation;/**< landscapre orientation of the pdf */
        bool m_isAllPhotoRemoved;   /**< true if all the photo of the list have been removed */
        bool m_cutLines;            /**< draw cut lines ? */
        bool m_zExternMargins;      /**< draw external margins ? (only in preview) */
        bool m_zInterMargins;       /**< draw internal margins ? (only in preview) */
        bool m_zPhotos;             /**< draw the photos zone ? (only in preview) */
        bool m_zConsigns;           /**< draw the consign zone ? (only in preview) */

        int m_consignAlignment;     /**< alignment of the consign */
        int m_imageAlignment;       /**< alignment of the images */
        int m_nbImagesPageH;        /**< horizontal number of images per page */
        int m_nbImagesPageV;        /**< vertical number of images per page */

        double m_ratio;             /**< ratio bewteen the consign height and the images height */
        double m_leftMargin;        /**< ratio of the left margin of the pdf */
        double m_rightMargin;       /**< ratio of the right margin of the pdf */
        double m_topMargin;         /**< ratio of the top margin of the pdf */
        double m_bottomMargin;      /**< ratio of the bottom margin of the pdf */
        double m_innerMargin;       /**< ratio of the inner margin of the pdf */

        QFont m_font;               /**< font of the consign text */
        QString m_consignText;      /**< text of the consign */
        QColor m_consignColor;      /**< consign text color */
        ImageLabel *m_preview;      /**< preview label pointer */

        QVector<bool> m_removedImageList;   /**< mask of the removed images */
        QVector<QImage> m_loadedImages;     /**< loaded images to be used for the pdf generation */
};





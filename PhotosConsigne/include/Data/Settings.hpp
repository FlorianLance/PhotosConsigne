
/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once

/**
 * \file Settings.hpp
 * \brief defines StrUtility/MarginsSettings/BordersSettings/MiscSettings/SetsPositionSettings/ColorsSettings/BackGroundSettings/StyleSettings
 * PhotosSettings/TextSettings/HeaderSettings/FooterSettings/SetSettings/PageSettings/DocumentSettings/PathsSettings/SoftSettings/GlobalSettings
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Photo.hpp"

// Qt
#include <QDoubleSpinBox>
#include <QCoreApplication>
#include <QDir>

namespace pc {   

    // define classes
    struct MarginsSettings : public Settings{

        bool exteriorMarginsEnabled;
        bool interiorMarginsEnabled;
        bool footerHeaderMarginEnabled;
        qreal left;
        qreal right;
        qreal top;
        qreal bottom;
        qreal interWidth;
        qreal interHeight;
        qreal footer;
        qreal header;
    };

    struct BordersSettings  : public Settings{

        bool display = false;
        bool left = true;
        bool top = true;
        bool right = true;
        bool bottom = true;
        bool between = false;
        qreal width = 1.;
        QPen pen;
    };

    struct MiscSettings : public Settings{

        bool doNotDisplayHeader;
        bool doNotDisplayFooter;
    };

    struct SetsPositionSettings  : public Settings{

        bool customMode;

        int nbPhotos;
        int nbPhotosH;
        int nbPhotosV;

        QVector<qreal> columnsWidth;
        QVector<qreal> linesHeight;

        QVector<QRectF> relativePosCustom;
    };

    struct ColorsSettings  : public Settings{

        ColorType type;

        QColor color1;
        QColor color2;

        QPointF start;
        QPointF end;
        DegradedType degradedType;
    };

    struct BackGroundSettings  : public Settings{

        bool displayPhoto = false;
        ImagePositionSettings imagePosition;
        ColorsSettings colors;
        SPhoto photo = nullptr;
    };

    struct StyleSettings  : public Settings{

        ImagePositionSettings imagePosition;
        qreal ratioTextPhoto;
        Position textPositionFromPhotos;
    };

    struct PhotosSettings  : public Settings{

        int previousId     = 0;
        int currentId      = 0;    /**< id of the current selcted photo (photo list widget) */
        SPhotos loaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
        SPhotos valided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    };

    struct TextSettings  : public Settings{
        std::shared_ptr<QString> html = nullptr;
    };

    struct HeaderSettings  : public Settings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct FooterSettings : public Settings{

        bool enabled;
        qreal ratio;
        BackGroundSettings background;
        TextSettings text;
    };

    struct SetSettings : public Settings{

        int currentId            = 0;    /**< id of the current set (Photo + consign) */
        int currentIdDisplayed   = -1;

        StyleSettings style;
        BordersSettings borders;
        TextSettings text;
    };

    struct PageSettings  : public Settings{

        int currentId          =  0;    /**< id of the current selected page (page list widget) */
        int nb                 = 1;     /**< number of pages for the document */
        QString name;

        MarginsSettings margins;
        BackGroundSettings background;
        SetsPositionSettings positions;
        MiscSettings misc;        
    };

    struct DocumentSettings : public Settings{

        bool grayScale           = false;
        bool saveOnlyCurrentPage = false;
        bool noPreviewGeneration = false;

        PaperFormat paperFormat;
    };

    struct PathsSettings : public Settings{

        QString importPhotos;
        QString savePDF;
        QString works = QCoreApplication::applicationDirPath() + "/data/saves";
        QString profile = QCoreApplication::applicationDirPath() + "/data/profile.info";

        PathsSettings() {

            QFile pathsFile(profile);
            if(!pathsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
                return;
            }

            QTextStream in(&pathsFile);
            QString line;
            while (in.readLineInto(&line)) {

                if(line.size() == 0){
                    break;
                }

                QStringList splits = line.split(" ");
                if(splits[0] == "[PHOTOS]"){

                    int index = line.indexOf(' ');
                    QString path = line.remove(0, index+1);

                    if(path == "..."){
                        importPhotos = QDir::homePath();
                    }else{
                        importPhotos = path;
                    }

                }else if(splits[0] == "[PDF]"){

                    int index = line.indexOf(' ');
                    QString path = line.remove(0, index+1);

                    if(path == "..."){
                        savePDF = QDir::homePath();
                    }else{
                        savePDF = path;
                    }
                }
            }
        }

        void write_new_paths(){

            QFile file(profile);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
                return;
            }

            QTextStream out(&file);
            out << "[PHOTOS] " + importPhotos + "\n";
            out << "[PDF] " + savePDF +"\n";
            file.close();
        }
    };

    struct SoftSettings : public Settings{

        // paths
        PathsSettings paths;
    };

    struct GlobalSettings : public Settings{

        // photos
        PhotosSettings photos;

        // pages
        PageSettings pages;

        // sets
        SetSettings sets;

        // header
        HeaderSettings header;

        // footer
        FooterSettings footer;

        // document
        DocumentSettings document;

        // soft
        SoftSettings soft;
    };

}

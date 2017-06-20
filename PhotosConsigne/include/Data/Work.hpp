
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
 * \file Work.hpp
 * \brief defines Work
 * \author Florian Lance
 * \date 04/04/2017
 */


// local
#include "DocumentElements.hpp"
#include "UIElements.hpp"

namespace pc {

    class Work{

    public:

//        static bool save(const QString &saveFilePath, SPhotos loaded, const UIElements &ui){

//            QFile file(saveFilePath);
//            if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
//                return false;
//            }

//            QXmlStreamWriter xml;
//            xml.setDevice(&file);
//            xml.setAutoFormatting(true);

//            xml.writeStartElement("Work");
//            xml.writeStartElement("Photos");
//            xml.writeAttribute("number", QString::number(loaded->size()));
//            for(const auto &photo : *loaded){
//                xml.writeStartElement("Photo");
//                xml.writeAttribute("path", photo->pathPhoto);
//                xml.writeAttribute("id", QString::number(photo->id));
//                xml.writeAttribute("pageId", QString::number(photo->pageId));
//                xml.writeAttribute("loadedId", QString::number(photo->loadedId));
//                xml.writeAttribute("rotation", QString::number(photo->rotation));
//                xml.writeAttribute("white", QString::number(photo->isWhiteSpace));
//                xml.writeAttribute("duplicate", QString::number(photo->isADuplicate));
//                xml.writeAttribute("onDoc", QString::number(photo->isOnDocument));
//                xml.writeAttribute("removed", QString::number(photo->isRemoved));
//                xml.writeEndElement();
//            }
//            xml.writeEndElement();
//            ui.write_to_xml(xml);
//            xml.writeEndElement();
//            xml.writeEndDocument();

//            return true;
//        }
    };

}



#pragma once

// local
#include "DocumentElements.hpp"
#include "UIElements.hpp"


namespace pc {

    class Work{

    public:

        static bool save(const QString &saveFilePath, SPhotos loaded, const UIElements &ui){

            QFile file(saveFilePath);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
                return false;
            }

            QXmlStreamWriter xml;
            xml.setDevice(&file);
            xml.setAutoFormatting(true);

            xml.writeStartElement("Work");
            xml.writeStartElement("Photos");
            xml.writeAttribute("number", QString::number(loaded->size()));
            for(const auto &photo : *loaded){
                xml.writeStartElement("Photo");
                xml.writeAttribute("path", photo->pathPhoto);
                xml.writeAttribute("id", QString::number(photo->id));
                xml.writeAttribute("pageId", QString::number(photo->pageId));
                xml.writeAttribute("loadedId", QString::number(photo->loadedId));
                xml.writeAttribute("rotation", QString::number(photo->rotation));
                xml.writeAttribute("white", QString::number(photo->isWhiteSpace));
                xml.writeAttribute("duplicate", QString::number(photo->isADuplicate));
                xml.writeAttribute("onDoc", QString::number(photo->isOnDocument));
                xml.writeAttribute("removed", QString::number(photo->isRemoved));                
                xml.writeEndElement();
            }
            xml.writeEndElement();            
            ui.write_to_xml(xml);
            xml.writeEndElement();
            xml.writeEndDocument();

            return true;
        }
    };

}

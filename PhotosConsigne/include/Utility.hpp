
/**
 * \file Utility.hpp
 * \brief ...
 * \author Florian Lance
 * \date 04/04/2017
 */


#pragma once


// std
//#include <memory>

// Qt
// # widgets
#include <QWidget>
// # others
#include <QDate>
#include <QElapsedTimer>
#include <QReadWriteLock>
// # debug
#include <QDebug>


// local
// # data
#include "Settings.hpp"


namespace pc
{
    // define classes
    struct MobileWidget{

        ~MobileWidget(){
            if(widget->parent() == nullptr){
                delete widget;
            }
        }

        QWidget *widget = new QWidget();
    };

    struct DebugMessage{

        DebugMessage(QString message): m_message(message){
            qDebug() << "[->" << m_message;
        }

        ~DebugMessage(){
            qDebug() << "   " << m_message << "<-]";
        }

        QString m_message;
    };


    struct Drawing{

        static void draw_filled_rect(QPainter &painter, const QRectF &rect, QRgb color, qreal opacity);


        // define static functions
        static QString format_html_for_generation(QString html, ExtraPCInfo infos = ExtraPCInfo());
    };
}

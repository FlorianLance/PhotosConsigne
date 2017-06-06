
#pragma once

// Qt
#include <QDebug>
#include <QString>
#include <QElapsedTimer>

namespace pc
{
    struct DebugMessage{

        DebugMessage(QString message): m_message(message){
            timer.start();
            qDebug() << "[->" << m_message;
        }

        ~DebugMessage(){
            qDebug() << "   " << m_message << " -> time: " << timer.elapsed() << "<-]";
        }

        QString m_message;
        QElapsedTimer timer;

    };
}

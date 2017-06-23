

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

/**
 * \file CustomPageW.cpp
 * \brief defines CustomPageW
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "CustomPageW.hpp"

// debug
#include <QDebug>

using namespace pc;

CustomPageW::CustomPageW() : SettingsW(){

    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);

//    this->setMinimumWidth(400);
//    this->setMaximumWidth(400);
    layout->addWidget(&m_frame);

    m_layout = new QGridLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_frame.setLayout(m_layout);
}


void CustomPageW::init(int gridW, int gridH){

    sizeGrid = QSize(gridW, gridH);
    currentFirstPoint = QPoint(-1,-1);
    currentSecondPoint = QPoint(-1,-1);
    relativePositions.clear();

    for(auto &&hCBoxes : m_points){
        for(auto &&cb : hCBoxes){
            delete cb;
        }
    }
    m_points.clear();


    QString style =

            /*
            "QCheckBox::indicator {"
                "width: 10px;"
                "height: 10px;"
            "}"
                    */
            "QCheckBox::indicator:unchecked:hover {"
                "image: url(:/images/transparent/tcb_unchecked_hover);"
            "}"

            "QCheckBox::indicator:unchecked {"
                "image: url(:/images/transparent/tcb_unchecked);"
            "}"

            "QCheckBox::indicator:unchecked:pressed {"
                "image: url(:/images/transparent/tcb_unchecked_pressed);"
            "}"

            "QCheckBox::indicator:checked {"
                "image: url(:/images/transparent/tcb_checked);"
            "}"

            "QCheckBox::indicator:checked:hover {"
                "image: url(:/images/transparent/tcb_checked_hover);"
            "}"

            "QCheckBox::indicator:checked:pressed {"
                "image: url(:/images/transparent/tcb_checked_pressed);"
            "}"

        ;

    for(int ii = 0; ii < gridH; ++ii){
        QVector<QCheckBox*> hBBoxes;
        for(int jj = 0; jj < gridW; ++jj){

            hBBoxes.push_back(new QCheckBox());

            hBBoxes.last()->setStyleSheet(style);

            m_layout->addWidget(hBBoxes.last(),ii,jj, Qt::AlignCenter);

            connect(hBBoxes.last(), &QCheckBox::clicked, this, [=]{

                QVector<QPoint> checkedPos;
                for(int aa = 0; aa < m_points.size(); ++aa){
                    for(int bb = 0; bb < m_points[aa].size(); ++bb){
                        if(m_points[aa][bb]->isChecked()){
                            checkedPos.push_back(QPoint(aa,bb));
                        }
                    }
                }

                currentFirstPoint = QPoint(-1,-1);
                currentSecondPoint = QPoint(-1,-1);

                if(checkedPos.size() == 1){
                    currentFirstPoint = checkedPos[0];

                    // disable line
                    for(int aa = 0; aa < m_points.size(); ++aa){
                        for(int bb = 0; bb < m_points[aa].size(); ++bb){
                            if(aa == currentFirstPoint.x() && bb == currentFirstPoint.y()){
                                m_points[aa][bb]->setEnabled(true);
                            }else if(aa == currentFirstPoint.x() || bb == currentFirstPoint.y()){
                                m_points[aa][bb]->setEnabled(false);
                            }else{
                                m_points[aa][bb]->setEnabled(true);
                            }
                        }
                    }
                }
                else if(checkedPos.size() == 2){
                    currentFirstPoint = checkedPos[0];
                    currentSecondPoint = checkedPos[1];

                    for(int aa = 0; aa < m_points.size(); ++aa){
                        for(int bb = 0; bb < m_points[aa].size(); ++bb){
                            m_points[aa][bb]->setEnabled(false);
                        }
                    }
                     m_points[ii][jj]->setEnabled(true);

                     QCheckBox* firstCB  = m_points.first().first();
                     QCheckBox* lastCB   = m_points.last().last();

                     QPoint pStart = firstCB->pos() + m_frame.pos() + QPoint(firstCB->width()/2, firstCB->height()/2);
                     QPoint pEnd   = lastCB->pos()  + m_frame.pos() + QPoint(lastCB->width()/2, lastCB->height()/2);;
                     int height = pEnd.y()-pStart.y();
                     int width =  pEnd.x()-pStart.x();
                     QCheckBox* currFirstCB  = m_points[currentFirstPoint.x()][currentFirstPoint.y()];
                     QCheckBox* currSecondCB = m_points[currentSecondPoint.x()][currentSecondPoint.y()];
                     QPoint pCurrFirst  = currFirstCB->pos() + m_frame.pos() + QPoint(currFirstCB->width()/2, currFirstCB->height()/2);
                     QPoint pCurrSecond = currSecondCB->pos() + m_frame.pos() + QPoint(currSecondCB->width()/2, currSecondCB->height()/2);

                     int minX = std::min(pCurrFirst.x(), pCurrSecond.x());
                     int minY = std::min(pCurrFirst.y(), pCurrSecond.y());

                     int maxX = std::max(pCurrFirst.x(), pCurrSecond.x());
                     int maxY = std::max(pCurrFirst.y(), pCurrSecond.y());
                     currRelativePos = QRectF(  (1.*minX-pStart.x())/width,
                                                (1.*minY-pStart.y())/height,
                                                (1.*maxX-minX)/width,
                                                (1.*maxY-minY)/height);


                }else{
                    for(int aa = 0; aa < m_points.size(); ++aa){
                        for(int bb = 0; bb < m_points[aa].size(); ++bb){
                            m_points[aa][bb]->setEnabled(true);
                        }
                    }
                }

                emit current_state_signal(currentFirstPoint, currentSecondPoint, relativePositions.size());
                update();
            });


        }
        m_points.push_back(hBBoxes);
    }

    emit current_state_signal(currentFirstPoint, currentSecondPoint, relativePositions.size());
    update();
}

void CustomPageW::init_ui(CustomPageW &p1, const CustomPageW &p2){

    p1.blockSignals(true);
    p1.init(p2.m_points[0].size(), p2.m_points.size());
    p1.currentFirstPoint    = p2.currentFirstPoint;
    p1.currentSecondPoint   = p2.currentSecondPoint;
    p1.relativePositions    = p2.relativePositions;
    p1.currRelativePos      = p2.currRelativePos;

    for(int ii = 0; ii < p1.m_points.size(); ++ii){
        for(int jj = 0; jj < p1.m_points[ii].size(); ++jj){
            p1.m_points[ii][jj]->setEnabled(p2.m_points[ii][jj]->isEnabled());
            Utility::safe_init_checkboxe_checked_state(p1.m_points[ii][jj], p2.m_points[ii][jj]->isChecked());
        }
    }

    p1.blockSignals(false);
    p1.update();
}

void CustomPageW::update_ui_with_settings(const SetsPositionSettings &pos){

    blockSignals(true);
    init(pos.nbPhotosH, pos.nbPhotosV);

    currentFirstPoint = QPoint(-1,-1);
    currentSecondPoint = QPoint(-1,-1);
    relativePositions =  pos.relativePosCustom;

    blockSignals(false);
    update();
}

void CustomPageW::update_format(const PaperFormat &format){

    constexpr qreal maxW = 350.;
    constexpr qreal maxH = 350.;

    qreal width,height;
    if(format.ratioMM.height() > format.ratioMM.width()){
        height = maxH;
        width  = maxW *format.ratioMM.width() / format.ratioMM.height();
    }else{
        width   = maxW;
        height  = maxH *format.ratioMM.height() / format.ratioMM.width();
    }

    m_frame.setMinimumWidth(static_cast<int>(width));
    m_frame.setMinimumHeight(static_cast<int>(height));
}


void CustomPageW::paintEvent(QPaintEvent *event){

    Q_UNUSED(event)


    int nbChecked= 0;
    for(auto &&line : m_points){
        for(auto &&point : line){
            if(point->isChecked()){
                ++nbChecked;
            }
        }
    }

    if(nbChecked == 0){
        for(auto &&line : m_points){
            for(auto &&point : line){
                point->setEnabled(true);
            }
        }
    }

    QPainter painter(this);
    QCheckBox* firstCB  = m_points.first().first();
    QCheckBox* lastCB   = m_points.last().last();

    QPoint pStart = firstCB->pos() + m_frame.pos() + QPoint(firstCB->width()/2, firstCB->height()/2);
    QPoint pEnd   = lastCB->pos()  + m_frame.pos() + QPoint(lastCB->width()/2, lastCB->height()/2);;

    painter.fillRect(QRectF(m_frame.x(), m_frame.y(), m_frame.width(), m_frame.height()),Qt::gray);

    QRectF pageRect(pStart.x(), pStart.y(), pEnd.x()-pStart.x(),pEnd.y()-pStart.y());
    painter.fillRect(pageRect, Qt::white);
    painter.setPen(Qt::black);

    // draw valid sets
    int color = 0;
    for(const auto &pos : relativePositions){

        QRectF rect(pos.x()*pageRect.width()+pageRect.x(),
                    pos.y()*pageRect.height()+pageRect.y(),
                    pos.width()*pageRect.width(),
                    pos.height()*pageRect.height());

        painter.fillRect(rect, UIColors[color++]);
        painter.drawRect(rect);
        color = color%UIColors.size();
    }

    // draw current set
    if(currentSecondPoint.x() > -1){

        QCheckBox* currFirstCB  = m_points[currentFirstPoint.x()][currentFirstPoint.y()];
        QCheckBox* currSecondCB = m_points[currentSecondPoint.x()][currentSecondPoint.y()];
        QPoint pCurrFirst  = currFirstCB->pos() + m_frame.pos() + QPoint(currFirstCB->width()/2, currFirstCB->height()/2);
        QPoint pCurrSecond = currSecondCB->pos() + m_frame.pos() + QPoint(currSecondCB->width()/2, currSecondCB->height()/2);

        int minX = std::min(pCurrFirst.x(), pCurrSecond.x());
        int minY = std::min(pCurrFirst.y(), pCurrSecond.y());

        int maxX = std::max(pCurrFirst.x(), pCurrSecond.x());
        int maxY = std::max(pCurrFirst.y(), pCurrSecond.y());

        color = relativePositions.size()% UIColors.size();
        painter.fillRect(QRectF(minX, minY, maxX-minX, maxY-minY),  UIColors[color]);
        painter.drawRect(QRectF(minX, minY, maxX-minX, maxY-minY));
    }

    painter.drawRect(pageRect);
}

void CustomPageW::write_to_xml(QXmlStreamWriter &xml) const{

    xml.writeStartElement("CustomPage");
    xml.writeAttribute("sizeGrid", StrUtility::to_str(sizeGrid));
    xml.writeAttribute("currRelPos", StrUtility::to_str(currRelativePos));
    xml.writeAttribute("currFirstPoint", StrUtility::to_str(currentFirstPoint));
    xml.writeAttribute("currSecondPoint", StrUtility::to_str(currentSecondPoint));
    xml.writeAttribute("nbRelPositions", QString::number(relativePositions.size()));

    QXmlStreamAttributes relPositions;
    int currV = 0;
    for(const auto &relPos : relativePositions){
        relPositions.append("h"+QString::number(currV++), StrUtility::to_str(relPos));
    }
    xml.writeAttributes(relPositions);

    xml.writeEndElement();
}

void CustomPageW::load_from_xml(QXmlStreamReader &xml){

    QStringList sizeGrid = xml.attributes().value("sizeGrid").toString().split(" ");
    init(sizeGrid[0].toInt(), sizeGrid[1].toInt());

    QStringList rect = xml.attributes().value("currRelPos").toString().split(" ");
    currRelativePos = QRectF(rect[0].toDouble(),rect[1].toDouble(),rect[2].toDouble(),rect[3].toDouble());

    QStringList pt = xml.attributes().value("currFirstPoint").toString().split(" ");
    currentFirstPoint = QPoint(pt[0].toInt(), pt[1].toInt());
    pt = xml.attributes().value("currSecondPoint").toString().split(" ");
    currentSecondPoint = QPoint(pt[0].toInt(), pt[1].toInt());

    int nbRelPositions = xml.attributes().value("nbRelPositions").toInt();
    relativePositions.clear();
    for(int ii = 0; ii < nbRelPositions; ++ii){

        rect = xml.attributes().value("h" + QString::number(ii)).toString().split(" ");
        relativePositions.push_back(QRectF(rect[0].toDouble(),rect[1].toDouble(),rect[2].toDouble(),rect[3].toDouble()));
    }
}

void CustomPageW::validate_current_set(){

    if(currentSecondPoint.x() == -1){
        return;
    }

    QCheckBox* firstCB  = m_points.first().first();
    QCheckBox* lastCB   = m_points.last().last();
    QPoint pStart = firstCB->pos() + m_frame.pos() + QPoint(firstCB->width()/2, firstCB->height()/2);
    QPoint pEnd   = lastCB->pos()  + m_frame.pos() + QPoint(lastCB->width()/2, lastCB->height()/2);
    int width =  pEnd.x()-pStart.x();
    int height = pEnd.y()-pStart.y();

    QCheckBox* currFirstCB  = m_points[currentFirstPoint.x()][currentFirstPoint.y()];
    QCheckBox* currSecondCB = m_points[currentSecondPoint.x()][currentSecondPoint.y()];
    QPoint pCurrFirst  = currFirstCB->pos() + m_frame.pos() + QPoint(currFirstCB->width()/2, currFirstCB->height()/2);
    QPoint pCurrSecond = currSecondCB->pos() + m_frame.pos() + QPoint(currSecondCB->width()/2, currSecondCB->height()/2);

    int minX = std::min(pCurrFirst.x(), pCurrSecond.x());
    int minY = std::min(pCurrFirst.y(), pCurrSecond.y());

    int maxX = std::max(pCurrFirst.x(), pCurrSecond.x());
    int maxY = std::max(pCurrFirst.y(), pCurrSecond.y());

    relativePositions.push_back(QRectF(  (1.*minX-pStart.x())/width,
                                         (1.*minY-pStart.y())/height,
                                         (1.*maxX-minX)/width,
                                         (1.*maxY-minY)/height));

    currentFirstPoint = QPoint(-1,-1);
    currentSecondPoint = QPoint(-1,-1);
    for(int aa = 0; aa < m_points.size(); ++aa){
        for(int bb = 0; bb < m_points[aa].size(); ++bb){
            m_points[aa][bb]->setEnabled(true);
            m_points[aa][bb]->setChecked(false);
        }
    }

    emit current_state_signal(currentFirstPoint, currentSecondPoint, relativePositions.size());
    update();
}

void CustomPageW::remove_last_set(){

    if(currentSecondPoint.x() != -1){

        currentFirstPoint = QPoint(-1,-1);
        currentSecondPoint = QPoint(-1,-1);
        for(auto &&line : m_points){
            for(auto &&point : line){
                point->setEnabled(true);
                point->setChecked(false);
            }
        }

        emit current_state_signal(currentFirstPoint, currentSecondPoint, relativePositions.size());
        update();
        return;
    }

    if(relativePositions.size() ==0){
        return;
    }

    relativePositions.removeLast();

    emit current_state_signal(currentFirstPoint, currentSecondPoint, relativePositions.size());
    update();
}

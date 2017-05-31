



// local
#include "CustomPageW.hpp"

// debug
#include <QDebug>

using namespace pc;

CustomPageW::CustomPageW() : SettingsW(){

    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);

    this->setMaximumWidth(350);
    layout->addWidget(&m_frame);

    m_layout = new QGridLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_frame.setLayout(m_layout);
}


void CustomPageW::init(int gridW, int gridH){

    currentFirstPoint = QPoint(-1,-1);
    currentSecondPoint = QPoint(-1,-1);
    m_relativePositions.clear();

    for(auto &&hCBoxes : m_points){
        for(auto &&cb : hCBoxes){
            delete cb;
        }
    }
    m_points.clear();


    for(int ii = 0; ii < gridH; ++ii){
        QVector<QCheckBox*> hBBoxes;
        for(int jj = 0; jj < gridW; ++jj){

            hBBoxes.push_back(new QCheckBox());
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

                }else{
                    for(int aa = 0; aa < m_points.size(); ++aa){
                        for(int bb = 0; bb < m_points[aa].size(); ++bb){
                            m_points[aa][bb]->setEnabled(true);
                        }
                    }
                }

                emit current_state_signal(currentFirstPoint, currentSecondPoint, m_relativePositions.size());
                update();
            });

        }
        m_points.push_back(hBBoxes);
    }

    update();
}

void CustomPageW::update_format(const PaperFormat &format){

    constexpr qreal maxW = 300.;
    constexpr qreal maxH = 300.;

    qreal width,height;
    if(format.heightRatio > format.widthRatio){
        height = maxH;
        width  = maxW *format.widthRatio / format.heightRatio;
    }else{
        width   = maxW;
        height  = maxH *format.heightRatio / format.widthRatio;
    }

    m_frame.setMinimumWidth(static_cast<int>(width));
    m_frame.setMinimumHeight(static_cast<int>(height));
}


void CustomPageW::paintEvent(QPaintEvent *event){

    Q_UNUSED(event)

    QPainter painter(this);
    QCheckBox* firstCB  = m_points.first().first();
    QCheckBox* lastCB   = m_points.last().last();


    QPoint pStart = firstCB->pos() + m_frame.pos() + QPoint(firstCB->width()/2, firstCB->height()/2);
    QPoint pEnd   = lastCB->pos()  + m_frame.pos() + QPoint(lastCB->width()/2, lastCB->height()/2);;
//    qDebug() << "p1 : " << pStart << " " << m_frame.pos() << " " << firstCB->width();

    painter.fillRect(QRectF(m_frame.x(), m_frame.y(), m_frame.width(), m_frame.height()),Qt::gray);
    QRectF pageRect(pStart.x(), pStart.y(), pEnd.x()-pStart.x(),pEnd.y()-pStart.y());
    painter.fillRect(pageRect, Qt::white);

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

        painter.fillRect(QRectF(minX, minY, maxX-minX, maxY-minY), Qt::blue);
    }

    // draw valid sets

    painter.setPen(Qt::black);
    painter.drawRect(pageRect);


    //    painter.fillRect(m_frame.pos().x(), m_frame.pos().y(), 25,25, Qt::green);
}

void CustomPageW::validate_current_set(){

    QCheckBox* currFirstCB  = m_points[currentFirstPoint.x()][currentFirstPoint.y()];
    QCheckBox* currSecondCB = m_points[currentSecondPoint.x()][currentSecondPoint.y()];
    QPoint pCurrFirst  = currFirstCB->pos() + m_frame.pos() + QPoint(currFirstCB->width()/2, currFirstCB->height()/2);
    QPoint pCurrSecond = currSecondCB->pos() + m_frame.pos() + QPoint(currSecondCB->width()/2, currSecondCB->height()/2);

    int minX = std::min(pCurrFirst.x(), pCurrSecond.x());
    int minY = std::min(pCurrFirst.y(), pCurrSecond.y());

    int maxX = std::max(pCurrFirst.x(), pCurrSecond.x());
    int maxY = std::max(pCurrFirst.y(), pCurrSecond.y());

    m_relativePositions.push_back(QRectF(1.*minX/m_points.size(), 1.*minY/m_points[0].size(),
                          1.*(maxX-minX)/m_points.size(), 1.*(maxY-minY)/m_points[0].size()));
}

void CustomPageW::remove_last_set(){
    m_relativePositions.removeLast();
}

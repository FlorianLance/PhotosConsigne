
/**
 * \file ImageLabel.hpp
 * \brief defines ImageLabel
 * \author Florian Lance
 * \date 01/11/15
 */

#pragma once

// Qt
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

/**
 * @brief Define a QWidget displaying an image.
 */
class ImageLabel : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief ImageLabel constructor
     * @param [in] parent
     */
    explicit ImageLabel(QWidget *parent = 0);

    /**
     * @brief Return current image pointer
     */
    const QImage* Image() const;

public slots:

    /**
     * @brief Set the current image
     */
    void set_image(const QImage&);

protected:

    /**
     * @brief paintEvent for drawing in the widget
     */
    virtual void paintEvent(QPaintEvent *);

    QImage m_image;
    QRectF m_imageRect;
};


class PreviewLabel : public ImageLabel
{
    Q_OBJECT

public slots:


    void update_PC_rects(QVector<QRectF> pcRects, int currentPageId){
        m_pcRects = pcRects;

        if(m_currentPCSelectedId >= m_pcRects.size() || currentPageId != m_currentPageId)
            m_currentPCSelectedId = -1;
        m_currentPageId = currentPageId;
    }

    void resize_rect(){
        qreal ratioH= 0,ratioV = 0;
        if(m_imageRect.height() > 0)
            ratioH = 1.0* m_imageRect.height()/m_image.height();
        if(m_imageRect.width() > 0)
            ratioV = 1.0* m_imageRect.width()/m_image.width();

        m_pcRectsTransfo.clear();
        m_pcRectsTransfo.reserve(m_pcRects.size());
        for(auto &&pcRect : m_pcRects){
            m_pcRectsTransfo.push_back(QRectF(pcRect.x()*ratioH + m_imageRect.x(),  pcRect.y()*ratioV + m_imageRect.y(),
                             pcRect.width()*ratioH, pcRect.height()*ratioV));
        }
    }

protected:

    void mousePressEvent(QMouseEvent * ev )
    {
        resize_rect();

        QPoint p = ev->pos();
        if(m_imageRect.contains(p.x(),p.y())){
            for(int ii = 0; ii < m_pcRectsTransfo.size(); ++ii)
            {
                if(m_pcRectsTransfo[ii].contains(p)){
                    if(m_currentPCSelectedId == ii)
                        m_currentPCSelectedId = -1;
                    else
                        m_currentPCSelectedId = ii;

                    emit send_current_PC_selected_signal(m_currentPageId, m_currentPCSelectedId);
                    break;
                }
            }
            update();
        }
    }


    void paintEvent(QPaintEvent *event){
        ImageLabel::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if(m_currentPCSelectedId > -1){
            qDebug() << "paint preview";
            resize_rect();

            QSize imageSize(m_pcRectsTransfo[m_currentPCSelectedId].size().width(), m_pcRectsTransfo[m_currentPCSelectedId].size().height());
            QImage scaledImage(imageSize, QImage::Format_ARGB32);
            scaledImage.fill(QColor(255,0,0,50));

            QPixmap scaledPix = QPixmap::fromImage(scaledImage);
            painter.drawPixmap(m_pcRectsTransfo[m_currentPCSelectedId].x(), m_pcRectsTransfo[m_currentPCSelectedId].y(), scaledPix);
        }
    }

signals:

    void send_current_PC_selected_signal(int idPage, int currentIdPC);

private:


    QVector<QRectF> m_pcRects;
    QVector<QRectF> m_pcRectsTransfo;

    int m_currentPageId = 0;
    int m_currentPCSelectedId = -1;
};

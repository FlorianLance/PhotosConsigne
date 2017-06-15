
/**
 * \file ImageLabel.hpp
 * \brief defines ImageLabel
 * \author Florian Lance
 * \date 01/11/15
 */

#pragma once

// Qt
#include <QWidget>
#include <QMouseEvent>
#include <QTimer>


/**
 * @brief Define a QWidget displaying an image.
 */
class PhotoW : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief ImageLabel constructor
     * @param [in] parent
     */
    explicit PhotoW(QWidget *parent = 0);

    /**
     * @brief Return current image pointer
     */
    const QImage* Image() const;

public slots:

    /**
     * @brief Set the current image
     */
    void set_image(QImage image);

signals:

    void double_click_signal();

    void click_inside_signal(QPointF relPos);

protected:

    virtual void mousePressEvent(QMouseEvent *ev){

        bool inside = m_imageRect.contains(ev->pos());
        if(inside){

            QPointF pos((ev->pos().x()-m_imageRect.x())/m_imageRect.width(), (ev->pos().y()-m_imageRect.y())/m_imageRect.height());
            emit click_inside_signal(pos);

            if(m_doubleClickTimer.isActive()){
                emit double_click_signal();
            }else{
                m_doubleClickTimer.start(300);
            }
        }
    }

    /**
     * @brief paintEvent for drawing in the widget
     */
    virtual void paintEvent(QPaintEvent *);

    QImage m_image;
    QRectF m_imageRect;
    QTimer m_doubleClickTimer;
};




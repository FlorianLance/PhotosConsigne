
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
#include <QTimer>
#include <QThread>
#include <memory>
#include <QCoreApplication>
#include <QReadWriteLock>

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


class PreviewWorker : public  QObject
{
 Q_OBJECT

public slots:

    void loop_update();

    void stop_loop();

private :

    bool m_isLooping = false;
    QReadWriteLock m_locker;

signals:

    void update_preview_signal();


};

class PreviewLabel : public ImageLabel
{
    Q_OBJECT

public :
    PreviewLabel();

    ~PreviewLabel();

public slots:

    void draw_current_pc_rect(QRectF pcRectRelative);

protected:

    void mousePressEvent(QMouseEvent * ev )
    {
        bool inside = m_imageRect.contains(ev->pos());
        if(inside){
            QPointF posRelative =(ev->pos() - m_imageRect.topLeft());
            posRelative.setX(posRelative.x()/m_imageRect.width());
            posRelative.setY(posRelative.y()/m_imageRect.height());

            emit click_on_page_signal(posRelative);
        }
    }

    void paintEvent(QPaintEvent *event){
        ImageLabel::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);


        if(m_pcRectRelative.width() > 0 && m_rectTimer.isActive()){
            QRect currentPCRect(m_imageRect.x() + m_pcRectRelative.x()*m_imageRect.width(),
                                    m_imageRect.y() + m_pcRectRelative.y()*m_imageRect.height(),
                                    m_pcRectRelative.width()*m_imageRect.width(),
                                    m_pcRectRelative.height()*m_imageRect.height());

            int alpha = (m_rectTimer.remainingTime() > 2500) ? 90 : (90*m_rectTimer.remainingTime()/2500.);
            painter.fillRect(currentPCRect, QColor(255,0,0,alpha));
        }
    }

signals:

    void click_on_page_signal(QPointF pos);

    void start_update_loop_signal();

    void stop_update_loop_signal();

private:

    QRectF m_pcRectRelative;
    QTimer m_rectTimer;

    QThread m_workerThread;
    std::unique_ptr<PreviewWorker> m_worker = nullptr;

};

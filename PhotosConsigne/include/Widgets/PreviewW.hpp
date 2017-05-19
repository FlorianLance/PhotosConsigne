

#pragma once

// local
#include "PhotoW.hpp"

// Qt
#include <QReadWriteLock>
#include <QThread>

// std
#include <memory>

namespace pc{


class PreviewWorker : public  QObject
{
 Q_OBJECT

public:

    PreviewWorker(){}

public slots:

    void loop_update();

    void stop_loop();


private :

    bool m_isLooping = false;
    QReadWriteLock m_locker;


signals:

    void update_preview_signal();


};

class PreviewW : public PhotoW
{
    Q_OBJECT

public :
    PreviewW();

    ~PreviewW();

public slots:

    void draw_current_pc_rect(int idRect, QRectF pcRectRelative);

protected:

    virtual void mousePressEvent(QMouseEvent * ev ) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void click_on_page_signal(QPointF pos);

    void start_update_loop_signal();

    void stop_update_loop_signal();

    void double_click_on_photo_signal(int idTotalPhoto);

private:

    int m_currentPCRectId;
    QRectF m_currentPCRect;
    QRectF m_pcRectRelative;

    QTimer m_rectTimer;

    QThread m_workerThread;
    std::unique_ptr<PreviewWorker> m_worker = nullptr;

};






}

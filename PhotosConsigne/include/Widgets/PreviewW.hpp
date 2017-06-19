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

// local
#include "PhotoW.hpp"
#include "DebugMessage.hpp"
#include "DocumentElements.hpp"

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

    void set_page(SPCPage previewPage);

    void set_current_pc(int idPc);

protected:

    virtual void mousePressEvent(QMouseEvent * ev ) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void click_on_page_signal();

    void start_update_loop_signal();

    void stop_update_loop_signal();

    void double_click_on_photo_signal(int idTotalPhoto);

    void current_pc_selected_signal(int totalId);

private:

    int m_currentRectId;
    QRectF m_currentPCRect;
    QRectF m_rectRelative;

    QTimer m_rectTimer;

    SPCPage m_previewPage = nullptr;

    QThread m_workerThread;
    std::unique_ptr<PreviewWorker> m_worker = nullptr;        

};






}

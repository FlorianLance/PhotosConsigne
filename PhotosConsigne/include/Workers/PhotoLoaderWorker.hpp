

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

/**
 * \file PhotoLoaderWorker.hpp
 * \brief defines PhotoLoaderWorker
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Photo.hpp"
#include "Utility.hpp"

namespace pc {

class PhotoLoaderWorker : public QObject{

    Q_OBJECT

public :

    PhotoLoaderWorker();

public slots :

    void load_photos_directory(QStringList photosPath, int startIndexToInsert);

    void kill();

signals :

    void set_progress_bar_state_signal(int state);

    void set_progress_bar_text_signal(QString text);

    void photo_loaded_signal(SPhoto photo, int indexToInsert);

    void end_loading_photos_signal();


private :

    QReadWriteLock m_locker;
    bool m_continueLoop = true;
};

}

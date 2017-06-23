
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
 * \file main.cpp
 * \brief entry point
 * \author Florian Lance
 * \date 01/11/15
 */


// local
#include "PCMainUI.hpp"

// Qt
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDesktopWidget>

int main(int argc,char** argv)
{    

    qSetMessagePattern("\033[32m%{time h:mm:ss.zzz}%{if-category}\033[32m %{category}:%{endif} "
                       "%{if-debug}\033[34m%{function}%{endif}"
                       "%{if-warning}\033[31m%}%{endif}"
                       "%{if-critical}\033[31m%%{endif}"
                       "%{if-fatal}\033[31m%%{endif}"
                       "\033[0m %{message}");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);
    app.installTranslator(&qtTranslator);


    pc::PCMainUI w(&app);
    QDesktopWidget dw;
    QSize sizeWindow(1500,1100);
    QRect rectScreen = dw.availableGeometry();
    if(rectScreen.width() > 0){
        if(rectScreen.width() < sizeWindow.width()){
            sizeWindow.setWidth(rectScreen.width());
        }
        if(rectScreen.height() < sizeWindow.height()){
            sizeWindow.setHeight(rectScreen.height());
        }
    }

    w.resize(sizeWindow);
    w.show();

    return app.exec();
}

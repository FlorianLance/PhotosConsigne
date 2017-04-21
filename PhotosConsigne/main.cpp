/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file main.cpp
 * \brief entry point
 * \author Florian Lance
 * \date 01/11/15
 */


// Qt
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDesktopWidget>

// Photos consigne
//#include "MainInterface.h"
#include "MainUI.h"
#include "PCMainUI.hpp"


int main(int argc,char** argv)
{
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);
    app.installTranslator(&qtTranslator);

    pc::PCMainUI w(&app);

    QDesktopWidget dw;
    QSize sizeWindow(1500,900);
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

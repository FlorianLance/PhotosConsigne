

#include <QApplication>
#include "mainInterface.h"

#include <QTranslator>
#include <QLibraryInfo>

int main(int argc,char** argv)
{
    //1 : Get the language setting of the system
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication a(argc, argv);

    //2 : Installation of language setting
    a.installTranslator(&qtTranslator);

    MainInterface w;
    w.show();

    return a.exec();
}

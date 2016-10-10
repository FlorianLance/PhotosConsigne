#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T10:18:52
#
#-------------------------------------------------

TARGET = launch
TEMPLATE = app

#include(./config/config.pri)
include(../PhotosConsigne/config/config.pri)

####################################### CONFIG
CONFIG -= console
CONFIG -= app_bundle
CONFIG += qt
QT += core gui widgets printsupport

####################################### INCLUDES
INCLUDEPATH += $$PHOTOSCONSIGNE_INCLUDES   # PHOTOSCONSIGNE

INCLUDEPATH += "include/UI" \
               "include/Workers" \

####################################### LIBRAIRIES
# ...

####################################### PROJECT FILES

SOURCES += \
    main.cpp \
    src/MainInterface.cpp \
    src/InterfaceWorker.cpp \
    src/ImageLabel.cpp \
    src/MainUI.cpp


HEADERS += \
    include/InterfaceWorker.h \
    include/MainInterface.h \
    include/ImageLabel.h \
    include/Utility.h \
    include/UI/LeftMenuUI.h \
    include/UI/MainUI.h \
    include/UI/PhotosExplorerUI.h \
    include/UI/PreviewUI.h \
    include/Workers/LeftMenuWorker.h \
    include/Workers/PreviewWorker.h


FORMS += \
    ui/LeftMenuUI.ui \
    ui/PhotosExplorerUI.ui \
    ui/PreviewUI.ui \
    ui/MainUI.ui

DISTFILES += \
    deploiement/PhotosConsigne_x64.iss \
    deploiement/PhotosConsigne_x86.iss \
    config/config.pri \
    config/home.pri \
    config/laptopHome.pri \
    config/laptopWork.pri \
    config/work.pri

RESOURCES += \
    src/resources.qrc


equals(ARCH, "x86"){
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}else{
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
}




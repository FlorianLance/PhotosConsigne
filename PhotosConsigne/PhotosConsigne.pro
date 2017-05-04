#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T10:18:52
#
#-------------------------------------------------

TARGET = PhotosConsigne
TEMPLATE = app

#include(./config/config.pri)
include(../PhotosConsigne/config/config.pri)

####################################### CONFIG
CONFIG -= console
CONFIG -= app_bundle
#CONFIG -= opengl
CONFIG += qt
QT += core gui widgets printsupport concurrent

####################################### INCLUDES
INCLUDEPATH += $$PHOTOSCONSIGNE_INCLUDES   # PHOTOSCONSIGNE

INCLUDEPATH += "include/UI" \
               "include/Workers" \

####################################### LIBRAIRIES
# ...

####################################### PROJECT FILES

SOURCES += \
    main.cpp \
    src/UI/PCMainUI.cpp \
    src/UI/RichTextUI.cpp \
    src/UI/ImageLabel.cpp \
    src/Utility.cpp \
    src/UI/UIElements.cpp

HEADERS += \
    include/Utility.h \
    include/UI/PCMainUI.hpp \
    include/UI/ImageLabel.hpp \
    include/UI/RichTextUI.hpp \
    include/Workers/PCMainWorker.hpp \
    include/thirdparty/asyncfuture/asyncfuture.h \
    include/Workers/ImageReader.hpp \
    include/UI/UIElements.hpp

FORMS += \
    ui/MainUI.ui \
    ui/PhotosConsigneMainW.ui \
    ui/IndividualConsign.ui \
    ui/Support.ui \
    ui/Help.ui \
    ui/IndividualPage.ui

DISTFILES += \
    deploiement/PhotosConsigne_x64.iss \
    deploiement/PhotosConsigne_x86.iss \
    config/config.pri \
    config/home.pri \
    config/laptopHome.pri \
    config/laptopWork.pri \
    config/work.pri \
    myapp.rc

RESOURCES += \
    resources.qrc


equals(ARCH, "x86"){
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}else{
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
}

RC_FILE = myapp.rc


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
CONFIG += c++14
CONFIG -= console
CONFIG -= app_bundle
#CONFIG -= opengl
CONFIG += qt
QT += core gui widgets printsupport concurrent

####################################### INCLUDES
INCLUDEPATH += $$PHOTOSCONSIGNE_INCLUDES   # PHOTOSCONSIGNE

INCLUDEPATH += "include/UI" \
               "include/Workers" \
               "include/Widgets" \
               "include/Data" \

####################################### LIBRAIRIES
# ...

####################################### PROJECT FILES

SOURCES += \
    main.cpp \
    src/UI/PCMainUI.cpp \
    src/Utility.cpp \
    src/UI/UIElements.cpp \
    src/Workers/PDFGeneratorWorker.cpp \
    src/Workers/PhotoLoaderWorker.cpp \
    src/Widgets/PreviewW.cpp \
    src/Widgets/PhotoW.cpp \
    src/Widgets/CustomPageW.cpp \
    src/Widgets/PageW.cpp \
    src/Data/Photo.cpp \
    src/Widgets/SettingsW.cpp \
    src/Widgets/RichTextEditW.cpp \
    src/Data/DocumentElements.cpp \
    src/Data/PaperFormat.cpp \
    src/Widgets/PageSetsW.cpp

HEADERS += \
    include/UI/PCMainUI.hpp \
#    include/thirdparty/asyncfuture/asyncfuture.h \
#    include/Workers/ImageReader.hpp \
    include/UI/UIElements.hpp \
    include/Workers/PDFGeneratorWorker.hpp \
    include/Utility.hpp \
    include/Workers/PhotoLoaderWorker.hpp \
    include/Widgets/PreviewW.hpp \
    include/Widgets/PhotoW.hpp \
    include/Widgets/CustomPageW.hpp \
    include/Widgets/BackgroundW.hpp \
    include/Widgets/BordersW.hpp \
    include/Widgets/MarginsW.hpp \
    include/Widgets/PageW.hpp \
    include/Widgets/SettingsW.hpp \
    include/Data/Photo.hpp \
    include/Data/RectPageItem.hpp \
    include/Widgets/SetStyleW.hpp \
    include/Widgets/RichTextEditW.hpp \
    include/Widgets/FooterW.hpp \
    include/Widgets/HeaderW.hpp \
    include/Widgets/SetW.hpp \
    include/Widgets/SectionStyleW.hpp \
    include/Data/Settings.hpp \
    include/Data/DocumentElements.hpp \
    include/Data/PaperFormat.hpp \
    include/Widgets/PageSetsW.hpp \
    include/Widgets/RightSettingsW.hpp \
    include/DebugMessage.hpp \
    include/Widgets/MiscW.hpp \
    include/Widgets/DegradedW.hpp \
    include/Widgets/ImagePositionW.hpp \
    include/Data/Work.hpp

FORMS += \
    ui/PhotosConsigneMainW.ui \
    ui/Support.ui \
    ui/Help.ui \
    ui/InsertLink.ui \
    ui/Background.ui \
    ui/Borders.ui \
    ui/Margins.ui \
    ui/Page.ui \
    ui/SetStyle.ui \
    ui/Set.ui \
    ui/Header.ui \
    ui/SectionStyle.ui \
    ui/Footer.ui \
    ui/PageSets.ui \
    ui/RightSettings.ui \
    ui/MiscPage.ui \
    ui/MiscSet.ui \
    ui/Degraded.ui \
    ui/ImagePosition.ui

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


#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T10:18:52
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = launch
TEMPLATE = app

CONFIG+= warn_on
CONFIG += gui

QMAKE_CXXFLAGS_RELEASE += /O2 /openmp # Od

DEFINES += _CRT_SECURE_NO_WARNINGS


############# CFG
CFG = ""
CONFIG(debug, debug|release){
    CFG = "Debug"
}else{
    CFG = "Release"
}

############# ARCH
ARCH = ""
win32-msvc*:contains(QMAKE_TARGET.arch, x86_64):{
    ARCH = "x64"
}
win32-msvc*:contains(QMAKE_TARGET.arch, x86):{
    ARCH = "x86"
}

############# INCLUDEPATH
# hbp
INCLUDEPATH += "./include"
#HBP_DIR = "D:/_projects/HBP/HBP-flo/qt-creator/"
#HBP_BASE_DIR = $$HBP_DIR"hbp_base/"
#INCLUDEPATH += $$HBP_BASE_DIR"include"
#INCLUDEPATH += $$HBP_BASE_DIR"include/niftilib"
## boost
#INCLUDEPATH += "D:/_software/boost_1_57_0/boost_1_57_0"
## opencv
#INCLUDEPATH += "D:/_software/opencv/build/include"


############# LIBS
# opencv
#CV_BASE = "D:/_software/opencv/build"
#CV = "opencv_"
#CV_VER = "2411"
#equals(ARCH, "x86"){
#    CV_BASE = $$CV_BASE"/x86/vc12"
#}else{
#    CV_BASE = $$CV_BASE"/x64/vc12"
#}
#CV_BIN = $$CV_BASE"/bin"
#CV_LIB = $$CV_BASE"/lib"
#CV_EXT = ""
#equals(CFG, "Debug"){
#    CV_EXT = "d"
#}
#LIBS += -L$$CV_BIN -L$$CV_LIB -l$$CV"imgproc"$$CV_VER$$CV_EXT -l$$CV"core"$$CV_VER$$CV_EXT -l$$CV"highgui"$$CV_VER$$CV_EXT
# hbp
#OBJ_D = $$HBP_DIR"hbp_base_"
#equals(ARCH, "x86"){
#    OBJ_D = $$OBJ_D"x86/"
#}else{
#    OBJ_D = $$OBJ_D"x64/"
#}
#equals(CFG, "Debug"){
#    OBJ_D = $$OBJ_D"debug/"
#}else{
#    OBJ_D = $$OBJ_D"release/"
#}

message("Build : "$$ARCH" "$$CFG)
#D:\_software\GnuWin32\lib



SOURCES += \
    main.cpp \
    src/MainInterface.cpp \
    src/InterfaceWorker.cpp \
    src/ImageLabel.cpp




HEADERS += \
    include/InterfaceWorker.h \
    include/MainInterface.h \
    include/ImageLabel.h



#include/demo/grid_sheet.h



FORMS += \
    ui/mainUI.ui

DISTFILES += \
    deploiement/PhotosConsigne_x64.iss \
    deploiement/PhotosConsigne_x86.iss

RESOURCES += \
    src/resources.qrc





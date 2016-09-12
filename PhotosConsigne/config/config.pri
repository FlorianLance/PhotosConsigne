
####################################### DEFINE COMPUTER
COMPUTER = "home" # laptop_home / work / laptop_work / home

####################################### CONFIG
CONFIG += warn_on
CONFIG += c++14

####################################### OS
COMPILER = "" # vs / gcc / ...
ARCH = "" # x86 / x64
OS = "" # Windows / Linux / MACOS
EXT_O = "" # .obj / .o

win32:{
    OS = "Windows"
    COMPILER = "vs"
    EXT_O = ".obj"
}

macx {
    OS = "MACOS"
    COMPILER = "..." # TODO
    EXT_O = "..." # TODO
}

unix:!macx{
    OS = "Linux"
    COMPILER = "gcc"
    EXT_O = ".o"
}

####################################### ARCH
equals(COMPILER, "gcc"){
    ARCH = "x64"  # TODO

    # FLAGS
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS +=  -fopenmp
    QMAKE_CXXFLAGS_RELEASE += -O3
}
equals(COMPILER, "vs"){
    win32-msvc*:contains(QMAKE_TARGET.arch, x86_64):{
        ARCH = "x64"
    }
    win32-msvc*:contains(QMAKE_TARGET.arch, x86):{
        ARCH = "x86"
    }

    # FLAGS
    QMAKE_CXXFLAGS_RELEASE += /O2 /openmp
}

####################################### CFG
CFG = "" # Debug Release
CONFIG(debug, debug|release){
    CFG = "Debug"
}else{
    CFG = "Release"
}


####################################### GLOBAL FLAGS

equals(COMPILER, "gcc"){
    # WARNINGS
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-result -Wno-unused-function -Wfatal-errors -Werror
}
equals(COMPILER, "vs"){
    # DEFINES
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

####################################### INCLUDE/LIBRARY

PHOTOCONSIGNE = ""
SOFTWARE_DIR = ""

BOOST_LIBS = ""
OPENCV_LIBS = ""
PCL_LIBS = ""
GEOTOOLS_LIBS = ""

################### CHOOSE COMPUTER SETTINGS

equals(COMPUTER, "work"){
    include(work.pri)
}
equals(COMPUTER, "home"){
    include(home.pri)
}
equals(COMPUTER, "laptop_work"){
    include(laptopWork.pri)
}
equals(COMPUTER, "laptop_home"){
    include(laptopHome.pri)
}

####################################### INCLUDEPATH LOCAL

message($$PHOTOSCONSIGNE_BASE_DIR)
PHOTOSCONSIGNE_SRC_DIR = $$PHOTOSCONSIGNE_BASE_DIR"/PhotosConsigne"
message("after")
message($$PHOTOSCONSIGNE_SRC_DIR)

####################################### HBP BASE INCLUDES

PHOTOSCONSIGNE_INCLUDES = $$PHOTOSCONSIGNE_SRC_DIR/include

####################################### HBP BASE LIB

PHOTOSCONSIGNE_DEBUG = $$PHOTOSCONSIGNE_BASE_DIR"/PhotosConsigne_"
PHOTOSCONSIGNE_RELEASE = $$PHOTOSCONSIGNE_BASE_DIR"/PhotosConsigne_"
PHOTOSCONSIGNE_OBJ =""

equals(ARCH, "x86"){
    PHOTOSCONSIGNE_DEBUG   = $$PHOTOSCONSIGNE_DEBUG"x86/PhotosConsigne/debug"
    PHOTOSCONSIGNE_RELEASE = $$PHOTOSCONSIGNE_RELEASE"x86/PhotosConsigne/release"
}
equals(ARCH, "x64"){
    PHOTOSCONSIGNE_DEBUG   = $$PHOTOSCONSIGNE_DEBUG"x64/PhotosConsigne/debug"
    PHOTOSCONSIGNE_RELEASE = $$PHOTOSCONSIGNE_RELEASE"x64/PhotosConsigne/release"
}

equals(CFG, "Debug"){
    PHOTOSCONSIGNE_OBJ = $$PHOTOSCONSIGNE_DEBUG
}
equals(CFG, "Release"){
    PHOTOSCONSIGNE_OBJ = $$PHOTOSCONSIGNE_RELEASE
}

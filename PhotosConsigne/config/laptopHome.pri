
equals(COMPILER, "vs"){ # TODO
    # ...
}
equals(COMPILER, "gcc"){ # TODO

    PHOTOSCONSIGNE_BASE_DIR = "/home/ulpaso/..."
    SOFTWARE_DIR = "/home/ulpaso"

    ####################################### BOOST
    INCLUDEPATH += $$SOFTWARE_DIR"/boost_1_61_0"

    ####################################### GEO TOOLSs
    INCLUDEPATH += $$SOFTWARE_DIR"/GeometricTools/GTEngine/Include"

    ####################################### OPENCV
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv-2.4.13/modules/highgui/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv-2.4.13/modules/core/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv-2.4.13/modules/imgproc/include"
    CV_BASE = "/home/ulpaso/opencv-2.4.13/release"
    CV_BIN = $$CV_BASE"/bin"
    CV_LIB = $$CV_BASE"/lib"
    OPENCV_LIBS = -L$$CV_BIN -L$$CV_LIB -lopencv_highgui -lopencv_core -lopencv_imgproc
}


equals(COMPILER, "vs"){
    PHOTOSCONSIGNE_BASE_DIR = "G:/_projects/PhotosConsigne"
    SOFTWARE_DIR = "G:/Programmes"

    ####################################### BOOST
    INCLUDEPATH += $$SOFTWARE_DIR"/boost_1_61_0"

    ####################################### GEO TOOLS
    INCLUDEPATH += $$SOFTWARE_DIR"/GeometricTools/GTEngine/Include"

    ####################################### OPENCV
    CV_DIR = $$SOFTWARE_DIR"/opencv/build-vs14-x64"
    INCLUDEPATH += $$CV_DIR
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/core/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/highgui/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/imgproc/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/imgcodecs/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/video/include"
    INCLUDEPATH += $$SOFTWARE_DIR"/opencv/modules/videoio/include"
    CV = "opencv_"
    CV_VER = "310"

    CV_BIN = $$CV_DIR"/bin"
    CV_LIB = $$CV_DIR"/lib"
    CV_EXT = ""
    equals(CFG, "Debug"){ # debug
        CV_BIN = $$CV_BIN"/Debug/"
        CV_LIB = $$CV_LIB"/Debug/"
        CV_EXT = "d"
    }else{ # release
        CV_BIN = $$CV_BIN"/Release/"
        CV_LIB = $$CV_LIB"/Release/"
    }

    OPENCV_LIBS = -L$$CV_BIN -L$$CV_LIB -l$$CV"imgproc"$$CV_VER$$CV_EXT -l$$CV"core"$$CV_VER$$CV_EXT \
                                        -l$$CV"highgui"$$CV_VER$$CV_EXT -l$$CV"imgcodecs"$$CV_VER$$CV_EXT \
                                        -l$$CV"features2d"$$CV_VER$$CV_EXT -l$$CV"calib3d"$$CV_VER$$CV_EXT \
                                        -l$$CV"photo"$$CV_VER$$CV_EXT -l$$CV"video"$$CV_VER$$CV_EXT \
}
equals(COMPILER, "gcc"){
    # ...
}

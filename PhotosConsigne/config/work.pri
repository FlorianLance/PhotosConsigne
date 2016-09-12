
equals(COMPILER, "vs"){

    PHOTOSCONSIGNE_DIR = "G:/_projects/PhotosConsigne/PhotosConsigne"

    SOFTWARE_DIR = "D:/_software"

    PCL_DIR = $$SOFTWARE_DIR"/PCL 1.7.2-x64"
    CV_DIR = $$SOFTWARE_DIR"/opencv-2413-vs2015"
    GEO_DIR = $$SOFTWARE_DIR"/GeometricTools"

    ####################################### GEO TOOLS
    INCLUDEPATH += $$GEO_DIR"/GTEngine/Include"
    GEO_VER = "v140"
    GEO_TOOLS_BASE = $$GEO_DIR"/GTEngine/_Output/"$$GEO_VER

    equals(ARCH, "x86"){ # x86
        GEO_TOOLS_BASE = $$GEO_TOOLS_BASE"/x86/"$$CFG
    }else{ # x64
        GEO_TOOLS_BASE = $$GEO_TOOLS_BASE"/x64/"$$CFG
    }

    GEOTOOLS_LIBS = -L$$GEO_TOOLS_BASE -l"GTEngine.v14"

    ####################################### PCL
    INCLUDEPATH += $$PCL_DIR"/include/pcl-1.7"
    PCL_CFG = "";
    equals(CFG, "Debug"){ # debug
        PCL_CFG = "debug"
    }else{ # release
        PCL_CFG = "release"
    }

    PCL_LIBS += -L$$PCL_DIR"bin/" -L$$PCL_DIR"lib/" -l"pcl_common_"$$PCL_CFG -l"pcl_features_"$$PCL_CFG -l"pcl_search_"$$PCL_CFG -l"pcl_registration_"$$PCL_CFG \
                                              -l"pcl_io_"$$PCL_CFG -l"pcl_io_ply_"$$PCL_CFG \

    ####################################### EIGEN
    INCLUDEPATH += $$PCL_DIR"/3rdParty/Eigen/eigen3"

    ####################################### FLANN
    INCLUDEPATH += $$PCL_DIR"/3rdParty/FLANN/include"

    ####################################### BOOST
    INCLUDEPATH += $$SOFTWARE_DIR"/boost_1_57_0/boost_1_57_0"
    BOOST_BASE = $$PCL_DIR"/3rdParty/Boost/"
    BOOST_LIBS = -L$$BOOST_BASE"lib/" -l"libboost_thread-vc140-mt-1_57"


    ####################################### OPENCV
    INCLUDEPATH += $$CV_DIR"/build/include"
    CV = "opencv_"
    CV_VER = "2413"

    equals(ARCH, "x86"){ # x86
        CV_DIR = $$CV_DIR"/build2-x86"
    }else{ # x64
        CV_DIR = $$CV_DIR"/build2"
    }

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

    OPENCV_LIBS = -L$$CV_BIN -L$$CV_LIB -l$$CV"imgproc"$$CV_VER$$CV_EXT -l$$CV"core"$$CV_VER$$CV_EXT -l$$CV"highgui"$$CV_VER$$CV_EXT
}
equals(COMPILER, "gcc"){
    # ...
}

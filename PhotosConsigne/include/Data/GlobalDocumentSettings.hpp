
#pragma once


// local
#include "Settings.hpp"


namespace pc {


struct GlobalDocumentSettings{

    virtual ~GlobalDocumentSettings(){
        DebugMessage("~GlobalDocumentSettings");
    }

    bool grayScale           = false;
    bool saveOnlyCurrentPage = false;
    bool noPreviewGeneration = false;

    // only global
    PaperFormat     paperFormat;

    // set
    // # text
    std::shared_ptr<QString> consignText = nullptr;
    // # style
    int photoAlignment;
    qreal PCRatio;
    Position consignPositionFromPhotos;
    PhotoAdjust photoAdust = PhotoAdjust::adjust;

    // margins
    MarginsSettings marginsSettings;

    // border
    BordersSettings bordersSettings;

    // backgound
    BackGroundSettings backgroundSettings;

    // page sets
    PageSetsSettings setsSettings;

    // header
    HeaderSettings headerSettings;

    // footer
    FooterSettings footerSettings;

    // page
    int nbPages;                        /**< number of pages for the document */
    int previousPageId      = -2;
    int currentPageId       = 0;    /**< id of the current selected page (page list widget) */
    int currentPageIdDisplayed = -1;

    // sets
    int currentSetId        = 0;    /**< id of the current set (Photo + consign) */
    int currentSetIdDisplayed  = -1;

    // photos
    int previousPhotoId     = 0;
    int currentPhotoId      = 0;    /**< id of the current selcted photo (photo list widget) */
    QString lastPhotosDirectoryPath= "";
    SPhotos photosLoaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    SPhotos photosValided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
};

}

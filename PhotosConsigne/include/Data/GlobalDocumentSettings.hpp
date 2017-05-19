
#pragma once


// local
#include "Settings.hpp"


namespace pc {


struct GlobalDocumentSettings{

    bool resetPages          = false;   /**< if true, the number of pages will be reseted and initialized in function of the number of valid photos and H/V parameters */

    bool grayScale           = false;
    bool saveOnlyCurrentPage = false;
    bool noPreviewGeneration = false;

    // only global
    PageOrientation orientation;

    // set
    // # text
    std::shared_ptr<QString> consignText = nullptr;
    // # style
    int photoAlignment;
    qreal PCRatio;
    Position consignPositionFromPhotos;
    PhotoAdjust photoAdust = PhotoAdjust::adjust;

    // margins
    RatioMargins marginsSettings;

    // border
    BordersSettings bordersSettings;

    // backgound
    BackGroundSettings backgroundSettings;

    // header
    HeaderSettings headerSettings;

    // footer
    FooterSettings footerSettings;

    // page
    int nbPages;                        /**< number of pages for the document */
    int previousPageId      = 0;
    int currentPageId       = 0;    /**< id of the current selected page (page list widget) */
    int currentPageIdDisplayed = -1;

    // sets
    int currentSetId        = 0;    /**< id of the current set (Photo + consign) */
    int currentSetIdDisplayed  = -1;

    // photos
    int nbPhotosPageWidth;                  /**< size of the grid horizontally */
    int nbPhotosPageHeight;                  /**< size of the grid vertically */
    int nbPhotosPerPage;                /**< number of photos per page (< H*V) */
    int previousPhotoId     = 0;
    int currentPhotoId      = 0;    /**< id of the current selcted photo (photo list widget) */
    QString lastPhotosDirectoryPath= "";
    SPhotos photosLoaded  = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
    SPhotos photosValided = std::make_shared<QList<SPhoto>>(QList<SPhoto>());
};

}

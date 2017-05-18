
#pragma once

/**
 * \file UIElements.hpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Utility.hpp"
// # ui
#include "PageW.hpp"
#include "BackgroundW.hpp"
#include "MarginsW.hpp"
#include "BordersW.hpp"
#include "SetStyleW.hpp"
#include "IndividualSetW.hpp"
// # widgets
#include "PreviewW.hpp"


// # generated ui
#include "ui_PhotosConsigneMainW.h"
#include "ui_Support.h"
#include "ui_Help.h"


namespace Ui {
    class PhotosConsigneMainW;
    class SupportW;
    class HelpW;
}

namespace pc{

struct ImageResource{

    bool hasBeenAdded = false;
    QUrl url;
    QImage image;
};

struct TitleUI{

    TitleUI();

    std::shared_ptr<QString>        html = nullptr;
    std::shared_ptr<RichTextEdit>   richTextEdit = std::make_shared<RichTextEdit>();
};


class UIElements : public QObject{

    Q_OBJECT

public :


    UIElements(QWidget *parent, std::shared_ptr<Ui::PhotosConsigneMainW> mainUI);

    ~UIElements();

    // consign related
    void insert_individual_set(int index);
    void remove_individual_set(int index);
    void reset_individual_sets(int nbPhotos);

    // pages related
    void update_individual_pages(const GlobalData &settings);

    // members
    QTimer zonesTimer;

    std::unique_ptr<QWidget> wSupport   = nullptr;
    std::unique_ptr<QWidget> wHelp      = nullptr;

    PhotoW   *selectedPhotoW        = nullptr;
    PreviewW *previewW              = nullptr;

    TitleUI         titleUI;

    RichTextUI      globalConsignUI;


    BackgroundW     globalBackgroundW;
    BordersW        globalBordersW;
    MarginsW        globalMarginsW;
    SetStyleW       globalSetStyleW;

    QList<SPageUI>         individualPageUI;
    QList<SIndividualSetW> individualSetsLoadedUI;
    QList<SIndividualSetW> individuaSetsValidedUI;

public slots:

    void ask_for_update(bool displayZones){
        if(displayZones){
            zonesTimer.start(1000);
        }
        emit update_settings_signal();
    }


private:

    std::shared_ptr<Ui::PhotosConsigneMainW> m_mUI = nullptr;
    QWidget *m_parent = nullptr;

signals:

    void update_settings_signal();
    void set_progress_bar_state_signal(int state);
    void set_progress_bar_text_signal(QString text);
    void send_page_color_signal(QColor col);
    void resource_added_signal(QUrl url, QImage image);

};
}

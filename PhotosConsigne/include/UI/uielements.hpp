
#pragma once

/**
 * \file UIElements.hpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Utility.hpp"
#include "DocumentElements.hpp"
// # widgets
#include "PreviewW.hpp"
#include "RightSettingsW.hpp"
// # generated ui
#include "ui_PhotosConsigneMainW.h"
#include "ui_Support.h"
#include "ui_Help.h"


namespace Ui {
    class PhotosConsigneMainUI;
    class SupportUI;
    class HelpUI;
}

namespace pc{

struct ImageResource{

    bool hasBeenAdded = false;
    QUrl url;
    QImage image;
};


class UIElements : public QObject{

    Q_OBJECT

public :

    UIElements(QMainWindow *parent);

    QTimer zonesTimer;  /**< timer for displaying the zones in the preview */

    std::unique_ptr<QWidget> supportW   = nullptr;  /**< support window */
    std::unique_ptr<QWidget> helpW      = nullptr;  /**< help window */

    RightSettingsW settingsW;           /**< right settings widgets */
    PhotoW   photoW;                    /**< photo widget */
    PreviewW previewW;                  /**< preview widget */

    Ui::PhotosConsigneMainUI mainUI;    /**< ui of the main window */

public slots:

    void ask_for_update(bool displayZones);

    void display_donate_window();
    void display_help_window();

    void display_photo_panel();
    void display_preview_panel();

    void set_ui_state_for_adding_photos(bool state = true);
    void set_ui_state_for_generating_pdf(bool state = true);

    void update_global_settings(GlobalSettings &settings) const;

    void update_photos_list(const GlobalSettings &settings);
    void update_pages_list(const PCPages &pcPages);

    void display_current_individual_set_ui(const GlobalSettings &settings);
    void display_current_individual_page_ui(const GlobalSettings &settings);

    void update_UI(const GlobalSettings &settings);

private:

    QMainWindow *m_parent = nullptr;

signals:

    void settings_updated_signal();
    void resource_added_signal(QUrl url, QImage image);

    void set_progress_bar_state_signal(int state);
    void set_progress_bar_text_signal(QString text);
};
}

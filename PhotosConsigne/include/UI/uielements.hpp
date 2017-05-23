
#pragma once

/**
 * \file UIElements.hpp
 * \brief defines UIElements
 * \author Florian Lance
 * \date 04/04/2017
 */

// local
#include "Utility.hpp"
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

    PhotoW   photoW;                    /**< photo widget */
    PreviewW previewW;                  /**< preview widget */
    RightSettingsW settingsW;           /**< right settings widgets */

    Ui::PhotosConsigneMainUI mainUI;    /**< ui of the main window */

public slots:

    void ask_for_update(bool displayZones);

    void display_donate_window();

    void display_help_window();

    void update_global_settings(GlobalDocumentSettings &settings) const;

private:

    QMainWindow *m_parent = nullptr;

signals:

    void settings_updated_signal();
    void resource_added_signal(QUrl url, QImage image);

    void set_progress_bar_state_signal(int state);
    void set_progress_bar_text_signal(QString text);
};
}

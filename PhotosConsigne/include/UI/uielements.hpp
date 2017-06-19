
/*******************************************************************************
** PhotosConsigne                                                             **
** MIT License                                                                **
** Copyright (c) [2016] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

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


    void write_to_xml(QXmlStreamWriter &xml) const;
    void load_from_xml(QXmlStreamReader &xml, bool firstPart);

    void display_donate_window();
    void display_help_window();

    void display_photo_panel();
    void display_preview_panel();

    void set_ui_state_for_adding_photos(bool state = true);
    void set_ui_state_for_generating_pdf(bool state = true);
    void set_ui_state_for_loading_work(bool state = true);

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

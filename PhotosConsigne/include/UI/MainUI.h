
/*******************************************************************************
**                                                                            **
**  PhotosConsigne                                                            **
**  An interface for generating pdf containing images sequences with a common **
**  text. Intended to be used by teachers for making printed documents        **
**  containing child work from infant school.                                 **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
********************************************************************************/

/**
 * \file MainUI.h
 * \brief defines MainUI
 * \author Florian Lance
 * \date 19/09/2016
 */


#pragma once

// Qt
#include <QMainWindow>

// ui
#include "ui_mainUI.h"

// Photos consigne
#include "LeftMenuUI.h"
#include "PreviewUI.h"
#include "PhotosExplorerUI.h"


namespace Ui {
class MainInterface;
}

namespace pc {


/**
 * @brief ...
 */
class MainUI : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief MainUI constructor
     * @param [in,out] parent
     */
    MainUI(QApplication *parent = 0);

    /**
     * @brief MainUI destructor
     */
    ~MainUI();


public slots:   


private slots:

    void openOnlineDocumentation();
    void openDonatePage();
    void openAboutWindow();


signals :


private:


    Ui::MainUI *m_ui = nullptr;                     /**< main interface*/
    PreviewUI *m_previewUI = nullptr;               /**< preview windows interface */
    LeftMenuUI *m_leftMenuUI = nullptr;             /**< left menu interface */
    PhotosExplorerUI *m_photosExplorerUI = nullptr; /**< photos explorer interface */

    int m_currentImageDisplayed = 0;

    SPDFSettings m_settings = SPDFSettings(new PDFSettings());
    SImages m_images = SImages(new QList<SImage>);
};
}

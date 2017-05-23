
/**
 * \file PCMainUI.hpp
 * \brief defines PCMainUI
 * \author Florian Lance
 * \date 04/04/2017
 */

#pragma once

// Qt
#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <QTimer>

// # workers
#include "PhotoLoaderWorker.hpp"
#include "PDFGeneratorWorker.hpp"
#include "UIElements.hpp"


namespace pc {

/**
 * @brief ...
 */
class PCMainUI : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief PCMainUI constructor
     * @param [in,out] parent
     */
    PCMainUI(QApplication *parent = 0);

    /**
     * @brief PCMainUI destructor
     */
    ~PCMainUI();


public slots:

    // events
    void closeEvent(QCloseEvent *event);
    void keyPressEvent( QKeyEvent * event );

    // actions
    void add_photos_directory();
    void load_new_photos();
    void remove_all_photos();
    void insert_transparent_space();

    // selection
    void define_selected_page(int index);
    void select_photo(int index, bool displayPhotoPanel = true);
    void define_selected_page_from_current_photo();
    void define_selected_set_from_current_photo();

    // ui
    // # update lists
    void update_photos_list();
    void update_pages_list();
    // # display pannels
    // ## tab middle panels
    void display_photo_panel();
    void display_preview_panel();
    // # dynamic ui display
    void display_current_individual_set_ui();
    void display_current_individual_page_ui();
    // # photo
    void update_photo_to_display(SPhoto photo);

    // data
    // # generate sets/pages from ui
    void build_valid_sets();        
    void build_pages();

    // preview
    void update_settings_with_no_preview();
    void update_settings();

private :

    // state
    void set_ui_state_for_adding_photos(bool state = true);
    void set_ui_state_for_generating_pdf(bool state = true);

    // conections
    void from_main_UI_connections();
    void from_main_module_connections();
    void from_pdf_generator_worker_connections();
    void from_photos_loader_worker_connections();
    void from_UI_elements_connections();

signals :

    void init_document_signal();    
    void start_loading_photos_signal(QStringList photosPath, int startIdToInsert);
    void stop_loading_photos_signal();
    void start_preview_generation_signal(PCPages pcPages, int idPageToDraw, bool drawZones);
    void start_PDF_generation_signal(PCPages pcPages);
    void kill_signal();

private:

    bool m_isLoadingPhotos      = false;
    bool m_isPreviewComputing   = false;
    bool m_generatePreviewAgain = false;
    QReadWriteLock m_previewLocker;    
    QString m_version;

    PCPages m_pcPages;                  /**< document pages to be drawn */
    GlobalDocumentSettings m_settings;  /**< global parameters of the document */

    // ui
    UIElements m_ui; /**< dynamic ui elements */

    // threads
    std::unique_ptr<PhotoLoaderWorker> m_loadPhotoWorker = nullptr;
    std::unique_ptr<PDFGeneratorWorker> m_pdfGeneratorWorker = nullptr;

    // workers
    QThread m_displayPhotoWorkerThread;
    QThread m_pdfGeneratorWorkerThread;
};
}


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

    void add_photos_directory();
    void insert_photos(SPhotos photos);
    void insert_transparent_space();
    void insert_new_photo();
    void remove_all_photos();
    void define_selected_photo(int index);
    void update_valid_photos();
    void update_photo_to_display(SPhoto photo);

    void update_pages();
    void define_selected_page(int index);
    void define_selected_page_from_current_photo();
    void define_current_individual_page_ui();

    void swap_loaded_pc(int currentIndex, int newIndex);
    void define_selected_pc(int index);
    void define_selected_pc_from_current_photo();

    void update_settings_with_no_preview();
    void update_settings();

    void display_photo_panel();
    void display_preview_panel();

    void display_title_panel();
    void display_global_consign_panel();
    void display_individual_consign_settings_panel();
    void display_individual_page_settings_panel();


private :

    // ### state
    void enable_ui();
    void update_photos_list_style();
    void update_pages_list_style();

    // ### windows
    void display_donate_window();

    // ### conections    
    void from_main_UI_connections();
    void from_main_module_connections();
    void from_pdf_generator_worker_connections();
    void from_photos_loader_worker_connections();
    void from_UI_elements_connections();


signals :

    void init_document_signal();
    void kill_signal();
    void send_photos_dir_signal(QString photosDir, QStringList photosNames);
    void start_preview_generation_signal(GlobalData settings, PCPages pcPages);
    void start_PDF_generation_signal(GlobalData settings, PCPages pcPages);

private:

    bool m_isPreviewComputing   = false;
    bool m_generatePreviewAgain = false;
    bool m_isLoadingPhotos      = false;
    QReadWriteLock m_previewLocker;    
    QString m_version;

    PCPages m_pcPages;            /**< document pages to be drawn */
    GlobalData m_settings;  /**< global parameters of the document */

    // ui
    std::shared_ptr<Ui::PhotosConsigneMainW>  m_mainUI = nullptr; /**< main widget */
    std::unique_ptr<UIElements>               m_dynUI  = nullptr; /**< dynamic ui elements */

    // threads
    std::unique_ptr<PhotoLoaderWorker> m_loadPhotoWorker = nullptr;
    std::unique_ptr<PDFGeneratorWorker> m_pdfGeneratorWorker = nullptr;

    // workers
    QThread m_displayPhotoWorkerThread;
    QThread m_pdfGeneratorWorkerThread;
};
}

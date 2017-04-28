
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
#include "PCMainWorker.hpp"


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





//    void insert_pc(int index);

//    void remove_pc(int index);


    void update_pages();



    void set_photos_directory();
    void define_selected_photo(int index, bool showPhotoDisplayTab = true);
    void update_valid_photos();
    void update_photo_to_display(SPhoto photo);

    void swap_loaded_pc(int currentIndex, int newIndex);

    void update_settings_with_no_preview();
    void update_settings();


private :

    // UI
    // ### initialization
    void init_misc_elements();
    void init_images_labels_widgets();    
    void init_rich_text_edit_widgets();

    // ### state
    void enable_ui();

    // ### windows
    void display_donate_window();

    // ### conections
    void define_workers_connections();
    void define_main_UI_connections();

signals :

    void kill_signal();

    void send_photos_dir_signal(QString photosDir, QStringList photosNames);

    void start_preview_generation_signal(QReadWriteLock *docLocker, GlobalData settings, PCPages pcPages);

    void start_PDF_generation_signal(QReadWriteLock *docLocker, GlobalData settings, PCPages pcPages);

private:

    bool m_isPreviewComputing   = false;
    bool m_generatePreviewAgain = false;
    QReadWriteLock m_previewLocker;    
    QString m_version;

    PCPages m_pcPages;            /**< document pages to be drawn */
    GlobalData m_settings;  /**< global parameters of the document */

    // ui
    std::unique_ptr<UIElements> m_dynUI  = nullptr; /**< dynamic ui elements */
    Ui::PhotosConsigneMainW    *m_mainUI = nullptr; /**< main widget */

    // threads
    std::unique_ptr<PhotoDisplayWorker> m_displayPhotoWorker = nullptr;
    std::unique_ptr<PDFGeneratorWorker> m_pdfGeneratorWorker = nullptr;

    // workers
    QThread m_displayPhotoWorkerThread;
    QThread m_pdfGeneratorWorkerThread;    
};
}

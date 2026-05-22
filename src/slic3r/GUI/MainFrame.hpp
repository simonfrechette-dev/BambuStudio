#ifndef slic3r_MainFrame_hpp_
#define slic3r_MainFrame_hpp_

#include "libslic3r/PrintConfig.hpp"

#include <boost/property_tree/ptree_fwd.hpp>

#include <QString>
#include <QStringList>
#include <QLabel>
#include <QStatusBar>
// Phase 4 TODO: macOS taskbar icon via Qt

#include <string>
#include <map>

#include "GUI_Utils.hpp"
#include "Event.hpp"
//BBS: GUI refactor
#include "ParamsPanel.hpp"
#include "Monitor.hpp"
#include "Auxiliary.hpp"
#include "Project.hpp"
#include "UnsavedChangesDialog.hpp"
#include "Widgets/SideButton.hpp"
#include "Widgets/SideMenuPopup.hpp"

// BBS

#define ENABEL_PRINT_ALL 0

class Notebook;
class QTabWidget;
class QProgressDialog;
class BBLTopbar;

// Forward declarations for classes in namespace Slic3r::GUI
namespace Slic3r { namespace GUI {
    class CalibrationPanel;
    class MaxVolumetricSpeed_Test_Dlg;
    class VFA_Test_Dlg;
    class Retraction_Test_Dlg;
    class SecondaryCheckDialog;
    class WebViewPanel;
    class PrinterWebView;
    class PA_Calibration_Dlg;
    class Temp_Calibration_Dlg;
    class MultiMachinePage;
} }

namespace Slic3r {

namespace GUI
{

class Tab;
class PrintHostQueueDialog;
class Plater;
class MainFrame;
class ParamsDialog;
class FilamentGroupPopup;
class DeviceWebPage;
class SimpleGLView;

enum QuickSlice
{
    qsUndef = 0,
    qsReslice = 1,
    qsSaveAs = 2,
    qsExportSVG = 4,
    qsExportPNG = 8
};

struct PresetTab {
    std::string       name;
    Tab*              panel;
    PrinterTechnology technology;
};

// ----------------------------------------------------------------------------
// SettingsDialog
// ----------------------------------------------------------------------------

class SettingsDialog : public DPIDialog//DPIDialog
{
    //QTabWidget* m_tabpanel { nullptr };
    Notebook* m_tabpanel{ nullptr };
    MainFrame*      m_main_frame { nullptr };
    QMenuBar*      m_menubar{ nullptr };
public:
    SettingsDialog(MainFrame* mainframe);
    ~SettingsDialog() = default;
    //void set_tabpanel(QTabWidget* tabpanel) { m_tabpanel = tabpanel; }
    void set_tabpanel(Notebook* tabpanel) { m_tabpanel = tabpanel; }
    QMenuBar* menubar() { return m_menubar; }

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;
};

class MainFrame : public DPIFrame
{
#ifdef __APPLE__
    bool     m_mac_fullscreen{false};
#endif
    bool     m_loaded {false};
    QTimer* m_reset_title_text_colour_timer{ nullptr };
    QTimer* m_idle_topbar_timer{ nullptr };  // polls can_undo/can_redo/can_save

    QString    m_qs_last_input_file = QString();
    QString    m_qs_last_output_file = QString();
    QString    m_last_config = QString();

    QMenuBar*  m_menubar{ nullptr };
    //QMenu* publishMenu{ nullptr };
    QMenu *    m_calib_menu{nullptr};
    QMenu *    m_recent_menu{nullptr};  // "Open Recent" submenu
    bool        enable_multi_machine{ false };

#if 0
    QAction* m_menu_item_repeat { nullptr }; // doesn't used now
#endif
    QAction* m_menu_item_reslice_now { nullptr };
    QLayout*    m_main_sizer{ nullptr };

    size_t      m_last_selected_tab;

    std::string     get_base_name(const QString &full_name, const char *extension = nullptr) const;
    std::string     get_dir_name(const QString &full_name) const;

    void on_presets_changed(QEvent&);
    void on_value_changed(QEvent&);

    bool can_start_new_project() const;
    bool can_open_project() const;
    bool can_add_models() const;
    bool can_export_model() const;
    bool can_export_toolpaths() const;
    bool can_export_supports() const;
    bool can_export_gcode() const;
    bool can_export_all_gcode() const;
    bool can_print_3mf() const;
    bool can_send_gcode() const;
    //bool can_export_gcode_sd() const;
    //bool can_eject() const;
    bool can_slice() const;
    bool can_change_view() const;
    bool can_toggle_camera_fullscreen() const;
    void toggle_camera_fullscreen();
    bool can_select() const;
    bool can_deselect() const;
    bool can_clone() const;
    bool can_delete() const;
    bool can_delete_all() const;
    bool can_reslice() const;

    // BBS
    QBoxLayout* create_side_tools();

    // MenuBar items changeable in respect to printer technology
    enum MenuItems
    {                   //   FFF                  SLA
        miExport = 0,   // Export G-code        Export
        miSend,         // Send G-code          Send to print
        miMaterialTab,  // Filament Settings    Material Settings
        miPrinterTab,   // Different bitmap for Printer Settings
    };

    // vector of a MenuBar items changeable in respect to printer technology
    std::vector<QAction*> m_changeable_menu_items;

    struct FileHistory
    {
        FileHistory(int max = 9) : m_max(max) {}
        std::wstring GetThumbnailUrl(int index) const;

        virtual void AddFileToHistory(const QString &file);
        virtual void RemoveFileFromHistory(size_t i);
        size_t FindFileInHistory(const QString &file);

        void LoadThumbnails();

        void SetMaxFiles(int max);

        // Returns the list of recently opened files (most recent first)
        const std::deque<QString>& files() const { return m_files; }
        void load(const QString& settings_key);
        void save(const QString& settings_key) const;
    private:
        int    m_max{9};
        std::deque<QString> m_files;
        std::deque<std::string> m_thumbnails;
        bool m_load_called = false;
    };

    FileHistory m_recent_projects;

    enum class ESettingsLayout
    {
        //BBS GUI refactor: remove unused layout
        Unknown,
        Old,
        //New,
        //Dlg,
        GCodeViewer
    };

    ESettingsLayout m_layout{ ESettingsLayout::Unknown };

    enum SliceSelectType
    {
        eSliceAll = 0,
        eSlicePlate = 1,
    };

    //jump to editor under preview only mode
    bool preview_only_to_editor = false;

protected:
    virtual void on_dpi_changed(const QRect &suggested_rect) override;
    virtual void on_sys_color_changed() override;

#ifdef __WIN32__
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
#endif

public:
    MainFrame();
    ~MainFrame() = default;
    void toggle_settings_panel();  // Show/hide the parameters sidebar
#ifdef __APPLE__
    bool get_mac_full_screen() { return m_mac_fullscreen; }
#endif
    DeviceWebPage* web_device() const { return m_web_device; }
    //BBS GUI refactor
    enum TabPosition
    {
        tpHome          = 0,
        tp3DEditor      = 1,
        tpPreview       = 2,
        tpMonitor       = 3,
        tpMultiDevice   = 4,
        tpProject       = 5,
        tpCalibration      = 6,
        tpAuxiliary        = 7,
        toDebugTool        = 8,
        tpFilamentManager  = 9,
        tpWebDevice        = 10,
    };

    //BBS: add slice&&print status update logic
    enum SlicePrintEventType
    {
        eEventObjectUpdate = 0,
        eEventPlateUpdate = 1,
        eEventParamUpdate = 2,
        eEventSliceUpdate = 3,
        eEventPrintUpdate = 4
    };

    // BBS GUI refactor
    enum PrintSelectType {
        ePrintAll            = 0,
        ePrintPlate          = 1,
        eExportSlicedFile    = 2,
        eExportGcode         = 3,
        eSendGcode           = 4,
        eSendToPrinter       = 5,
        eSendToPrinterAll    = 6,
        eUploadGcode         = 7,
        eExportAllSlicedFile = 8,
        ePrintMultiMachine   = 9,
        eSendMultiApp        = 10
    };

    void update_layout();

	// Called when closing the application and when switching the application language.
	void 		shutdown();

    Plater*     plater() { return m_plater; }

    // BBS
    BBLTopbar* topbar() { return m_topbar; }

    // for cali to update tab when save new preset
    void update_filament_tab_ui();

    void        update_title();
    void        set_max_recent_count(int max);

    void        show_calibration_button(bool show, bool is_BBL);

	void        update_title_colour_after_set_title();
    void        show_option(bool show);
    void        init_tabpanel();
    void        create_preset_tabs();
    //BBS: GUI refactor
    void        add_created_tab(Tab* panel, const std::string& bmp_name = "");
    bool        is_active_and_shown_tab(QWidget* panel);

    // Register Win32 RawInput callbacks (3DConnexion) and removable media insert / remove callbacks.
    // Called from wxEVT_ACTIVATE, as wxEVT_CREATE was not reliable (bug in wxWidgets?).
    void        register_win32_callbacks();
    void        init_menubar_as_editor();
    bool        check_bbl_farm_client_installed();
    void        init_menubar_as_gcodeviewer();
    void        update_menubar();
    void        update_calibration_button_status();
    // Open item in menu by menu and item name (in actual language)
    void        open_menubar_item(const QString& menu_name,const QString& item_name);
#ifdef _WIN32
    void        show_tabs_menu(bool show);
#endif
    //BBS
    void        show_log_window();

    void        update_ui_from_settings();
    //BBS
    void        show_sync_dialog();
    void        update_side_preset_ui();
    void        on_select_default_preset(QEvent& evt);

    bool        is_loaded() const { return m_loaded; }
    bool        is_last_input_file() const  { return !m_qs_last_input_file.isEmpty(); }
    //BBS GUI refactor: remove unused layout new/dlg
    //bool        is_dlg_layout() const { return m_layout == ESettingsLayout::Dlg; }

    void        reslice_now();
    void        export_config();
    // Query user for the config file and open it.
    void        load_config_file();
    // Open a config file. Return true if loaded.
    bool        load_config_file(const std::string &path);

    //BBS: export current config bundle as BBL default reference
    //void        export_current_configbundle();
    //BBS: export all the system preset configs to seperate files
    //void        export_system_configs();
    //void        export_configbundle(bool export_physical_printers = false);
    //void        load_configbundle(QString file = QString());
    void        load_config(const DynamicPrintConfig& config);
    //BBS: jump to monitor
    void        jump_to_monitor(std::string dev_id = "");
    void        jump_to_multipage();
    //BBS: hint when jump to 3Deditor under preview only mode
    bool        preview_only_hint();
    // Select tab in m_tabpanel
    // When tab == -1, will be selected last selected tab
    //BBS: GUI refactor
    void        select_tab(QWidget* panel);
    void        select_tab(size_t tab = size_t(-1));
    void        request_select_tab(TabPosition pos);
    int         get_calibration_curr_tab();
    void        select_view(const std::string& direction);
    void        view_zoom_to_fit() const;
    // Propagate changed configuration from the Tab to the Plater and save changes to the AppConfig
    void        on_config_changed(DynamicPrintConfig* cfg) const ;
    void        set_print_button_to_default(PrintSelectType select_type);

    bool can_save() const;
    bool can_save_as() const;
    //BBS
    bool can_upload() const;
    void save_project();
    bool save_project_as(const QString& filename = QString());

    void        add_to_recent_projects(const QString& filename);
    void        update_recent_menu();  // Rebuild the "Open Recent" submenu
    void        set_status_message(const QString& msg);  // Update status bar text
    // Update the estimates panel on the Preview tab after a successful slice
    void        update_estimates(const QString& time, const QString& filament,
                                 const QString& weight, const QString& layers);
    // Set layer slider range + Z-height data after slicing
    void        set_layer_data(QVector<float> layer_zs);
    void        set_status_objects(int count);  // Update object count in status bar
    void        get_recent_projects(boost::property_tree::wptree &tree, int images);
    void        open_recent_project(size_t file_id, QString const & filename);
    void        remove_recent_project(size_t file_id, QString const &filename);

    void        technology_changed();


    //BBS
    void        load_url(QString url);
    void        load_printer_url(QString url);
    void        load_printer_url();
    bool        is_printer_view() const;
    void        refresh_plugin_tips();
    void RunScript(QString js);
    void RunScriptLeft(QString js);
    void show_device(bool bBBLPrinter);

    // OrcaSlicer calibration
    PA_Calibration_Dlg *         m_pa_calib_dlg{nullptr};
    Temp_Calibration_Dlg *       m_temp_calib_dlg{nullptr};
    MaxVolumetricSpeed_Test_Dlg *m_vol_test_dlg{nullptr};
    VFA_Test_Dlg *               m_vfa_test_dlg{nullptr};
    Retraction_Test_Dlg *        m_retraction_calib_dlg{nullptr};
    SecondaryCheckDialog*        m_confirm_download_plugin_dlg{ nullptr };

    // BBS. Replace title bar and menu bar with top bar.
    BBLTopbar*            m_topbar{ nullptr };
    PrintHostQueueDialog* printhost_queue_dlg() { return m_printhost_queue_dlg; }
    Plater*               m_plater { nullptr };
    //BBS: GUI refactor
    MonitorPanel*         m_monitor{ nullptr };

    AuxiliaryPanel*       m_auxiliary{ nullptr };
    MultiMachinePage*     m_multi_machine{ nullptr };
    ProjectPanel*         m_project{ nullptr };

    CalibrationPanel*     m_calibration{ nullptr };
    DeviceWebPage*        m_web_device{ nullptr };
    WebViewPanel*         m_webview { nullptr };
    PrinterWebView*       m_printer_view{nullptr};
    // wxLogWindow → not ported
    void*          m_log_window { nullptr };
    // BBS
    //wxBookCtrlBase*       m_tabpanel { nullptr };
    Notebook*             m_tabpanel{ nullptr };
    QBoxLayout*           m_side_tools{ nullptr };
    ParamsPanel*          m_param_panel{ nullptr };
    ParamsDialog*         m_param_dialog{ nullptr };
    // Status bar labels
    QLabel*               m_status_label{ nullptr };   // main status message
    QLabel*               m_status_objects{ nullptr }; // object count
    SimpleGLView*         m_preview_gl{ nullptr };     // GL view on Preview tab
    QSlider*              m_layer_slider{ nullptr };   // Z cut-plane slider on Preview tab
    QLabel*               m_layer_z_label{ nullptr };  // current Z height label on Preview tab
    // Preview estimates panel value labels (set by create_preview_panel, updated after slice)
    QLabel*               m_lbl_est_time    { nullptr };
    QLabel*               m_lbl_est_filament{ nullptr };
    QLabel*               m_lbl_est_weight  { nullptr };
    QLabel*               m_lbl_est_layers  { nullptr };
    // Z heights for each layer — populated after a successful slice
    QVector<float>        m_layer_zs;
    //BBS
    SettingsDialog        m_settings_dialog;
    DiffPresetDialog      diff_dialog;
    QWidget*             m_plater_page{ nullptr };
    PrintHostQueueDialog* m_printhost_queue_dlg;

    // BBS
    mutable int m_print_select{ ePrintAll };
    mutable int m_slice_select{ eSliceAll };
    SideButton* m_slice_btn{ nullptr };
    SideButton* m_slice_option_btn{ nullptr };
    SideButton* m_print_btn{ nullptr };
    SideButton* m_print_option_btn{ nullptr };

    int expand_program_id = 1000;
    int expand_helio_id = 1001;

    QLabel* split_line_icon{nullptr};
    ExpandButtonHolder* expand_program_holder{nullptr};

    SidePopup*  m_slice_option_pop_up{ nullptr };

    FilamentGroupPopup* m_filament_group_popup{ nullptr };
    mutable bool          m_slice_enable{ true };
    mutable bool          m_print_enable{ true };
    bool get_enable_slice_status();
    bool get_enable_print_status();
    //BBS
    void update_side_button_style();
    void update_slice_print_status(SlicePrintEventType event, bool can_slice = true, bool can_print = true);

    int select_device_page_count{ 0 };

#ifdef __APPLE__
    std::unique_ptr<QSystemTrayIcon> m_taskbar_icon;
#endif // __APPLE__

#ifdef _WIN32
    void*				m_hDeviceNotify { nullptr };
    uint32_t  			m_ulSHChangeNotifyRegister { 0 };
	static constexpr int WM_USER_MEDIACHANGED { 0x7FFF }; // WM_USER from 0x0400 to 0x7FFF, picking the last one to not interfere with wxWidgets allocation
    bool                m_is_in_move_or_resize { false };
    ULONGLONG           m_last_resize_layout_ms { 0 };
#endif // _WIN32
};

// Qt event: wxDECLARE_EVENT(EVT_HTTP_ERROR, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_USER_LOGIN, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_USER_LOGIN_HANDLE, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_CHECK_PRIVACY_VER, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_CHECK_PRIVACY_SHOW, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_SHOW_IP_DIALOG, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_UPDATE_MACHINE_LIST, QEvent)
// Qt event: wxDECLARE_EVENT(EVT_UPDATE_PRESET_CB, QEvent)



} // GUI
} //Slic3r

#endif // slic3r_MainFrame_hpp_

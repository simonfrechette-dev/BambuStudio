#ifndef slic3r_GUI_SendToSDcard_hpp_
#define slic3r_GUI_SendToSDcard_hpp_
#include <QWidget>
#include "Widgets/RadioBox.hpp"
#include <QString>


#include "SelectMachine.hpp"
#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "DeviceManager.hpp"
#include "Plater.hpp"
#include "BBLStatusBar.hpp"
#include "BBLStatusBarSend.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/ScrolledWindow.hpp"
#include "Widgets/AnimaController.hpp"


namespace Slic3r {
class FileTransferTunnel;
class FileTransferJob;

namespace GUI {

class SendToPrinterDialog : public DPIDialog
{
private:
    void init_bind();
    void init_timer();

    int                                 m_print_plate_idx;
    int                                 m_current_filament_id;
    int                                 m_print_error_code = 0;
    int                                 timeout_count = 0;
    int                                 m_connect_try_times = 0;
    bool                                m_is_in_sending_mode{ false };
    bool                                m_is_rename_mode{ false };
    bool                                enable_prepare_mode{ true };
    bool                                m_need_adaptation_screen{ false };
    bool                                m_export_3mf_cancel{ false };
    bool                                m_is_canceled{ false };
    bool                                m_tcp_try_connect{true};
    bool                                m_tutk_try_connect{false};
    bool                                m_ftp_try_connect{false};
    std::string                         m_print_error_msg;
    std::string                         m_print_error_extra;
    std::string                         m_print_info;
    std::string                         m_printer_last_select;
    std::string                         m_device_select;
    QString                            m_current_project_name;

    TextInput*                          m_rename_input{ nullptr };
    QStackedWidget*                       m_rename_switch_panel{ nullptr };
    Plater*                             m_plater{ nullptr };
    QLabel*                     m_staticbitmap{ nullptr };
    ThumbnailPanel*                     m_thumbnailPanel{ nullptr };
    ComboBox*                           m_comboBox_printer{ nullptr };
    ComboBox*                           m_comboBox_bed{ nullptr };
    Button*                             m_rename_button{ nullptr };
    Button*                             m_button_refresh{ nullptr };
    Button*                             m_button_ensure{ nullptr };
    QWidget*                            m_scrollable_region;
    QWidget*                            m_line_schedule{ nullptr };
    QWidget*                            m_panel_sending{ nullptr };
    QWidget*                            m_panel_prepare{ nullptr };
    QWidget*                            m_panel_finish{ nullptr };
    QWidget*                            m_line_top{ nullptr };
    QWidget*                            m_panel_image{ nullptr };
    QWidget*                            m_rename_normal_panel{ nullptr };
    QWidget*                            m_line_materia{ nullptr };
    QBoxLayout*                         m_storage_sizer{ nullptr };
    QWidget*                            m_storage_panel{ nullptr };
    QWidget *                           m_connecting_panel{nullptr};
    QStackedWidget*                       m_simplebook{ nullptr };
    QLabel*                       m_statictext_finish{ nullptr };
    QLabel*                       m_stext_sending{ nullptr };
    QLabel*                       m_staticText_bed_title{ nullptr };
    QLabel*                       m_statictext_printer_msg{ nullptr };
    QLabel *                      m_connecting_printer_msg{nullptr};
    QLabel*                       m_stext_printer_title{ nullptr };
    QLabel*                       m_rename_text{ nullptr };
    QLabel*                       m_stext_time{ nullptr };
    QLabel*                       m_stext_weight{ nullptr };
    Label*                              m_st_txt_error_code{ nullptr };
    Label*                              m_st_txt_error_desc{ nullptr };
    Label*                              m_st_txt_extra_info{ nullptr };
    QLabel*                    m_link_network_state{ nullptr };
    StateColor                          btn_bg_enable;
    QBoxLayout*                         rename_sizer_v{ nullptr };
    QBoxLayout*                         rename_sizer_h{ nullptr };
    QBoxLayout*                         sizer_thumbnail;
    QBoxLayout*                         m_sizer_scrollable_region;
    QBoxLayout*                         m_sizer_main;
    QLabel*                       m_file_name;
    PrintDialogStatus                   m_print_status{ PrintStatusInit };
    AnimaIcon *                         m_animaicon{nullptr};

    std::shared_ptr<SendJob>            m_send_job{nullptr};
    std::vector<QString>               m_bedtype_list;
    std::map<std::string, ::CheckBox*>  m_checkbox_list;
    std::vector<MachineObject*>         m_list;
    QColor                            m_colour_def_color{ QColor(255, 255, 255) };
    QColor                            m_colour_bold_color{ QColor(38, 46, 48) };
    QTimer*                            m_refresh_timer{ nullptr };
    std::unique_ptr<QTimer>            m_task_timer{ nullptr };
    std::unique_ptr<QTimer>            m_url_timer{nullptr};
    std::shared_ptr<BBLStatusBarSend>   m_status_bar;
    QScrollArea                   *m_sw_print_failed_info{nullptr};
    std::shared_ptr<int>                m_token = std::make_shared<int>(0);
    std::vector<RadioBox *>             m_storage_radioBox;
    std::string                         m_selected_storage;
    bool                                m_if_has_sdcard;
    bool                                m_waiting_support{ false };
    bool                                m_waiting_enable{ false };
    std::vector<std::string>            m_ability_list;

public:
    enum {
        SUCCESS                  = 0,
        CONTINUE                 = 1,
        ERROR_JSON               = 2,
        ERROR_PIPE               = 3,
        ERROR_CANCEL             = 4,
        ERROR_RES_BUSY           = 5,
        ERROR_TIME_OUT           = 6,
        FILE_NO_EXIST            = 10,
        FILE_NAME_INVALID        = 11,
        FILE_SIZE_ERR            = 12,
        FILE_OPEN_ERR            = 13,
        FILE_READ_WRITE_ERR      = 14,
        FILE_CHECK_ERR           = 15,
        FILE_TYPE_ERR            = 16,
        STORAGE_UNAVAILABLE      = 17,
        API_VERSION_UNSUPPORT    = 18,
        FILE_EXIST               = 19,
        STORAGE_SPACE_NOT_ENOUGH = 20,
        FILE_CREATE_ERR          = 21,
        FILE_WRITE_ERR           = 22,
        MD5_COMPARE_ERR          = 23,
        FILE_RENAME_ERR          = 24,
        SEND_ERR                 = 25,
};

private:
    enum ConnectionStatus { NOT_START, CONNECTING, CONNECTED, CONNECTION_FAILED, DISCONNECTED };
    ConnectionStatus m_connection_status{ConnectionStatus::NOT_START};

    std::unique_ptr<FileTransferTunnel> m_filetransfer_tunnel;
    std::unique_ptr<FileTransferJob>    m_filetransfer_mediability_job;
    std::unique_ptr<FileTransferJob>    m_filetransfer_uploadfile_job;
    QDateTime                          m_last_refresh_time;

public:
    SendToPrinterDialog(Plater *plater = nullptr);
    ~SendToPrinterDialog();

    bool Show(bool show);
    bool is_timeout();
    void on_rename_click(QEvent& event);
    void on_rename_enter();
    void stripWhiteSpace(std::string& str);
    void prepare_mode();
    void sending_mode();
    void reset_timeout();
    void update_user_printer();
    void update_show_status();
    bool is_blocking_printing(MachineObject* obj_);
    void prepare(int print_plate_idx);
    void check_focus(QWidget* window);
    void check_fcous_state(QWidget* window);
    void update_priner_status_msg(QString msg, bool is_warning = false);
    void update_print_status_msg(QString msg, bool is_warning = false, bool is_printer = true);
    void update_printer_combobox(QEvent& event);
    void on_cancel(QCloseEvent& event);
    void on_ok(QEvent& event);
    void clear_ip_address_config(QEvent& e);
    void on_refresh(QEvent& event);
    void on_print_job_cancel(QEvent& evt);
    void set_default();
    void on_timer(QTimerEvent& event);
    void on_selection_changed(QEvent& event);
    void Enable_Refresh_Button(bool en);
    void show_status(PrintDialogStatus status, std::vector<QString> params = std::vector<QString>());
    void Enable_Send_Button(bool en);
    void on_dpi_changed(const QRect& suggested_rect) override;
    void update_user_machine_list();
    void show_print_failed_info(bool show, int code = 0, QString description = QString(), QString extra = QString());
    void update_print_error_info(int code, std::string msg, std::string extra);
    void on_change_color_mode() { GUI::wxGetApp().UpdateDlgDarkUI(this); }
    void update_storage_list(const std::vector<std::string>& storages);
    std::string get_storage_selected();

    QString format_text(QString& m_msg);
    std::vector<std::string> sort_string(std::vector<std::string> strArray);
    void GetConnection();

private:
    void ResetConnectMethod();
    void ResetTunnelAndJob();
    void OnConnection(bool is_success, int error_code, std::string error_msg);
    void CreateMediaAbilityJob();
    void CreateUploadFileJob(const std::string &path, const std::string &name);
    void ChangeConnectMethod();
    void UploadFileProgressCallback(int progress);
    void UploadFileRessultCallback(int res, int resp_ec, std::string json_res, std::vector<std::byte> bin_res);
    void Reset();
};


}
}

#endif

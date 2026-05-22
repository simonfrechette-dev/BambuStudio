#ifndef slic3r_GUI_ReleaseNote_hpp_
#define slic3r_GUI_ReleaseNote_hpp_
#include <QWidget>
#include <QLabel>
#include <QString>
#include <QDialog>
#include <QCheckBox>
#include <QNetworkRequest>
#include <QTextBrowser>

#include <limits>


#include "AmsMappingPopup.hpp"
#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "DeviceManager.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/ScrolledWindow.hpp"


namespace Slic3r { namespace GUI {


class ReleaseNoteDialog : public DPIDialog
{
public:
    ReleaseNoteDialog(Plater *plater = nullptr);
    ~ReleaseNoteDialog();

    void on_dpi_changed(const QRect &suggested_rect) override;
    void update_release_note(QString release_note, std::string version);

    Label *    m_text_up_info{nullptr};
    QScrollArea *m_vebview_release_note {nullptr};
};

class UpdatePluginDialog : public DPIDialog
{
public:
    UpdatePluginDialog(QWidget* parent = nullptr);
    ~UpdatePluginDialog();

    void on_dpi_changed(const QRect& suggested_rect) override;
    void update_info(std::string json_path);

    Label* m_text_up_info{ nullptr };
    Label* operation_tips{ nullptr };
    QScrollArea* m_vebview_release_note{ nullptr };
};

class UpdateVersionDialog : public DPIDialog
{
public:
    UpdateVersionDialog(QWidget *parent = nullptr);
    ~UpdateVersionDialog();

    QWidget* CreateTipView(QWidget* parent);
    void OnLoaded(QEvent& event);
    void OnTitleChanged(QEvent& event);
    void OnError(QEvent& event);
    bool ShowReleaseNote(std::string content);
    void RunScript(std::string script);
    void on_dpi_changed(const QRect& suggested_rect) override;
    void update_version_info(QString release_note, QString version);
    std::vector<std::string> splitWithStl(std::string str, std::string pattern);

    QLabel*   m_brand{nullptr};
    Label *           m_text_up_info{nullptr};
    QWidget*        m_vebview_release_note{nullptr};
    QStackedWidget*     m_simplebook_release_note{nullptr};
    QScrollArea* m_scrollwindows_release_note{nullptr};
    QBoxLayout *      sizer_text_release_note{nullptr};
    Label *           m_staticText_release_note{nullptr};
    QLabel*   m_bitmap_open_in_browser;
    QLabel*  m_link_open_in_browser;
    Button*           m_button_skip_version;
    Button*           m_button_download;
    Button*           m_button_cancel;
    std::string       url_line;
};

class SecondaryCheckDialog : public DPIFrame
{
private:
    QWidget* event_parent { nullptr };
public:
    enum ButtonStyle {
        ONLY_CONFIRM        = 0,
        CONFIRM_AND_CANCEL  = 1,
        CONFIRM_AND_DONE    = 2,
        CONFIRM_AND_RETRY   = 3,
        CONFIRM_AND_RESUME  = 4,
        DONE_AND_RETRY      = 5,
        MAX_STYLE_NUM       = 6
    };
    SecondaryCheckDialog(
        QWidget* parent,
        int      id = -1,
        const QString& title = QString(),
        enum ButtonStyle btn_style = CONFIRM_AND_CANCEL,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0 | 0,
        bool not_show_again_check = false
    );
    void update_text(QString text);
    void on_show();
    void on_hide();
    void update_btn_label(QString ok_btn_text, QString cancel_btn_text);
    void update_title_style(QString title, SecondaryCheckDialog::ButtonStyle style, QWidget* parent = nullptr);
    void post_event(QEvent&& event);
    void rescale();
    ~SecondaryCheckDialog();
    void on_dpi_changed(const QRect& suggested_rect);
    void msw_rescale();


    StateColor btn_bg_green;
    StateColor btn_bg_white;
    Label* m_staticText_release_note {nullptr};
    QBoxLayout* m_sizer_main;
    QScrollArea *m_vebview_release_note {nullptr};
    Button* m_button_ok { nullptr };
    Button* m_button_retry { nullptr };
    Button* m_button_cancel { nullptr };
    Button* m_button_fn { nullptr };
    Button* m_button_resume { nullptr };
    QCheckBox* m_show_again_checkbox;
    ButtonStyle m_button_style;
    bool not_show_again = false;
    std::string show_again_config_text = "";
};

class PrintErrorDialog : public DPIFrame
{
private:
    QWidget* event_parent{ nullptr };
public:
    enum PrintErrorButton : int {
        RESUME_PRINTING = 2,
        RESUME_PRINTING_DEFECTS = 3,
        RESUME_PRINTING_PROBELM_SOLVED = 4,
        STOP_PRINTING = 5,
        CHECK_ASSISTANT = 6,
        FILAMENT_EXTRUDED = 7,
        RETRY_FILAMENT_EXTRUDED = 8,
        CONTINUE = 9,
        LOAD_VIRTUAL_TRAY = 10,
        OK_BUTTON = 11,
        FILAMENT_LOAD_RESUME = 12,
        JUMP_TO_LIVEVIEW,

        NO_REMINDER_NEXT_TIME = 23,
        IGNORE_NO_REMINDER_NEXT_TIME = 25,
        //LOAD_FILAMENT = 26*/
        IGNORE_RESUME = 27,
        PROBLEM_SOLVED_RESUME = 28,
        TURN_OFF_FIRE_ALARM = 29,

        RETRY_PROBLEM_SOLVED = 34,
        STOP_DRYING = 35,
        REMOVE_CLOSE_BTN = 39, // special case, do not show close button

        ERROR_BUTTON_COUNT
    };
    PrintErrorDialog(
        QWidget* parent,
        int      id = -1,
        const QString& title = QString(),
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0 | 0
    );
    void update_text_image(const QString& text, const QString& error_code,const QString& image_url);
    void on_show();
    void on_hide();
    void update_title_style(QString title, std::vector<int> style, QWidget* parent = nullptr);
    void post_event(QEvent& event);
    void post_event(QEvent&& event);
    void rescale();
    ~PrintErrorDialog();
    void on_dpi_changed(const QRect& suggested_rect);
    void msw_rescale();
    void init_button(PrintErrorButton style, QString buton_text);
    void init_button_list();
    void on_webrequest_state(QEvent& evt);

    StateColor btn_bg_white;
    QNetworkRequest web_request;
    QLabel* m_error_prompt_pic_static;
    Label* m_staticText_release_note{ nullptr };
    Label* m_staticText_error_code{ nullptr };
    QBoxLayout* m_sizer_main;
    QBoxLayout* m_sizer_button;
    QScrollArea* m_vebview_release_note{ nullptr };
    std::map<int, Button*> m_button_list;
    std::vector<int> m_used_button;
};

struct ConfirmBeforeSendInfo
{
public:
    enum InfoLevel {
        Normal = 0,
        Warning = 1
    };
    InfoLevel level;
    QString text;
    QString wiki_url;
    ConfirmBeforeSendInfo(const QString& txt, const QString& url = QString(), InfoLevel lev = Normal) : text(txt), wiki_url(url), level(lev){}
};

class ConfirmBeforeSendDialog : public DPIDialog
{
public:
    enum ButtonStyle {
        ONLY_CONFIRM = 0,
        CONFIRM_AND_CANCEL = 1,
        MAX_STYLE_NUM = 2
    };
    ConfirmBeforeSendDialog(
        QWidget* parent,
        int      id = -1,
        const QString& title = QString(),
        enum ButtonStyle btn_style = CONFIRM_AND_CANCEL,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0 | 0,
        bool not_show_again_check = false
    );
    void update_text(QString text);
    void update_text(std::vector<ConfirmBeforeSendInfo> texts, bool enable_warning_clr = true);
    void on_show();
    void on_hide();
    void update_btn_label(QString ok_btn_text, QString cancel_btn_text);
    void rescale();
    void on_dpi_changed(const QRect& suggested_rect);
    void show_update_nozzle_button(bool show = false);
    void hide_button_ok();
    void edit_cancel_button_txt(const QString& txt, bool switch_green = false);
    void disable_button_ok();
    void enable_button_ok();
    QString format_text(QString str, int warp);

    ~ConfirmBeforeSendDialog();

protected:
    QBoxLayout* m_sizer_main;
    QScrollArea* m_vebview_release_note{ nullptr };
    Label* m_staticText_release_note{ nullptr };
    Button* m_button_ok;
    Button* m_button_cancel;
    Button* m_button_update_nozzle;
    QCheckBox* m_show_again_checkbox;
    bool not_show_again = false;
    std::string show_again_config_text = "";
};

class InputIpAddressDialog : public DPIDialog
{
public:
    QString comfirm_before_check_text;
    QString comfirm_before_enter_text;
    QString comfirm_after_enter_text;
    QString comfirm_last_enter_text;

    std::shared_ptr<InputIpAddressDialog> token_;
    boost::thread* m_thread{nullptr};

    std::string m_ip;
    QWidget* m_step_icon_panel3{ nullptr };
    Label* m_tip0{ nullptr };
    Label* m_tip1{ nullptr };
    Label* m_tip2{ nullptr };
    Label* m_tip3{ nullptr };
    Label* m_tip4{ nullptr };
    InputIpAddressDialog(QWidget* parent = nullptr);
    ~InputIpAddressDialog();

    MachineObject* m_obj{nullptr};
    QWidget * ip_input_top_panel{ nullptr };
    QWidget * ip_input_bot_panel{ nullptr };
    Button* m_button_ok{ nullptr };
    Label* m_tips_ip{ nullptr };
    Label* m_tips_access_code{ nullptr };
    Label* m_tips_sn{nullptr};
    Label* m_tips_modelID{nullptr};
    Label* m_test_right_msg{ nullptr };
    Label* m_test_wrong_msg{ nullptr };
    TextInput* m_input_ip{ nullptr };
    TextInput* m_input_access_code{ nullptr };
    TextInput* m_input_sn{ nullptr };
    ComboBox*  m_input_modelID{ nullptr };
    QLabel* m_img_help{ nullptr };
    QLabel* m_img_step1{ nullptr };
    QLabel* m_img_step2{ nullptr };
    QLabel* m_img_step3{ nullptr };
    QLabel* m_trouble_shoot{ nullptr };
    QTimer* closeTimer{ nullptr };
    int     closeCount{3};
    bool   m_show_access_code{ false };
    bool   m_need_input_sn{true};
    int    m_result;
    int    current_input_index {0};
    std::shared_ptr<SendJob> m_send_job{nullptr};
    std::shared_ptr<BBLStatusBarSend> m_status_bar;
    std::map<std::string, std::string> m_models_map;// display_name -> model_id

    void switch_input_panel(int index);
    void on_cancel();
    void update_title(QString title);
    void set_machine_obj(MachineObject* obj);
    void update_test_msg(QString msg, bool connected);
    bool isIp(std::string ipstr);
    void check_ip_address_failed(int result);
    void on_check_ip_address_failed(QEvent& evt);
    void on_ok(QMouseEvent& evt);
    void on_send_retry();
    void update_test_msg_event(QEvent &evt);
    void post_update_test_msg(std::weak_ptr<InputIpAddressDialog> w, QString text, bool beconnect);
    void workerThreadFunc(std::string str_ip, std::string str_access_code, std::string sn, std::string model_id);
    void OnTimer(QTimerEvent& event);
    void on_text(QEvent& evt);
    void on_dpi_changed(const QRect& suggested_rect) override;
};

class SendFailedConfirm : public DPIDialog
{
public:
    SendFailedConfirm(QWidget *parent = nullptr);
    ~SendFailedConfirm(){};

    //void on_ok(QMouseEvent &evt);
    void on_dpi_changed(const QRect &suggested_rect) override;
};

class ExpandCenterDialog : public DPIDialog
{
public:
    ExpandCenterDialog(QWidget* parent = nullptr);
    ~ExpandCenterDialog() {};

    //void on_ok(QMouseEvent &evt);
    void on_dpi_changed(const QRect& suggested_rect) override;
    void on_open_expand(const QMouseEvent& evt);
    void on_uninstall(const QMouseEvent& evt);
    void report_consent_unstall();
};



}} // namespace Slic3r::GUI

#endif

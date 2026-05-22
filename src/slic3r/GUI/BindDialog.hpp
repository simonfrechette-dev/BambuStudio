#ifndef slic3r_BindDialog_hpp_
#define slic3r_BindDialog_hpp_
#include <QWidget>
#include <QString>

#include "I18N.hpp"

#include <curl/curl.h>
#include "wxExtensions.hpp"
// Forward-declare Plater instead of including wx-heavy Plater.hpp
namespace Slic3r { namespace GUI { class Plater; } }
#include "Widgets/StepCtrl.hpp"
#include "Widgets/ProgressDialog.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/ProgressBar.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Jobs/BindJob.hpp"
#include "BBLStatusBar.hpp"
#include "DeviceManager.hpp"
#include "BBLStatusBarBind.hpp"

#define BIND_DIALOG_GREY200 QColor(248, 248, 248)
#define BIND_DIALOG_GREY800 QColor(50, 58, 61)
#define BIND_DIALOG_GREY900 QColor(38, 46, 48)
#define BIND_DIALOG_BUTTON_SIZE QSize(FromDIP(68), FromDIP(24))
#define BIND_DIALOG_BUTTON_PANEL_SIZE QSize(FromDIP(450), FromDIP(30))
#define PING_CODE_LENGTH 6

namespace Slic3r { namespace GUI {

struct MemoryStruct
{
    char * memory;
    size_t read_pos;
    size_t size;
};

class PingCodeBindDialog : public DPIDialog
{
private:

    Label* m_status_text;
    QLabel* m_text_input_title;
    QLabel* m_link_show_ping_code_wiki;
    TextInput* m_text_input_single_code[PING_CODE_LENGTH];
    Button* m_button_bind;
    Button* m_button_cancel;
    Button* m_button_close;
    QStackedWidget* m_simplebook;
    QWidget* request_bind_panel;
    QWidget* binding_panel;

    QScrollArea* m_sw_bind_failed_info;
    Label* m_bind_failed_info;
    Label* m_st_txt_error_code{ nullptr };
    Label* m_st_txt_error_desc{ nullptr };
    Label* m_st_txt_extra_info{ nullptr };
    QLabel* m_link_network_state{ nullptr };
    QString        m_result_info;
    QString        m_result_extra;
    QString        m_ping_code_wiki;
    bool            m_show_error_info_state = true;

    int             m_result_code;
    std::shared_ptr<BBLStatusBarBind> m_status_bar;

public:
    PingCodeBindDialog(Plater* plater = nullptr);
    ~PingCodeBindDialog();

    void     on_key_input(QKeyEvent& evt);
    void     on_text_changed(QEvent& event);
    void     on_key_backspace(QKeyEvent& event);
    void     on_cancel(QEvent& event);
    void     on_bind_printer(QEvent& event);
    void     on_dpi_changed(const QRect& suggested_rect) override;
};

class BindMachineDialog : public DPIDialog
{
private:
    QWidget*      m_panel_agreement;
    QLabel * m_printer_name;
    QLabel * m_user_name;
    StaticBox *   m_panel_left;
    StaticBox *   m_panel_right;
    QLabel *m_status_text;
    QLabel* m_link_show_error;
    Button *      m_button_bind;
    Button *      m_button_cancel;
    QStackedWidget *m_simplebook;
    QLabel *m_avatar;
    QLabel *m_printer_img;
    QLabel *m_static_bitmap_show_error;
    QPixmap      m_bitmap_show_error_close;
    QPixmap      m_bitmap_show_error_open;
    QScrollArea* m_sw_bind_failed_info;
    Label*          m_bind_failed_info;
    Label*          m_st_txt_error_code{ nullptr };
    Label*          m_st_txt_error_desc{ nullptr };
    Label*          m_st_txt_extra_info{ nullptr };
    QLabel* m_link_network_state{ nullptr };
    QString        m_result_info;
    QString        m_result_extra;
    bool            m_show_error_info_state = true;
    bool            m_allow_privacy{false};
    bool            m_allow_notice{false};
    int             m_result_code;
    std::shared_ptr<int>     m_tocken;

    MachineObject *                   m_machine_info{nullptr};
    std::shared_ptr<BindJob>          m_bind_job;
    std::shared_ptr<BBLStatusBarBind> m_status_bar;

public:
    BindMachineDialog(Plater *plater = nullptr);
    ~BindMachineDialog();

    void     show_bind_failed_info(bool show, int code = 0, QString description = QString(), QString extra = QString());
    void     on_cancel(QEvent& event);
    void     on_bind_fail(QEvent &event);
    void     on_update_message(QEvent &event);
    void     on_bind_success(QEvent &event);
    void     on_bind_printer(QEvent &event);
    void     on_dpi_changed(const QRect &suggested_rect) override;
    void     update_machine_info(MachineObject *info);
    void     on_show(QShowEvent &event);
    void     on_close(QCloseEvent& event);
    void     on_destroy();
    QString get_print_error(QString str);
};

class UnBindMachineDialog : public DPIDialog
{
protected:
    QLabel *  m_printer_name;
    QLabel *  m_user_name;
    QLabel *m_status_text;
    Button *      m_button_unbind;
    Button *      m_button_cancel;
    MachineObject *m_machine_info{nullptr};
    QLabel *m_avatar;
    QLabel *m_printer_img;
    std::shared_ptr<int>     m_tocken;

public:
    UnBindMachineDialog(Plater *plater = nullptr);
    ~UnBindMachineDialog();

    void on_cancel(QEvent &event);
    void on_unbind_printer(QEvent &event);
    void on_dpi_changed(const QRect &suggested_rect) override;
    void update_machine_info(MachineObject *info) { m_machine_info = info; };
    void on_show(QShowEvent &event);
};

}} // namespace Slic3r::GUI

#endif /* slic3r_BindDialog_hpp_ */

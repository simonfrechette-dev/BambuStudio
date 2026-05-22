#ifndef slic3r_GUI_SelectMachinePop_hpp_
#define slic3r_GUI_SelectMachinePop_hpp_
#include <QWidget>
#include <QString>


#include "ReleaseNote.hpp"
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
#include "Widgets/PopupWindow.hpp"

namespace Slic3r { namespace GUI {

enum PrinterState {
    OFFLINE,
    IDLE,
    BUSY,
    LOCK,
    IN_LAN
};

enum PrinterBindState {
    NONE,
    ALLOW_BIND,
    ALLOW_UNBIND
};


#define SELECT_MACHINE_POPUP_SIZE QSize(FromDIP(216), FromDIP(364))
#define SELECT_MACHINE_LIST_SIZE QSize(FromDIP(212), FromDIP(360))
#define SELECT_MACHINE_ITEM_SIZE QSize(FromDIP(190), FromDIP(35))
#define SELECT_MACHINE_GREY900 QColor(38, 46, 48)
#define SELECT_MACHINE_GREY600 QColor(144, 144, 144)
#define SELECT_MACHINE_GREY400 QColor(206, 206, 206)
#define SELECT_MACHINE_BRAND QColor(0, 174, 66)
#define SELECT_MACHINE_REMIND QColor(255, 111, 0)
#define SELECT_MACHINE_LIGHT_GREEN QColor(219, 253, 231)

class MachineObjectPanel : public QWidget
{
private:
    bool        m_is_my_devices {false};
    bool        m_show_edit{false};
    bool        m_show_bind{false};
    bool        m_hover {false};
    bool        m_is_macos_special_version{false};


    PrinterBindState   m_bind_state;
    PrinterState       m_state;

    ScalableBitmap m_unbind_img;
    ScalableBitmap m_edit_name_img;
    ScalableBitmap m_select_unbind_img;

    ScalableBitmap m_printer_status_offline;
    ScalableBitmap m_printer_status_busy;
    ScalableBitmap m_printer_status_idle;
    ScalableBitmap m_printer_status_lock;
    ScalableBitmap m_printer_in_lan;

    MachineObject *m_info;

protected:
    QLabel *m_bitmap_info;
    QLabel *m_bitmap_bind;

public:
    MachineObjectPanel(QWidget *      parent,
                       int      id    = -1,
                       const QPoint & pos   = QPoint(),
                       const QSize &  size  = QSize(),
                       long            style = 0,
                       const QString &name  = QString());

    ~MachineObjectPanel();

    void show_bind_dialog();
    void set_printer_state(PrinterState state);
    void show_printer_bind(bool show, PrinterBindState state);
    void show_edit_printer_name(bool show);
    void update_machine_info(MachineObject *info, bool is_my_devices = false);
protected:
    void OnPaint(QPaintEvent &event);
    void render(QPainter &dc);
    void doRender(QPainter &dc);
    void on_mouse_enter(QMouseEvent &evt);
    void on_mouse_leave(QMouseEvent &evt);
    void on_mouse_left_up(QMouseEvent &evt);
};

class MachinePanel
{
public:
    QString mIndex;
    MachineObjectPanel *mPanel;
};

class PinCodePanel : public QWidget
{
public:
    PinCodePanel(QWidget* parent,
        int type,
        int      winid = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize());
    ~PinCodePanel() {};

    ScalableBitmap       m_bitmap;
    bool           m_hover{false};
    int            m_type{0};

    void OnPaint(QPaintEvent& event);
    void render(QPainter& dc);
    void doRender(QPainter& dc);

    void on_mouse_enter(QMouseEvent& evt);
    void on_mouse_leave(QMouseEvent& evt);
    void on_mouse_left_up(QMouseEvent& evt);
};

class SelectMachinePopup : public PopupWindow
{
public:
    SelectMachinePopup(QWidget *parent);
    ~SelectMachinePopup();

    // PopupWindow virtual methods are all overridden to log them
    virtual void Popup(QWidget *focus = NULL) override;
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);
    virtual void setVisible(bool show) override;

    void update_machine_list(QEvent &event);
    void start_ssdp(bool on_off);
    bool was_dismiss() { return m_dismiss; }

private:
    int                               m_my_devices_count{0};
    int                               m_other_devices_count{0};
    PinCodePanel*                     m_panel_ping_code{nullptr};
    PinCodePanel*                     m_panel_direct_connection{nullptr};
    QWidget*                         m_placeholder_panel{nullptr};
    QLabel*                  m_hyperlink{nullptr};
    Label*                            m_ping_code_text{nullptr};
    QLabel*                   m_img_ping_code{nullptr};
    QBoxLayout *                      m_sizer_body{nullptr};
    QBoxLayout *                      m_sizer_my_devices{nullptr};
    QBoxLayout *                      m_sizer_other_devices{nullptr};
    QBoxLayout *                      m_sizer_search_bar{nullptr};
    QLineEdit*                     m_search_bar{nullptr};
    QScrollArea *                m_scrolledWindow{nullptr};
    QWidget *                        m_panel_body{nullptr};
    QTimer *                         m_refresh_timer{nullptr};
    std::vector<MachinePanel*>        m_user_list_machine_panel;
    std::vector<MachinePanel*>        m_other_list_machine_panel;
    boost::thread*                    get_print_info_thread{ nullptr };
    std::shared_ptr<int>              m_token = std::make_shared<int>(0);
    std::string                       m_print_info = "";
    bool                              m_dismiss { false };

    std::map<std::string, MachineObject*> m_bind_machine_list;
    std::map<std::string, MachineObject*> m_free_machine_list;

private:
    void OnLeftUp(QMouseEvent &event);
    void on_timer(QTimerEvent &event);

	void      update_other_devices();
    void      update_user_devices();
    bool      search_for_printer(MachineObject* obj);
    void      on_dissmiss_win(QEvent &event);
    QWidget *create_title_panel(QString text);
};

class EditDevNameDialog : public DPIDialog
{
public:
    EditDevNameDialog(Plater *plater = nullptr);
    ~EditDevNameDialog();

    void set_machine_obj(MachineObject *obj);
    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_edit_name(QEvent &e);

    Button*             m_button_confirm{nullptr};
    TextInput*          m_textCtr{nullptr};
    QLabel*       m_static_valid{nullptr};
    MachineObject*      m_info{nullptr};
};

}} // namespace Slic3r::GUI

#endif

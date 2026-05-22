#ifndef slic3r_GUI_CalibrationPanel_hpp_
#define slic3r_GUI_CalibrationPanel_hpp_

#include "CalibrationWizard.hpp"
#include "SelectMachinePop.hpp"
#include "Tabbook.hpp"
#include "Widgets/SideTools.hpp"

namespace Slic3r { namespace GUI {

#define CALI_MODE_COUNT  2

QString get_calibration_type_name(CalibMode cali_mode);

class MObjectPanel : public QWidget
{
private:
    bool        m_is_my_devices{ false };
    bool        m_hover{ false };

    PrinterState       m_state;
    ScalableBitmap m_printer_status_offline;
    ScalableBitmap m_printer_status_busy;
    ScalableBitmap m_printer_status_idle;
    ScalableBitmap m_printer_status_lock;
    ScalableBitmap m_printer_in_lan;
    MachineObject* m_info;

public:
    MObjectPanel(QWidget* parent,
            int      id = -1,
            const QPoint& pos = QPoint(),
            const QSize& size = QSize(),
            long            style = 0,
            const QString& name = QString());

    ~MObjectPanel();

    void set_printer_state(PrinterState state);
    void update_machine_info(MachineObject* info, bool is_my_devices = false);
protected:
    void OnPaint(QPaintEvent& event);
    void render(QPainter& dc);
    void doRender(QPainter& dc);
    void on_mouse_enter(QMouseEvent& evt);
    void on_mouse_leave(QMouseEvent& evt);
    void on_mouse_left_up(QMouseEvent& evt);
};

class MPanel
{
public:
    QString mIndex;
    MObjectPanel* mPanel;
};

class SelectMObjectPopup : public PopupWindow
{
public:
    SelectMObjectPopup(QWidget* parent);
    ~SelectMObjectPopup();

    // PopupWindow virtual methods are all overridden to log them
    virtual void Popup(QWidget* focus = NULL) override;
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent& event);
    void setVisible(bool show) override;

    void update_machine_list(QEvent& event);
    bool was_dismiss() { return m_dismiss; }

private:
    int                                 m_my_devices_count{ 0 };
    int                                 m_other_devices_count{ 0 };
    bool                                m_dismiss{ false };
    QWidget*                           m_placeholder_panel   { nullptr };
    QWidget*                           m_panel_body{ nullptr };
    QBoxLayout*                         m_sizer_body{ nullptr };
    QBoxLayout*                         m_sizer_my_devices{ nullptr };
    QScrollArea*                   m_scrolledWindow{ nullptr };
    QTimer*                            m_refresh_timer{ nullptr };
    std::vector<MPanel*>                m_user_list_machine_panel;
    boost::thread*                      get_print_info_thread{ nullptr };
    std::string                         m_print_info;
    std::shared_ptr<int>                m_token = std::make_shared<int>(0);
    std::map<std::string, MachineObject*> m_bind_machine_list;

private:
    void OnLeftUp(QMouseEvent& event);
    void on_timer(QTimerEvent& event);
    void update_user_devices();
    void on_dissmiss_win(QEvent& event);
};


class CalibrationPanel : public QWidget
{
public:
    CalibrationPanel(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);
    ~CalibrationPanel();
    Tabbook* get_tabpanel() { return m_tabpanel; };
    void update_print_error_info(int code, std::string msg, std::string extra);
    void update_all();
    void show_status(int status);
    bool Show(bool show);
    void on_printer_clicked(QMouseEvent& event);
    void set_default();
    void msw_rescale();
    void on_sys_color_changed();
protected:
    void init_tabpanel();
    void init_timer();
    void on_timer(QTimerEvent& event);


    int                     last_status;
    bool                    m_initialized { false };
    std::string             last_conn_type = "undedefined";
    MachineObject*          obj{ nullptr };
    MachineObject*          last_obj { nullptr };
    SideTools*              m_side_tools{ nullptr };
    Tabbook*                m_tabpanel{ nullptr };
    SelectMObjectPopup      m_mobjectlist_popup;
    CalibrationWizard*      m_cali_panels[CALI_MODE_COUNT];
    QTimer*                m_refresh_timer = nullptr;
};
}} // namespace Slic3r::GUI

#endif
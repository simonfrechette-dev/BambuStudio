#ifndef slic3r_MultiMachinePage_hpp_
#define slic3r_MultiMachinePage_hpp_
#include <QWidget>
#include <QString>

#include "libslic3r/libslic3r.h"
#include "GUI_App.hpp"
#include "GUI_Utils.hpp"
#include "MultiTaskManagerPage.hpp"
#include "MultiMachineManagerPage.hpp"
#include "Tabbook.hpp"


namespace Slic3r { 
namespace GUI {

#define PICK_LEFT_PADDING_LEFT 15
#define PICK_LEFT_PRINTABLE    40
#define PICK_LEFT_DEV_NAME 250
#define PICK_LEFT_DEV_STATUS 250
#define PICK_DEVICE_MAX 6
    
class MultiMachinePage : public QWidget
{
private:
    QTimer*                    m_refresh_timer      = nullptr;
    QLayout*                    m_main_sizer{ nullptr };
    LocalTaskManagerPage*       m_local_task_manager{ nullptr };
    CloudTaskManagerPage*       m_cloud_task_manager{ nullptr };
    MultiMachineManagerPage*    m_machine_manager{ nullptr };
    Tabbook*                    m_tabpanel{ nullptr };

public:
    MultiMachinePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);
    ~MultiMachinePage();

    void jump_to_send_page();

    void on_sys_color_changed();
    void msw_rescale();
    bool Show(bool show);

    void init_tabpanel();
    void init_timer();
    void on_timer(QTimerEvent& event);

    void clear_page();
};


class DevicePickItem : public DeviceItem
{

public:
    DevicePickItem(QWidget* parent, MachineObject* obj);
    ~DevicePickItem() {};

    void DrawTextWithEllipsis(QPainter& dc, const QString& text, int maxWidth, int left, int top = 0);
    void OnEnterWindow(QMouseEvent& evt);
    void OnLeaveWindow(QMouseEvent& evt);
    void OnSelectedDevice(QEvent& evt);
    void OnLeftDown(QMouseEvent& evt);
    void OnMove(QMouseEvent& evt);

    void         paintEvent(QPaintEvent& evt);
    void         render(QPainter& dc);
    void         doRender(QPainter& dc);
    void         post_event(QEvent&& event);
    // DoSetSize removed for Qt port

public:
    bool m_hover{ false };
    ScalableBitmap m_bitmap_check_disable;
    ScalableBitmap m_bitmap_check_off;
    ScalableBitmap m_bitmap_check_on;
};


class MultiMachinePickPage : public DPIDialog
{
private:
    AppConfig*          app_config;
    Label*              m_label{ nullptr };
    QScrollArea*     scroll_macine_list{ nullptr };
    QBoxLayout*         m_sizer_body{ nullptr };
    QBoxLayout*                         sizer_machine_list{ nullptr };
    std::map<std::string, DevicePickItem*>  m_device_items;
    int                 m_selected_count{0};
public:
    MultiMachinePickPage(Plater* plater = nullptr);
    ~MultiMachinePickPage();

    int get_selected_count();
    void update_selected_count();
    void on_dpi_changed(const QRect& suggested_rect);
    void on_sys_color_changed();
    void refresh_user_device();
    void on_confirm(QEvent& event);
    bool Show(bool show);
};

} // namespace GUI
} // namespace Slic3r

#endif

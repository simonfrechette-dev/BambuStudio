#ifndef slic3r_MultiMachineMangerPage_hpp_
#define slic3r_MultiMachineMangerPage_hpp_

#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "Widgets/TextInput.hpp"
#include "MultiMachine.hpp"

namespace Slic3r { 
namespace GUI {

#define DEVICE_LEFT_PADDING_LEFT 15
#define DEVICE_LEFT_DEV_NAME 180
#define DEVICE_LEFT_PRO_NAME 180
#define DEVICE_LEFT_PRO_INFO 320

class MultiMachineItem : public DeviceItem
{

public:
    MultiMachineItem(QWidget* parent, MachineObject* obj);
    ~MultiMachineItem() {};

    void OnEnterWindow(QMouseEvent& evt);
    void OnLeaveWindow(QMouseEvent& evt);
    void OnLeftDown(QMouseEvent& evt);
    void OnMove(QMouseEvent& evt);

    void         paintEvent(QPaintEvent& evt);
    void         render(QPainter& dc);
    void         DrawTextWithEllipsis(QPainter& dc, const QString& text, int maxWidth,  int left, int top = 0);
    void         doRender(QPainter& dc);
    void         post_event(QEvent&& event);
    // DoSetSize removed for Qt port

public:
    bool m_hover{ false };
    ScalableBitmap m_bitmap_check_disable;
    ScalableBitmap m_bitmap_check_off;
    ScalableBitmap m_bitmap_check_on;
    QString get_left_time(int mc_left_time);
};
    
class MultiMachineManagerPage : public QWidget
{
public:
    MultiMachineManagerPage(QWidget* parent);
    ~MultiMachineManagerPage() {};

    void update_page();
    void refresh_user_device(bool clear = false);
    
    void sync_state(MachineObject* obj_);
    bool Show(bool show);

    std::vector<ObjState> extractRange(const std::vector<ObjState>& source, int start, int end);

    void start_timer();
    void update_page_number();
    void on_timer(QTimerEvent& event);
    void clear_page();

    void page_num_enter_evt();

    void msw_rescale();

private:
    std::vector<ObjState>          m_state_objs;
    std::vector<MultiMachineItem*> m_device_items;
    SortItem                m_sort;
    bool                    device_dev_name_big{ true };
    bool                    device_state_big{ true };


    Button*                 m_button_edit{nullptr};
    QBoxLayout*             page_sizer{ nullptr };
    QWidget*                m_main_panel{ nullptr };
    QBoxLayout*             m_main_sizer{nullptr};
    QBoxLayout*             m_sizer_machine_list{nullptr};
    QScrollArea*       m_machine_list{ nullptr };
    QLabel*           m_selected_num{ nullptr };

    // table head
    QWidget*                m_table_head_panel{ nullptr };
    QBoxLayout*             m_table_head_sizer{ nullptr };
    Button*                 m_printer_name{ nullptr };
    Button*                 m_task_name{ nullptr };
    Button*                 m_status{ nullptr };
    Button*                 m_action{ nullptr };
    Button*                 m_stop_all_botton{nullptr};

    // tip when no device
    QLabel*           m_tip_text{ nullptr };
    Button*                 m_button_add{ nullptr };

    // Flipping pages
    int                         m_current_page{ 0 };
    int                         m_total_page{ 0 };
    int                         m_total_count{ 0 };
    int                         m_count_page_item{ 10 };

    bool                        prev{ false };
    bool                        next{ false };
    Button*                     btn_last_page{ nullptr };
    Button*                     btn_next_page{ nullptr };
    QLabel*               st_page_number{ nullptr };
    QBoxLayout*                 m_flipping_page_sizer{ nullptr };
    QBoxLayout*                 m_page_sizer{ nullptr };
    QWidget*                    m_flipping_panel{ nullptr };
    QTimer*                    m_flipping_timer{ nullptr };
    TextInput*                  m_page_num_input{ nullptr };
    Button*                     m_page_num_enter{ nullptr };
};

} // namespace GUI
} // namespace Slic3r

#endif

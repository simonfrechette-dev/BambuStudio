#ifndef slic3r_MultiTaskManagerPage_hpp_
#define slic3r_MultiTaskManagerPage_hpp_

#include "GUI_App.hpp"
#include "GUI_Utils.hpp"
#include "MultiMachine.hpp"
#include "DeviceManager.hpp"
#include "TaskManager.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/ScrolledWindow.hpp"
#include "Widgets/PopupWindow.hpp"
#include "Widgets/TextInput.hpp"

namespace Slic3r { 
namespace GUI {

#define CLOUD_TASK_ITEM_MAX_WIDTH 1100
#define TASK_ITEM_MAX_WIDTH    900
#define TASK_LEFT_PADDING_LEFT 15
#define TASK_LEFT_PRINTABLE    40
#define TASK_LEFT_PRO_NAME     180
#define TASK_LEFT_DEV_NAME     150
#define TASK_LEFT_PRO_STATE    170
#define TASK_LEFT_PRO_INFO     230
#define TASK_LEFT_SEND_TIME    180

class MultiTaskItem : public DeviceItem
{
public:
    MultiTaskItem(QWidget* parent, MachineObject* obj, int type);
    ~MultiTaskItem() {};


    void OnEnterWindow(QMouseEvent& evt);
    void OnLeaveWindow(QMouseEvent& evt);
    void OnSelectedDevice(QEvent& evt);
    void OnLeftDown(QMouseEvent& evt);
    void OnMove(QMouseEvent& evt);

    void         paintEvent(QPaintEvent& evt);
    void         render(QPainter& dc);
    void         doRender(QPainter& dc);
    void         DrawTextWithEllipsis(QPainter& dc, const QString& text, int maxWidth, int left, int top = 0);
    void         post_event(QEvent&& event);
    // DoSetSize removed for Qt port

    bool m_hover{ false };
    QString get_left_time(int mc_left_time);
    
    ScalableBitmap m_bitmap_check_disable;
    ScalableBitmap m_bitmap_check_off;
    ScalableBitmap m_bitmap_check_on;

    int          m_sending_percent{0};
    int          m_task_type{0}; //0-local 1-cloud
    QString     m_project_name;
    QString     m_dev_name;
    std::string  m_dev_id;
    TaskStateInfo* task_obj { nullptr };
    std::string  m_job_id;
    //std::string  m_sent_time;

    Button* m_button_resume{ nullptr };
    Button* m_button_cancel{ nullptr };
    Button* m_button_pause{ nullptr };
    Button* m_button_stop{ nullptr };

    void update_info();
    void onPause();
    void onResume();
    void onStop();
    void onCancel();
};

class LocalTaskManagerPage : public QWidget
{
public:
    LocalTaskManagerPage(QWidget* parent);
    ~LocalTaskManagerPage() {};

    void update_page();
    void refresh_user_device(bool clear = false);
    bool Show(bool show);
    void cancel_all(QEvent& evt);
    void msw_rescale();

private:
    SortItem                    m_sort;
    std::map<int, MultiTaskItem*> m_task_items;
    bool                        device_name_big{ true };
    bool                        device_state_big{ true };
    bool                        device_send_time{ true };

    QWidget* m_main_panel{ nullptr };
    QBoxLayout* m_main_sizer{ nullptr };
    QBoxLayout* page_sizer{ nullptr };
    QBoxLayout* m_sizer_task_list{ nullptr };
    QScrollArea* m_task_list{ nullptr };
    QLabel* m_selected_num{ nullptr };

    // table head
    QWidget* m_table_head_panel{ nullptr };
    QBoxLayout* m_table_head_sizer{ nullptr };
    CheckBox* m_select_checkbox{ nullptr };
    Button* m_task_name{ nullptr };
    Button* m_printer_name{ nullptr };
    Button* m_status{ nullptr };
    Button* m_info{ nullptr };
    Button* m_send_time{ nullptr };
    Button* m_action{ nullptr };

    // ctrl button for all
    int m_sel_number{0};
    QWidget* m_ctrl_btn_panel{ nullptr };
    QBoxLayout* m_btn_sizer{ nullptr };
    Button* btn_stop_all{ nullptr };
    QLabel* m_sel_text{ nullptr };

    // tip when no device
    QLabel* m_tip_text{ nullptr };
};

class CloudTaskManagerPage : public QWidget
{
public:
    CloudTaskManagerPage(QWidget* parent);
    ~CloudTaskManagerPage();

    void update_page();
    void refresh_user_device(bool clear = false);
    std::string utc_time_to_date(std::string utc_time);
    bool Show(bool show);
    void update_page_number();
    void start_timer();
    void on_timer(QTimerEvent& event);

    void pause_all(QEvent& evt);
    void resume_all(QEvent& evt);
    void stop_all(QEvent& evt);

    void enable_buttons(bool enable);
    void page_num_enter_evt();

    void msw_rescale();

private:
    SortItem                    m_sort;
    bool                        device_name_big{ true };
    bool                        device_state_big{ true };
    bool                        device_send_time{ true };

    /* job_id -> sel */
    std::map <std::string, MultiTaskItem*> m_task_items;

    QWidget* m_main_panel{ nullptr };
    QBoxLayout* page_sizer{ nullptr };
    QBoxLayout* m_sizer_task_list{ nullptr };
    QBoxLayout* m_main_sizer{ nullptr };
    QScrollArea* m_task_list{ nullptr };
    QLabel* m_selected_num{ nullptr };

    // Flipping pages
    int                         m_current_page{ 0 };
    int                         m_total_page{0};
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

    // table head
    QWidget*                    m_table_head_panel{ nullptr };
    QBoxLayout*                 m_table_head_sizer{ nullptr };
    CheckBox*                   m_select_checkbox{ nullptr };
    Button*                     m_task_name{ nullptr };
    Button*                     m_printer_name{ nullptr };
    Button*                     m_status{ nullptr };
    Button*                     m_info{ nullptr };
    Button*                     m_send_time{ nullptr };
    Button*                     m_action{ nullptr };

    // ctrl button for all
    int                         m_sel_number;
    QWidget*                    m_ctrl_btn_panel{ nullptr };
    QBoxLayout*                 m_btn_sizer{ nullptr };
    Button*                     btn_pause_all{ nullptr };
    Button*                     btn_continue_all{ nullptr };
    Button*                     btn_stop_all{ nullptr };
    QLabel*               m_sel_text{ nullptr };

    // tip when no device
    QLabel*               m_tip_text{ nullptr };
    QLabel*               m_loading_text{ nullptr };
};


} // namespace GUI
} // namespace Slic3r

#endif

#ifndef slic3r_SendMultiMachinePage_hpp_
#define slic3r_SendMultiMachinePage_hpp_

#include "GUI_Utils.hpp"
#include "MultiMachine.hpp"
#include "DeviceManager.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/ScrolledWindow.hpp"
#include "Widgets/PopupWindow.hpp"
#include "Widgets/TextInput.hpp"
#include "AmsMappingPopup.hpp"
#include "SelectMachine.hpp"

namespace Slic3r {
namespace GUI {
#define SEND_LEFT_PADDING_LEFT 15
#define SEND_LEFT_PRINTABLE    40
#define SEND_LEFT_DEV_NAME 250
#define SEND_LEFT_DEV_STATUS 250
#define SEND_LEFT_TAKS_STATUS 180

#define  DESIGN_SELECTOR_NOMORE_COLOR QColor(248, 248, 248)
#define  DESIGN_GRAY900_COLOR QColor(38, 46, 48)
#define  DESIGN_GRAY800_COLOR QColor(50, 58, 61)
#define  DESIGN_GRAY600_COLOR QColor(144, 144, 144)
#define  DESIGN_GRAY400_COLOR QColor(166, 169, 170)
#define  DESIGN_RESOUTION_PREFERENCES QSize(FromDIP(540), -1)
#define  DESIGN_COMBOBOX_SIZE QSize(FromDIP(140), -1)
#define  DESIGN_LARGE_COMBOBOX_SIZE QSize(FromDIP(160), -1)
#define  DESIGN_INPUT_SIZE QSize(FromDIP(50), -1)




#define THUMBNAIL_SIZE FromDIP(128)

class RadioBox;
class AmsRadioSelector
{
public:
    QString  m_param_name;
    int       m_groupid;
    RadioBox* m_radiobox;
    bool      m_selected = false;
};

using AmsRadioSelectorList = std::vector<AmsRadioSelector*>;

class SendDeviceItem : public DeviceItem
{

public:
    SendDeviceItem(QWidget* parent, MachineObject* obj);
    ~SendDeviceItem() {};

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
    bool m_hover{false};
    ScalableBitmap m_bitmap_check_disable;
    ScalableBitmap m_bitmap_check_off;
    ScalableBitmap m_bitmap_check_on;
};

class Plater;
class SendMultiMachinePage : public DPIDialog
{
private:
    /* dev_id -> device_item */
    std::map<std::string, SendDeviceItem*>  m_device_items;

    QTimer*                            m_refresh_timer      = nullptr;

    // sort
    SortItem                            m_sort;
    bool                                device_name_big{ true };
    bool                                device_printable_big{ true };
    bool                                device_en_ams_big{ true };

    Button*                             m_button_send{ nullptr };
    QScrollArea*                   scroll_macine_list{ nullptr };
    QBoxLayout*                         sizer_machine_list{ nullptr };
    Plater*                             m_plater{ nullptr };

    int                                 m_print_plate_idx;
    bool                                m_is_canceled{ false };
    bool                                m_export_3mf_cancel{ false };
    AppConfig*                          app_config;

    QWidget*                            m_main_page{ nullptr };
    QScrollArea*                   m_main_scroll{ nullptr };
    QBoxLayout*                         m_sizer_body{ nullptr };
    QGridLayout*                        m_ams_list_sizer{ nullptr };
    AmsMapingPopup*                     m_mapping_popup{ nullptr };

    AmsRadioSelectorList                m_radio_group;
    MaterialHash                        m_material_list;
    std::map<std::string, CheckBox*>    m_checkbox_map;
    std::map<std::string, TextInput*>   m_input_map;
    std::vector<FilamentInfo>           m_filaments;
    std::vector<FilamentInfo>           m_ams_mapping_result;
    int                                 m_current_filament_id{ 0 };

    StateColor                          btn_bg_enable;

    // table head
    QWidget*                            m_table_head_panel{ nullptr };
    QBoxLayout*                         m_table_head_sizer{ nullptr };
    CheckBox*                           m_select_checkbox{ nullptr };
    Button*                             m_printer_name{ nullptr };
    Button*                             m_device_status{ nullptr };
    //Button*                             m_task_status{ nullptr };
    Button*                             m_ams{ nullptr };
    Button*                             m_refresh_button{ nullptr };

    // rename
    QStackedWidget*                       m_rename_switch_panel{ nullptr };
    QWidget*                            m_rename_normal_panel{ nullptr };
    QWidget*                            m_rename_edit_panel{ nullptr };
    TextInput*                          m_rename_input{ nullptr };
    ScalableButton*                     m_rename_button{ nullptr };
    QBoxLayout*                         rename_sizer_v{ nullptr };
    QBoxLayout*                         rename_sizer_h{ nullptr };
    QLabel*                       m_task_name{ nullptr };
    QString                            m_current_project_name;
    bool                                m_is_rename_mode{ false };

    // title and thumbnail
    QWidget*                            m_title_panel{ nullptr };
    QBoxLayout*                         m_title_sizer{ nullptr };
    QBoxLayout*                         m_text_sizer{ nullptr };
    QLabel*                       m_stext_time{ nullptr };
    QLabel*                       m_stext_weight{ nullptr };
    QLabel*                     timeimg{ nullptr };
    ScalableBitmap*                     print_time{ nullptr };
    QLabel*                     weightimg{ nullptr };
    ScalableBitmap*                     print_weight{ nullptr };
    QBoxLayout*                         m_thumbnail_sizer{ nullptr };
    ThumbnailPanel*                     m_thumbnail_panel{nullptr};
    QWidget*                            m_panel_image{ nullptr };
    QBoxLayout*                         m_image_sizer{ nullptr };

    // tip when no device
    QLabel*                       m_tip_text{ nullptr };
    Button*                             m_button_add{ nullptr };

public:
    SendMultiMachinePage(Plater* plater = nullptr);
    ~SendMultiMachinePage();

    void prepare(int plate_idx);

    void on_dpi_changed(const QRect& suggested_rect);
    void on_sys_color_changed();
    void refresh_user_device();
    void on_send(QEvent& event);
    bool Show(bool show);

    BBL::PrintParams request_params(MachineObject* obj);

    bool get_ams_mapping_result(std::string &mapping_array_str, std::string &mapping_array_str2, std::string &ams_mapping_info);
    QBoxLayout* create_item_title(QString title, QWidget* parent, QString tooltip);
    QBoxLayout* create_item_checkbox(QString title, QWidget* parent, QString tooltip, int padding_left, std::string param);
    QBoxLayout* create_item_input(QString str_before, QString str_after, QWidget* parent, QString tooltip, std::string param);
    QBoxLayout* create_item_radiobox(QString title, QWidget* parent, QString tooltip, int groupid, std::string param);

    QWidget* create_page();
    void sync_ams_list();
    void set_default_normal(const ThumbnailData& data);
    void set_default();
    void on_rename_enter();
    void check_fcous_state(QWidget* window);
    void check_focus(QWidget* window);

protected:
    void OnSelectRadio(QMouseEvent& event);
    void on_select_radio(std::string param);
    bool get_value_radio(std::string param);
    void on_set_finish_mapping(QEvent& evt);
    void on_rename_click(QEvent& event);

    void on_timer(QTimerEvent& event);
    void init_timer();

private:

};


} // namespace GUI
} // namespace Slic3r

#endif

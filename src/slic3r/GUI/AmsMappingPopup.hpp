#ifndef slic3r_GUI_AmsMappingPopup_hpp_
#define slic3r_GUI_AmsMappingPopup_hpp_


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

#include "slic3r/GUI/DeviceCore/DevUtil.h"

#define MAPPING_ITEM_INVALID_REMAIN -1

// Previous definitions
class SwitchBoard;
namespace Slic3r
{
class DevNozzleRack;
namespace GUI
{

class wgtDeviceNozzleRackSelect;
class wgtMsgPanel;
};
};

namespace Slic3r {
namespace GUI {


#define AMS_TOTAL_COUNT 4

enum TrayType {
    NORMAL,
    THIRD,
    EMPTY
};

enum ShowType {
    LEFT,   //  only show left ams and left ext
    RIGHT,  //only show right ams and right ext
    LEFT_AND_RIGHT,  //show left and right ams at the same time
    LEFT_AND_RIGHT_DYNAMIC  //show all left and right at one panel when use_dynamic_switch
};

struct TrayData
{
    TrayType        type;
    int             id;
    int             ctype = 0;
    int             remain = MAPPING_ITEM_INVALID_REMAIN;
    std::string     name;
    std::string     filament_type;
    QColor        colour;
    std::vector<QColor> material_cols = std::vector<QColor>();

    int             ams_id = 0;
    int             slot_id = 0;
};

class MaterialItem: public QWidget
{
protected:
    int m_text_pos_y = -1;
    bool m_dropdown_allow_painted = true;
    QString m_mapping_text;

public:
    MaterialItem(QWidget *parent, QColor mcolour, QString mname, std::string filament_id);
    ~MaterialItem();

    void allow_paint_dropdown(bool flag);

    void set_ams_text(const QString& txt);
    void set_ams_info(QColor col, QString txt,
                      int ctype = 0, std::vector<QColor> cols = std::vector<QColor>(),
                      bool record_back_info = false);

    void set_material_cols(int ctype, const std::vector<QColor>& cols);

    void reset_ams_info();
    virtual void reset_valid_info();

    void set_nozzle_info(const QString& mapped_nozzle_str);

    void disable();
    void enable();
    void on_normal();
    void on_selected();
    void on_warning();

    bool is_selected() const { return m_selected;}
    bool is_warning() const { return m_warning;}

    void msw_rescale();

protected:
    void messure_size();

public:
    std::string m_filament_id;

    QColor    m_material_coloul;
    int         m_material_ctype = 0;
    std::vector<QColor> m_material_cols;
    QString    m_material_name;
    QString    m_mapped_nozzle_str;

    //info
    QColor m_ams_coloul;
    QString m_ams_name;
    int      m_ams_ctype = 0;
    std::vector<QColor> m_ams_cols = std::vector<QColor>();

    //reset
    QColor              m_back_ams_coloul;
    QString              m_back_ams_name;
    int                   m_back_ams_ctype = 0;
    std::vector<QColor> m_back_ams_cols  = std::vector<QColor>();

    ScalableBitmap m_arraw_bitmap_gray;
    ScalableBitmap m_arraw_bitmap_white;
    ScalableBitmap m_transparent_mitem;
    ScalableBitmap m_filament_wheel_transparent;
    ScalableBitmap m_ams_wheel_mitem;
    ScalableBitmap m_ams_not_match;
    ScalableBitmap m_rack_nozzle_bitmap;

    bool m_selected {false};
    bool m_warning{false};
    bool m_match {true};
    bool m_enable {true};

    void paintEvent(QPaintEvent &evt);
    virtual void render(QPainter &dc);
    void match(bool mat);
    virtual void doRender(QPainter &dc);
};

class MaterialSyncItem : public MaterialItem
{
public:
    MaterialSyncItem(QWidget *parent, QColor mcolour, QString mname, std::string filament_id);
    ~MaterialSyncItem();
    int  get_real_offset();
    void render(QPainter &dc) override;
    void doRender(QPainter &dc) override;
    void set_material_index_str(std::string str);
    const std::string &get_material_index_str() { return m_material_index; }
private:
    std::string m_material_index;
};

class MappingItem : public QWidget
{
public:
    MappingItem(QWidget *parent);
    ~MappingItem();

    QWidget*send_win{nullptr};
    QString m_tray_index;
    QColor m_coloul;
    QString m_name;
    TrayData m_tray_data;
    ScalableBitmap m_transparent_mapping_item;
    ScalableBitmap mapping_item_checked;
    bool     m_unmatch{false};

    int     m_ams_id{255};
    int     m_slot_id{255};

public:
    void update_data(TrayData data);
    void send_event(int fliament_id);
    void set_data(const QString& tag_name, QColor colour, QString name, bool remain_detect, TrayData data, bool unmatch = false, std::optional<QString> tooltip_opt = std::nullopt);
    void set_checked(bool checked);
    void set_tray_index(QString t_index) { m_tray_index = t_index; };

    void msw_rescale();

private:
    void paintEvent(QPaintEvent &evt);
    void render(QPainter &dc);
    void doRender(QPainter &dc);

    int get_remain_area_height() const;

private:
    bool m_checked = false;
    bool m_support_remain_detect = false;/*paint the area as 100 percent*/
    bool m_to_paint_remain = false;/*do not paint the area*/
};

class MappingContainer : public QWidget
{
private:
    int       m_slots_num = 4;/*1 or 4*/
    QString  m_ams_type;
    QPixmap  ams_mapping_item_container;

public:
    MappingContainer(QWidget* parent, const QString& ams_type, int slots_num = 4);
    ~MappingContainer();

public:
    int   get_slots_num() const { return m_slots_num;}

    void  msw_rescale();

protected:
    void paintEvent(QPaintEvent& evt);
    void render(QPainter& dc);
    void doRender(QPainter& dc);
};

class DevIconLabel : public QWidget
{
public:
    DevIconLabel(QWidget* parent, const QString& icon, const QString& label);

public:
    void SetAllBackgroundColor(const QColor& color);

    Label* GetLabelItem() const { return m_label; }
    void SetLabel(const QString& label);
    void SetIcon(const QString& icon);

    void Rescale();

private:
    void CreateGui();

private:
    Label*          m_label{ nullptr };
    QString        m_icon_str;
    QLabel* m_icon{ nullptr };
};

class AmsMapingPopup : public PopupWindow
{
    bool m_use_in_sync_dialog = false;
    bool m_ext_mapping_filatype_check = true;
    QLabel* m_title_text{ nullptr };

    wgtDeviceNozzleRackSelect *m_rack_nozzle_select{nullptr};
    DevIconLabel* m_flush_warning_panel;

public:
    AmsMapingPopup(QWidget *parent,bool use_in_sync_dialog = false);
    ~AmsMapingPopup() {};

    MaterialItem* m_parent_item{ nullptr };

    QWidget* send_win{ nullptr };
    std::vector<std::string> m_materials_list;
    std::vector<QBoxLayout*> m_amsmapping_container_sizer_list;
    std::vector<MappingContainer*> m_amsmapping_container_list;
    std::vector<MappingItem*> m_mapping_item_list;

    bool        m_has_unmatch_filament {false};
    int         m_current_filament_id;
    ShowType    m_show_type{ShowType::RIGHT};
    std::string m_tag_material;
    QScrollArea *m_scrolled_window{nullptr};
    QBoxLayout *m_sizer_main{nullptr};
    QBoxLayout *m_sizer_main_h{nullptr};
    QBoxLayout *m_sizer_ams_v{nullptr};
    QBoxLayout *m_sizer_ams{nullptr};
    QBoxLayout *m_sizer_ams_left{nullptr};
    QBoxLayout *m_sizer_ams_right{nullptr};
    QBoxLayout *m_sizer_ams_left_horizonal{nullptr};
    QBoxLayout *m_sizer_ams_right_horizonal{nullptr};
    QBoxLayout* m_sizer_ams_basket_left{ nullptr };
    QBoxLayout* m_sizer_ams_basket_right{ nullptr };
    QBoxLayout *m_sizer_list{nullptr};

    MappingItem* m_left_extra_slot{nullptr};
    MappingItem* m_right_extra_slot{nullptr};

    QWidget *    m_left_marea_panel{nullptr};
    QWidget*     m_right_marea_panel{ nullptr }; // used as right if both left and right sides shown. used as single panel if only one side shown.
    QWidget *    m_left_first_text_panel{nullptr};
    QWidget *    m_right_first_text_panel{nullptr};
    wgtMsgPanel* m_ams_tips_msg_panel{nullptr};
    QWidget *    m_split_line_panel{nullptr};
    QBoxLayout * m_left_split_ams_sizer{nullptr};
    QBoxLayout * m_right_split_ams_sizer{nullptr};
    QBoxLayout * m_right_split_ext_sizer{ nullptr };
    Label *      m_left_tips{nullptr};
    Label *      m_right_tips{nullptr};

    ScalableButton* m_reset_btn{nullptr};
    QString     m_single_tip_text;
    QString     m_left_tip_text;
    QString     m_right_tip_text;
    QString     m_ams_tips_panel_text;
    QBoxLayout* m_sizer_split_ams_left;
    QBoxLayout* m_sizer_split_ams_right;
    bool        m_mapping_from_multi_machines {false};

    bool         get_use_in_sync_dialog() { return m_use_in_sync_dialog; }
    void         set_sizer_title(QBoxLayout *sizer, QString text);
    QBoxLayout*  create_split_sizer(QWidget* parent, QString text);
    void         set_send_win(QWidget* win) {send_win = win;};
    void         update_materials_list(std::vector<std::string> list);
    void         set_tag_texture(std::string texture);
    void         update(MachineObject* obj, const std::vector<FilamentInfo>& ams_mapping_result, bool use_dynamic_switch = false, std::optional<PrintFromType> print_type = std::nullopt);
    void         update_rack_select(MachineObject* obj, bool use_dynamic_switch, std::optional<PrintFromType> print_type);
    void         update_items_check_state(const std::vector<FilamentInfo>& ams_mapping_result);
    void         update_ams_data_multi_machines();
    void         add_ams_mapping(std::vector<TrayData> tray_data, bool remain_detect_flag, QWidget *container, QBoxLayout *sizer);
    void         add_ext_ams_mapping(TrayData tray_data, MappingItem *item);
    void         set_current_filament_id(int id) { m_current_filament_id = id; };
    int          get_current_filament_id(){return m_current_filament_id;};
    bool         is_match_material(std::string material) const;
    void         on_left_down(QMouseEvent &evt);
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);
    void         paintEvent(QPaintEvent &evt);
    void         set_parent_item(MaterialItem* item) {m_parent_item = item;};
    void         set_show_type(ShowType type) { m_show_type = type; };

#ifdef __APPLE__
    void on_mouse_move(QMouseEvent &evt);
    QWidget * m_tip_popup{nullptr};
    QLabel* m_tip_label{nullptr};
#endif

    using ResetCallback = std::function<void(const std::string&)>;
    void reset_ams_info();
    void set_reset_callback(ResetCallback callback);
    void  show_reset_button();
    void  set_material_index_str(std::string str) { m_material_index = str; }
    const std::string &get_material_index_str() { return m_material_index; }

public:
    void msw_rescale();

    void EnableExtMappingFilaTypeCheck(bool to_check = true) { m_ext_mapping_filatype_check = to_check;} ;

private:
    // update
    void update_title(MachineObject* obj);
    void update_ams_tips(MachineObject* obj);
    void update_mapping_items(MachineObject* obj, const std::vector<FilamentInfo>& ams_mapping_result, bool use_dynamic_switch);

    // events
    void OnNozzleMappingSelected(QEvent& evt);
    void update_flush_waste(MachineObject* obj);

private:
    std::weak_ptr<DevNozzleRack> m_rack;

    ResetCallback m_reset_callback{nullptr};
    std::string m_material_index;
};

class AmsMapingTipPopup : public PopupWindow
{
public:
    AmsMapingTipPopup(QWidget *parent);
    ~AmsMapingTipPopup(){};
    void paintEvent(QPaintEvent &evt);

    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);

public:
    QWidget *        m_panel_enable_ams;
    QLabel *   m_title_enable_ams;
    QLabel *   m_tip_enable_ams;
    QWidget *        m_split_lines;
    QWidget *        m_panel_disable_ams;
    QLabel *   m_title_disable_ams;
    QLabel *   m_tip_disable_ams;
};

class AmsHumidityLevelList : public QWidget
{
public:
    AmsHumidityLevelList(QWidget* parent);
    ~AmsHumidityLevelList() {};

public:
    void msw_rescale();

private:
    void paintEvent(QPaintEvent& evt);
    void render(QPainter& dc);
    void doRender(QPainter& dc);

private:
    ScalableBitmap background_img;
    std::vector<ScalableBitmap> hum_level_img_light;
    std::vector<ScalableBitmap> hum_level_img_dark;
};

class AmsHumidityTipPopup : public PopupWindow
{
public:
    AmsHumidityTipPopup(QWidget* parent);
    ~AmsHumidityTipPopup() {};

public:
    void set_humidity_level(int level);
    void msw_rescale();

private:
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);

    void paintEvent(QPaintEvent& evt);
    void render(QPainter& dc);
    void doRender(QPainter& dc);

private:
    int current_humidity_level = 0;

    ScalableBitmap close_img;

    QLabel* curr_humidity_img;
    QLabel* m_img;

    Label* m_staticText;;
    Label* m_staticText_note;

    AmsHumidityLevelList* humidity_level_list{nullptr};
};

class AmsTutorialPopup : public PopupWindow
{
public:
    Label* text_title;
    QLabel* img_top;
    QLabel* arrows_top;
    QLabel* tip_top;
    QLabel* arrows_bottom;
    QLabel* tip_bottom;
    QLabel* img_middle;
    QLabel* tip_middle;
    QLabel* img_botton;

    AmsTutorialPopup(QWidget* parent);
    ~AmsTutorialPopup() {};

    void paintEvent(QPaintEvent& evt);
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);
};


class AmsIntroducePopup : public PopupWindow
{
public:
    bool          is_enable_ams = {false};
    Label* m_staticText_top;
    Label* m_staticText_bottom;
    QLabel* m_img_enable_ams;
    QLabel* m_img_disable_ams;

    AmsIntroducePopup(QWidget* parent);
    ~AmsIntroducePopup() {};

    void set_mode(bool enable_ams);
    void paintEvent(QPaintEvent& evt);
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent&);
};


class AmsRMGroup : public QWidget
{
public:
    AmsRMGroup(QWidget* parent, std::map<std::string, QColor> group_info, QString mname, QString group_index);
    ~AmsRMGroup() {};

public:
    void set_index(std::string index) {m_selected_index = index;};
    void paintEvent(QPaintEvent& evt);
    void render(QPainter& dc);
    void doRender(QPainter& dc);
    void on_mouse_move(QMouseEvent& evt);

    double GetAngle(QPoint pointA, QPoint pointB);
    QPoint CalculateEndpoint(const QPoint& startPoint, int angle, int length);
private:
    std::map<std::string, QColor> m_group_info;
    std::string     m_selected_index;
    ScalableBitmap  backup_current_use_white;
    ScalableBitmap  backup_current_use_black;
    ScalableBitmap  bitmap_backup_tips_0;
    ScalableBitmap  bitmap_backup_tips_1;
    ScalableBitmap  bitmap_editable;
    ScalableBitmap  bitmap_bg;
    ScalableBitmap  bitmap_editable_light;
    QString        m_material_name;
    QString        m_group_index;
};

class AmsReplaceMaterialDialog : public DPIDialog
{
public:
    AmsReplaceMaterialDialog(QWidget* parent);
    ~AmsReplaceMaterialDialog() {};

public:
    void        update_machine_obj(MachineObject* obj);
    void        paintEvent(QPaintEvent& evt);
    void        on_dpi_changed(const QRect& suggested_rect);

public:
    MachineObject* m_obj{ nullptr };

    QScrollArea* m_scrollview_groups{ nullptr };
    QBoxLayout* m_scrollview_sizer{ nullptr };
    QBoxLayout* m_main_sizer{ nullptr };
    QLayout* m_groups_sizer{ nullptr };
    SwitchBoard* m_nozzle_btn_panel { nullptr};

    std::vector<std::string> m_tray_used;
    Label* label_txt{nullptr};
    Label* identical_filament;

private:
    void        create();
    AmsRMGroup* create_backup_group(QString gname, std::map<std::string, QColor> group_info, QString material);

    // update to nozzle
    void  on_nozzle_selected(QEvent& event) { update_to_nozzle(0); };
    void  update_to_nozzle(int nozzle_id);
};



}} // namespace Slic3r::GUI

#endif

#pragma once
#include "GUI_ObjectLayers.hpp"
#include "GUI_Utils.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "slic3r/GUI/Widgets/AMSItem.hpp"
#include "slic3r/GUI/Widgets/Label.hpp"
#include "slic3r/GUI/Widgets/PopupWindow.hpp"

#include "slic3r/GUI/wxExtensions.hpp"
#include "slic3r/GUI/DeviceCore/DevFilaSystem.h"

#include <chrono>
#include <optional>
#include <QStackedWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QComboBox>
#include <QCloseEvent>
#include <QShowEvent>

//Previous defintions
// class QTableWidget forward-declared — replaced by QTableWidget
class QTableWidget;

namespace Slic3r {
    
namespace GUI {


enum class DryCtrState {
    IDLE,
    DRY_WHEN_PRINT,
    UNKNOWN
};

enum class DryCtrDev {
    N3S,
    N3F,
    UNKNOWN
};

struct DryingPreset {
    DryCtrState state;
    DryCtrDev dev;
    int dry_temp;
    int dry_time;
};

class FilamentItemPanel : public QWidget
{
public:
    FilamentItemPanel(QWidget * parent, const QString& text, const std::string& icon_name = "", 
                      int id = -1);
    
    void SetText(const QString& text);
    void SetIcon(const std::string& icon_name);
    void msw_rescale();
    
private:
    void OnPaint(QPaintEvent * event);
    void OnSize(QResizeEvent * event);
    
    Label* m_text_label;
    QLabel * m_icon_bitmap;
    int m_target_size;
    std::string m_icon_name;
    ScalableBitmap m_icon;
};

class AMSFilamentPanel : public QWidget
{
    QLayout * m_filament_sizer;
    Label* m_ams_name_label;
    int m_border_radius;
    QWidget* m_filament_container{nullptr};
    std::vector<FilamentItemPanel*> m_filament_items;
    
public:
    AMSFilamentPanel(QWidget * parent, const QString& ams_name, int id = -1);
    
    void AddFilamentItem(const QString& text, const std::string& icon_name);
    void AddFilamentItem(FilamentItemPanel* panel);
    void SetAmsName(const QString& ams_name);
    void Clear();
    void msw_rescale();
private:
    void OnPaint(QPaintEvent * event);
};


class AMSDryCtrWin : public DPIDialog
{
public:
    AMSDryCtrWin(QWidget *parent);
    ~AMSDryCtrWin();

    void msw_rescale();
    void update(std::shared_ptr<DevFilaSystem> fila_system, MachineObject* obj);
    void set_ams_id(const std::string& ams_id);

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

private:
    QStackedWidget* m_main_simplebook{nullptr};
    QWidget* m_original_page{nullptr};

    QWidget * m_amswin{nullptr};
    QLayout * m_sizer_ams_items{nullptr};
    QScrollArea* m_panel_prv_left {nullptr};
    QScrollArea* m_panel_prv_right{nullptr};
    QLayout * m_sizer_prv_left{nullptr};
    QLayout * m_sizer_prv_right{nullptr};

    // left panel related members
    ScalableBitmap m_humidity_image;
    QLabel * m_humidity_img{nullptr};
    Label* m_image_description{nullptr};
    QLabel * m_image_description_icon{nullptr};
    ScalableBitmap m_description_icon_bitmap;

    Label* m_humidity_data_label = nullptr;
    Label* m_temperature_data_label = nullptr;
    Label* m_time_data_label = nullptr;
    QLayout * m_time_descrition_container = nullptr;

    // right panel related members
    QLayout * m_normal_state_sizer{nullptr};
    QLayout * m_cannot_dry_sizer{nullptr};
    QLayout * m_dry_error_sizer{nullptr};
    Label* m_cannot_dry_description_label = nullptr;

    // right panel normal state
    QComboBox* m_trays_combo{nullptr};
    std::vector<FilamentBaseInfo> m_tray_ids;
    QLineEdit* m_temperature_input{nullptr};
    QLineEdit* m_time_input{nullptr};
    Label* m_normal_description;
    Button* m_start_button{nullptr};
    Button* m_next_button{nullptr};
    Button* m_stop_button{nullptr};
    Button* m_back_button{nullptr};
    Button* m_unload_button{nullptr};

    // guide page description
    Label* m_guide_title_label{nullptr};
    Label* m_guide_description_label{nullptr};

    QCheckBox* m_rotate_spool_toggle{nullptr};

    QWidget* m_progress_page;
    QProgressBar* m_progress_gauge{nullptr};
    QTimer * m_progress_timer;

    std::optional<std::chrono::steady_clock::time_point> m_stop_button_restore_deadline;
    std::optional<std::chrono::steady_clock::time_point> m_unload_button_restore_deadline;

    Label* m_progress_title;
    int m_progress_value;
    int m_progress_message_index;
    std::vector<QString> m_progress_text; // initialized in constructor

    // Guide page
    QWidget* m_guide_page{nullptr};
    AMSFilamentPanel* m_ams_filament_panel{nullptr};
    std::map<std::string, FilamentItemPanel*> m_filament_items;
    QLabel * m_image_placeholder{nullptr};
    ScalableBitmap m_guide_image;


    bool m_is_ams_changed = false;
    std::weak_ptr<DevFilaSystem> m_fila_system;
    struct {
        std::string m_ams_id;
        DevAmsType m_model = DevAmsType::EXT_SPOOL;
        DevAms::DryStatus m_dry_status = DevAms::DryStatus::Off;
        DevAms::DrySubStatus m_dry_sub_status = DevAms::DrySubStatus::Off;
        int m_humidity_percent;
        int m_temperature;
        int m_left_dry_time;
        float m_recommand_dry_temp;
    } m_ams_info;

    struct {
        std::unordered_map<std::string, std::string> m_filament_names;
        std::unordered_map<std::string, std::string> m_filament_type;
        std::unordered_map<std::string, int> m_dry_temp;
        std::unordered_map<std::string, int> m_dry_time;
    } m_dry_setting;

    struct {
        bool m_is_printing = false;
    } m_printer_status;

private:
    void create();
    QLayout * create_guide_page_sizer(QWidget* parent);
    QLayout * create_main_content_section(QWidget* parent);
    QLayout * create_guide_info_filament(QWidget* parent);
    QLayout * create_guide_info_section(QWidget* parent);
    QLayout * create_guide_right_section(QWidget* parent);
    QLayout * create_main_page_sizer(QWidget* parent);
    QLayout * create_left_panel(QWidget* parent);
    QLayout * create_humidity_status_section(QWidget* parent);
    QLayout * create_description_item(QWidget* parent, const QString& title, Label*& dataLabel);
    QLayout * create_status_descriptions_section(QWidget* parent);
    
    QLayout * create_right_panel(QWidget* parent);
    QLayout * create_normal_state_panel(QWidget* parent);
    QLayout * create_cannot_dry_panel(QWidget* parent);
    QLayout * create_drying_error_panel(QWidget* parent);
    Button* create_button(QWidget* parent, const QString& title,
        const QColor& background_color, const QColor& border_color, const QColor& text_color);

    QLayout * create_progress_page_sizer(QWidget* parent);
    void OnProgressTimer();
    void OnClose(QCloseEvent* event);
    void OnShow(QShowEvent* event);

    void OnFilamentSelectionChanged(int index);


    QScrollArea* create_preview_scrolled_window(QWidget * parent);

    bool check_values_changed(DevAms* dev_ams);
    int update_image(DevAmsType type, DevAms::DryStatus status, DevAms::DrySubStatus sub_status, int humidity_percent);
    void update_img_description(DevAms::DryStatus status, DevAms::DrySubStatus sub_status);
    void update_normal_description(DevAms* dev_ams);
    int update_state(DevAms* dev_ams);
    int update_dryness_status(DevAms* dev_ams);
    int update_ams_change(DevAms* dev_ams);
    int update_filament_list(DevAms* dev_ams, MachineObject* obj);
    void update_filament_guide_info(DevAms* dev_ams);
    void update_normal_state(DevAms* dev_ams);
    void update_printer_state(MachineObject* obj);

    std::shared_ptr<DevFilaSystem> get_fila_system() const;
    void start_sending_drying_command();
    void restore_stop_button_if_deadline_passed();
    void restore_unload_button_if_deadline_passed();
    void update_button_size(Button* button);

    bool is_dry_status_changed(DevAms* dev_ams);
    bool is_dry_ctr_idle(DevAms* dev_ams);
    bool is_ams_changed(DevAms* dev_ams);
    bool is_dry_ctr_idle();
    bool is_tray_changed(DevAms* dev_ams);
    bool is_dry_ctr_err(DevAms* dev_ams);

};

} // GUI
} // Slic3r
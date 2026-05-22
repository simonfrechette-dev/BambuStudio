#ifndef slic3r_StatusPanel_hpp_
#define slic3r_StatusPanel_hpp_

#include "libslic3r/ProjectTask.hpp"
#include "DeviceManager.hpp"
#include "MonitorPage.hpp"
#include "SliceInfoPanel.hpp"
#include "CameraPopup.hpp"
#include "GUI.hpp"
#include "ThermalPreconditioningDialog.hpp"
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QBoxLayout>
#include <QGridLayout>
#include <QNetworkReply>
#include <QNetworkAccessManager>
// Forward declarations to avoid wx cascade
class PartSkipDialog;
class PrintOptionsDialog;
class PrinterPartsDialog;
class SafetyOptionsDialog;
class CalibrationDialog;
class SecondaryCheckDialog;
class AMSMaterialsSetting;
class ExtrusionCalibration;
class ReleaseNote;
// CalibrationMethod enum forward declaration (defined in CalibrationWizardPage.hpp)
// NOTE: must be at global scope as placeholder; real type is Slic3r::GUI::CalibrationMethod
// MediaPlayCtrl.h forward-declared to avoid wxMediaCtrl3/wx/uri.h cascade
class MediaPlayCtrl;
#include "Widgets/AnimaController.hpp"
// Forward-declared to avoid wx cascade (AMSSetting is wx-based)
class AMSSetting;
#include "Widgets/SwitchButton.hpp"
#include "Widgets/AxisCtrlButton.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/TempInput.hpp"
#include "Widgets/StaticLine.hpp"
#include "Widgets/ProgressBar.hpp"
#include "Widgets/ImageSwitchButton.hpp"
#include "Widgets/AMSControl.hpp"
#include "Widgets/FilamentLoad.hpp"
#include "Widgets/FanControl.hpp"
#include "HMS.hpp"
#include "DeviceErrorDialog.hpp"

class StepIndicator;

#define COMMAND_TIMEOUT         5

namespace Slic3r {

class DevExtderSystem;

namespace GUI {

// CalibrationMethod forward declaration (full enum is in CalibrationWizardPage.hpp)
enum CalibrationMethod : int;

// Previous definitions
class MessageDialog;
class wgtDeviceNozzleRack;
class CameraFullscreenFrame;

enum CameraRecordingStatus {
    RECORDING_NONE,
    RECORDING_OFF_NORMAL,
    RECORDING_OFF_HOVER,
    RECORDING_ON_NORMAL,
    RECORDING_ON_HOVER,
};

enum CameraTimelapseStatus {
    TIMELAPSE_NONE,
    TIMELAPSE_OFF_NORMAL,
    TIMELAPSE_OFF_HOVER,
    TIMELAPSE_ON_NORMAL,
    TIMELAPSE_ON_HOVER,
};

enum PrintingTaskType {
    PRINGINT,
    CALIBRATION,
    NOT_CLEAR
};

enum ExtruderState {
    FILLED_LOAD,
    FILLED_UNLOAD,
    EMPTY_LOAD,
    EMPTY_UNLOAD
};

struct ScoreData
{
    int                                            rating_id;
    int                                            design_id;
    std::string                                    model_id;
    int                                            profile_id;
    int                                            star_count;
    bool                                           success_printed;
    QString                                       comment_text;
    std::vector<std::string>                       image_url_paths;
    std::set<QString>                             need_upload_images;
    std::vector<std::pair<QString, std::string>>  local_to_url_image;
};

typedef std::function<void(BBLModelTask* subtask)> OnGetSubTaskFn;

class ExtruderImage : public QWidget
{
    ScalableBitmap *m_pipe_filled_load;
    ScalableBitmap *m_pipe_filled_unload;
    ScalableBitmap *m_pipe_empty_load;
    ScalableBitmap *m_pipe_empty_unload;

    ScalableBitmap *m_pipe_filled_load_unselected;
    ScalableBitmap *m_pipe_filled_unload_unselected;
    ScalableBitmap *m_pipe_empty_load_unselected;
    ScalableBitmap *m_pipe_empty_unload_unselected;

    ScalableBitmap *m_left_extruder_active_filled;
    ScalableBitmap *m_left_extruder_active_empty;
    ScalableBitmap *m_left_extruder_unactive_filled;
    ScalableBitmap *m_left_extruder_unactive_empty;
    ScalableBitmap *m_right_extruder_active_filled;
    ScalableBitmap *m_right_extruder_active_empty;
    ScalableBitmap *m_right_extruder_unactive_filled;
    ScalableBitmap *m_right_extruder_unactive_empty;

    ScalableBitmap *m_extruder_single_nozzle_empty_load;
    ScalableBitmap *m_extruder_single_nozzle_empty_unload;
    ScalableBitmap *m_extruder_single_nozzle_filled_load;
    ScalableBitmap *m_extruder_single_nozzle_filled_unload;

    ExtruderState m_left_ext_state   = {ExtruderState::EMPTY_LOAD};
    ExtruderState m_right_ext_state  = {ExtruderState::EMPTY_LOAD};
    ExtruderState m_single_ext_state = {ExtruderState::EMPTY_LOAD};

public:
    void update(int nozzle_num, int nozzle_id);
    void update(ExtruderState single_state);
    void update(ExtruderState right_state, ExtruderState left_state);

    void msw_rescale();
    void setExtruderCount(int nozzle_num);
    void setExtruderUsed(std::string loc);
    void paintEvent(QPaintEvent &evt);

    void     render(QPainter &dc);
    bool     m_show_state       = {false};
    int      m_nozzle_num       = 1;
    int      current_nozzle_idx = 0;
    std::string current_nozzle_loc = "";
    QColor m_colour;

    string m_file_name;
    bool   m_ams_loading{false};
    void   doRender(QPainter &dc);
    ExtruderImage(QWidget *parent, int id, int nozzle_num, const QPoint &pos = QPoint(), const QSize &size = QSize());
    ~ExtruderImage();
};

class ExtruderSwithingStatus : public QWidget
{
public:
    ExtruderSwithingStatus(QWidget *parent);
    ~ExtruderSwithingStatus() = default;

public:
    void updateBy(MachineObject *obj);
    bool has_content_shown() const;

    void msw_rescale();

private:
    void updateBy(const DevExtderSystem* ext_system);
    void showQuitBtn(bool show);
    void showRetryBtn(bool show);

    void on_quit(QEvent &event);
    void on_retry(QEvent &event);

private:
    MachineObject *m_obj = nullptr;

    Label  *m_switching_status_label = nullptr;
    Button *m_button_quit      = nullptr;
    Button *m_button_retry     = nullptr;

    /*the last control time*/
    time_t m_last_ctrl_time = 0;
};

class ScoreDialog : public GUI::DPIDialog
{
public:
    ScoreDialog(QWidget *parent, int design_id, std::string model_id, int profile_id, int rating_id, bool success_printed, int star_count = 0);
    ScoreDialog(QWidget *parent, ScoreData *score_data);
    ~ScoreDialog();

    int       get_rating_id() { return m_rating_id; }
    ScoreData get_score_data();
    void      set_comment(std::string comment);
    void      set_cloud_bitmap(std::vector<std::string> cloud_bitmaps);

protected:
    enum StatusCode {
        UPLOAD_PROGRESS = 0,
        UPLOAD_EXIST_ISSUE,
        UPLOAD_IMG_FAILED,
        CODE_NUMBER
    };

    std::shared_ptr<int>     m_tocken;
    const int                m_photo_nums = 16;
    int                      m_rating_id;
    int                      m_design_id;
    std::string              m_model_id;
    int                      m_profile_id;
    int                      m_star_count;
    bool                     m_success_printed;
    std::vector<std::string> m_image_url_paths;
    StatusCode               m_upload_status_code;

    struct ImageMsg
    {
        QString          local_image_url; //local image path
        std::string       img_url_paths; // oss url path
        vector<QWidget *> image_broad;
        bool              is_selected;
        bool              is_uploaded; // load
        QBoxLayout *      image_tb_broad = nullptr;
    };

    std::vector<ScalableButton *>                  m_score_star;
    QLineEdit *                                   m_comment_text  = nullptr;
    Button *                                       m_button_ok     = nullptr;
    Button *                                       m_button_cancel = nullptr;
    Label *                                        m_add_photo     = nullptr;
    Label *                                        m_delete_photo  = nullptr;
    QGridLayout *                                  m_image_sizer   = nullptr;
    QLabel *                                 warning_text    = nullptr;
    std::unordered_map<QLabel *, ImageMsg> m_image;
    std::unordered_set<QLabel *>           m_selected_image_list;

    void init();
    void update_static_bitmap(QLabel *static_bitmap, QImage image);
    void create_comment_text(const QString &comment = "");
    void load_photo(const std::vector<std::pair<QString, std::string>> &filePaths);
    void on_dpi_changed(const QRect &suggested_rect) override;
    void OnBitmapClicked(QMouseEvent &event);

    QBoxLayout * create_broad_sizer(QLabel *bitmap, ImageMsg &cur_image_msg);
    QBoxLayout * get_score_sizer();
    QBoxLayout * get_star_sizer();
    QBoxLayout * get_comment_text_sizer();
    QBoxLayout * get_photo_btn_sizer();
    QBoxLayout * get_button_sizer();
    QBoxLayout * get_main_sizer(const std::vector<std::pair<QString, std::string>> &images = std::vector<std::pair<QString, std::string>>(), const QString &comment = "");

    std::set<std::pair<QLabel *, QString>>        add_need_upload_imgs();
    std::pair<QLabel *, ImageMsg>                  create_local_thumbnail(QString &local_path);
    std::pair<QLabel *, ImageMsg>                  create_oss_thumbnail(std::string &oss_path);

};

class RectTextPanel : public QWidget
{
public:
    RectTextPanel(QWidget *parent);

public:
    void setText(const QString text);
    QString getText() const { return text; }

    void Rescale();

protected:
    void OnPaint(QPaintEvent &event);
    void paintEvent(QPaintEvent *event) override;

private:
    QString text;
};

class PrintingTaskPanel : public QWidget
{
public:
    PrintingTaskPanel(QWidget* parent, PrintingTaskType type);
    ~PrintingTaskPanel();
    void create_panel(QWidget* parent);


private:
    MachineObject*  m_obj{nullptr};
    ScalableBitmap  m_thumbnail_placeholder;
    std::string     m_thumbnail_bmp_display_name;
    QPixmap        m_thumbnail_bmp_display;
    ScalableBitmap  m_bitmap_use_time;
    ScalableBitmap  m_bitmap_use_weight;
    ScalableBitmap  m_bitmap_background;

    QWidget *       m_panel_printing_title;
    QWidget*        m_staticline;
    QWidget*        m_panel_error_txt;

    QBoxLayout*     m_printing_sizer;
    QLabel *  m_staticText_printing;
    QLabel*   m_staticText_subtask_value;
    QLabel*   m_staticText_consumption_of_time;
    QLabel*   m_staticText_consumption_of_weight;
    QLabel*   m_printing_stage_value;
    ScalableButton* m_question_button;
    QLabel*   m_staticText_profile_value;
    QLabel*   m_staticText_progress_percent;
    QLabel*   m_staticText_progress_percent_icon;
    QLabel*   m_staticText_progress_left;
    Label*          m_staticText_finish_time;
    RectTextPanel*  m_staticText_finish_day;
    QLabel*   m_staticText_layers;
    QLabel *  m_has_rated_prompt;
    QLabel *  m_request_failed_info;
    QLabel* m_bitmap_thumbnail;
    int             m_plate_index { -1 };
    QLabel* m_bitmap_static_use_time;
    QLabel* m_bitmap_static_use_weight;
    AnimaIcon*      m_pausing_icon;
    AnimaIcon*      m_stopping_icon;
    ScalableButton* m_button_pause_resume;
    ScalableButton* m_button_abort;
    Button*         m_button_partskip;
    Button*         m_button_market_scoring;
    Button*         m_button_clean;
    Button *                      m_button_market_retry;
    QWidget *                     m_score_subtask_info;
    QWidget *                     m_score_staticline;
    QWidget *                     m_request_failed_panel;
    QWidget                      *m_printing_stage_underline;
    QWidget                      *m_printing_stage_panel;

    // score page
    int                           m_star_count;
    std::vector<ScalableButton *> m_score_star;
    bool                          m_star_count_dirty = false;

    // partskip button
    int m_part_skipped_count{ 0 };
    int m_part_skipped_dirty{ 0 };

    ProgressBar*    m_gauge_progress;
    Label* m_error_text;
    PrintingTaskType m_type;
    int m_brightness_value{ -1 };

public:
    void init_bitmaps();
    void init_scaled_buttons();
    void error_info_reset();
    void show_error_msg(QString msg);
    void reset_printing_value();
    void msw_rescale();

public:
    void enable_partskip_button(MachineObject* obj, bool enable);
    void update_pausing_state(bool enter);
    void update_stopping_state(bool enter);
    void enable_pause_resume_button(bool enable, std::string type);
    void enable_abort_button(bool enable);
    void update_subtask_name(QString name);
    void update_stage_value(QString stage, int val);
    void update_stage_value_with_machine(QString stage, int val, MachineObject* obj = nullptr);
    void on_stage_clicked(QMouseEvent& event);

    // Public interface to update remaining time text in the thermal dialog
    void update_progress_percent(QString percent, QString icon);
    void update_left_time(QString time);
    void update_finish_time(QString finish_time);
    void update_left_time(int mc_left_time);
    void show_layers_num(bool show) { m_staticText_layers->setVisible(show); }
    void update_layers_num(bool show, QString num = QString());
    void show_priting_use_info(bool show, QString time = QString(), QString weight = QString());
    void show_profile_info(bool show, QString profile = QString());
    void set_thumbnail_img(const QPixmap& bmp, const std::string& bmp_name);
    void set_brightness_value(int value) { m_brightness_value = value; }
    void set_plate_index(int plate_idx = -1);
    void market_scoring_show(bool show);
    bool is_market_scoring_show();

public:
    ScalableButton* get_abort_button() {return m_button_abort;};
    ScalableButton* get_pause_resume_button() {return m_button_pause_resume;};
    Button* get_partskip_button() { return m_button_partskip; };
    Button* get_market_scoring_button() {return m_button_market_scoring;};
    Button * get_market_retry_buttom() { return m_button_market_retry; };
    Button* get_clean_button() {return m_button_clean;};
    QLabel* get_bitmap_thumbnail() {return m_bitmap_thumbnail;};
    QWidget *  get_request_failed_panel() { return m_request_failed_panel; }
    int get_star_count() { return m_star_count; }
    void set_star_count(int star_count);
    std::vector<ScalableButton *> &get_score_star() { return m_score_star; }
    bool get_star_count_dirty() { return m_star_count_dirty; }
    void set_star_count_dirty(bool dirty) { m_star_count_dirty = dirty; }
    int get_part_skipped_count() { return m_part_skipped_count; }
    void set_part_skipped_count(int count) { m_part_skipped_count = count; }
    int get_part_skipped_dirty() { return m_part_skipped_dirty; }
    void set_part_skipped_dirty(int dirty) { m_part_skipped_dirty = dirty; }
    void                           set_has_reted_text(bool has_rated);

private:
    void paint(QPaintEvent&);
};

class StatusBasePanel : public QScrollArea
{
protected:
    QPixmap m_item_placeholder;
    ScalableBitmap m_thumbnail_placeholder;
    ScalableBitmap m_thumbnail_brokenimg;
    ScalableBitmap m_thumbnail_sdcard;
    QPixmap m_bitmap_item_prediction;
    QPixmap m_bitmap_item_cost;
    QPixmap m_bitmap_item_print;
    ScalableBitmap m_bitmap_speed;
    ScalableBitmap m_bitmap_speed_active;
    ScalableBitmap m_bitmap_axis_home;
    ScalableBitmap m_bitmap_lamp_on;
    ScalableBitmap m_bitmap_lamp_off;
    ScalableBitmap m_bitmap_fan_on;
    ScalableBitmap m_bitmap_fan_off;
    ScalableBitmap m_bitmap_use_time;
    ScalableBitmap m_bitmap_use_weight;
    QPixmap m_bitmap_extruder_empty_load;
    QPixmap m_bitmap_extruder_filled_load;
    QPixmap m_bitmap_extruder_empty_unload;
    QPixmap m_bitmap_extruder_filled_unload;
    QPixmap m_bitmap_extruder_now;

    CameraRecordingStatus m_state_recording{CameraRecordingStatus::RECORDING_NONE};
    CameraTimelapseStatus m_state_timelapse{CameraTimelapseStatus::TIMELAPSE_NONE};

    CameraItem *m_setting_button;
    CameraItem *m_camera_fullscreen_button{ nullptr };
    QBoxLayout *m_camera_media_sizer{ nullptr };
    CameraFullscreenFrame *m_camera_fullscreen_frame{ nullptr };

    QPixmap m_bitmap_camera;
    ScalableBitmap m_bitmap_sdcard_state_normal;
    ScalableBitmap m_bitmap_sdcard_state_abnormal;
    ScalableBitmap m_bitmap_sdcard_state_no;
    ScalableBitmap m_bitmap_recording_on;
    ScalableBitmap m_bitmap_recording_off;
    ScalableBitmap m_bitmap_timelapse_on;
    ScalableBitmap m_bitmap_timelapse_off;
    ScalableBitmap m_bitmap_vcamera_on;
    ScalableBitmap m_bitmap_vcamera_off;

    /* title panel */
    QWidget *       media_ctrl_panel;
    QWidget *       m_panel_monitoring_title;
    QWidget *       m_panel_printing_title;
    QWidget *       m_panel_control_title;

    QLabel*   m_staticText_consumption_of_time;
    QLabel *  m_staticText_consumption_of_weight;
    Label *         m_staticText_monitoring;
    QLabel *  m_staticText_timelapse;
    SwitchButton *  m_bmToggleBtn_timelapse;

    QLabel *m_mqtt_source;

    QLabel *m_bitmap_camera_img;
    QLabel *m_bitmap_recording_img;
    QLabel *m_bitmap_timelapse_img;
    QLabel* m_bitmap_vcamera_img;
    QLabel *m_bitmap_sdcard_img;
    QLabel *m_bitmap_static_use_time;
    QLabel *m_bitmap_static_use_weight;


    QWidget  /* wxMediaCtrl3 */ *  m_media_ctrl;
    MediaPlayCtrl * m_media_play_ctrl;

    Label *         m_staticText_printing;
    QLabel *m_bitmap_thumbnail;
    QLabel *  m_staticText_subtask_value;
    QLabel *  m_printing_stage_value;
    QLabel *  m_staticText_profile_value;
    ProgressBar*    m_gauge_progress;
    QLabel *  m_staticText_progress_percent;
    QLabel *  m_staticText_progress_percent_icon;
    QLabel *  m_staticText_progress_left;
    QLabel *  m_staticText_layers;
    Button *        m_button_report;
    Button *        m_button_partskip;
    ScalableButton *m_button_pause_resume;
    ScalableButton *m_button_abort;
    Button *        m_button_clean;
    QStackedWidget*   m_extruder_book;
    std::vector<ExtruderImage *> m_extruderImage;

    SwitchBoard *   m_nozzle_btn_panel;

    QLabel *  m_text_tasklist_caption;

    Label *  m_staticText_control;
    ImageSwitchButton *m_switch_lamp;
    int               m_switch_lamp_timeout{0};
    ImageSwitchButton *m_switch_speed;

    /* TempInput */
    QBoxLayout *    m_misc_ctrl_sizer;
    StaticBox*      m_fan_panel;
    StaticLine *    m_line_nozzle;
    TempInput*      m_tempCtrl_nozzle;
    int             m_temp_nozzle_timeout{ 0 };
    TempInput*      m_tempCtrl_nozzle_deputy;
    int             m_temp_nozzle_deputy_timeout{ 0 };
    TempInput *     m_tempCtrl_bed;
    int             m_temp_bed_timeout {0};
    TempInput *     m_tempCtrl_chamber;
    int             m_temp_chamber_timeout {0};
    FanSwitchButton *m_switch_nozzle_fan;
    int             m_switch_nozzle_fan_timeout{0};
    FanSwitchButton *m_switch_printing_fan;
    int             m_switch_printing_fan_timeout{0};
    FanSwitchButton *m_switch_cham_fan;
    FanSwitchButton *m_switch_fan;
    int             m_switch_cham_fan_timeout{0};
    QWidget*        m_switch_block_fan;
    int             m_nozzle_num{ 0 };
    int             m_current_nozzle_id{ 0 };

    float           m_fixed_aspect_ratio{1.8};

    AxisCtrlButton *m_bpButton_xy;
    Button *        m_bpButton_z_10;
    Button *        m_bpButton_z_1;
    Button *        m_bpButton_z_down_1;
    Button *        m_bpButton_z_down_10;
    QLabel *  m_staticText_z_tip;
    Label *         m_extruder_label;
    Button *        m_bpButton_e_10;
    Button *        m_bpButton_e_down_10;
    ExtruderSwithingStatus *m_extruder_switching_status;

    QWidget *       m_temp_temp_line;
    QWidget *       m_temp_extruder_line;
    bool            m_show_filament_group{ false };

    /* switch*/
    SwitchBoard*    m_ams_rack_switch;

    AMSControl*     m_ams_control;
    StaticBox*      m_ams_control_box;
    QLabel *m_ams_extruder_img;
    QLabel* m_bitmap_extruder_img;

    wgtDeviceNozzleRack* m_panel_nozzle_rack{ nullptr };

    QWidget *       m_panel_separator_right;
    QWidget *       m_panel_separotor_bottom;
    QGridLayout *m_tasklist_info_sizer{nullptr};
    QBoxLayout *    m_printing_sizer;
    QBoxLayout *    m_tasklist_sizer;
    QBoxLayout *    m_tasklist_caption_sizer;
    QWidget*        m_panel_error_txt;
    QWidget*        m_staticline;
    Label *         m_error_text;
    QLabel*   m_staticText_calibration_caption;
    QLabel*   m_staticText_calibration_caption_top;
    QLabel*   m_calibration_text;
    Button*         m_parts_btn;
    Button*         m_options_btn;
    Button*         m_safety_btn;
    Button*         m_calibration_btn;
    StepIndicator*  m_calibration_flow;

    QWidget *       m_machine_ctrl_panel;
    QWidget *       m_scale_panel;
    QLabel* m_img_filament_loading;
    PrintingTaskPanel *       m_project_task_panel;

    FilamentLoad* m_filament_step;
    QLabel *m_filament_load_img;

    Button *m_button_retry {nullptr};
    Button *m_fila_change_abort {nullptr};
    StaticBox* m_filament_load_box;

    // Virtual event handlers, override them in your derived class
    virtual void on_subtask_partskip(QEvent &event) {}
    virtual void on_subtask_pause_resume(QEvent &event) {}
    virtual void on_subtask_abort(QEvent &event) {}
    virtual void on_lamp_switch(QEvent &event) {}
    virtual void on_bed_temp_kill_focus(QFocusEvent &event) {}
    virtual void on_bed_temp_set_focus(QFocusEvent &event) {}
    virtual void on_nozzle_temp_kill_focus(QFocusEvent &event) {}
    virtual void on_nozzle_temp_set_focus(QFocusEvent &event) {}
    virtual void on_nozzle_fan_switch(QEvent &event) {}
    virtual void on_printing_fan_switch(QEvent &event) {}
    virtual void on_axis_ctrl_z_up_10(QEvent &event) {}
    virtual void on_axis_ctrl_z_up_1(QEvent &event) {}
    virtual void on_axis_ctrl_z_down_1(QEvent &event) {}
    virtual void on_axis_ctrl_z_down_10(QEvent &event) {}
    virtual void on_axis_ctrl_e_up_10(QEvent &event) {}
    virtual void on_axis_ctrl_e_down_10(QEvent &event) {}
    virtual void on_nozzle_selected(QEvent &event) {}

public:
    StatusBasePanel(QWidget *      parent,
                    int      id    = -1,
                    const QPoint & pos   = QPoint(),
                    const QSize &  size  = QSize(),
                    long            style = 0,
                    const QString &name  = QString());

    ~StatusBasePanel();

    MachineObject* obj{ nullptr };
    void init_bitmaps();
    QBoxLayout *create_monitoring_page();
    QBoxLayout *create_machine_control_page(QWidget *parent);

    QBoxLayout *create_temp_axis_group(QWidget *parent);
    QBoxLayout *create_temp_control(QWidget *parent);
    QBoxLayout *create_misc_control(QWidget *parent);
    QBoxLayout *create_axis_control(QWidget *parent);
    QWidget *create_bed_control(QWidget *parent);
    QBoxLayout *create_extruder_control(QWidget *parent);

    void reset_temp_misc_control();
    int before_error_code = 0;
    int skip_print_error = 0;
    StaticBox*  create_ams_group(QWidget *parent);
    QBoxLayout* create_settings_group(QWidget *parent);
    QBoxLayout* create_filament_group(QWidget* parent);

	void           expand_filament_loading(QMouseEvent &e);
    void           show_ams_group(bool show = true);
    void show_filament_load_group(bool show = true);
    MediaPlayCtrl* get_media_play_ctrl() {return m_media_play_ctrl;};

    void jump_to_Rack();

    bool can_show_camera_fullscreen() const;
    bool is_camera_fullscreen() const;
    void toggle_camera_fullscreen();
    void close_camera_fullscreen();
    void on_camera_fullscreen(QMouseEvent& event);

private:
    void on_ams_rack_switch(QEvent& event);
    void show_camera_fullscreen();
};

class StatusPanel : public StatusBasePanel
{
private:
    friend class MonitorPanel;

protected:
    std::shared_ptr<SliceInfoPopup> m_slice_info_popup;
    std::shared_ptr<ImageTransientPopup> m_image_popup;
    std::shared_ptr<CameraPopup> m_camera_popup;
    std::set<int> rated_model_id;
    AMSSetting *m_ams_setting_dlg{nullptr};
    PrinterPartsDialog*  print_parts_dlg { nullptr };
    PrintOptionsDialog*  print_options_dlg { nullptr };
    SafetyOptionsDialog* safety_options_dlg { nullptr };
    CalibrationDialog*   calibration_dlg {nullptr};
    AMSMaterialsSetting *m_filament_setting_dlg{nullptr};

    DeviceErrorDialog* m_print_error_dlg = nullptr;
    SecondaryCheckDialog* abort_dlg = nullptr;
    SecondaryCheckDialog* con_load_dlg = nullptr;
    MessageDialog *       ctrl_e_hint_dlg             = nullptr;

    SecondaryCheckDialog* sdcard_hint_dlg = nullptr;
    SecondaryCheckDialog* axis_go_home_dlg = nullptr;

    FanControlPopupNew* m_fan_control_popup{nullptr};

    ExtrusionCalibration *m_extrusion_cali_dlg{nullptr};
    PartSkipDialog       *m_partskip_dlg{nullptr};

    QString     m_request_url;
    bool         m_start_loading_thumbnail = false;
    bool         m_load_sdcard_thumbnail = false;
    int          m_last_sdcard    = -1;
    int          m_last_recording = -1;
    int          m_last_timelapse = -1;
    int          m_last_extrusion = -1;
    int          m_last_vcamera   = -1;
    int          m_model_mall_request_count = 0;
    bool         m_is_load_with_temp = false;
    json         m_rating_result;

    QNetworkReply*          web_request{nullptr};
    QNetworkAccessManager*  m_net_manager{nullptr};
    bool bed_temp_input    = false;
    bool nozzle_temp_input = false;
    bool cham_temp_input   = false;
    bool request_model_info_flag = false;
    int speed_lvl = 1; // 0 - 3
    int speed_lvl_timeout {0};
    boost::posix_time::ptime speed_dismiss_time;
    bool m_show_mode_changed = false;
    std::map<QString, QImage> img_list; // key: url, value: QPixmap png Image
    std::map<std::string, std::string> m_print_connect_types;
    std::vector<Button *>       m_buttons;
    int last_status;
    ScoreData *m_score_data;
    QPixmap* calib_bitmap = nullptr;
    CalibMode m_calib_mode;
    CalibrationMethod m_calib_method;
    int cali_stage;
    PrintingTaskType m_current_print_mode = PrintingTaskType::NOT_CLEAR;

    void init_scaled_buttons();
    void create_tasklist_info();
    void show_task_list_info(bool show = true);
    void update_tasklist_info();

    void on_market_scoring(QEvent &event);
    void on_market_retry(QEvent &event);
    void on_subtask_partskip(QEvent &event);
    void on_subtask_pause_resume(QEvent &event);
    void on_subtask_abort(QEvent &event);
    void on_print_error_clean(QEvent &event);
    void error_info_reset();
    void show_recenter_dialog();

    /* axis control */
    bool check_axis_z_at_home(MachineObject* obj);
    void on_axis_ctrl_xy(QEvent &event);
    void on_axis_ctrl_z_up_10(QEvent &event);
    void on_axis_ctrl_z_up_1(QEvent &event);
    void on_axis_ctrl_z_down_1(QEvent &event);
    void on_axis_ctrl_z_down_10(QEvent &event);
    void on_axis_ctrl_e_up_10(QEvent &event);
    void on_axis_ctrl_e_down_10(QEvent &event);
    void axis_ctrl_e_hint(bool up_down);

    void on_nozzle_selected(QEvent &event);
    /* temp control */
    void on_bed_temp_kill_focus(QFocusEvent &event);
    void on_bed_temp_set_focus(QFocusEvent &event);
    void on_set_bed_temp();
    void on_nozzle_temp_kill_focus(QFocusEvent &event);
    void on_nozzle_temp_set_focus(QFocusEvent &event);
    void on_set_nozzle_temp(int nozzle_id);
    void on_set_chamber_temp();

    /* extruder apis */
    void on_ams_load(QEvent &event);
    void update_load_with_temp();
    void on_ams_load_curr();
    void on_ams_load_vams(QEvent& event);
    void on_ams_switch(QEvent &event);
    void on_ams_unload(QEvent &event);
    void on_ams_filament_backup(QEvent& event);
    void on_ams_setting_click(QEvent& event);
    void on_filament_edit(QEvent &event);
    void on_ext_spool_edit(QEvent &event);
    void on_filament_extrusion_cali(QEvent &event);
    void on_ams_refresh_rfid(QEvent &event);
    void on_ams_selected(QEvent &event);
    void on_ams_guide(QEvent &event);
    void on_ams_retry(QEvent &event);

    void on_fan_changed(QEvent& event);
    void on_cham_temp_kill_focus(QFocusEvent& event);
    void on_cham_temp_set_focus(QFocusEvent& event);
    void on_switch_speed(QEvent& event);
    void on_lamp_switch(QEvent &event);
    void on_printing_fan_switch(QEvent &event);
    void on_nozzle_fan_switch(QEvent &event);
    void on_thumbnail_enter(QMouseEvent &event);
    void on_thumbnail_leave(QMouseEvent &event);
    void refresh_thumbnail_webrequest(QMouseEvent& event);
    void on_switch_vcamera(QEvent &event);
    void on_camera_enter(QMouseEvent &event);
    void on_camera_leave(QMouseEvent& event);

    void on_show_parts_options(QEvent& event);
    /* print options */
    void on_show_print_options(QEvent &event);
    /* safety options */
    void on_show_safety_options(QEvent &event);

    /* calibration */
    void on_start_calibration(QEvent &event);


    /* update apis */
    void update(MachineObject* obj);

    void show_printing_status(bool ctrl_area = true, bool temp_area = true);
    void update_left_time(int mc_left_time);
    void update_basic_print_data(bool def = false);
    void update_model_info();
    void update_subtask(MachineObject* obj);
    void update_partskip_subtask(MachineObject *obj);
    void update_cloud_subtask(MachineObject *obj);
    void update_sdcard_subtask(MachineObject *obj);
    void update_temp_ctrl(MachineObject *obj);
    void update_misc_ctrl(MachineObject *obj);
    void update_ams(MachineObject* obj);
    void update_filament_loading_panel(MachineObject* obj);

    void update_extruder_status(MachineObject* obj);
    void update_ams_control_state(std::string ams_id, std::string slot_id);
    void update_rack(MachineObject* obj);
    void update_cali(MachineObject* obj);
    void update_calib_bitmap();

    void update_market_scoring(bool show);
    void reset_printing_values();
    void on_webrequest_state(QNetworkReply* reply);
    bool is_task_changed(MachineObject* obj);

    /* camera */
    void update_camera_state(MachineObject* obj);
    bool show_vcamera = false;

    // partskip button
    void update_partskip_button(MachineObject* obj);

    // printer parts options
    void update_printer_parts_options(MachineObject* obj);

    //get tray name
    QString getTrayName(const std::string amsID, const std::string slotID);

public:
    void update_error_message();

public:
    StatusPanel(QWidget *      parent,
                int      id    = -1,
                const QPoint & pos   = QPoint(),
                const QSize  & size  = QSize(),
                long            style = 0,
                const QString &name  = QString());
    ~StatusPanel();

    enum ThumbnailState {
        PLACE_HOLDER = 0,
        BROKEN_IMG = 1,
        TASK_THUMBNAIL = 2,
        SDCARD_THUMBNAIL = 3,
        STATE_COUNT = 4
    };

    BBLSubTask *   last_subtask{nullptr};
    std::string    last_profile_id;
    std::string    last_task_id;
    long           last_tray_exist_bits { -1 };
    long           last_ams_exist_bits { -1 };
    long           last_tray_is_bbl_bits{ -1 };
    long           last_read_done_bits{ -1 };
    long           last_reading_bits { -1 };
    long           last_ams_version { -1 };
    std::optional<int> last_cali_version;

    enum ThumbnailState task_thumbnail_state {ThumbnailState::PLACE_HOLDER};
    std::vector<int> last_stage_list_info;

    bool is_stage_list_info_changed(MachineObject* obj);

    void set_default();
    void show_status(int status);
    void set_hold_count(int& count);

    void rescale_camera_icons();
    void on_sys_color_changed();
    void msw_rescale();
};
}
}
#endif

#ifndef _SyncAmsInfo_DIALOG_H_
#define _SyncAmsInfo_DIALOG_H_
#include <QWidget>
#include <QString>

#include <future>
#include <thread>
#include "GUI_App.hpp"
#include "GUI_Utils.hpp"

#include "SelectMachine.hpp"
#include "DeviceManager.hpp"
#include "BaseTransparentDPIFrame.hpp"
class Button;
class CheckBox;
class Label;
namespace Slic3r { namespace GUI {
class CapsuleButton;
class SyncAmsInfoDialog : public DPIDialog
{
    enum PageType { ptColorMap = 0, ptOverride };
    bool              m_only_exist_ext_spool_flag{false};
    int               m_current_filament_id{0};
    int               m_print_plate_idx{0};
    int               m_print_plate_total{0};
    int               m_timeout_count{0};
    int               m_print_error_code{0};
    bool              m_is_in_sending_mode{false};
    bool              m_ams_mapping_res{false};
    bool              m_ams_mapping_valid{false};
    bool              m_export_3mf_cancel{false};
    bool              m_is_canceled{false};
    bool              m_is_rename_mode{false};
    bool              m_check_flag{false};
    PrintPageMode     m_print_page_mode{PrintPageMode::PrintPageModePrepare};
    std::string       m_print_error_msg;
    std::string       m_print_error_extra;
    std::string       m_print_info;
    QString          m_current_project_name;
    PrintDialogStatus m_print_status{PrintStatusInit};
    QColor          m_colour_def_color{QColor(255, 255, 255)};
    QColor          m_colour_bold_color{QColor(38, 46, 48)};

    std::shared_ptr<int>                 m_token = std::make_shared<int>(0);
    std::map<std::string, PrintOption *> m_checkbox_list;
    std::vector<QString>                m_bedtype_list;
    std::vector<MachineObject *>         m_list;
    std::vector<FilamentInfo>            m_filaments;
    std::vector<FilamentInfo>            m_ams_mapping_result;
    std::vector<FilamentInfo>            m_back_ams_mapping_result;
    std::vector<int>                     m_filaments_map;


    Slic3r::DynamicPrintConfig m_required_data_config;
    Slic3r::Model              m_required_data_model;
    Slic3r::PlateDataPtrs      m_required_data_plate_data_list;
    std::string                m_required_data_file_name;
    std::string                m_required_data_file_path;

    std::vector<POItem> ops_auto;
    std::vector<POItem> ops_no_auto;

protected:
    PrintFromType     m_print_type{FROM_NORMAL};
    AmsMapingPopup    m_mapping_popup{nullptr};
    AmsMapingTipPopup m_mapping_tip_popup{nullptr};
    AmsTutorialPopup  m_mapping_tutorial_popup{nullptr};
    MaterialHash      m_materialList;
    Plater *          m_plater{nullptr};
    QWidget *         m_options_other{nullptr};
    QBoxLayout *      m_sizer_options_timelapse{nullptr};
    QBoxLayout *      m_sizer_options_other{nullptr};
    QBoxLayout *      m_sizer_thumbnail{nullptr};

    QScrollArea *        m_scrolledWindow{nullptr};
    QBoxLayout *              m_sizer_main{nullptr};
    QBoxLayout *              m_sizer_this{nullptr};

    QBoxLayout *              m_basicl_sizer{nullptr};
    QBoxLayout *              m_sizer_autorefill{nullptr};

    QLabel *          m_staticbitmap{nullptr};
    QLabel *          img_amsmapping_tip{nullptr};
    ThumbnailPanel *          m_thumbnailPanel{nullptr};
    QWidget *                 m_panel_status{nullptr};
    QWidget *                 m_basic_panel;
    QWidget *                 m_line_top{nullptr};
    Label *                   m_ams_backup_tip{nullptr};
    Label *                   m_statictext_ams_msg{nullptr};
    Label *                   m_text_printer_msg{nullptr};
    QLabel *            m_staticText_bed_title{nullptr};
    QLabel *            m_original_in_colormap{nullptr};
    QLabel *            m_original_in_override{nullptr};
    QLabel *            m_ams_or_ext_text_in_colormap{nullptr};
    QLabel *            m_ams_or_ext_text_in_override{nullptr};
    QTimer *                 m_refresh_timer{nullptr};

    ScalableBitmap *          ams_mapping_help_icon{nullptr};
    QLabel *          img_ams_backup{nullptr};
    ThumbnailData             m_cur_input_thumbnail_data;
    ThumbnailData             m_cur_no_light_thumbnail_data;
    ThumbnailData             m_preview_thumbnail_data; // when ams map change
    std::vector<QColor>     m_preview_colors_in_thumbnail;
    std::vector<QColor>     m_cur_colors_in_thumbnail;
    std::vector<QColor>     m_back_cur_colors_in_thumbnail;
    std::vector<bool>         m_edge_pixels;

    StaticBox * m_two_image_panel{nullptr};
    QBoxLayout *m_two_image_panel_sizer{nullptr};
    StaticBox *m_two_thumbnail_panel{nullptr};
    QBoxLayout *m_two_thumbnail_panel_sizer{nullptr};
    QBoxLayout *m_choose_plate_sizer{nullptr};
    ComboBox *  m_combobox_plate{nullptr};
    //TextInput *m_plate_number{nullptr};
    QStringList    m_plate_number_choices_str;
    std::vector<int> m_plate_choices;
    ScalableButton * m_swipe_left_button{nullptr};
    ScalableButton * m_swipe_right_button{nullptr};
    bool             m_swipe_left_button_enable;
    bool             m_swipe_right_button_enable;
    int              m_bmp_pix_cont = 32;
    ScalableBitmap   m_swipe_left_bmp_disable;
    ScalableBitmap   m_swipe_left_bmp_normal;
    ScalableBitmap   m_swipe_left_bmp_hover;
    ScalableBitmap   m_swipe_right_bmp_disable;
    ScalableBitmap   m_swipe_right_bmp_normal;
    ScalableBitmap   m_swipe_right_bmp_hover;

    StaticBox *m_filament_panel{nullptr};
    StaticBox *m_filament_left_panel{nullptr};
    StaticBox *m_filament_right_panel{nullptr};

    StaticBox * m_fix_filament_panel{nullptr};
    QGridLayout *m_fix_sizer_ams_mapping{nullptr};
    MaterialHash     m_fix_materialList;
    QBoxLayout *     m_fix_filament_panel_sizer{nullptr};
    std::vector<FilamentInfo> m_fix_filaments;
    bool             m_generate_fix_sizer_ams_mapping = false;

    QBoxLayout *m_filament_panel_sizer;
    QBoxLayout *m_filament_panel_left_sizer;
    QBoxLayout *m_filament_panel_right_sizer;
    QBoxLayout *m_sizer_filament_2extruder;

    QGridLayout *m_sizer_ams_mapping{nullptr};
    QGridLayout *m_sizer_ams_mapping_left{nullptr};
    QGridLayout *m_sizer_ams_mapping_right{nullptr};

    QStackedWidget *m_pages{nullptr};
    QWidget *     m_loading_page{nullptr};
    QWidget *     m_show_page{nullptr};
    QBoxLayout *  m_sizer_show_page{nullptr};
    QLabel *m_gif_ctrl{nullptr};

public:
    void check_empty_project();
    void reinit_dialog();
    void init_bind();
    void init_timer();
    void show_print_failed_info(bool show, int code = 0, QString description = QString(), QString extra = QString());
    void check_fcous_state(QWidget *window);
    void popup_filament_backup();

    void     prepare_mode(bool refresh_button = true);
    void     finish_mode();
    void     sync_ams_mapping_result(std::vector<FilamentInfo> &result);
    void     prepare(int print_plate_idx);
    void     show_status(PrintDialogStatus status, std::vector<QString> params = std::vector<QString>());
    void     reset_timeout();
    void     update_user_printer();
    void     reset_ams_material();
    void     reset_all_ams_info();
    void     reset_one_ams_material(const std::string & index_str,bool reset_to_first =false);
    void     update_show_status();
    void     update_printer_combobox(QEvent &event);
    void     on_cancel(QCloseEvent &event);
    void     show_errors(QString &info);
    void     Enable_Auto_Refill(bool enable);
    void     on_refresh(QEvent &event);
    void     on_set_finish_mapping(QEvent &evt);
    void     on_print_job_cancel(QEvent &evt);
    void     reset_and_sync_ams_list();
    void     generate_override_fix_ams_list();
    void     clone_thumbnail_data();
    void     record_edge_pixels_data();
    QColor adjust_color_for_render(const QColor &color);
    void     final_deal_edge_pixels_data(ThumbnailData &data);
    void     updata_thumbnail_data_after_connected_printer();
    void     show_ams_controls(bool flag);
    void     show_advanced_settings(bool flag, bool update_layout = true);
    void     update_thumbnail_data_accord_plate_index(bool allow_clone_ams_color);
    void     update_final_thumbnail_data();
    void     unify_deal_thumbnail_data(ThumbnailData &input_data, ThumbnailData &no_light_data,bool allow_clone_ams_color);
    void     change_default_normal(int old_filament_id, QColor temp_ams_color);
    void     on_timer(QTimerEvent &event);
    void     update_user_machine_list();
    void     update_lan_machine_list();
    void     stripWhiteSpace(std::string &str);
    void     update_ams_status_msg(QString msg, bool is_warning = false);
    void     update_priner_status_msg(QString msg, bool is_warning = false);
    void     update_print_status_msg(QString msg, bool is_warning = false, bool is_printer = true);
    void     update_print_error_info(int code, std::string msg, std::string extra);
    bool     has_timelapse_warning();
    void     update_timelapse_enable_status();
    bool     is_same_printer_model();
    bool     is_blocking_printing(MachineObject *obj_);
    bool     is_same_nozzle_diameters(NozzleType &tag_nozzle_type, float &nozzle_diameter);
    bool     is_same_nozzle_type(std::string &filament_type, NozzleType &tag_nozzle_type);
    bool     is_timeout();
    int  update_print_required_data(Slic3r::DynamicPrintConfig config, Slic3r::Model model, Slic3r::PlateDataPtrs plate_data_list, std::string file_name, std::string file_path);
    void set_print_type(PrintFromType type) { m_print_type = type; };
    bool has_selector(MachineObject *obj_) const;
    bool do_ams_mapping(MachineObject *obj_);
    void deal_only_exist_ext_spool(MachineObject *obj_);
    void show_thumbnail_page();
    bool get_ams_mapping_result(std::string &mapping_array_str, std::string &mapping_array_str2, std::string &ams_mapping_info);
    bool build_nozzles_info(std::string &nozzles_info);
    void auto_supply_with_ext(std::vector<DevAmsTray> slots);
    bool is_nozzle_type_match(DevExtderSystem data, QString &error_message) const;
    int  convert_filament_map_nozzle_id_to_task_nozzle_id(int nozzle_id);

    PrintFromType            get_print_type() { return m_print_type; };
    QString                 format_text(QString &m_msg);
    PrintDialogStatus        get_status() { return m_print_status; }
    std::vector<std::string> sort_string(std::vector<std::string> strArray);

    const std::vector<FilamentInfo> &get_ams_mapping_result() { return m_ams_mapping_result; }

public:
    struct SyncInfo
    {
        QPoint dialog_pos;
        bool use_dialog_pos = false;
        bool first_sync = false;
        bool cancel_text_to_later = false;
    };
    struct SyncResult
    {
        bool direct_sync = true;
        bool is_same_printer = true;
        std::map<int, AMSMapInfo> sync_maps;
    };
    SyncAmsInfoDialog(QWidget *parent, SyncInfo &info);
    ~SyncAmsInfoDialog();
    void set_info(SyncInfo &info);
    void on_dpi_changed(const QRect &suggested_rect) override;
    const SyncResult &get_result() { return m_result; }

public:
    void setVisible(bool show) override;
    void updata_ui_data_after_connected_printer();
    void set_default(bool hide_some = false);
    void update_select_layout(MachineObject *obj);
    void set_default_normal(const ThumbnailData &);
    bool is_must_finish_slice_then_connected_printer() ;
    void hide_no_use_controls();
    void show_sizer(QLayout *sizer, bool show);
    void deal_ok();
    bool get_is_double_extruder();
    bool is_dirty_filament();
    bool is_need_show();
    void set_check_dirty_fialment(bool flag) { m_check_dirty_fialment = flag; };

private:
    QBoxLayout *create_sizer_thumbnail(QPushButton *image_button, bool left);
    void        update_when_change_plate(int);
    void        update_when_change_map_mode(int);
    void        update_plate_combox();
    void        update_map_when_change_map_mode();
    QColor    decode_ams_color(const std::string &color);
    void        update_when_change_map_mode(QEvent &e);
    void        update_panel_status(PageType page);
    void        show_color_panel(bool,bool update_layout = true);
    void        update_more_setting(bool layout = true,bool from_more_seting_text = false);
    void        add_two_image_control();
    void        to_next_plate(QEvent &event);
    void        to_previous_plate(QEvent &event);
    void        update_swipe_button_state();
    void        updata_ui_when_priner_not_same();
    void        init_bitmaps();

private:
    SyncInfo   m_input_info;
    SyncResult m_result;
    Button *   m_button_ok     = nullptr;
    Button *   m_button_cancel = nullptr;

    AMSComboInfo  m_ams_combo_info;
    Label *m_after_map_text{nullptr};
    QLabel *m_attention_text{nullptr};
    QString m_tip_attention_color_map;
    QString m_tip_attention_override;
    Label* m_tip_text{nullptr};
    //QLabel *m_specify_color_cluster_title = nullptr;
    //QLabel*  m_used_colors_tip_text{nullptr};
    QLabel* m_warning_text{nullptr};
    QBoxLayout *  m_left_sizer_thumbnail{nullptr};
    QBoxLayout *  m_right_sizer_thumbnail{nullptr};
    QPushButton *      m_left_image_button = nullptr;
    QPushButton *      m_right_image_button = nullptr;
    QBoxLayout *    sizer_basic_right_info = nullptr;
    QBoxLayout *    sizer_advanced_options_title = nullptr;
    Label *  m_confirm_title                = nullptr;
    QString        m_undone_str;
    QString        m_override_undone_str;
    QLabel *  m_are_you_sure_title                = nullptr;

   // QBoxLayout *    m_plate_combox_sizer          = nullptr;
    QBoxLayout *    m_mode_combox_sizer           = nullptr;
    //QLabel *  m_printer_title                = nullptr;
    QLabel *  m_printer_device_name          = nullptr;
    QLabel *  m_printer_is_map_title         = nullptr;

    CapsuleButton *  m_colormap_btn = nullptr;
    CapsuleButton *  m_override_btn = nullptr;
    ScalableButton * m_reset_all_btn{nullptr};
    QBoxLayout *     m_advace_setting_sizer = nullptr;
    QLabel *   m_more_setting_tips = nullptr;
    QLabel * m_advanced_options_icon{nullptr};
    QBoxLayout *     m_append_color_sizer = nullptr;
    ::CheckBox*      m_append_color_checkbox = nullptr;
    Label *          m_append_color_text = nullptr;
    QBoxLayout *     m_merge_color_sizer     = nullptr;
    ::CheckBox*      m_merge_color_checkbox = nullptr;
    Label *          m_merge_color_text     = nullptr;
    bool m_is_empty_project = true;

    bool m_check_dirty_fialment  = true;
    bool m_expand_more_settings  = true;
    bool m_image_is_top          = false;

    const int LEFT_THUMBNAIL_SIZE_WIDTH = 100;
    const int RIGHT_THUMBNAIL_SIZE_WIDTH = 300;
    int      m_specify_plate_idx{0};
    QString m_printer_name;

    enum class MapModeEnum {
        ColorMap = 0,
        Override,
    };
    MapModeEnum m_map_mode{MapModeEnum::ColorMap};
};

class SyncNozzleAndAmsDialog : public Slic3r::GUI::BaseTransparentDPIFrame
{
public:
    struct InputInfo
    {
        QPoint dialog_pos{QPoint(400, 200)};
        QPoint ams_btn_pos{QPoint(400, 200)};
    };
    SyncNozzleAndAmsDialog(InputInfo &input_info);
    ~SyncNozzleAndAmsDialog() override;
    void deal_ok() override;
    void deal_cancel() override;
    void update_info(InputInfo& info);

private:
    InputInfo m_input_info;
};

class FinishSyncAmsDialog : public Slic3r::GUI::BaseTransparentDPIFrame
{
public:
    struct InputInfo
    {
        QPoint dialog_pos{QPoint(400, 200)};
        QPoint ams_btn_pos{QPoint(400, 200)};
    };
    FinishSyncAmsDialog(InputInfo &input_info);
    ~FinishSyncAmsDialog() override;
    void deal_ok();
    void update_info(InputInfo &info);

    void set_custom_message(const QString &message)
    {
        if (m_finish_text) {
            m_finish_text->setText(message);
            if (layout()) layout()->activate();
            adjustSize();
        }
    }

    void set_disappearance_mode(DisappearanceMode mode)
    {
        m_timed_disappearance_mode = mode;
    }

private:
    InputInfo m_input_info;
};

class ExtruderWarningDialog : public Slic3r::GUI::BaseTransparentDPIFrame
{
public:
    struct InputInfo
    {
        QPoint dialog_pos{QPoint(400, 200)};
        QPoint ams_btn_pos{QPoint(400, 200)};
    };
    ExtruderWarningDialog(InputInfo &input_info, std::string icon_name = "warning");
    ~ExtruderWarningDialog() override;
    void deal_ok();
    void update_info(InputInfo &info);

    void set_custom_message(const QString &message)
    {
        if (m_finish_text) {
            m_finish_text->setText(message);
            if (layout()) layout()->activate();
            adjustSize();
        }
    }

private:
    InputInfo m_input_info;
};

}}     // namespace Slic3r::GUI
#endif  // _STEP_MESH_DIALOG_H_

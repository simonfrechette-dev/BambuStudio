#ifndef slic3r_GUI_CalibrationWizardPresetPage_hpp_
#include <QRadioButton>
#define slic3r_GUI_CalibrationWizardPresetPage_hpp_

#include "CalibrationWizardPage.hpp"
#include "DeviceCore/DevNozzleSystem.h"

namespace Slic3r { namespace GUI {

enum CaliPresetStage {
    CALI_MANULA_STAGE_NONE = 0,
    CALI_MANUAL_STAGE_1,
    CALI_MANUAL_STAGE_2,
};

enum FlowRatioCaliSource {
    FROM_PRESET_PAGE = 0,
    FROM_COARSE_PAGE,
};

class CalibrationPresetPage;

class CaliPresetCaliStagePanel : public QWidget
{
public:
    CaliPresetCaliStagePanel(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);
    void create_panel(QWidget* parent);

    void msw_rescale();

    void set_cali_stage(CaliPresetStage stage, float value);
    void get_cali_stage(CaliPresetStage& stage, float& value);

    void set_flow_ratio_value(float flow_ratio);
    void set_parent(CalibrationPresetPage* parent) { m_stage_panel_parent = parent; }
    void set_flow_ratio_calibration_type(FlowRatioCalibrationType type);
protected:
    CaliPresetStage m_stage;
    QBoxLayout*   m_top_sizer;
    QRadioButton* m_complete_radioBox;
    QRadioButton* m_fine_radioBox;
    TextInput *    flow_ratio_input;
    QWidget*       input_panel;
    float m_flow_ratio_value;
    CalibrationPresetPage* m_stage_panel_parent;
};

class CaliComboBox : public QWidget
{
public:
    CaliComboBox(QWidget *parent,
        QString title,
        QStringList values,
        int default_index = 0,  // default delected id
        std::function<void(QEvent &)> on_value_change = nullptr,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    int get_selection() const;
    QString get_value() const;
    void set_values(const QStringList& values);

private:
    QBoxLayout* m_top_sizer;
    QString m_title;
    ComboBox* m_combo_box;
    std::function<void(QEvent&)> m_on_value_change_call_back;
};

class CaliPresetWarningPanel : public QWidget
{
public:
    CaliPresetWarningPanel(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_panel(QWidget* parent);

    void set_warning(QString text);

    void set_color(QColor color);

protected:
    QBoxLayout*   m_top_sizer;
    Label* m_warning_text;
};

class CaliPresetTipsPanel : public QWidget
{
public:
    CaliPresetTipsPanel(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_panel(QWidget* parent);

    void set_params(int nozzle_temp, int bed_temp, float max_volumetric);
    void get_params(int& nozzle_temp, int& bed_temp, float& max_volumetric);
protected:
    QBoxLayout*     m_top_sizer;
    TextInput*      m_nozzle_temp;
    Label*   m_bed_temp;
    TextInput*      m_max_volumetric_speed;
};

class CaliPresetCustomRangePanel : public QWidget
{
public:
    CaliPresetCustomRangePanel(QWidget* parent,
        int input_value_nums = 3,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_panel(QWidget* parent);

    void msw_rescale();

    void set_unit(QString unit);
    void set_titles(QStringList titles);
    void set_values(QStringList values);
    QStringList get_values();

protected:
    QBoxLayout*     m_top_sizer;
    int                       m_input_value_nums;
    std::vector<Label*> m_title_texts;
    std::vector<TextInput*>    m_value_inputs;
};

enum CaliPresetPageStatus
{
    CaliPresetStatusInit = 0,
    CaliPresetStatusNormal,
    CaliPresetStatusNoUserLogin,
    CaliPresetStatusInvalidPrinter,
    CaliPresetStatusConnectingServer,
    CaliPresetStatusInUpgrading,
    CaliPresetStatusInSystemPrinting,
    CaliPresetStatusInPrinting,
    CaliPresetStatusLanModeNoSdcard,
    CaliPresetStatusNoSdcard,
    CaliPresetStatusNeedForceUpgrading,
    CaliPresetStatusNeedConsistencyUpgrading,
    CaliPresetStatusUnsupportedPrinter,
    CaliPresetStatusInConnecting,
    CaliPresetStatusFilamentIncompatible,
    CaliPresetStatusLanModeSDcardNotAvailable,
    CaliPresetStatusDifferentNozzleDiameters
};

struct CaliFilamentInfo
{
    Preset *    filament_preset{nullptr};
    int         nozzle_pos_id{-1};
    std::string nozzle_sn;

    int         extruder_id;
    int         tray_id;

    CaliFilamentInfo() {}
    CaliFilamentInfo(Preset *preset, int nozzle_pos_id_, const std::string &nozzle_sn_, int extruder_id_, int tray_id_)
        : filament_preset(preset), nozzle_pos_id(nozzle_pos_id_), nozzle_sn(nozzle_sn_), extruder_id(extruder_id_), tray_id(tray_id_)
    {}
};

class CalibrationPresetPage : public CalibrationWizardPage
{
public:
    CalibrationPresetPage(QWidget* parent,
        CalibMode cali_mode,
        bool custom_range = false,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_page(QWidget* parent);

    void update_print_status_msg(QString msg, bool is_warning);
    QString format_text(QString& m_msg);
    void stripWhiteSpace(std::string& str);
    void update_priner_status_msg(QString msg, bool is_warning);
    void update(MachineObject* obj) override;
    void update_flow_ratio_type(FlowRatioCalibrationType type);

    void on_device_connected(MachineObject* obj) override;

    void update_print_error_info(int code, const std::string& msg, const std::string& extra) { m_sending_panel->update_print_error_info(code, msg, extra); }

    void set_cali_filament_mode(CalibrationFilamentMode mode) override;

    void set_cali_method(CalibrationMethod method) override;

    void on_cali_start_job();

    void on_cali_finished_job();

    void on_cali_cancel_job();

    void init_with_machine(MachineObject* obj);

    void sync_ams_info(MachineObject* obj);

    void select_default_compatible_filament();

    int get_index_by_extruder_tray_id(int extruder_id, int tray_id);

    std::vector<FilamentComboBox*> get_selected_filament_combobox();

    // key is tray_id
    std::vector<CaliFilamentInfo> get_selected_filaments();

    std::map<int, DynamicPrintConfig> get_filament_ams_list() const { return m_filament_ams_list; }

    void get_preset_info(
        float& nozzle_dia,
        BedType& plate_type);

    void get_cali_stage(CaliPresetStage& stage, float& value);

    std::shared_ptr<ProgressIndicator> get_sending_progress_bar() {
        return m_sending_panel->get_sending_progress_bar();
    }

    Preset* get_printer_preset(MachineObject* obj, float nozzle_value);
    Preset* get_print_preset();
    std::string get_print_preset_name();

    QStringList get_custom_range_values();
    CalibMode     get_pa_cali_method();

    CaliPresetPageStatus get_page_status() { return m_page_status; }
    MachineObject* get_current_object() { return curr_obj; }
    void msw_rescale() override;
    void on_sys_color_changed() override;

    float get_nozzle_diameter(int extruder_id) const;
    NozzleVolumeType get_nozzle_volume_type(int extruder_id) const;
    ExtruderType get_extruder_type(int extruder_id) const;

protected:
    void create_selection_panel(QWidget* parent);
    void create_filament_list_panel(QWidget* parent);
    QLayout* create_ams_items_sizer(QWidget* ams_preview_panel, std::vector<AMSPreview*> &ams_preview_list, std::vector<AMSinfo> &ams_info, ExtruderRole role);
    QLayout* create_slot_items_sizer(QWidget* slot_items_panel, FilamentComboBoxList& filament_comboBox_list, ExtruderRole extuder_role);

    void manage_filament_radio_btn(ExtruderRole extuder_role);

    void init_selection_values();

    void update_nozzle_id_combox();

    void on_select_nozzle(QEvent& evt);
    void on_select_plate_type(QEvent& evt);

    void on_choose_ams(QEvent& event);
    void on_choose_ext_spool(QEvent& event);

    void on_select_tray(QEvent& event);

    void on_switch_ams(std::string ams_id = "");

    void on_recommend_input_value();

    void check_nozzle_diameter_for_auto_cali();
    void check_filament_compatible();
    bool is_filaments_compatiable(const std::vector<CaliFilamentInfo>& prests);
    bool is_filament_in_blacklist(int tray_id, Preset* preset, std::string& error_tips);
    bool is_filaments_compatiable(const std::vector<CaliFilamentInfo> &prests,
        int& bed_temp,
        std::string& incompatiable_filament_name,
        std::string& error_tips);

    void check_filament_cali_reliability(const std::vector<Preset *> &prests);

    float get_nozzle_value();

    void update_slots_panel(FilamentComboBoxList& fila_combox_list, const std::string& ams_id, std::map<int, DynamicPrintConfig>& fila_ams_list);

    void update_plate_type_collection(CalibrationMethod method);
    void update_combobox_filaments(MachineObject* obj);
    void update_show_status();
    void update_sync_button_status();
    void show_status(CaliPresetPageStatus status);
    void Enable_Send_Button(bool enable);
    bool is_blocking_printing();
    bool need_check_sdcard(MachineObject* obj);

    void disable_bowden_extuder_auto_dyn_cali(QWidget* cali_panel);
    std::vector<std::pair<QString, int>> make_nozzles_info(const DevNozzle& r_nozzle, const std::map<int, DevNozzle>& nozzle_map, const NozzleDiameterType& nozzle_diameter, const std::vector<NozzleFlowType>& nozzle_flows);

    std::map<std::string, std::pair<bool, QString>> m_tips_map;
    void init_filament_list_tips();
    QString get_filament_tips();

    CaliPresetPageStatus  get_status() { return m_page_status; }

    CaliPageStepGuide* m_step_panel{ nullptr };
    CaliComboBox *            m_pa_cali_method_combox{nullptr};
    CaliPresetCaliStagePanel* m_cali_stage_panel { nullptr };
    QWidget*                  m_selection_panel { nullptr };
    QWidget*                  m_filament_from_panel { nullptr };
    Label*             m_filament_list_tips{ nullptr };
    QWidget*                  m_filament_list_panel { nullptr };
    CaliPresetWarningPanel*   m_warning_panel{nullptr};
    CaliPresetWarningPanel*   m_error_panel { nullptr };
    CaliPresetCustomRangePanel* m_custom_range_panel { nullptr };
    CaliPresetTipsPanel*      m_tips_panel { nullptr };
    CaliPageSendingPanel*     m_sending_panel { nullptr };

    QBoxLayout* m_top_sizer;

    // m_selection_panel widgets
    ComboBox*       m_comboBox_nozzle_dia;
    ComboBox*       m_comboBox_nozzle_volume;
    ComboBox*       m_comboBox_bed_type;
    ComboBox*       m_comboBox_process;

    std::vector<BedType> m_displayed_bed_types;

    void update_extruder_filament_combobox(ExtruderRole role, const std::string &ams_id);

    // multi_extruder
    void create_multi_extruder_filament_list_panel(QWidget *parent);
    void on_select_nozzle_volume_type(QEvent &evt, size_t extruder_id);

    Button *m_btn_sync{nullptr};
    Label* m_sync_button_text;

    QWidget*    m_single_nozzle_info_panel{nullptr};
    QWidget*    m_multi_nozzle_info_panel{nullptr};
    QWidget*    m_multi_exutrder_filament_list_panel{nullptr};

    ComboBox * m_left_comboBox_nozzle_dia;
    ComboBox * m_right_comboBox_nozzle_dia;
    ComboBox * m_left_comboBox_nozzle_volume;
    ComboBox * m_right_comboBox_nozzle_volume;

    QWidget*    m_single_ams_preview_panel{nullptr};
    QWidget*    m_main_ams_preview_panel{nullptr};
    QWidget*    m_deputy_ams_preview_panel{nullptr};
    QWidget*    m_single_ams_items_panel{nullptr};
    QWidget*    m_main_ams_items_panel{nullptr};
    QWidget*    m_deputy_ams_items_panel{nullptr};

    std::vector<AMSPreview *> m_single_ams_preview_list;
    std::vector<AMSPreview *> m_main_ams_preview_list;
    std::vector<AMSPreview *> m_deputy_ams_preview_list;
    FilamentComboBoxList      m_single_filament_comboBox_list;
    FilamentComboBoxList      m_main_filament_comboBox_list;
    FilamentComboBoxList      m_deputy_filament_comboBox_list;

    std::unordered_map<int, int> m_ams_id_to_extruder_id_map;
    std::vector<ExtruderType>     m_extrder_types;
    std::vector<NozzleVolumeType> m_extruder_nozzle_types;
    bool                          m_main_extruder_on_left{true};

    QBoxLayout* m_multi_extruder_ams_panel_sizer;
    QBoxLayout *       m_multi_exturder_ams_sizer;
    QVBoxLayout * m_main_sizer;
    QVBoxLayout * m_deputy_sizer;
    QVBoxLayout * m_left_nozzle_volume_type_sizer;
    QVBoxLayout * m_right_nozzle_volume_type_sizer;
    QWidget*           m_main_filament_cali_panel;
    QWidget*           m_deputy_filament_cali_panel;

    Label*          m_nozzle_diameter_tips{nullptr};

    ScalableButton*      m_ams_sync_button;
    FilamentComboBoxList m_filament_comboBox_list;

    // for update filament combobox, key : tray_id
    std::map<int, DynamicPrintConfig> m_filament_ams_list;

    CaliPresetPageStatus    m_page_status { CaliPresetPageStatus::CaliPresetStatusInit };
    bool                    m_stop_update_page_status{ false };

    bool m_show_custom_range { false };
    bool m_has_filament_incompatible { false };
    MachineObject* curr_obj { nullptr };
};

class MaxVolumetricSpeedPresetPage : public CalibrationPresetPage
{
public:
    MaxVolumetricSpeedPresetPage(QWidget *     parent,
                                 CalibMode      cali_mode,
                                 bool           custom_range = false,
                                 int     id           = -1,
                                 const QPoint &pos          = QPoint(),
                                 const QSize & size         = QSize(),
                                 long           style        = 0);
};

}} // namespace Slic3r::GUI

#endif

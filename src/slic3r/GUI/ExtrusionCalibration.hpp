#ifndef slic3r_ExtrusionCalibration_hpp_
#define slic3r_ExtrusionCalibration_hpp_

#include "libslic3r/Preset.hpp"
#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include <QColor>
#include <QLabel>
#include <QComboBox>
#include "DeviceManager.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/TextInput.hpp"
#include "ParamsDialog.hpp"
#include "GUI_App.hpp"

#define EXTRUSION_CALIBRATION_DEF_COLOUR    QColor(255, 255, 255)
#define EXTRUSION_CALIBRATION_GREY900       QColor(38, 46, 48)
#define EXTRUSION_CALIBRATION_GREY800       QColor(50, 58, 61)
#define EXTRUSION_CALIBRATION_GREY700       QColor(107, 107, 107)
#define EXTRUSION_CALIBRATION_GREY300       QColor(238, 238, 238)
#define EXTRUSION_CALIBRATION_GREY200       QColor(248, 248, 248)
#define EXTRUSION_CALIBRATION_BODY_WIDTH    380
#define EXTRUSION_CALIBRATION_LABEL_WIDTH   80
#define EXTRUSION_CALIBRATION_WIDGET_GAP    18
#define EXTRUSION_CALIBRATION_DIALOG_SIZE   QSize(400, -1)
//#define EXTRUSION_CALIBRATION_DIALOG_SIZE   QSize(520, -1)
#define EXTRUSION_CALIBRATION_BED_COMBOX    QSize(200, 24)
#define EXTRUSION_CALIBRATION_BUTTON_SIZE   QSize(72, 24)
#define EXTRUSION_CALIBRATION_INPUT_SIZE    QSize(100, 24)
#define EXTRUSION_CALIBRATION_BMP_SIZE      QSize(256, 256)
#define EXTRUSION_CALIBRATION_BMP_TIP_BAR   QSize(256, 40)
#define EXTRUSION_CALIBRATION_BMP_BTN_SIZE  QSize(16, 16)



namespace Slic3r { namespace GUI {

class ExtrusionCalibration : public DPIDialog
{
public:
    ExtrusionCalibration(QWidget *parent = nullptr);
    ~ExtrusionCalibration();
    void create();

    void input_value_finish();
    void update();
    void Show(bool show = true); // not override — QDialog uses show()/hide()
    void Popup();

	void post_select_event();
    void update_machine_obj(MachineObject* obj_) { obj = obj_; };

    // input is 1 or 2
    void set_step(int step_index);

    static bool check_k_n_validation(const QString& k_text, const QString& n_text);
    static bool check_k_validation(const QString& k_text);

    MachineObject *obj { nullptr };
    int            m_ams_id { 0 };        /* 0 ~ 3 */
    int            m_slot_id { 0 };       /* 0 ~ 3 | 254 for virtual tray id*/

    std::string    ams_filament_id;
    std::string    m_filament_type;

    std::vector<Preset*> user_filaments;

protected:
    void init_bitmaps();
    void on_dpi_changed(const QRect &suggested_rect) override;
    void paint(QPaintEvent*&);
    void open_bitmap(QMouseEvent& event);
    void on_select_filament();
    void on_select_bed_type();
    void on_select_nozzle_dia();
    void on_click_cali();
    void on_click_cancel();
    void on_click_save();
    void on_click_last();
    void on_click_next();

    void update_filament_info();
    void update_combobox_filaments();
    QString get_bed_type_incompatible(bool incompatible);
    void show_info(bool show, bool is_error, const QString& text);

    int get_bed_temp(DynamicPrintConfig* config);

protected:
    StateColor          m_btn_bg_green;
    StateColor          m_btn_bg_gray;

    QWidget*            m_step_1_panel{nullptr};
    QWidget*            m_step_2_panel{nullptr};

    // title of select filament preset
    Label*       m_filament_preset_title;
    // select a filament preset
    ComboBox* m_comboBox_filament{nullptr};

    ComboBox* m_comboBox_bed_type{nullptr};

    ComboBox* m_comboBox_nozzle_dia{nullptr};

    TextInput*          m_nozzle_temp;
    TextInput*          m_bed_temp;
    TextInput*          m_max_flow_ratio;
    Button*             m_cali_cancel;
    Button*             m_button_cali;
    Button*             m_button_next_step;
    Label*              m_save_cali_result_title;
    QLabel*             m_fill_cali_params_tips{nullptr};
    QLabel*             m_info_text{nullptr};
    QLabel*             m_error_text{nullptr};

    QPixmap             m_calibration_tips_open_btn_bmp;
    QPixmap            m_calibration_tips_bmp_zh;
    QPixmap            m_calibration_tips_bmp_en;
    QLabel*     m_calibration_tips_static_bmp;
    // save n and k result
    QLabel*       m_k_param;
    TextInput*          m_k_val;
    QLabel*       m_n_param;
    TextInput*          m_n_val;
    Button*             m_button_last_step;
    Button*             m_button_save_result;

    bool m_is_zh{ false };
};

}} // namespace Slic3r::GUI

#endif

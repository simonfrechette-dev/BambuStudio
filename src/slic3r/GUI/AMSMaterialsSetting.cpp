// AMSMaterialsSetting.cpp — stubbed for Qt6 port
#include "AMSMaterialsSetting.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include <QPainter>
#include <QVBoxLayout>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// ColorPicker
// ---------------------------------------------------------------------------

ColorPicker::ColorPicker(QWidget* parent) : QWidget(parent) {}
ColorPicker::~ColorPicker() {}
void ColorPicker::msw_rescale() {}
void ColorPicker::set_color(QColor col) { m_colour = col; update(); }
void ColorPicker::set_colors(std::vector<QColor> cols) { m_cols = cols; update(); }
void ColorPicker::paintEvent(QPaintEvent* evt) { (void)evt; QPainter p(this); render(p); }
void ColorPicker::render(QPainter& dc) { doRender(dc); }
void ColorPicker::doRender(QPainter& dc) { (void)dc; }

// ---------------------------------------------------------------------------
// ColorPickerPopup
// ---------------------------------------------------------------------------

ColorPickerPopup::ColorPickerPopup(QWidget* parent)
    : PopupWindow(parent)
    , m_ts_stbitmap_custom(nullptr)
    , m_custom_cp(nullptr)
    , m_def_color_box(nullptr)
    , m_ams_fg_sizer(nullptr)
{}
void ColorPickerPopup::on_custom_clr_picker() {}
void ColorPickerPopup::set_ams_colours(std::vector<QColor> ams) { m_ams_colors = ams; }
void ColorPickerPopup::set_def_colour(QColor col) { m_def_col = col; }
void ColorPickerPopup::paintEvent(QPaintEvent* evt) { (void)evt; }
void ColorPickerPopup::Popup(QWidget* focus) { (void)focus; }

// ---------------------------------------------------------------------------
// AMSMaterialsSetting
// ---------------------------------------------------------------------------

AMSMaterialsSetting::AMSMaterialsSetting(QWidget *parent)
    : DPIDialog(parent)
    , m_is_third(false)
    , m_color_picker_popup(this)
    , m_clr_picker(nullptr)
    , m_clr_name(nullptr)
    , m_panel_normal(nullptr)
    , m_panel_SN(nullptr)
    , m_sn_number(nullptr)
    , warning_text(nullptr)
    , m_title_filament(nullptr)
    , m_title_nozzle_type(nullptr)
    , m_title_pa_profile(nullptr)
    , m_title_colour(nullptr)
    , m_title_temperature(nullptr)
    , m_input_nozzle_min(nullptr)
    , m_input_nozzle_max(nullptr)
    , degree(nullptr)
    , bitmap_max_degree(nullptr)
    , bitmap_min_degree(nullptr)
    , m_button_reset(nullptr)
    , m_button_confirm(nullptr)
    , m_tip_readonly(nullptr)
    , m_button_close(nullptr)
    , m_panel_kn(nullptr)
    , m_ratio_text(nullptr)
    , m_wiki_ctrl(nullptr)
    , m_k_param(nullptr)
    , m_input_k_val(nullptr)
    , m_n_param(nullptr)
    , m_input_n_val(nullptr)
    , m_filament_selection(0)
    , m_comboBox_filament(nullptr)
    , m_comboBox_nozzle_type(nullptr)
    , m_comboBox_cali_result(nullptr)
    , m_readonly_filament(nullptr)
{
    create();
}

AMSMaterialsSetting::~AMSMaterialsSetting() {}
void AMSMaterialsSetting::create() {}
void AMSMaterialsSetting::create_panel_normal(QWidget* parent) { (void)parent; }
void AMSMaterialsSetting::create_panel_kn(QWidget* parent) { (void)parent; }
void AMSMaterialsSetting::paintEvent(QPaintEvent* evt) { (void)evt; }
void AMSMaterialsSetting::input_min_finish() {}
void AMSMaterialsSetting::input_max_finish() {}
void AMSMaterialsSetting::update() {}
void AMSMaterialsSetting::Show(bool show) { if(show) QDialog::show(); else QDialog::hide(); }
void AMSMaterialsSetting::Popup(QString filament, QString sn,
    QString temp_min, QString temp_max, QString k, QString n)
{
    (void)filament; (void)sn; (void)temp_min; (void)temp_max; (void)k; (void)n;
}
void AMSMaterialsSetting::post_select_event(int index) { (void)index; }
void AMSMaterialsSetting::TryRefreshPAProfiles() {}
void AMSMaterialsSetting::set_color(QColor color) { (void)color; }
void AMSMaterialsSetting::set_empty_color(QColor color) { (void)color; }
void AMSMaterialsSetting::set_colors(std::vector<QColor> colors) { (void)colors; }
void AMSMaterialsSetting::set_ctype(int ctype) { (void)ctype; }
void AMSMaterialsSetting::on_picker_color() {}
void AMSMaterialsSetting::on_dpi_changed(const QRect& r) { (void)r; }
void AMSMaterialsSetting::on_select_nozzle_id(int idx) { (void)idx; }
void AMSMaterialsSetting::on_select_filament(int idx) { (void)idx; }
void AMSMaterialsSetting::on_select_cali_result(int idx) { (void)idx; }
void AMSMaterialsSetting::on_select_nozzle_pos_id(int idx) { (void)idx; }
void AMSMaterialsSetting::on_select_ok() {}
void AMSMaterialsSetting::on_select_reset() {}
void AMSMaterialsSetting::on_select_close() {}
void AMSMaterialsSetting::on_clr_picker() {}
bool AMSMaterialsSetting::is_virtual_tray() { return false; }
void AMSMaterialsSetting::update_widgets() {}
void AMSMaterialsSetting::update_pa_profile_items() {}
void AMSMaterialsSetting::update_filament_editing(bool is_printing) { (void)is_printing; }
void AMSMaterialsSetting::update_nozzle_combo(MachineObject* obj) { (void)obj; }
int  AMSMaterialsSetting::get_nozzle_combo_id_code() const { return 0; }
int  AMSMaterialsSetting::get_nozzle_sel_by_sn(MachineObject* obj, const std::string& sn)
{ (void)obj; (void)sn; return 0; }
int  AMSMaterialsSetting::get_cali_index_by_ams_slot(MachineObject* obj, int ams_id, int slot_id)
{ (void)obj; (void)ams_id; (void)slot_id; return 0; }
void AMSMaterialsSetting::get_filaments_info(const MachineObject* obj,
    const std::string& nozzle_diameter_str,
    QStringList& filament_items,
    std::map<std::string, FilamentInfos>& map_filament_items,
    std::unordered_map<QString, QString>& query_filament_vendors,
    std::unordered_map<QString, QString>& query_filament_types)
{
    (void)obj; (void)nozzle_diameter_str; (void)filament_items;
    (void)map_filament_items; (void)query_filament_vendors; (void)query_filament_types;
}
Preset* AMSMaterialsSetting::get_filament_by_id(const std::string& filament_id, bool is_system)
{ (void)filament_id; (void)is_system; return nullptr; }

QEvent::Type getSelectedColorEventType()
{
    static QEvent::Type type = static_cast<QEvent::Type>(QEvent::registerEventType());
    return type;
}

}} // namespace Slic3r::GUI

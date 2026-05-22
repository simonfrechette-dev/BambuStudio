// PresetComboBoxes.cpp — stubbed for Qt6 port
#include "PresetComboBoxes.hpp"
#include "BitmapCache.hpp"
#include "GUI_App.hpp"
#include <QVariant>

namespace Slic3r { namespace GUI {

// ---- PresetComboBox ----

PresetComboBox::PresetComboBox(QWidget* parent, Preset::Type preset_type, const QSize& size, PresetBundle* preset_bundle)
    : ComboBox(parent)
    , m_preset_bundle(preset_bundle)
    , m_type(preset_type)
    , m_last_selected(0)
    , m_em_unit(10)
    , icon_height(16), norm_icon_width(16), thin_icon_width(8)
    , wide_icon_width(24), space_icon_width(2)
    , thin_space_icon_width(1), wide_space_icon_width(4)
{
    (void)size;
}

PresetComboBox::~PresetComboBox() {}

void PresetComboBox::set_label_marker(int item, LabelItemType label_item_type) { (void)item; (void)label_item_type; }
bool PresetComboBox::set_printer_technology(PrinterTechnology pt) { (void)pt; return false; }
bool PresetComboBox::is_selected_physical_printer() { return false; }
bool PresetComboBox::is_selected_printer_model() { return false; }
bool PresetComboBox::selection_is_changed_according_to_physical_printers() { return false; }
void PresetComboBox::update(std::string select_preset) { (void)select_preset; }
void PresetComboBox::update_from_bundle() {}
void PresetComboBox::add_connected_printers(std::string selected, bool alias_name) { (void)selected; (void)alias_name; }
int  PresetComboBox::selected_connected_printer(int index) const { (void)index; return -1; }
bool PresetComboBox::add_ams_filaments(std::string selected, bool alias_name) { (void)selected; (void)alias_name; return false; }
int  PresetComboBox::selected_ams_filament() const { return -1; }
QString PresetComboBox::get_tooltip(const Preset& preset) { (void)preset; return {}; }
QString PresetComboBox::get_preset_item_name(unsigned int index) { (void)index; return {}; }
QString PresetComboBox::get_preset_name(const Preset& preset) { (void)preset; return {}; }
void PresetComboBox::show_all(bool show_all) { m_show_all = show_all; }
void PresetComboBox::update() {}
void PresetComboBox::msw_rescale() {}
void PresetComboBox::sys_color_changed() {}
void PresetComboBox::OnSelect(QVariant& evt) { (void)evt; }

void PresetComboBox::invalidate_selection() {}
void PresetComboBox::validate_selection(bool predicate) { (void)predicate; }
void PresetComboBox::update_selection() {}
int  PresetComboBox::update_ams_color() { return 0; }

QString PresetComboBox::separator(const std::string& label) { return QString::fromStdString(label); }

QPixmap* PresetComboBox::get_bmp(std::string bitmap_key, bool wide_icons, const std::string& main_icon_name,
    bool is_compatible, bool is_system, bool is_single_bar,
    const std::string& filament_rgb, const std::string& extruder_rgb, const std::string& material_rgb)
{
    (void)bitmap_key; (void)wide_icons; (void)main_icon_name;
    (void)is_compatible; (void)is_system; (void)is_single_bar;
    (void)filament_rgb; (void)extruder_rgb; (void)material_rgb;
    return nullptr;
}

QPixmap* PresetComboBox::get_bmp(std::string bitmap_key, const std::string& main_icon_name, const std::string& next_icon_name,
    bool is_enabled, bool is_compatible, bool is_system)
{
    (void)bitmap_key; (void)main_icon_name; (void)next_icon_name;
    (void)is_enabled; (void)is_compatible; (void)is_system;
    return nullptr;
}

QPixmap* PresetComboBox::get_bmp(Preset const& preset) { (void)preset; return nullptr; }
void PresetComboBox::fill_width_height() {}

// ---- PlaterPresetComboBox ----

PlaterPresetComboBox::PlaterPresetComboBox(QWidget* parent, Preset::Type preset_type)
    : PresetComboBox(parent, preset_type)
{}

PlaterPresetComboBox::~PlaterPresetComboBox() {}

bool PlaterPresetComboBox::switch_to_tab() { return false; }
void PlaterPresetComboBox::change_extruder_color() {}
void PlaterPresetComboBox::show_add_menu() {}
void PlaterPresetComboBox::show_edit_menu() {}
QString PlaterPresetComboBox::get_preset_name(const Preset& preset) { (void)preset; return {}; }
void PlaterPresetComboBox::update() {}
void PlaterPresetComboBox::msw_rescale() {}
void PlaterPresetComboBox::OnSelect(QVariant& evt) { (void)evt; }
void PlaterPresetComboBox::update_badge_according_flag() {}
void PlaterPresetComboBox::show_default_color_picker() {}
void PlaterPresetComboBox::sys_color_changed() {}

// ---- TabPresetComboBox ----

TabPresetComboBox::TabPresetComboBox(QWidget* parent, Preset::Type preset_type)
    : PresetComboBox(parent, preset_type)
{
    // Wire up the preset collection from the app's bundle.
    m_preset_bundle = wxGetApp().preset_bundle;
    if (m_preset_bundle) {
        switch (preset_type) {
        case Preset::TYPE_PRINT:    m_collection = &m_preset_bundle->prints;    break;
        case Preset::TYPE_FILAMENT: m_collection = &m_preset_bundle->filaments; break;
        case Preset::TYPE_PRINTER:  m_collection = &m_preset_bundle->printers;  break;
        default: break;
        }
    }
}

void TabPresetComboBox::update()
{
    if (!m_collection) return;

    // Remember the currently selected name.
    const std::string sel_name = m_collection->get_selected_preset().name;

    DoClear();

    int sel_idx = 0;
    int idx = 0;
    for (const Preset& p : *m_collection) {
        Append(QString::fromStdString(p.name));
        if (p.name == sel_name)
            sel_idx = idx;
        ++idx;
    }

    if (GetCount() > 0)
        SetSelection(sel_idx);
}
void TabPresetComboBox::update_dirty() {}
void TabPresetComboBox::msw_rescale() {}
void TabPresetComboBox::OnSelect(QVariant& evt) { (void)evt; }
QString TabPresetComboBox::get_preset_name(const Preset&) { return {}; }

// ---- CalibrateFilamentComboBox ----

CalibrateFilamentComboBox::CalibrateFilamentComboBox(QWidget* parent)
    : PlaterPresetComboBox(parent, Preset::Type::TYPE_FILAMENT)
{}

CalibrateFilamentComboBox::~CalibrateFilamentComboBox() {}
void CalibrateFilamentComboBox::load_tray(const DynamicPrintConfig& config) { (void)config; }
void CalibrateFilamentComboBox::update() {}
void CalibrateFilamentComboBox::msw_rescale() {}
void CalibrateFilamentComboBox::OnSelect(QVariant& evt) { (void)evt; }

}} // namespace Slic3r::GUI

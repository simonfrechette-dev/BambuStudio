// SavePresetDialog.cpp — stubbed for Qt6 port
#include "SavePresetDialog.hpp"
#include <QVBoxLayout>

namespace Slic3r { namespace GUI {

SavePresetDialog::SavePresetDialog(QWidget* parent, Preset::Type type, std::string suffix)
    : DPIDialog(parent) { build({type}, suffix); }

SavePresetDialog::SavePresetDialog(QWidget* parent, std::vector<Preset::Type> types, std::string suffix)
    : DPIDialog(parent) { build(types, suffix); }

SavePresetDialog::~SavePresetDialog() {}
void SavePresetDialog::build(std::vector<Preset::Type> types, std::string suffix) { (void)types; (void)suffix; }
void SavePresetDialog::AddItem(Preset::Type type, const std::string& suffix) { (void)type; (void)suffix; }
std::string SavePresetDialog::get_name() { return {}; }
std::string SavePresetDialog::get_name(Preset::Type type) { (void)type; return {}; }
void SavePresetDialog::input_name_from_other(std::string new_preset_name) { (void)new_preset_name; }
void SavePresetDialog::confirm_from_other() {}
bool SavePresetDialog::enable_ok_btn() const { return true; }
void SavePresetDialog::add_info_for_edit_ph_printer(QLayout* sizer) { (void)sizer; }
void SavePresetDialog::update_info_for_edit_ph_printer(const std::string& preset_name) { (void)preset_name; }
void SavePresetDialog::layout() {}
bool SavePresetDialog::get_save_to_project_selection(Preset::Type type) { (void)type; return false; }
void SavePresetDialog::on_dpi_changed(const QRect& r) { (void)r; }
void SavePresetDialog::on_select_cancel() { reject(); }
void SavePresetDialog::update_physical_printers(const std::string& preset_name) { (void)preset_name; }
void SavePresetDialog::accept() { QDialog::accept(); }

}} // namespace Slic3r::GUI

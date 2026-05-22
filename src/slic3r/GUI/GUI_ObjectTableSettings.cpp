// GUI_ObjectTableSettings.cpp — stubbed for Qt6 port
#include "GUI_ObjectTableSettings.hpp"
#include "GUI_ObjectTable.hpp"

namespace Slic3r { namespace GUI {

OTG_Settings::OTG_Settings(QWidget* parent, const bool staticbox) : m_parent(parent) { (void)staticbox; }
bool OTG_Settings::IsShown() { return false; }
void OTG_Settings::Show(const bool show) { (void)show; }
void OTG_Settings::Hide() {}
void OTG_Settings::UpdateAndShow(const bool show) { (void)show; }
QLayout* OTG_Settings::get_sizer() { return nullptr; }

ObjectTableSettings::ObjectTableSettings(QWidget* parent, ObjectGridTable* table)
    : OTG_Settings(parent, false), m_table(table) {}

bool ObjectTableSettings::update_settings_list(bool is_object, bool is_multiple_selection, ModelObject* object, ModelConfig* config, const std::string& category) { (void)is_object; (void)is_multiple_selection; (void)object; (void)config; (void)category; return false; }
bool ObjectTableSettings::add_missed_options(ModelConfig* config_to, const DynamicPrintConfig& config_from) { (void)config_to; (void)config_from; return false; }
int  ObjectTableSettings::update_extra_column_visible_status(ConfigOptionsGroup* option_group, const std::vector<SimpleSettingData>& option_keys, ModelConfig* config) { (void)option_group; (void)option_keys; (void)config; return 0; }
void ObjectTableSettings::update_config_values(bool is_object, ModelObject* object, ModelConfig* config, const std::string& category) { (void)is_object; (void)object; (void)config; (void)category; }
void ObjectTableSettings::UpdateAndShow(int row, const bool show, bool is_object, bool is_multiple_selection, ModelObject* object, ModelConfig* config, const std::string& category) { (void)row; (void)show; (void)is_object; (void)is_multiple_selection; (void)object; (void)config; (void)category; }
void ObjectTableSettings::ValueChanged(int row, bool is_object, ModelObject* object, ModelConfig* config, const std::string& category, const std::string& key) { (void)row; (void)is_object; (void)object; (void)config; (void)category; (void)key; }
void ObjectTableSettings::resetAllValues(int row, bool is_object, ModelObject* object, ModelConfig* config, const std::string& category) { (void)row; (void)is_object; (void)object; (void)config; (void)category; }
void ObjectTableSettings::msw_rescale() {}

}} // namespace Slic3r::GUI

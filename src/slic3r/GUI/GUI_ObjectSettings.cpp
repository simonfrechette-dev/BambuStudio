// GUI_ObjectSettings.cpp — stubbed for Qt6 port
#include "GUI_ObjectSettings.hpp"

namespace Slic3r { namespace GUI {

OG_Settings::OG_Settings(QWidget* parent, bool staticbox) : m_parent(parent) { (void)staticbox; }
bool OG_Settings::IsShown() { return false; }
void OG_Settings::Show(bool show) { (void)show; }
void OG_Settings::Hide() {}
void OG_Settings::UpdateAndShow(bool show) { (void)show; }
QLayout* OG_Settings::get_sizer() { return nullptr; }

ObjectSettings::ObjectSettings(QWidget* parent) : m_parent(parent) {}
bool ObjectSettings::update_settings_list() { return false; }
bool ObjectSettings::add_missed_options(ModelConfig* config_to, const DynamicPrintConfig& config_from) { (void)config_to; (void)config_from; return false; }
void ObjectSettings::update_config_values(ModelConfig* config) { (void)config; }
void ObjectSettings::UpdateAndShow(bool show) { (void)show; }
void ObjectSettings::msw_rescale() {}
void ObjectSettings::sys_color_changed() {}

}} // namespace Slic3r::GUI

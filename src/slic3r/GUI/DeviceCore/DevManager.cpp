// Qt6 stub for DeviceCore/DevManager.cpp
#include "DevManager.h"
#include "slic3r/GUI/DeviceManager.hpp"

namespace Slic3r {

DeviceManagerRefresher::~DeviceManagerRefresher() {}

// DeviceManager stubs
DeviceManager::DeviceManager(NetworkAgent* agent) : m_agent(agent) {}
DeviceManager::~DeviceManager() {}

MachineObject* DeviceManager::get_selected_machine() { return nullptr; }
bool           DeviceManager::set_selected_machine(std::string /*dev_id*/) { return false; }
void           DeviceManager::load_last_machine() {}
MachineObject* DeviceManager::get_my_machine(std::string /*dev_id*/) { return nullptr; }
std::map<std::string, MachineObject*> DeviceManager::get_my_machine_list() { return {}; }
MachineObject* DeviceManager::get_user_machine(std::string /*dev_id*/) { return nullptr; }
void           DeviceManager::EnableMultiMachine(bool enable) { m_enable_mutil_machine = enable; }

} // namespace Slic3r

// Bridge stub: DevExtruderSystem.cpp (namespace Slic3r) calls
// GUI::get_printer_preset(const MachineObject*) where MachineObject is Slic3r::MachineObject.
// Plater.cpp defines it with Slic3r::GUI::MachineObject (due to HMS.hpp forward-decl).
// Provide the Slic3r::MachineObject overload here, in a context where MachineObject
// resolves correctly to Slic3r::MachineObject.
#include "slic3r/GUI/Plater.hpp"         // for Preset forward-decl
#include "slic3r/GUI/DeviceManager.hpp"  // for MachineObject (Slic3r::)
namespace Slic3r { namespace GUI {
Preset* get_printer_preset(const ::Slic3r::MachineObject*) { return nullptr; }
}} // namespace Slic3r::GUI

namespace Slic3r {
void DeviceManager::on_machine_alive(std::string /*json_str*/) { /* TODO: Qt port stub */ }
} // namespace Slic3r


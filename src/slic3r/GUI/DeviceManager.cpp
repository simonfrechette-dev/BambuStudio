// Qt6 stub for DeviceManager.cpp
// Original wx-based implementation replaced with empty stub.
#include "DeviceManager.hpp"
#include "DeviceCore/DevFilaSystem.h"

// All implementations are stubs pending Qt port.

namespace Slic3r {

std::string MachineObject::get_dev_id() const { return {}; }
PrinterArch  MachineObject::get_printer_arch() const { return PrinterArch::ARCH_CORE_XY; }
std::string  MachineObject::get_printer_ams_type() const { return {}; }
int          MachineObject::command_ams_control(std::string) { return 0; }
bool         MachineObject::is_in_printing() { return false; }
bool         MachineObject::is_in_printing_pause() const { return false; }
int          MachineObject::publish_gcode(std::string) { return 0; }
int          MachineObject::publish_json(const json&, int, int) { return 0; }
std::string  MachineObject::setting_id_to_type(std::string, std::string) { return {}; }
std::optional<DevAmsTray> MachineObject::get_tray(const std::string&, const std::string&) const { return {}; }
int          MachineObject::get_flag_bits(int, int, int, int) const { return 0; }

// Networking callback stubs
int          MachineObject::command_request_push_all(bool /*from_server*/) { return 0; }
int          MachineObject::command_get_version(bool /*with_flag*/) { return 0; }
void         MachineObject::erase_user_access_code() {}
int          MachineObject::command_get_access_code() { return 0; }
void         MachineObject::record_user_access_dev_ip() {}
std::string  MachineObject::get_access_code() const { return {}; }
void         MachineObject::set_access_code(std::string /*code*/, bool /*save*/) {}
void         MachineObject::erase_user_access_dev_ip() {}
int          MachineObject::parse_json(std::string /*source*/, std::string /*payload*/, bool /*skip_ui*/) { return 0; }


// StatusPanel / Monitor stubs
bool         MachineObject::is_connected() { return false; }
bool         MachineObject::is_connecting() { return false; }
bool         MachineObject::is_filament_at_extruder() { return false; }
bool         MachineObject::can_resume() { return false; }
bool         MachineObject::is_info_ready(bool /*check_version*/) const { return false; }
bool         MachineObject::is_multi_extruders() const { return false; }
bool         MachineObject::is_series_o() const { return false; }
bool         MachineObject::is_series_o(const std::string& /*series_str*/) { return false; }
bool         MachineObject::is_security_control_ready() const { return false; }
bool         MachineObject::is_sdcard_printing() { return false; }
bool         MachineObject::is_system_printing() { return false; }
bool         MachineObject::is_calibration_running() { return false; }
bool         MachineObject::is_in_calibration() { return false; }
bool         MachineObject::is_in_prepare() { return false; }
bool         MachineObject::is_printing_finished() { return false; }
bool         MachineObject::is_recording() { return false; }
bool         MachineObject::is_timelapse() { return false; }
bool         MachineObject::is_makeworld_subtask() { return false; }
bool         MachineObject::is_target_slot_unload() const { return false; }
int          MachineObject::get_bed_temperature_limit() { return 0; }
QString      MachineObject::get_curr_stage() { return {}; }
std::string  MachineObject::get_printer_series_str() const { return {}; }
std::string  MachineObject::get_auto_pa_cali_thumbnail_img_str() const { return {}; }
DevAmsTray*  MachineObject::get_curr_tray() { return nullptr; }
DevAmsTray*  MachineObject::get_ams_tray(std::string /*ams_id*/, std::string /*tray_id*/) { return nullptr; }
std::shared_ptr<DevNozzleRack> MachineObject::GetNozzleRack() const { return nullptr; }
DevPrintingSpeedLevel MachineObject::GetPrintingSpeedLevel() const { return SPEED_LEVEL_INVALID; }
BBLSubTask*  MachineObject::get_subtask() { return nullptr; }
BBLModelTask* MachineObject::get_modeltask() { return nullptr; }
void         MachineObject::set_modeltask(BBLModelTask* /*task*/) {}
void         MachineObject::update_model_task() {}
void         MachineObject::check_ams_filament_valid() {}
void         MachineObject::free_slice_info() {}
int          MachineObject::command_ams_change_filament(bool /*load*/, std::string /*ams_id*/, std::string /*slot_id*/, int /*old_temp*/, int /*new_temp*/, std::optional<int> /*extruder_id*/) { return 0; }
int          MachineObject::command_ams_select_tray(std::string /*tray_id*/) { return 0; }
int          MachineObject::command_ams_refresh_rfid(std::string /*tray_id*/) { return 0; }
int          MachineObject::command_ams_refresh_rfid2(int /*ams_id*/, int /*slot_id*/) { return 0; }
int          MachineObject::command_extruder_control(int /*nozzle_id*/, double /*val*/) { return 0; }
int          MachineObject::command_set_bed(int /*temp*/) { return 0; }
int          MachineObject::command_set_nozzle(int /*temp*/) { return 0; }
int          MachineObject::command_set_nozzle_new(int /*nozzle_id*/, int /*temp*/) { return 0; }
int          MachineObject::command_task_abort() { return 0; }
int          MachineObject::command_task_pause() { return 0; }
int          MachineObject::command_task_resume() { return 0; }

} // namespace Slic3r

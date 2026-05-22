// Qt6 stub for DeviceCore/DevConfigUtil.cpp
// Original wx-based implementation replaced with empty stub.
#include "DevConfigUtil.h"

// Static member definition required by the linker

namespace Slic3r {

std::string DevPrinterConfigUtil::get_filament_load_img(const std::string& /*type_str*/, int /*ext_id*/, bool /*has_nozzle_rack*/) { return {}; }

} // namespace Slic3r
std::string Slic3r::DevPrinterConfigUtil::m_resource_file_path;

namespace Slic3r {

std::string DevPrinterConfigUtil::get_toolhead_display_name(
    const std::string&, int, ToolHeadComponent, ToolHeadNameCase, bool)
{ return {}; }

} // namespace Slic3r

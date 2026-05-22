// Qt6 stub for DeviceCore/DevNozzleSystem.cpp
#include "DevNozzleSystem.h"
// All implementations are stubs pending Qt port.

namespace Slic3r {

NozzleDiameterType DevNozzle::GetNozzleDiameterType() const { return NozzleDiameterType::NONE_DIAMETER_TYPE; }
std::string DevNozzle::GetNozzleFlowTypeString(NozzleFlowType) { return {}; }
DevNozzle DevNozzleSystem::GetExtNozzle(int) const { return {}; }

} // namespace Slic3r

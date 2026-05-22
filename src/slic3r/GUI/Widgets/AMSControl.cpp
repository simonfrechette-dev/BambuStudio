// AMSControl.cpp — Phase 2 stub. Full implementation deferred to Phase 3 (device layer port).
#include "AMSControl.hpp"

namespace Slic3r { namespace GUI {

AMSControl::AMSControl(QWidget *parent) : QStackedWidget(parent) {}

void AMSControl::on_retry() {}
void AMSControl::createAms(QStackedWidget *, int &, AMSinfo, AMSPanelPos) {}
void AMSControl::createAmsPanel(QStackedWidget *, int &, std::vector<AMSinfo>,
    const std::string &, const std::string &, AMSPanelPos, int) {}
void AMSControl::EnableLoadFilamentBtn(bool, const std::string &, const std::string &, const QString &) {}
void AMSControl::EnableUnLoadFilamentBtn(bool, const std::string &, const std::string &, const QString &) {}
void AMSControl::EnterNoneAMSMode() {}
void AMSControl::EnterGenericAMSMode() {}
void AMSControl::EnterExtraAMSMode() {}
void AMSControl::PlayRridLoading(const QString &, const QString &) {}
void AMSControl::StopRridLoading(const QString &, const QString &) {}
void AMSControl::ShowFilamentTip(bool) {}
void AMSControl::UpdatePassRoad(const std::string &, AMSPassRoadType, AMSPassRoadSTEP) {}
void AMSControl::CreateAmsDoubleNozzle(const std::string &, const std::string &) {}
void AMSControl::CreateAmsSingleNozzle(const std::string &, const std::string &) {}
void AMSControl::ClearAms() {}
void AMSControl::UpdateAmsDryControl(MachineObject *) {}
void AMSControl::UpdateAms(const std::string &, bool, bool) {}
void AMSControl::SetExtruder(bool, int, const std::string &, const std::string &) {}
void AMSControl::SetAmsStep(const std::string &, const std::string &, int, AMSPassRoadType, AMSPassRoadSTEP) {}
void AMSControl::SwitchAms(const std::string &ams_id) { m_current_ams_id = ams_id; }
void AMSControl::msw_rescale() { update(); }
void AMSControl::Reset() {}
bool AMSControl::isFilaSwitchInstalled() const { return false; }
void AMSControl::show_switcher_status(bool) {}
void AMSControl::show_noams_mode() {}
void AMSControl::show_auto_refill(bool) {}
void AMSControl::enable_ams_setting(bool) {}
void AMSControl::show_vams_kn_value(bool) {}
bool AMSControl::Enable(bool enable) { setEnabled(enable); return enable; }
void AMSControl::parse_object(MachineObject *) {}
void AMSControl::AddAms(AMSinfo, AMSPanelPos) {}
void AMSControl::AddAms(std::vector<AMSinfo>, const std::string &, const std::string &, AMSPanelPos) {}
void AMSControl::AddAmsPreview(AMSinfo, AMSPanelPos) {}
void AMSControl::AddAmsPreview(std::vector<AMSinfo>, AMSPanelPos) {}
void AMSControl::UpdateAmsPreviewSelection() {}
bool AMSControl::IsInSlotPair(const std::string &) const { return false; }
bool AMSControl::GetExtPairedDoubleMode(const std::string &, AMSPanelPos, AMSRoadShowMode &) const { return false; }

}} // namespace Slic3r::GUI

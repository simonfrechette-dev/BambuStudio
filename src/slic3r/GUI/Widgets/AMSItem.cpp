// AMSItem.cpp — Phase 2 stub. Full implementation deferred to Phase 3 (device layer port).
#include "AMSItem.hpp"

namespace Slic3r { namespace GUI {

// ---- Caninfo ----
bool Caninfo::operator==(const Caninfo &o) const {
    return can_id == o.can_id && material_name == o.material_name &&
           material_colour == o.material_colour && material_state == o.material_state &&
           ctype == o.ctype && material_remain == o.material_remain &&
           cali_idx == o.cali_idx && filament_id == o.filament_id &&
           k == o.k && n == o.n && material_cols == o.material_cols;
}

// ---- AMSinfo ----
bool AMSinfo::operator==(const AMSinfo &o) const {
    return ams_id == o.ams_id && cans == o.cans && current_can_id == o.current_can_id &&
           current_step == o.current_step && current_action == o.current_action &&
           curreent_filamentstep == o.curreent_filamentstep &&
           ams_humidity == o.ams_humidity && left_dray_time == o.left_dray_time &&
           m_ams_drying == o.m_ams_drying && current_temperature == o.current_temperature &&
           ams_type == o.ams_type && ext_type == o.ext_type &&
           binded_extruder_set == o.binded_extruder_set &&
           binded_switcher_pos == o.binded_switcher_pos;
}
bool AMSinfo::parse_ams_info(MachineObject*, DevAms*, bool, bool) { return false; }
void AMSinfo::parse_ext_info(MachineObject*, DevAmsTray) {}
bool AMSinfo::support_drying()   const { return ams_type == DevAmsType::N3S || ams_type == DevAmsType::N3F; }
bool AMSinfo::support_humidity() const { int h = get_humidity_display_idx(); return h >= 1 && h <= 5; }
Caninfo AMSinfo::get_caninfo(const std::string &can_id, bool &found) const {
    for (const auto &c : cans) if (c.can_id == can_id) { found = true; return c; }
    found = false; return {};
}
int  AMSinfo::get_humidity_display_idx() const { return ams_humidity_percent; }
AMSPanelPos AMSinfo::GetDefaultPanelPos(int) const { return AMSPanelPos::RIGHT_PANEL; }

// ---- Widget stubs ----
AMSExtText::AMSExtText(QWidget *p) : QWidget(p) {}
void AMSExtText::msw_rescale() {}

AMSrefresh::AMSrefresh(QWidget *p) : QWidget(p) {}
void AMSrefresh::msw_rescale() {}

AMSextruderImage::AMSextruderImage(QWidget *p) : QWidget(p) {}
void AMSextruderImage::msw_rescale() {}

SwitcherImage::SwitcherImage(QWidget *p) : QWidget(p) {}
void SwitcherImage::msw_rescale() {}

AMSExtImage::AMSExtImage(QWidget *p) : QWidget(p) {}
void AMSExtImage::msw_rescale() {}

AMSextruder::AMSextruder(QWidget *p) : QWidget(p) {}
void AMSextruder::msw_rescale() {}

AMSLib::AMSLib(QWidget *parent, Caninfo info) : StaticBox(parent), m_info(std::move(info)) {}
void AMSLib::Update(Caninfo info) { m_info = std::move(info); update(); }
void AMSLib::SetAmsModel(DevAmsType) {}
void AMSLib::msw_rescale() { update(); }

AMSRoad::AMSRoad(QWidget *p) : QWidget(p) {}
void AMSRoad::msw_rescale() {}
AMSRoadUpPart::AMSRoadUpPart(QWidget *p) : QWidget(p) {}
void AMSRoadUpPart::msw_rescale() {}
AMSRoadDownPart::AMSRoadDownPart(QWidget *p) : QWidget(p) {}
void AMSRoadDownPart::msw_rescale() {}
AMSPreview::AMSPreview(QWidget *p) : QWidget(p) {}
void AMSPreview::msw_rescale() {}
AMSHumidity::AMSHumidity(QWidget *p) : QWidget(p) {}
void AMSHumidity::msw_rescale() {}

AmsItem::AmsItem(QWidget *parent, AMSinfo info) : QWidget(parent), m_amsinfo(std::move(info)) {}
void AmsItem::Update(AMSinfo info) { m_amsinfo = std::move(info); update(); }
void AmsItem::SetAmsModel(DevAmsType) {}
void AmsItem::PlayRridLoading(const std::string &) {}
void AmsItem::StopRridLoading(const std::string &) {}
void AmsItem::SetExtruder(bool, int) {}
void AmsItem::SetAmsStep(const std::string &, int, AMSPassRoadType, AMSPassRoadSTEP) {}
void AmsItem::SwitchAms(const std::string &) {}
void AmsItem::msw_rescale() { update(); }

}} // namespace Slic3r::GUI

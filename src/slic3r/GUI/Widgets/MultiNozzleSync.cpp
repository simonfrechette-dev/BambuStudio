// MultiNozzleSync.cpp — Phase 2 stub. Full implementation deferred to Phase 3/4.
#include "MultiNozzleSync.hpp"
#include "slic3r/GUI/DeviceCore/DevNozzleRack.h"  // full type needed in .cpp only

namespace Slic3r::GUI {

// ---- ManualNozzleCountDialog ----
ManualNozzleCountDialog::ManualNozzleCountDialog(QWidget *parent, NozzleVolumeType volume_type,
    int /*standard_count*/, int /*highflow_count*/, int /*max_nozzle_count*/, bool /*force_no_zero*/)
    : QDialog(parent), m_volume_type(volume_type)
{}
int ManualNozzleCountDialog::GetNozzleCount(NozzleVolumeType) const { return 0; }

// ---- ExtruderBadge ----
ExtruderBadge::ExtruderBadge(QWidget *parent) : QWidget(parent) {}
void ExtruderBadge::SetExtruderInfo(int, const std::string &, const NozzleVolumeType &) {}
void ExtruderBadge::UnMarkRelatedItems(const NozzleOption &) {}
void ExtruderBadge::MarkRelatedItems(const NozzleOption &) {}
void ExtruderBadge::SetExtruderValid(bool right_on) { m_right_on = right_on; update(); }
void ExtruderBadge::SetExtruderStatus(bool, bool) {}

// ---- HotEndTable ----
HotEndTable::HotEndTable(QWidget *parent) : QWidget(parent) {}
void HotEndTable::UpdateRackInfo(std::weak_ptr<DevNozzleRack>) {}
void HotEndTable::MarkRelatedItems(const NozzleOption &) {}
void HotEndTable::UnMarkRelatedItems(const NozzleOption &) {}
StaticBox *HotEndTable::CreateNozzleBox(const std::vector<int> &) { return nullptr; }
void HotEndTable::UpdateNozzleItems(const std::unordered_map<int, wgtDeviceNozzleRackNozzleItem *> &,
                                    std::shared_ptr<DevNozzleRack>) {}
std::vector<int> HotEndTable::FilterHotEnds(const NozzleOption &) { return {}; }

// ---- NozzleListTable ----
NozzleListTable::NozzleListTable(QWidget *parent) : QWidget(parent) {}
int  NozzleListTable::GetSelectIdx() { return m_selected_idx; }
void NozzleListTable::SetOptions(const std::vector<NozzleOption> &options, int default_select) {
    m_nozzle_options = options;
    m_selected_idx   = default_select;
}
QString NozzleListTable::BuildTableObjStr() { return {}; }
QString NozzleListTable::BuildTextObjStr()  { return {}; }
void    NozzleListTable::SendSelectionChangedEvent() { emit selectionChanged(m_selected_idx); }

// ---- MultiNozzleStatusTable ----
MultiNozzleStatusTable::MultiNozzleStatusTable(QWidget *parent) : QWidget(parent) {}
void MultiNozzleStatusTable::UpdateRackInfo(std::weak_ptr<DevNozzleRack>) {}
void MultiNozzleStatusTable::MarkRelatedItems(const NozzleOption &) {}
void MultiNozzleStatusTable::UnMarkRelatedItems(const NozzleOption &) {}

// ---- MultiNozzleSyncDialog ----
MultiNozzleSyncDialog::MultiNozzleSyncDialog(QWidget *parent, std::weak_ptr<DevNozzleRack> rack)
    : QDialog(parent), m_nozzle_rack(rack)
{
    m_refresh_timer = new QTimer(this);
    connect(m_refresh_timer, &QTimer::timeout, this, &MultiNozzleSyncDialog::onRefreshTimer);
}
MultiNozzleSyncDialog::~MultiNozzleSyncDialog() {}
int MultiNozzleSyncDialog::exec() { return QDialog::exec(); }

std::vector<NozzleOption> MultiNozzleSyncDialog::GetNozzleOptions(
    const std::vector<MultiNozzleUtils::NozzleGroupInfo> &)
{ return {}; }

void MultiNozzleSyncDialog::UpdateRackInfo(std::weak_ptr<DevNozzleRack>) {}
bool MultiNozzleSyncDialog::hasMultiDiameters(const std::vector<MultiNozzleUtils::NozzleGroupInfo> &) { return false; }
void MultiNozzleSyncDialog::OnSelectRadio(int idx) { m_nozzle_option_idx = idx; }
bool MultiNozzleSyncDialog::UpdateUi(std::weak_ptr<DevNozzleRack>, bool, bool) { return false; }
bool MultiNozzleSyncDialog::UpdateOptionList(std::weak_ptr<DevNozzleRack>, bool, bool) { return false; }
void MultiNozzleSyncDialog::UpdateTip(std::weak_ptr<DevNozzleRack>, bool, bool) {}
void MultiNozzleSyncDialog::UpdateButton(std::weak_ptr<DevNozzleRack>, bool, bool) {}
void MultiNozzleSyncDialog::onRefreshTimer() {}

// ---- Free functions ----
std::optional<NozzleOption> tryPopUpMultiNozzleDialog(MachineObject *) { return std::nullopt; }
void setExtruderNozzleCount(PresetBundle *, int, NozzleVolumeType, int, bool) {}
void updateNozzleCountDisplay(PresetBundle *, int, NozzleVolumeType) {}
void manuallySetNozzleCount(int) {}

} // namespace Slic3r::GUI

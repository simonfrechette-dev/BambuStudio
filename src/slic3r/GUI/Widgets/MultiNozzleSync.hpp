#ifndef MULTI_NOZZLE_SYNC_HPP
#define MULTI_NOZZLE_SYNC_HPP

#include "../QtExtensions.hpp"
#include "RadioBox.hpp"
#include "Button.hpp"
#include "Label.hpp"
#include "libslic3r/MultiNozzleUtils.hpp"   // NozzleVolumeType, MultiNozzleUtils::NozzleGroupInfo

// Forward-declare DevNozzleRack in its correct namespace
namespace Slic3r { class DevNozzleRack; }

#include <QWidget>
#include <QDialog>
#include <QComboBox>
#include <QTimer>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// NozzleListTable uses QWebEngineView (Phase 4), forward-declare here
QT_BEGIN_NAMESPACE
class QWebEngineView;
QT_END_NAMESPACE

#define ENABLE_MIX_FLOW_PRINT 1

namespace Slic3r {
    class PresetBundle;
}

namespace Slic3r::GUI {

class MachineObject;

// wgtDeviceNozzleRackNozzleItem is a DeviceTab widget — forward-declare
class wgtDeviceNozzleRackNozzleItem;

#if ENABLE_MIX_FLOW_PRINT
struct NozzleOption {
    std::string diameter;
    std::unordered_map<int, std::unordered_map<NozzleVolumeType, int>> extruder_nozzle_stats;
};
#else
struct NozzleOption {
    std::string diameter;
    std::unordered_map<int, std::pair<NozzleVolumeType, int>> extruder_nozzle_stats;
};
#endif

// ---- ManualNozzleCountDialog ----
class ManualNozzleCountDialog : public QDialog
{
    Q_OBJECT
public:
    ManualNozzleCountDialog(QWidget *parent, NozzleVolumeType volume_type,
                            int standard_count, int highflow_count,
                            int max_nozzle_count, bool force_no_zero);
    int GetNozzleCount(NozzleVolumeType volume_type) const;
private:
    QComboBox *m_standard_choice = nullptr;
    QComboBox *m_highflow_choice = nullptr;
    Button    *m_confirm_btn     = nullptr;
    Label     *m_error_label     = nullptr;
    NozzleVolumeType m_volume_type;
};

// ---- ExtruderBadge ----
class ExtruderBadge : public QWidget
{
    Q_OBJECT
public:
    explicit ExtruderBadge(QWidget *parent = nullptr);
    void SetExtruderInfo(int extruder_id, const std::string &label, const NozzleVolumeType &flow);
    void UnMarkRelatedItems(const NozzleOption &option);
    void MarkRelatedItems(const NozzleOption &option);
    void SetExtruderValid(bool right_on);
private:
    void SetExtruderStatus(bool left_selected, bool right_selected);
    bool   m_right_on = true;
    Label *m_left = nullptr, *m_right = nullptr;
    Label *m_left_diameter_desp = nullptr, *m_right_diameter_desp = nullptr;
    Label *m_left_flow_desp = nullptr, *m_right_flow_desp = nullptr;
    std::vector<std::string>     m_diameter_list;
    std::vector<NozzleVolumeType> m_volume_type_list;
};

// ---- HotEndTable ----
class HotEndTable : public QWidget
{
    Q_OBJECT
public:
    explicit HotEndTable(QWidget *parent = nullptr);
    void UpdateRackInfo(std::weak_ptr<DevNozzleRack> rack);
    void MarkRelatedItems(const NozzleOption &option);
    void UnMarkRelatedItems(const NozzleOption &option);
private:
    StaticBox *CreateNozzleBox(const std::vector<int> &nozzle_indices);
    void UpdateNozzleItems(const std::unordered_map<int, wgtDeviceNozzleRackNozzleItem *> &items,
                           std::shared_ptr<DevNozzleRack> rack);

    struct HotEndAttr {
        std::string     diameter;
        int             extruder_id = 0;
        NozzleVolumeType volume_type{};
    };
    std::vector<int> FilterHotEnds(const NozzleOption &option);

    StaticBox *m_arow_nozzle_box = nullptr;
    StaticBox *m_brow_nozzle_box = nullptr;
    std::unordered_map<int, wgtDeviceNozzleRackNozzleItem *> m_nozzle_items;
    std::weak_ptr<DevNozzleRack> m_nozzle_rack;
};

// ---- NozzleListTable (uses QWebEngineView — Phase 4) ----
class NozzleListTable : public QWidget
{
    Q_OBJECT
public:
    explicit NozzleListTable(QWidget *parent = nullptr);
    int  GetSelectIdx();
    void SetOptions(const std::vector<NozzleOption> &options, int default_select);
signals:
    void selectionChanged(int idx);
private:
    QString BuildTableObjStr();
    QString BuildTextObjStr();
    void    SendSelectionChangedEvent();

    std::vector<NozzleOption> m_nozzle_options;
    QWebEngineView *m_web_view = nullptr;
    int             m_selected_idx = -1;
};

// ---- MultiNozzleStatusTable ----
class MultiNozzleStatusTable : public QWidget
{
    Q_OBJECT
public:
    explicit MultiNozzleStatusTable(QWidget *parent = nullptr);
    void UpdateRackInfo(std::weak_ptr<DevNozzleRack> rack);
    void MarkRelatedItems(const NozzleOption &option);
    void UnMarkRelatedItems(const NozzleOption &option);
private:
    ExtruderBadge *m_badge = nullptr;
    HotEndTable   *m_table = nullptr;
};

// ---- MultiNozzleSyncDialog ----
class MultiNozzleSyncDialog : public QDialog
{
    Q_OBJECT
public:
    MultiNozzleSyncDialog(QWidget *parent, std::weak_ptr<DevNozzleRack> rack);
    ~MultiNozzleSyncDialog() override;

    std::vector<NozzleOption> GetNozzleOptions(
        const std::vector<MultiNozzleUtils::NozzleGroupInfo> &group_infos);

    std::optional<NozzleOption> GetSelectedOption() const {
        if (m_nozzle_option_idx < 0 || m_nozzle_option_idx >= (int)m_nozzle_option_values.size())
            return std::nullopt;
        return m_nozzle_option_values[m_nozzle_option_idx];
    }

    int exec() override; // ShowModal → exec()

private:
    void UpdateRackInfo(std::weak_ptr<DevNozzleRack> rack);
    bool hasMultiDiameters(const std::vector<MultiNozzleUtils::NozzleGroupInfo> &group_infos);
    void OnSelectRadio(int select_idx);
    bool UpdateUi(std::weak_ptr<DevNozzleRack> rack,
                  bool ignore_unknown = false, bool ignore_unreliable = false);
    bool UpdateOptionList(std::weak_ptr<DevNozzleRack> rack,
                          bool ignore_unknown, bool ignore_unreliable);
    void UpdateTip(std::weak_ptr<DevNozzleRack> rack,
                   bool ignore_unknown, bool ignore_unreliable);
    void UpdateButton(std::weak_ptr<DevNozzleRack> rack,
                      bool ignore_unknown, bool ignore_unreliable);

private slots:
    void onRefreshTimer();

private:
    MultiNozzleStatusTable  *m_nozzle_table       = nullptr;
    NozzleListTable         *m_list_table          = nullptr;
    std::vector<NozzleOption> m_nozzle_option_values;
    int                      m_nozzle_option_idx  = -1;
    bool                     m_refreshing         = false;
    std::weak_ptr<DevNozzleRack> m_nozzle_rack;
    Label  *m_tips       = nullptr;
    Label  *m_caution    = nullptr;
    QTimer *m_refresh_timer = nullptr;
    size_t  m_rack_event_token = 0;
    Button *m_cancel_btn = nullptr;
    Button *m_confirm_btn = nullptr;
};

// ---- Free functions ----
std::optional<NozzleOption> tryPopUpMultiNozzleDialog(MachineObject *obj);
void setExtruderNozzleCount(PresetBundle *preset_bundle, int extruder_id,
                            NozzleVolumeType type, int nozzle_count, bool clear_before_set);
void updateNozzleCountDisplay(PresetBundle *preset_bundle, int extruder_id,
                              NozzleVolumeType volume_type);
void manuallySetNozzleCount(int extruder_id);

} // namespace Slic3r::GUI

#endif // MULTI_NOZZLE_SYNC_HPP

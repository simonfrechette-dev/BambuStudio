#ifndef slic3r_GUI_AMSCONTROL_hpp_
#define slic3r_GUI_AMSCONTROL_hpp_

#include "../QtExtensions.hpp"
#include "StaticBox.hpp"
#include "AMSItem.hpp"
#include "Button.hpp"

#include <QWidget>
#include <QStackedWidget>
#include <vector>
#include <string>
#include <map>

namespace Slic3r { namespace GUI {

class MachineObject;
class uiAmsPercentHumidityDryPopup;

class AMSControl : public QStackedWidget
{
    Q_OBJECT
public:
    AMSControl(QWidget *parent = nullptr);

    int  m_total_ext_count  = 1;
    int  m_left_page_index  = 0;
    int  m_right_page_index = 0;
    bool m_single_nozzle_no_ams = true;
    StaticBox *m_panel_virtual = nullptr;

    void on_retry();
    void createAms(QStackedWidget *parent, int &idx, AMSinfo info, AMSPanelPos pos);
    void createAmsPanel(QStackedWidget *parent, int &idx, std::vector<AMSinfo> infos,
                        const std::string &series_name, const std::string &printer_type,
                        AMSPanelPos pos, int total_ext_num);

    void SetAmsModel(DevAmsType mode, DevAmsType ext_mode) { m_ams_model = mode; m_ext_model = ext_mode; }
    void EnableLoadFilamentBtn(bool enable, const std::string &ams_id, const std::string &can_id, const QString &tips);
    void EnableUnLoadFilamentBtn(bool enable, const std::string &ams_id, const std::string &can_id, const QString &tips);
    void EnterNoneAMSMode();
    void EnterGenericAMSMode();
    void EnterExtraAMSMode();
    void PlayRridLoading(const QString &amsid, const QString &canid);
    void StopRridLoading(const QString &amsid, const QString &canid);
    void ShowFilamentTip(bool hasams = true);
    void UpdatePassRoad(const std::string &ams_id, AMSPassRoadType type, AMSPassRoadSTEP step);
    void CreateAmsDoubleNozzle(const std::string &series_name, const std::string &printer_type);
    void CreateAmsSingleNozzle(const std::string &series_name, const std::string &printer_type);
    void ClearAms();
    void UpdateAmsDryControl(MachineObject *obj);
    void UpdateAms(const std::string &series_name, bool is_reset = true, bool test = false);
    void SetExtruder(bool on_off, int nozzle_id, const std::string &ams_id, const std::string &slot_id);
    void SetAmsStep(const std::string &ams_id, const std::string &canid, int extruder_id,
                    AMSPassRoadType type, AMSPassRoadSTEP step);
    void SwitchAms(const std::string &ams_id);
    void msw_rescale();
    void Reset();
    // Current AMS/slot selection
    std::string GetCurentAms() const { return m_current_ams_id; }
    std::string GetCurrentCan(const std::string & /*ams_id*/) const { return m_current_can_id; }
    void SetCurrentSelection(const std::string &ams_id, const std::string &can_id)
        { m_current_ams_id = ams_id; m_current_can_id = can_id; }
    bool isFilaSwitchInstalled() const;
    void show_switcher_status(bool show);
    void show_noams_mode();
    void show_auto_refill(bool show);
    void enable_ams_setting(bool en);
    void show_vams_kn_value(bool show);
    bool Enable(bool enable = true);
    void parse_object(MachineObject *obj);
    void AddAms(AMSinfo info, AMSPanelPos pos);
    void AddAms(std::vector<AMSinfo> single_info, const std::string &series_name,
                const std::string &printer_type, AMSPanelPos pos);
    void AddAmsPreview(AMSinfo info, AMSPanelPos pos);
    void AddAmsPreview(std::vector<AMSinfo> single_info, AMSPanelPos pos);
    void UpdateAmsPreviewSelection();
    bool IsInSlotPair(const std::string &ams_id) const;
    bool GetExtPairedDoubleMode(const std::string &ams_id, AMSPanelPos panel_pos,
                                AMSRoadShowMode &out_mode) const;

signals:
    void amsSelectionChanged(const std::string &ams_id, const std::string &can_id);
    void loadClicked();
    void unloadClicked();

private:
    DevAmsType m_ams_model = DevAmsType::AMS;
    DevAmsType m_ext_model = DevAmsType::AMS_LITE;
    std::map<std::string, AmsItem *> m_ams_items;
    std::string m_current_ams_id;
    std::string m_current_can_id;
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_AMSCONTROL_hpp_

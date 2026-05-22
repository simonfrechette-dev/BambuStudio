#ifndef slic3r_GUI_AMSITEM_hpp_
#define slic3r_GUI_AMSITEM_hpp_

#include "../QtExtensions.hpp"
#include "StaticBox.hpp"
#include "StepCtrl.hpp"
#include "Button.hpp"
#include "../DeviceCore/DevDefs.h"      // DevAmsType, INVALID_AMS_TEMPERATURE
#include "../DeviceCore/DevFilaSystem.h" // DevAms
#include "../DeviceCore/DevFilaSwitch.h" // DevFilaSwitch::SwitchPos

#include <QWidget>
#include <QColor>
#include <QString>
#include <QSize>
#include <set>
#include <vector>
#include <string>
#include <optional>
#include <memory>

// Colour constants (replace wx macros)
inline constexpr QColor AMS_CONTROL_BRAND_COLOUR()        { return QColor(0, 174, 66); }
inline constexpr QColor AMS_CONTROL_GRAY700()              { return QColor(107, 107, 107); }
inline constexpr QColor AMS_CONTROL_GRAY800()              { return QColor(50, 58, 61); }
inline constexpr QColor AMS_CONTROL_GRAY500()              { return QColor(172, 172, 172); }
inline constexpr QColor AMS_CONTROL_DISABLE_COLOUR()       { return QColor(206, 206, 206); }
inline constexpr QColor AMS_CONTROL_DISABLE_TEXT_COLOUR()  { return QColor(144, 144, 144); }

#define AMS_CONTROL_MAX_COUNT 4

namespace Slic3r { namespace GUI {

class MachineObject;

// ---- Enums -----

enum AMSModelOriginType { GENERIC_EXT, LITE_EXT };
enum ActionButton { ACTION_BTN_CALI = 0, ACTION_BTN_LOAD = 1, ACTION_BTN_UNLOAD = 2, ACTION_BTN_COUNT = 3 };

enum class AMSRoadMode : int {
    AMS_ROAD_MODE_LEFT, AMS_ROAD_MODE_LEFT_RIGHT, AMS_ROAD_MODE_END,
    AMS_ROAD_MODE_END_ONLY, AMS_ROAD_MODE_NONE, AMS_ROAD_MODE_NONE_ANY_ROAD,
    AMS_ROAD_MODE_VIRTUAL_TRAY
};

enum class AMSPanelPos : int { RIGHT_PANEL = 0, LEFT_PANEL = 1 };

enum class AMSRoadShowMode : int {
    AMS_ROAD_MODE_FOUR, AMS_ROAD_MODE_DOUBLE, AMS_ROAD_MODE_SINGLE,
    AMS_ROAD_MODE_SINGLE_N3S, AMS_ROAD_MODE_AMS_LITE, AMS_ROAD_MODE_ARROW, AMS_ROAD_MODE_NONE,
    AMS_ROAD_MODE_DOUBLE_FAR_ONLY, AMS_ROAD_MODE_DOUBLE_NEAR_ONLY,
};

enum class AMSPassRoadMode : int {
    AMS_ROAD_MODE_NONE, AMS_ROAD_MODE_LEFT, AMS_ROAD_MODE_LEFT_RIGHT,
    AMS_ROAD_MODE_END_TOP, AMS_ROAD_MODE_END_RIGHT, AMS_ROAD_MODE_END_BOTTOM,
};

enum class AMSAction : int {
    AMS_ACTION_NONE, AMS_ACTION_LOAD, AMS_ACTION_UNLOAD, AMS_ACTION_CALI,
    AMS_ACTION_PRINTING, AMS_ACTION_NORMAL, AMS_ACTION_NOAMS,
};

enum class AMSPassRoadSTEP : int {
    AMS_ROAD_STEP_NONE = 0, AMS_ROAD_STEP_1 = 1, AMS_ROAD_STEP_2 = 2,
    AMS_ROAD_STEP_3 = 4,
    AMS_ROAD_STEP_COMBO_LOAD_STEP1, AMS_ROAD_STEP_COMBO_LOAD_STEP2, AMS_ROAD_STEP_COMBO_LOAD_STEP3,
};

enum class AMSPassRoadType : int {
    AMS_ROAD_TYPE_NONE, AMS_ROAD_TYPE_LOAD, AMS_ROAD_TYPE_UNLOAD,
};

enum class AMSCanType : int {
    AMS_CAN_TYPE_NONE, AMS_CAN_TYPE_BRAND, AMS_CAN_TYPE_THIRDBRAND,
    AMS_CAN_TYPE_EMPTY, AMS_CAN_TYPE_VIRTUAL,
};

#ifndef FILAMENT_STEP_TYPE_DEFINED
#define FILAMENT_STEP_TYPE_DEFINED
enum FilamentStepType { STEP_TYPE_LOAD = 0, STEP_TYPE_UNLOAD = 1, STEP_TYPE_VT_LOAD = 2 };
#endif

// ---- Data structures ----

struct Caninfo {
    std::string     can_id;
    QString         material_name;
    QColor          material_colour   = Qt::white;
    AMSCanType      material_state    = AMSCanType::AMS_CAN_TYPE_NONE;
    int             ctype             = 0;
    int             material_remain   = 100;
    int             cali_idx          = -1;
    std::string     filament_id;
    float           k = 0.f, n = 0.f;
    std::vector<QColor> material_cols;
    bool operator==(const Caninfo &o) const;
};

struct AMSinfo {
    std::string          ams_id;
    std::vector<Caninfo> cans;
    std::string          current_can_id;
    AMSPassRoadSTEP      current_step     = AMSPassRoadSTEP::AMS_ROAD_STEP_NONE;
    AMSAction            current_action   = AMSAction::AMS_ACTION_NONE;
    int                  curreent_filamentstep = 0;
    int                  ams_humidity     = 0;
    int                  ams_humidity_percent = -1;
    int                  left_dray_time   = 0;
    float                current_temperature = -1.0f;   // INVALID_AMS_TEMPERATURE
    DevAmsType           ams_type         = DevAmsType::AMS;
    AMSModelOriginType   ext_type         = AMSModelOriginType::GENERIC_EXT;
    bool                 m_ams_drying     = false;
    std::set<int>                             binded_extruder_set;
    std::optional<int>                        current_extruder_id;
    std::optional<DevFilaSwitch::SwitchPos>   binded_switcher_pos;

    bool operator==(const AMSinfo &o) const;
    bool operator!=(const AMSinfo &o) const { return !operator==(o); }

    bool parse_ams_info(MachineObject *obj, DevAms *ams,
                        bool remain_flag = false, bool humidity_flag = false);
    void parse_ext_info(MachineObject *obj, DevAmsTray tray);

    bool support_drying()   const;
    bool support_humidity() const;
    Caninfo get_caninfo(const std::string &can_id, bool &found) const;
    int  get_humidity_display_idx() const;
    AMSPanelPos GetDefaultPanelPos(int total_extruder_count) const;
};

// ---- Widget stub declarations ----
// Full implementations are deferred to Phase 3 (device layer port)

class AMSExtText    : public QWidget { Q_OBJECT public: AMSExtText(QWidget *p = nullptr); void msw_rescale(); };
class AMSrefresh    : public QWidget { Q_OBJECT public: AMSrefresh(QWidget *p = nullptr); void msw_rescale(); signals: void clicked(); };
class AMSextruderImage : public QWidget { Q_OBJECT public: AMSextruderImage(QWidget *p = nullptr); void msw_rescale(); };
class SwitcherImage : public QWidget { Q_OBJECT public: SwitcherImage(QWidget *p = nullptr); void msw_rescale(); };
class AMSExtImage   : public QWidget { Q_OBJECT public: AMSExtImage(QWidget *p = nullptr); void msw_rescale(); };
class AMSextruder   : public QWidget { Q_OBJECT public: AMSextruder(QWidget *p = nullptr); void msw_rescale(); };

class AMSLib : public StaticBox {
    Q_OBJECT
public:
    AMSLib(QWidget *parent = nullptr, Caninfo info = {});
    void Update(Caninfo info);
    void SetAmsModel(DevAmsType type);
    void msw_rescale();
    Caninfo m_info;
signals:
    void clicked(const std::string &ams_id, const std::string &can_id);
};

class AMSRoad       : public QWidget { Q_OBJECT public: AMSRoad(QWidget *p = nullptr); void msw_rescale(); };
class AMSRoadUpPart : public QWidget { Q_OBJECT public: AMSRoadUpPart(QWidget *p = nullptr); void msw_rescale(); };
class AMSRoadDownPart : public QWidget { Q_OBJECT public: AMSRoadDownPart(QWidget *p = nullptr); void msw_rescale(); };
class AMSPreview    : public QWidget { Q_OBJECT public: AMSPreview(QWidget *p = nullptr); void msw_rescale(); };
class AMSHumidity   : public QWidget { Q_OBJECT public: AMSHumidity(QWidget *p = nullptr); void msw_rescale(); };

class AmsItem : public QWidget {
    Q_OBJECT
public:
    AmsItem(QWidget *parent = nullptr, AMSinfo info = {});
    void Update(AMSinfo info);
    void SetAmsModel(DevAmsType type);
    void PlayRridLoading(const std::string &canid);
    void StopRridLoading(const std::string &canid);
    void SetExtruder(bool on_off, int nozzle_id);
    void SetAmsStep(const std::string &canid, int extruder_id, AMSPassRoadType type, AMSPassRoadSTEP step);
    void SwitchAms(const std::string &ams_id);
    void msw_rescale();
    AMSinfo m_amsinfo;
signals:
    void clicked(const std::string &ams_id, const std::string &can_id);
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_AMSITEM_hpp_

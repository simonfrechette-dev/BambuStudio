#ifndef slic3r_GUI_FILAMENTLOAD_hpp_
#define slic3r_GUI_FILAMENTLOAD_hpp_

#include "../QtExtensions.hpp"
#include "StaticBox.hpp"
#include "StepCtrl.hpp"
#include "../DeviceCore/DevDefs.h"   // DevFilamentStep, DevAmsType

#include <QWidget>
#include <QStackedWidget>
#include <QColor>
#include <map>
#include <string>

namespace Slic3r { namespace GUI {

class MachineObject;

// FilamentStepType forward from AMSItem; define here to avoid circular include
#ifndef FILAMENT_STEP_TYPE_DEFINED
#define FILAMENT_STEP_TYPE_DEFINED
enum FilamentStepType { STEP_LOAD = 0, STEP_UNLOAD, STEP_VT_LOAD };
#endif

class FilamentLoad : public QStackedWidget
{
    Q_OBJECT
public:
    FilamentLoad(QWidget *parent = nullptr);

    // Maps DevFilamentStep → display label
    std::map<DevFilamentStep, QString> FILAMENT_CHANGE_STEP_STRING;
    DevAmsType m_ams_model{ DevAmsType::AMS };
    DevAmsType m_ext_model{ DevAmsType::AMS_LITE };
    DevAmsType m_is_none_ams_mode{ DevAmsType::AMS_LITE };

    void SetAmsModel(DevAmsType mode, DevAmsType ext_mode) { m_ams_model = mode; m_ext_model = ext_mode; }
    void SetFilamentStep(MachineObject *obj, DevFilamentStep item_idx, FilamentStepType f_type);
    void ShowFilamentTip(bool hasams = true);
    void SetupSteps(MachineObject *obj, bool is_extrusion_exist);
    void show_nofilament_mode(bool show);
    void updateID(int ams_id, int slot_id) { m_ams_id = ams_id; m_slot_id = slot_id; }
    void SetExt(bool ext) { is_extrusion = ext; }
    void set_min_size(const QSize &minSize);
    void set_max_size(const QSize &maxSize);
    void set_background_color(const QColor &colour);

protected:
    FilamentStepIndicator *m_filament_load_steps    = nullptr;
    FilamentStepIndicator *m_filament_unload_steps  = nullptr;
    FilamentStepIndicator *m_filament_vt_load_steps = nullptr;
    int  m_ams_id    = 1;
    int  m_slot_id   = 1;
    bool is_extrusion = false;
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_FILAMENTLOAD_hpp_

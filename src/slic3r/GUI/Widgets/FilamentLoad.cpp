#include "FilamentLoad.hpp"
#include <QVBoxLayout>

namespace Slic3r { namespace GUI {

FilamentLoad::FilamentLoad(QWidget *parent)
    : QStackedWidget(parent)
{
    m_filament_load_steps    = new FilamentStepIndicator(this);
    m_filament_unload_steps  = new FilamentStepIndicator(this);
    m_filament_vt_load_steps = new FilamentStepIndicator(this);

    addWidget(m_filament_load_steps);
    addWidget(m_filament_unload_steps);
    addWidget(m_filament_vt_load_steps);

    setCurrentIndex(0);
}

void FilamentLoad::SetFilamentStep(MachineObject * /*obj*/, DevFilamentStep item_idx,
                                    FilamentStepType f_type)
{
    FilamentStepIndicator *indicator = nullptr;
    switch (f_type) {
    case STEP_LOAD:    indicator = m_filament_load_steps;    break;
    case STEP_UNLOAD:  indicator = m_filament_unload_steps;  break;
    case STEP_VT_LOAD: indicator = m_filament_vt_load_steps; break;
    }
    if (indicator) indicator->SelectItem((int)item_idx);
}

void FilamentLoad::ShowFilamentTip(bool /*hasams*/) {}
void FilamentLoad::SetupSteps(MachineObject * /*obj*/, bool /*is_extrusion_exist*/) {}
void FilamentLoad::show_nofilament_mode(bool show) { setVisible(!show); }
void FilamentLoad::set_min_size(const QSize &s) { setMinimumSize(s); }
void FilamentLoad::set_max_size(const QSize &s) { setMaximumSize(s); }
void FilamentLoad::set_background_color(const QColor &colour)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, colour);
    setPalette(p);
}

}} // namespace Slic3r::GUI

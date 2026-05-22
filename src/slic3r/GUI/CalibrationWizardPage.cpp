// Qt6 stub for CalibrationWizardPage.cpp
// Phantom forward-declaration must precede ALL headers.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "CalibrationWizardPage.hpp"
// All implementations are stubs pending Qt port.

namespace Slic3r { namespace GUI {

CalibMode get_obj_calibration_mode(const MachineObject* /*obj*/)
{ return CalibMode::Calib_None; }

CalibMode get_obj_calibration_mode(const MachineObject* /*obj*/, int& cali_stage)
{ cali_stage = 0; return CalibMode::Calib_None; }

CalibMode get_obj_calibration_mode(const MachineObject* /*obj*/, CalibrationMethod& method, int& cali_stage)
{ method = CalibrationMethod::CALI_METHOD_AUTO; cali_stage = 0; return CalibMode::Calib_None; }

CalibrationFilamentMode get_cali_filament_mode(MachineObject* /*obj*/, CalibMode /*mode*/)
{ return CALI_MODEL_SINGLE; }

}} // namespace Slic3r::GUI

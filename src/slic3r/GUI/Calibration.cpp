// Qt6 stub for Calibration.cpp
// Phantom forward-declaration must precede ALL headers.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "Calibration.hpp"
// All implementations are stubs pending Qt port.

namespace Slic3r { namespace GUI {

CalibrationDialog::CalibrationDialog(Plater* /*plater*/)
    : DPIDialog(nullptr) {}

CalibrationDialog::~CalibrationDialog() {}

void CalibrationDialog::update_machine_obj(MachineObject* /*obj*/) {}

void CalibrationDialog::update_cali(MachineObject* /*obj*/) {}

void CalibrationDialog::on_dpi_changed(const QRect& /*r*/) {}

void CalibrationDialog::setVisible(bool visible) { DPIDialog::setVisible(visible); }

}} // namespace Slic3r::GUI

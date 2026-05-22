// Qt6 stub for AmsMappingPopup.cpp
// Original wx-based implementation replaced with empty stub.
// Phantom forward-declaration must precede ALL headers.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "AmsMappingPopup.hpp"

// All implementations are stubs pending Qt port.

namespace Slic3r { namespace GUI {

AmsReplaceMaterialDialog::AmsReplaceMaterialDialog(QWidget* parent)
    : DPIDialog(parent) {}

void AmsReplaceMaterialDialog::update_machine_obj(MachineObject* /*obj*/) {}

void AmsReplaceMaterialDialog::on_dpi_changed(const QRect& /*r*/) {}

void AmsReplaceMaterialDialog::paintEvent(QPaintEvent& /*evt*/) {}

}} // namespace Slic3r::GUI

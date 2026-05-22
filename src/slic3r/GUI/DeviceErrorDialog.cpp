// Qt6 stub for DeviceErrorDialog.cpp
#include "DeviceErrorDialog.hpp"
#include "slic3r/GUI/DeviceManager.hpp"

namespace Slic3r { namespace GUI {

DeviceErrorDialog::DeviceErrorDialog(Slic3r::MachineObject* /*obj*/, QWidget* parent)
    : DPIDialog(parent) {}

DeviceErrorDialog::~DeviceErrorDialog() {}

QString DeviceErrorDialog::show_error_code(int /*error_code*/) { return {}; }

}} // namespace Slic3r::GUI


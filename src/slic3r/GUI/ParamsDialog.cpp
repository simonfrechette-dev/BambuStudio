// Qt6 port stub for ParamsDialog.cpp
#include "ParamsDialog.hpp"
#include "ParamsPanel.hpp"
#include "GUI_Utils.hpp"

namespace Slic3r {
namespace GUI {

QEvent::Type getEVT_MODIFY_FILAMENTEventType() {
    static QEvent::Type t = static_cast<QEvent::Type>(QEvent::registerEventType());
    return t;
}

ParamsDialog::ParamsDialog(QWidget* parent)
    : DPIDialog(parent)
{
}

void ParamsDialog::Popup() {}
void ParamsDialog::on_dpi_changed(const QRect&) {}

} // namespace GUI
} // namespace Slic3r

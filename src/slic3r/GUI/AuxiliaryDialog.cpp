// Phase 4 TODO: Qt port of AuxiliaryDialog
#include "AuxiliaryDialog.hpp"
#include "I18N.hpp"
#include "GUI_AuxiliaryList.hpp"

namespace Slic3r {
namespace GUI {

AuxiliaryDialog::AuxiliaryDialog(QWidget* parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Auxiliaries"));
    m_aux_list = new AuxiliaryList(this);
    // TODO: layout setup
    resize(800, 600);
}

} // namespace GUI
} // namespace Slic3r

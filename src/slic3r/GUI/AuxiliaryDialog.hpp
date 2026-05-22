#ifndef slic3r_GUI_AuxiliaryDialog_hpp_
#define slic3r_GUI_AuxiliaryDialog_hpp_

// Phase 4 TODO: Qt port of AuxiliaryDialog

#include "GUI_Utils.hpp"

class AuxiliaryList;

namespace Slic3r {
namespace GUI {

class AuxiliaryDialog : public DPIDialog
{
    Q_OBJECT
public:
    explicit AuxiliaryDialog(QWidget* parent = nullptr);
    AuxiliaryList* aux_list() { return m_aux_list; }

protected:
    void on_dpi_changed(const QRect&) override {}

private:
    AuxiliaryList* m_aux_list = nullptr;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_AuxiliaryDialog_hpp_

#ifndef slic3r_GUI_ParamsDialog_hpp_
#define slic3r_GUI_ParamsDialog_hpp_

#include "GUI_Utils.hpp"

namespace Slic3r {
namespace GUI {

QEvent::Type getEVT_MODIFY_FILAMENTEventType();

class FilamentInfomation
{
public:
    std::string filament_id;
    std::string filament_name;
};

class ParamsPanel;

class ParamsDialog : public DPIDialog
{
public:
    ParamsDialog(QWidget *parent = nullptr);

    ParamsPanel * panel() { return m_panel; }

    void Popup();

    void set_editing_filament_id(std::string id) { m_editing_filament_id = id; }

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;

private:
    std::string   m_editing_filament_id;
    ParamsPanel * m_panel{nullptr};
};

} // namespace GUI
} // namespace Slic3r

#endif

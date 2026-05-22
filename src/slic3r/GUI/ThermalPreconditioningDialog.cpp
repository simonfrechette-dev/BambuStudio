// Qt6 stub for ThermalPreconditioningDialog.cpp
#include "ThermalPreconditioningDialog.hpp"

namespace Slic3r {
namespace GUI {

ThermalPreconditioningDialog::ThermalPreconditioningDialog(QWidget* parent, std::string dev_id, bool is_show_remain_time)
    : QDialog(parent)
    , m_dev_id(dev_id)
    , m_refresh_timer(nullptr)
    , m_remaining_time_label(nullptr)
    , m_explanation_label(nullptr)
    , m_ok_button(nullptr)
    , m_title_bitmap(nullptr)
{}

ThermalPreconditioningDialog::~ThermalPreconditioningDialog() {}
void ThermalPreconditioningDialog::update_thermal_remaining_time() {}
void ThermalPreconditioningDialog::create_ui() {}
void ThermalPreconditioningDialog::on_ok_clicked() {}
void ThermalPreconditioningDialog::on_timer() {}

} // namespace GUI
} // namespace Slic3r

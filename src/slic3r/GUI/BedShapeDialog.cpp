// BedShapeDialog.cpp — stubbed for Qt6 port
#include "BedShapeDialog.hpp"
#include <QVBoxLayout>

namespace Slic3r { namespace GUI {

const std::string BedShapePanel::NONE = "(none)";
const std::string BedShapePanel::EMPTY_STRING;

BedShape::BedShape(const ConfigOptionPoints& points)
    : m_build_volume(points.values, 0., {}, {}) {}

void BedShape::append_option_line(ConfigOptionsGroupShp optgroup, Parameter param, bool can_edit) { (void)optgroup; (void)param; (void)can_edit; }
QString BedShape::get_name(PageType type) { (void)type; return {}; }
BedShape::PageType BedShape::get_page_type() { return PageType::Custom; }
QString BedShape::get_full_name_with_params() { return {}; }
void BedShape::apply_optgroup_values(ConfigOptionsGroupShp optgroup) { (void)optgroup; }

void BedShapePanel::build_panel(const ConfigOptionPoints& default_pt, const ConfigOptionString& custom_texture, const ConfigOptionString& custom_model) { (void)default_pt; (void)custom_texture; (void)custom_model; }
ConfigOptionsGroupShp BedShapePanel::init_shape_options_page(const QString& title) { (void)title; return nullptr; }
void BedShapePanel::activate_options_page(ConfigOptionsGroupShp options_group) { (void)options_group; }
QWidget* BedShapePanel::init_texture_panel() { return nullptr; }
QWidget* BedShapePanel::init_model_panel() { return nullptr; }
void BedShapePanel::set_shape(const ConfigOptionPoints& points) { (void)points; }
void BedShapePanel::update_preview() {}
void BedShapePanel::update_shape() {}
void BedShapePanel::load_stl() {}
void BedShapePanel::load_texture() {}
void BedShapePanel::load_model() {}

void BedShapeDialog::build_dialog(const ConfigOptionPoints& default_pt, const ConfigOptionString& custom_texture, const ConfigOptionString& custom_model, bool can_edit)
{
    (void)can_edit;
    m_panel = new BedShapePanel(this);
    m_panel->build_panel(default_pt, custom_texture, custom_model);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_panel);
    setLayout(layout);
}

void BedShapeDialog::on_dpi_changed(const QRect& r) { (void)r; }

}} // namespace Slic3r::GUI

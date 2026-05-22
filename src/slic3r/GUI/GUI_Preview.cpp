// GUI_Preview.cpp — stubbed for Qt6 port
#include "GUI_Preview.hpp"
#include <QResizeEvent>

namespace Slic3r { namespace GUI {

BaseView::BaseView(QWidget* parent) : QWidget(parent) {}
BaseView::~BaseView() {}
bool BaseView::Show(bool show) { if(show) QWidget::show(); else QWidget::hide(); return true; }

View3D::View3D(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process)
    : BaseView(parent) { (void)bed; (void)model; (void)config; (void)process; }
View3D::~View3D() {}
void View3D::set_as_dirty() {}
void View3D::bed_shape_changed() {}
void View3D::plates_count_changed() {}
void View3D::select_view(const std::string& direction) { (void)direction; }
void View3D::select_curr_plate_all() {}
void View3D::remove_curr_plate_all() {}
void View3D::select_all() {}
void View3D::deselect_all() {}
void View3D::exit_gizmo() {}
void View3D::delete_selected() {}
void View3D::center_selected() {}
void View3D::center_selected_plate(const int plate_idx) { (void)plate_idx; }
void View3D::mirror_selection(Axis axis) { (void)axis; }
bool View3D::is_layers_editing_enabled() const { return false; }
bool View3D::is_layers_editing_allowed() const { return false; }
void View3D::enable_layers_editing(bool enable) { (void)enable; }
bool View3D::is_dragging() const { return false; }
bool View3D::is_reload_delayed() const { return false; }
void View3D::reload_scene(bool refresh_immediately, bool force_full_scene_refresh) { (void)refresh_immediately; (void)force_full_scene_refresh; }
void View3D::render() {}
bool View3D::init(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process) { (void)parent; (void)bed; (void)model; (void)config; (void)process; return true; }

Preview::Preview(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process, GCodeProcessorResult* gcode_result, std::function<void()> schedule_background_process)
    : BaseView(parent) { (void)bed; (void)model; (void)config; (void)process; (void)gcode_result; (void)schedule_background_process; }
Preview::~Preview() {}
void Preview::update_gcode_result(GCodeProcessorResult* gcode_result) { (void)gcode_result; }
void Preview::set_as_dirty() {}
void Preview::bed_shape_changed() {}
void Preview::select_view(const std::string& direction) { (void)direction; }
void Preview::load_print(bool keep_z_range, bool only_gcode) { (void)keep_z_range; (void)only_gcode; }
void Preview::reload_print(bool keep_volumes, bool only_gcode) { (void)keep_volumes; (void)only_gcode; }
void Preview::refresh_print() {}
void Preview::load_shells(const Print& print, bool force_previewing) { (void)print; (void)force_previewing; }
void Preview::reset_shells() {}
void Preview::msw_rescale() {}
void Preview::sys_color_changed() {}
void Preview::on_tick_changed(CustomGCode::Type type) { (void)type; }
void Preview::show_sliders(bool show) { (void)show; }
void Preview::show_moves_sliders(bool show) { (void)show; }
void Preview::show_layers_sliders(bool show) { (void)show; }
bool Preview::init(QWidget* parent, Bed3D& bed, Model* model) { (void)parent; (void)bed; (void)model; return true; }
void Preview::bind_event_handlers() {}
void Preview::unbind_event_handlers() {}
void Preview::resizeEvent(QResizeEvent* event) { (void)event; }
void Preview::update_layers_slider_mode() {}
void Preview::load_print_as_fff(bool keep_z_range, bool only_gcode) { (void)keep_z_range; (void)only_gcode; }

AssembleView::AssembleView(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process)
    : BaseView(parent) { (void)bed; (void)model; (void)config; (void)process; }
AssembleView::~AssembleView() {}
void AssembleView::set_as_dirty() {}
void AssembleView::render() {}
bool AssembleView::is_reload_delayed() const { return false; }
void AssembleView::reload_scene(bool refresh_immediately, bool force_full_scene_refresh) { (void)refresh_immediately; (void)force_full_scene_refresh; }
void AssembleView::select_view(const std::string& direction) { (void)direction; }
bool AssembleView::init(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process) { (void)parent; (void)bed; (void)model; (void)config; (void)process; return true; }

}} // namespace Slic3r::GUI

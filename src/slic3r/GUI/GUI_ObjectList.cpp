// Qt6 port stub for GUI_ObjectList.cpp
// Original implementation was wx-based; this is a no-op stub to allow compilation.

#include "libslic3r/libslic3r.h"
#include "GUI_ObjectList.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include <QCursor>

namespace Slic3r {
namespace GUI {

ObjectList::ObjectList(QWidget* parent) : QTreeView(parent), m_objects_model(nullptr), m_config(nullptr), m_objects(nullptr) {}
ObjectList::~ObjectList() {}

void ObjectList::set_min_height() {}
void ObjectList::update_min_height() {}

ModelObject* ObjectList::object(const int) const { return nullptr; }

void ObjectList::create_objects_ctrl() {}
void ObjectList::update_objects_list_filament_column(size_t) {}
void ObjectList::update_objects_list_filament_column_when_delete_filament(size_t, size_t, int) {}
void ObjectList::update_filament_colors() {}
void ObjectList::set_filament_column_hidden(const bool) const {}
void ObjectList::set_variable_height_column_hidden(const bool) const {}
void ObjectList::set_color_paint_hidden(const bool) const {}
void ObjectList::set_support_paint_hidden(const bool) const {}
void ObjectList::set_fuzzy_skin_paint_hidden(const bool) const {}
void ObjectList::set_sinking_hidden(const bool) const {}

void ObjectList::update_filament_in_config(const QModelIndex&) {}
void ObjectList::update_name_in_model(const QModelIndex&) const {}
void ObjectList::update_name_in_list(int, int) const {}
void ObjectList::update_filament_values_for_items(const size_t) {}
void ObjectList::update_filament_values_for_items_when_delete_filament(const size_t, const int) {}
void ObjectList::update_plate_values_for_items() {}
void ObjectList::update_name_for_items() {}

void ObjectList::get_selected_item_indexes(int&, int&, const QModelIndex&) {}
void ObjectList::get_selection_indexes(std::vector<int>&, std::vector<int>&) {}
int  ObjectList::get_repaired_errors_count(const int, const int) const { return 0; }
MeshErrorsInfo ObjectList::get_mesh_errors_info(const int, const int, QString*, MeshIssueCounts*) const { return {}; }
MeshErrorsInfo ObjectList::get_mesh_errors_info(QString*, MeshIssueCounts*) { return {}; }
void ObjectList::set_tooltip_for_item(const QPoint&) {}

void ObjectList::selection_changed() {}
void ObjectList::show_context_menu(const bool) {}
void ObjectList::extruder_editing() {}
void ObjectList::key_event(QKeyEvent*) {}

void ObjectList::copy() {}
void ObjectList::paste() {}
void ObjectList::cut() {}
void ObjectList::clone() {}
bool ObjectList::cut_to_clipboard() { return false; }
bool ObjectList::copy_to_clipboard() { return false; }
bool ObjectList::paste_from_clipboard() { return false; }
void ObjectList::undo() {}
void ObjectList::redo() {}
void ObjectList::increase_instances() {}
void ObjectList::decrease_instances() {}

void ObjectList::add_category_to_settings_from_selection(const std::vector<std::pair<std::string,bool>>&, QModelIndex) {}
void ObjectList::add_category_to_settings_from_frequent(const std::vector<std::string>&, QModelIndex) {}
void ObjectList::show_settings(const QModelIndex) {}
bool ObjectList::is_instance_or_object_selected() { return false; }

void ObjectList::load_subobject(ModelVolumeType, bool) {}
void ObjectList::load_from_files(const QStringList&, ModelObject&, std::vector<ModelVolume*>&, ModelVolumeType, bool) {}
void ObjectList::load_generic_subobject(const std::string&, const ModelVolumeType) {}
void ObjectList::add_new_model_object_from_old_object() {}
void ObjectList::load_shape_object(const std::string&) {}
void ObjectList::load_mesh_object(const TriangleMesh&, const QString&, bool) {}
void ObjectList::switch_to_object_process() {}
int  ObjectList::load_mesh_part(const TriangleMesh&, const QString&, const TextInfo&, bool) { return -1; }
int  ObjectList::add_text_part(const TriangleMesh&, const QString&, const TextInfo&, const Transform3d&, bool) { return -1; }
bool ObjectList::del_object(const int, bool) { return false; }
void ObjectList::del_subobject_item(QModelIndex&) {}
void ObjectList::del_settings_from_config(const QModelIndex&) {}
void ObjectList::del_instances_from_object(const int) {}
void ObjectList::del_layer_from_object(const int, const t_layer_height_range&) {}
void ObjectList::del_layers_from_object(const int) {}
bool ObjectList::del_from_cut_object(bool, bool, bool) { return false; }
bool ObjectList::del_subobject_from_object(const int, const int, const int) { return false; }
void ObjectList::del_info_item(const int, InfoItemType) {}
void ObjectList::split() {}
void ObjectList::merge(bool) {}
void ObjectList::merge_volumes() {}
void ObjectList::layers_editing() {}
void ObjectList::boolean() {}

QModelIndex ObjectList::add_layer_root_item(const QModelIndex) { return {}; }
QModelIndex ObjectList::add_settings_item(QModelIndex, const DynamicPrintConfig*) { return {}; }

DynamicPrintConfig ObjectList::get_default_layer_config(const int) { return {}; }
bool ObjectList::get_volume_by_item(const QModelIndex&, ModelVolume*&) { return false; }
bool ObjectList::is_splittable(bool) { return false; }
bool ObjectList::selected_instances_of_same_object() { return false; }
bool ObjectList::can_split_instances() { return false; }
bool ObjectList::can_merge_to_multipart_object() const { return false; }
bool ObjectList::can_merge_to_single_object() const { return false; }
bool ObjectList::can_mesh_boolean() const { return false; }

bool ObjectList::has_selected_cut_object() const { return false; }
void ObjectList::invalidate_cut_info_for_selection() {}
void ObjectList::invalidate_cut_info_for_object(int) {}
void ObjectList::delete_all_connectors_for_selection() {}
void ObjectList::delete_all_connectors_for_object(int) {}

int  ObjectList::get_selected_obj_idx() const { return -1; }
ModelConfig& ObjectList::get_item_config(const QModelIndex&) const {
    static ModelConfig dummy; return dummy;
}

void ObjectList::changed_object(const int) const {}
void ObjectList::part_selection_changed() {}

void ObjectList::add_objects_to_list(std::vector<size_t>, bool, bool, bool) {}
void ObjectList::add_object_to_list(size_t, bool, bool, bool) {}
QModelIndexList ObjectList::add_volumes_to_object_in_list(size_t, std::function<bool(const ModelVolume*)>) { return {}; }
void ObjectList::delete_object_from_list() {}
void ObjectList::delete_object_from_list(const size_t) {}
void ObjectList::delete_volume_from_list(const size_t, const size_t) {}
void ObjectList::delete_instance_from_list(const size_t, const size_t) {}
void ObjectList::delete_from_model_and_list(const ItemType, const int, const int) {}
void ObjectList::delete_from_model_and_list(const std::vector<ItemForDelete>&) {}
void ObjectList::update_lock_icons_for_model() {}
void ObjectList::delete_all_objects_from_list() {}
void ObjectList::increase_object_instances(const size_t, const size_t) {}
void ObjectList::decrease_object_instances(const size_t, const size_t) {}

void ObjectList::unselect_objects() {}
void ObjectList::select_object_item(bool) {}

void ObjectList::remove() {}
void ObjectList::del_layer_range(const t_layer_height_range&) {}
void ObjectList::add_layer_range_after_current(const t_layer_height_range) {}
QString ObjectList::can_add_new_range_after_current(t_layer_height_range) { return {}; }
void ObjectList::add_layer_item(const t_layer_height_range&, const QModelIndex, const int) {}
bool ObjectList::edit_layer_range(const t_layer_height_range&, coordf_t) { return false; }
bool ObjectList::edit_layer_range(const t_layer_height_range&, const t_layer_height_range&, bool) { return false; }

void ObjectList::init() {}
bool ObjectList::multiple_selection() const { return false; }
bool ObjectList::is_selected(const ItemType) const { return false; }
bool ObjectList::is_connectors_item_selected() const { return false; }
bool ObjectList::is_connectors_item_selected(const QModelIndexList&) const { return false; }
int  ObjectList::get_selected_layers_range_idx() const { return m_selected_layers_range_idx; }
void ObjectList::update_selections() {}
void ObjectList::update_selections_on_canvas() {}
void ObjectList::select_item(const QModelIndex&) {}
void ObjectList::select_item(std::function<QModelIndex()>) {}
void ObjectList::select_items(const QModelIndexList&) {}
void ObjectList::select_item(const ObjectVolumeID&) {}
void ObjectList::select_items(const std::vector<ObjectVolumeID>&) {}
void ObjectList::select_all() {}
void ObjectList::expand_collapse_plate(int, bool) {}
void ObjectList::select_item_all_children() {}
void ObjectList::update_selection_mode() {}
bool ObjectList::check_last_selection(QString&) { return true; }
void ObjectList::fix_multiselection_conflicts() {}
void ObjectList::fix_cut_selection() {}
bool ObjectList::fix_cut_selection(QModelIndexList&) { return false; }

ModelVolume* ObjectList::get_selected_model_volume() { return nullptr; }
void ObjectList::change_part_type() {}
void ObjectList::set_volume_type(ModelVolumeType) {}
ModelVolumeType ObjectList::get_selected_volume_type() { return static_cast<ModelVolumeType>(0); }

void ObjectList::last_volume_is_deleted(const int) {}
void ObjectList::update_and_show_object_settings_item() {}
void ObjectList::update_settings_item_and_selection(QModelIndex, QModelIndexList&) {}
void ObjectList::update_object_list_by_printer_technology() {}
void ObjectList::update_info_items(size_t, QModelIndexList*, bool, bool) {}
void ObjectList::update_variable_layer_obj_num(ObjectDataViewModelNode*, size_t) {}

void ObjectList::instances_to_separated_object(const int, const std::set<int>&) {}
void ObjectList::instances_to_separated_objects(const int) {}
void ObjectList::split_instances() {}
void ObjectList::rename_item() {}
void ObjectList::fix_through_netfabb() {}
void ObjectList::simplify() {}
void ObjectList::smooth_mesh() {}
void ObjectList::update_item_error_icon(const int, int) const {}

void ObjectList::copy_layers_to_clipboard() {}
void ObjectList::paste_layers_into_list() {}
void ObjectList::copy_settings_to_clipboard() {}
void ObjectList::paste_settings_into_list() {}
bool ObjectList::can_paste_settings_into_list() { return false; }
void ObjectList::paste_volumes_into_list(int, const ModelVolumePtrs&) {}
void ObjectList::paste_objects_into_list(const std::vector<size_t>&) {}

void ObjectList::msw_rescale() {}
void ObjectList::sys_color_changed() {}

void ObjectList::update_after_undo_redo() {}
void ObjectList::update_printable_state(int, int) {}
void ObjectList::toggle_printable_state() {}
void ObjectList::enable_layers_editing() {}
void ObjectList::set_extruder_for_selected_items(const int) {}

void ObjectList::contextMenuEvent(QContextMenuEvent* /*event*/) {}
void ObjectList::dragEnterEvent(QDragEnterEvent* event) { event->ignore(); }
void ObjectList::dragMoveEvent(QDragMoveEvent* event)   { event->ignore(); }
void ObjectList::dropEvent(QDropEvent* event)           { event->ignore(); }
void ObjectList::keyPressEvent(QKeyEvent* event)        { QTreeView::keyPressEvent(event); }
void ObjectList::mousePressEvent(QMouseEvent* event)    { QTreeView::mousePressEvent(event); }

} // namespace GUI
} // namespace Slic3r

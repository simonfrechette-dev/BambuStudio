// GUI_ObjectLayers.cpp — stubbed for Qt6 port
#include "GUI_ObjectLayers.hpp"

namespace Slic3r { namespace GUI {

LayerRangeEditor::LayerRangeEditor(ObjectLayers* parent, const QString& value, EditorType type,
    std::function<void(EditorType)> set_focus_data_fn,
    std::function<bool(coordf_t, bool, bool)> edit_fn)
    : QLineEdit(nullptr), m_type(type), m_set_focus_data(set_focus_data_fn)
{
    (void)edit_fn; setText(value);
}

void LayerRangeEditor::msw_rescale() {}
coordf_t LayerRangeEditor::get_value() { return text().toDouble(); }

ObjectLayers::ObjectLayers(QWidget* parent)
    : OG_Settings(parent, false) {}

void ObjectLayers::select_editor(LayerRangeEditor* editor, const bool is_last_edited_range) { (void)editor; (void)is_last_edited_range; }
QLayout* ObjectLayers::create_layer(const t_layer_height_range& range, PlusMinusButton* del, PlusMinusButton* add) { (void)range; (void)del; (void)add; return nullptr; }
void ObjectLayers::create_layers_list() {}
void ObjectLayers::update_layers_list() {}
void ObjectLayers::update_scene_from_editor_selection() const {}
void ObjectLayers::UpdateAndShow(const bool show) { OG_Settings::UpdateAndShow(show); }
void ObjectLayers::msw_rescale() {}
void ObjectLayers::sys_color_changed() {}
void ObjectLayers::reset_selection() {}

}} // namespace Slic3r::GUI

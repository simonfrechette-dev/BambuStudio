// GUI_ObjectTable.cpp — stubbed for Qt6 port
#include "GUI_ObjectTable.hpp"
#include <QVBoxLayout>

namespace Slic3r { namespace GUI {

std::string ObjectGridTable::category_all;
std::string ObjectGridTable::plate_outside;

ObjectTablePanel::ObjectTablePanel(QWidget* parent, Plater* platerObj, Model* modelObj)
    : QWidget(parent), m_plater(platerObj), m_model(modelObj) {}
ObjectTablePanel::~ObjectTablePanel() {}
void ObjectTablePanel::load_data() {}
void ObjectTablePanel::SetSelection(int object_id, int volume_id) { (void)object_id; (void)volume_id; }
void ObjectTablePanel::sort_by_default() {}
QSize ObjectTablePanel::get_init_size() { return {512, 1024}; }
void ObjectTablePanel::resetAllValuesInSideWindow(int row, bool is_object, ModelObject* object, ModelConfig* config, const std::string& category) { (void)row; (void)is_object; (void)object; (void)config; (void)category; }
void ObjectTablePanel::msw_rescale() {}

ObjectTableDialog::ObjectTableDialog(QWidget* parent, Plater* platerObj, Model* modelObj, QSize maxSize)
    : DPIDialog(parent), m_plater(platerObj), m_model(modelObj)
{
    (void)maxSize;
    m_obj_panel = new ObjectTablePanel(this, platerObj, modelObj);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_obj_panel);
    setLayout(layout);
}
ObjectTableDialog::~ObjectTableDialog() {}
void ObjectTableDialog::Popup(int obj_idx, int vol_idx) { (void)obj_idx; (void)vol_idx; show(); }
void ObjectTableDialog::OnClose() { hide(); }
void ObjectTableDialog::OnSize() {}
void ObjectTableDialog::on_dpi_changed(const QRect& r) { (void)r; }
void ObjectTableDialog::on_sys_color_changed() {}

}} // namespace Slic3r::GUI

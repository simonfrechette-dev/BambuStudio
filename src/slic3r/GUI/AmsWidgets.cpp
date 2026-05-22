// Qt6 stub for AmsWidgets.cpp
#include "AmsWidgets.hpp"

namespace Slic3r { namespace GUI {

TrayListModel::TrayListModel() {}
QVariant TrayListModel::data(const QModelIndex&, int) const { return {}; }
bool TrayListModel::setData(const QModelIndex&, const QVariant&, int) { return false; }
int TrayListModel::columnCount(const QModelIndex&) const { return Col_Max; }
int TrayListModel::rowCount(const QModelIndex&) const { return 0; }
void TrayListModel::update(MachineObject*) {}
void TrayListModel::clear_data() {}

}} // namespace Slic3r::GUI

// Qt6 stub for UnsavedChangesDialog.cpp
#include "UnsavedChangesDialog.hpp"
#include "MainFrame.hpp"

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// DiffModel stubs
// ---------------------------------------------------------------------------

DiffModel::DiffModel(QWidget* parent)
    : QAbstractItemModel(parent)
{}

QModelIndex DiffModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return {};
    return createIndex(row, column);
}

QModelIndex DiffModel::parent(const QModelIndex& /*child*/) const
{
    return {};
}

int DiffModel::rowCount(const QModelIndex& /*parent*/) const { return 0; }

QVariant DiffModel::data(const QModelIndex& /*index*/, int /*role*/) const { return {}; }

bool DiffModel::setData(const QModelIndex& /*index*/, const QVariant& /*value*/, int /*role*/) { return false; }

Qt::ItemFlags DiffModel::flags(const QModelIndex& /*index*/) const { return Qt::NoItemFlags; }

// ---------------------------------------------------------------------------
// DiffViewCtrl stubs
// ---------------------------------------------------------------------------

DiffViewCtrl::DiffViewCtrl(QWidget* parent, QSize /*size*/)
    : QTreeView(parent)
{}

DiffViewCtrl::~DiffViewCtrl() {}

DiffPresetDialog::DiffPresetDialog(MainFrame* mainframe)
    : DPIDialog(mainframe)
{}

}} // namespace Slic3r::GUI


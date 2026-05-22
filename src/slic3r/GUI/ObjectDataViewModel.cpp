// Qt6 stub for ObjectDataViewModel.cpp
#include "ObjectDataViewModel.hpp"

namespace Slic3r { namespace GUI {

ObjectDataViewModel::ObjectDataViewModel()
    : QAbstractItemModel()
{}

ObjectDataViewModel::~ObjectDataViewModel() {}

QModelIndex ObjectDataViewModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return {};
    return createIndex(row, column);
}

QModelIndex ObjectDataViewModel::parent(const QModelIndex& /*child*/) const
{
    return {};
}

int ObjectDataViewModel::rowCount(const QModelIndex& /*parent*/) const { return 0; }

QVariant ObjectDataViewModel::data(const QModelIndex& /*index*/, int /*role*/) const { return {}; }

bool ObjectDataViewModel::setData(const QModelIndex& /*index*/, const QVariant& /*value*/, int /*role*/) { return false; }

Qt::ItemFlags ObjectDataViewModel::flags(const QModelIndex& /*index*/) const { return Qt::NoItemFlags; }

bool ObjectDataViewModel::hasChildren(const QModelIndex& /*parent*/) const { return false; }

}} // namespace Slic3r::GUI


// All implementations are stubs pending Qt port.

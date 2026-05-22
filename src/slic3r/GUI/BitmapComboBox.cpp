// BitmapComboBox.cpp — stubbed for Qt6 port
#include "BitmapComboBox.hpp"

namespace Slic3r { namespace GUI {

BitmapComboBox::BitmapComboBox(QWidget* parent)
    : QComboBox(parent)
{}

int BitmapComboBox::Append(const QString& item)
{
    addItem(item);
    return count() - 1;
}

int BitmapComboBox::Append(const QString& item, const QPixmap& bitmap)
{
    addItem(QIcon(bitmap), item);
    return count() - 1;
}

BitmapComboBox::~BitmapComboBox() {}

}} // namespace Slic3r::GUI

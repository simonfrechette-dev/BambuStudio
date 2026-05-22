// Qt6 stub for CameraPopup.cpp
#include "CameraPopup.hpp"

namespace Slic3r { namespace GUI {

CameraItem::CameraItem(QWidget* parent, const QString& /*name*/, const QString& /*img*/)
    : QWidget(parent)
{}

CameraPopup::CameraPopup(QWidget* parent)
    : QWidget(parent)
{}

bool CameraPopup::ProcessLeftDown(QMouseEvent* /*evt*/) { return false; }

}} // namespace Slic3r::GUI


// Qt6 stub for CapsuleButton.cpp
#include "CapsuleButton.hpp"
#include <QPainter>

namespace Slic3r { namespace GUI {

void CapsuleButton::paintEvent(QPaintEvent* /*event*/) {}
void CapsuleButton::enterEvent(QEnterEvent* /*event*/) {}
void CapsuleButton::leaveEvent(QEvent* /*event*/) {}
void CapsuleButton::mousePressEvent(QMouseEvent* event) { QWidget::mousePressEvent(event); }

}} // namespace Slic3r::GUI


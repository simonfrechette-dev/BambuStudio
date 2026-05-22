#include "AxisCtrlButton.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>

AxisCtrlButton::AxisCtrlButton(QWidget *parent, ScalableBitmap &icon, long /*style*/)
    : QWidget(parent)
    , state_handler(this)
    , m_icon(icon)
{
    setMinimumSize(120, 120);
    updateParams();
}

void AxisCtrlButton::SetMinSize(const QSize &sz) { minSize = sz; setMinimumSize(sz); updateParams(); }
void AxisCtrlButton::SetTextColor(const StateColor &c)        { text_color = c; }
void AxisCtrlButton::SetBorderColor(const StateColor &c)      { border_color = c; }
void AxisCtrlButton::SetBackgroundColor(const StateColor &c)  { background_color = c; }
void AxisCtrlButton::SetInnerBackgroundColor(const StateColor &c) { inner_background_color = c; }
void AxisCtrlButton::SetBitmap(ScalableBitmap &bmp)           { m_icon = bmp; update(); }
void AxisCtrlButton::Rescale()                                { updateParams(); update(); }

void AxisCtrlButton::updateParams()
{
    const QSize sz = size().isEmpty() ? QSize(120, 120) : size();
    center   = QPoint(sz.width() / 2, sz.height() / 2);
    r_outer  = std::min(sz.width(), sz.height()) / 2.0 - 2;
    r_inner  = r_outer * 0.5;
    r_home   = r_inner * 0.45;
    r_blank  = r_outer * 0.15;
    gap      = 2.0;
}

AxisCtrlButton::CurrentPos AxisCtrlButton::posFromPoint(const QPoint &pt) const
{
    const double dx = pt.x() - center.x();
    const double dy = pt.y() - center.y();
    const double dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= r_home) return INNER_HOME;
    if (dist <= r_inner) {
        const double angle = std::atan2(dy, dx) * 180.0 / PI;
        if (angle > -45 && angle <= 45)   return INNER_RIGHT;
        if (angle > 45  && angle <= 135)  return INNER_DOWN;
        if (angle > 135 || angle <= -135) return INNER_LEFT;
        return INNER_UP;
    }
    if (dist <= r_outer) {
        const double angle = std::atan2(dy, dx) * 180.0 / PI;
        if (angle > -45 && angle <= 45)   return OUTER_RIGHT;
        if (angle > 45  && angle <= 135)  return OUTER_DOWN;
        if (angle > 135 || angle <= -135) return OUTER_LEFT;
        return OUTER_UP;
    }
    return UNDEFINED;
}

void AxisCtrlButton::paintEvent(QPaintEvent *)
{
    updateParams();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int states = state_handler.states();

    // Outer ring
    const QColor bgOuter = background_color.count() > 0
        ? background_color.colorForStates(states)
        : QColor(220, 220, 220);
    p.setBrush(bgOuter);
    p.setPen(border_color.count() > 0 ? QPen(border_color.colorForStates(states), 1) : QPen(Qt::NoPen));
    p.drawEllipse(center, (int)r_outer, (int)r_outer);

    // Blank gap ring (erase inner area)
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, (int)(r_inner + gap), (int)(r_inner + gap));

    // Inner ring
    const QColor bgInner = inner_background_color.count() > 0
        ? inner_background_color.colorForStates(states)
        : QColor(200, 200, 200);
    p.setBrush(bgInner);
    p.drawEllipse(center, (int)r_inner, (int)r_inner);

    // Hover highlight
    if (current_pos != UNDEFINED && current_pos != INNER_HOME) {
        p.setBrush(QColor(0, 174, 66, 60));
        p.setPen(Qt::NoPen);
        if (current_pos < INNER_UP) {
            p.drawEllipse(center, (int)r_outer, (int)r_outer);
        } else {
            p.drawEllipse(center, (int)r_inner, (int)r_inner);
        }
    }

    // Home circle
    p.setBrush(current_pos == INNER_HOME ? QColor(0, 174, 66) : QColor(180, 180, 180));
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, (int)r_home, (int)r_home);

    // Icon in home zone
    if (m_icon.bmp().IsOk()) {
        const QSize isz = m_icon.GetBmpSize();
        p.drawPixmap(QPoint(center.x() - isz.width() / 2,
                            center.y() - isz.height() / 2),
                     m_icon.bmp());
    }

    // Arrows for outer sectors
    static const QString arrows[] = { "↑", "←", "↓", "→" };
    static const QPoint offsets[] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
    p.setFont(QFont{});
    p.setPen(Qt::black);
    const double arrowR = (r_inner + gap + r_outer) / 2;
    for (int i = 0; i < 4; ++i) {
        const double angle = (i * 90 - 90) * PI / 180.0;
        const int ax = center.x() + (int)(arrowR * std::cos(angle));
        const int ay = center.y() + (int)(arrowR * std::sin(angle));
        p.drawText(QRect(ax - 8, ay - 8, 16, 16), Qt::AlignCenter, arrows[i]);
    }
}

void AxisCtrlButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pressedDown = true;
        last_pos = posFromPoint(event->pos());
        current_pos = last_pos;
        update();
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void AxisCtrlButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && pressedDown) {
        pressedDown = false;
        const auto pos = posFromPoint(event->pos());
        current_pos = UNDEFINED;
        update();
        if (pos != UNDEFINED && rect().contains(event->pos()))
            emit buttonClicked((int)pos);
        event->accept();
    } else {
        pressedDown = false;
        QWidget::mouseReleaseEvent(event);
    }
}

void AxisCtrlButton::mouseMoveEvent(QMouseEvent *event)
{
    const auto pos = posFromPoint(event->pos());
    if (pos != current_pos) { current_pos = pos; update(); }
    QWidget::mouseMoveEvent(event);
}

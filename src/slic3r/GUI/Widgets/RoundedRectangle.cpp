#include "RoundedRectangle.hpp"
#include <QPainter>

RoundedRectangle::RoundedRectangle(QWidget *parent, QColor col,
                                   const QPoint &pos, const QSize &size,
                                   double radius, int type)
    : QWidget(parent)
    , m_radius(radius)
    , m_type(type)
    , m_color(col)
{
    move(pos);
    setFixedSize(size);
}

void RoundedRectangle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_color);
    p.setPen(Qt::NoPen);
    if (m_radius > 0)
        p.drawRoundedRect(rect(), m_radius, m_radius);
    else
        p.drawRect(rect());
}

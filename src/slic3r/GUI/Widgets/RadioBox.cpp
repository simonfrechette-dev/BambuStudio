#include "RadioBox.hpp"
#include <QPainter>
#include <QMouseEvent>

namespace Slic3r {
namespace GUI {

RadioBox::RadioBox(QWidget *parent)
    : QAbstractButton(parent)
    , m_on(this,  "radio_on",  18)
    , m_off(this, "radio_off", 18)
    , m_ban(this, "radio_ban", 18)
{
    setCheckable(true);
    const QSize sz = m_on.GetBmpSize();
    setFixedSize(sz);
}

void RadioBox::SetValue(bool value)
{
    if (m_checked != value) {
        m_checked = value;
        update();
        emit toggled(m_checked);
    }
}

void RadioBox::Rescale()
{
    setFixedSize(m_on.GetBmpSize());
    update();
}

QSize RadioBox::sizeHint() const { return m_on.GetBmpSize(); }

const QPixmap &RadioBox::currentPixmap() const
{
    if (!isEnabled()) return m_ban.bmp();
    return m_checked ? m_on.bmp() : m_off.bmp();
}

void RadioBox::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(QPoint(0, 0), currentPixmap());
}

void RadioBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) event->accept();
    else QAbstractButton::mousePressEvent(event);
}

void RadioBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        SetValue(!m_checked);
        event->accept();
    } else {
        QAbstractButton::mouseReleaseEvent(event);
    }
}

void RadioBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) update();
    QAbstractButton::changeEvent(event);
}

}} // namespace Slic3r::GUI

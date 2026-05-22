#include "CheckBox.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QEnterEvent>

CheckBox::CheckBox(QWidget *parent, int /*id*/)
    : QAbstractButton(parent)
    , m_on(this,            "check_on",           18)
    , m_half(this,          "check_half",         18)
    , m_off(this,           "check_off",          18)
    , m_on_disabled(this,   "check_on_disabled",  18)
    , m_half_disabled(this, "check_half_disabled",18)
    , m_off_disabled(this,  "check_off_disabled", 18)
    , m_on_focused(this,    "check_on_focused",   18)
    , m_half_focused(this,  "check_half_focused", 18)
    , m_off_focused(this,   "check_off_focused",  18)
{
    if (parent)
        setAutoFillBackground(false);
    setCheckable(true);
    const QSize sz = m_on.GetBmpSize();
    setFixedSize(sz);
    connect(this, &QAbstractButton::clicked, this, [this]() {
        m_half_checked = false;
        m_checked      = !m_checked;
        update();
        emit toggled(m_checked);
    });
}

void CheckBox::setChecked(bool value)
{
    if (m_checked != value) {
        m_checked = value;
        update();
    }
}

void CheckBox::SetHalfChecked(bool value)
{
    m_half_checked = value;
    update();
}

void CheckBox::Rescale()
{
    // msw_rescale is no-op on Qt; just resize to the new logical size.
    const QSize sz = m_on.GetBmpSize();
    setFixedSize(sz);
    update();
}

QSize CheckBox::sizeHint() const
{
    return m_on.GetBmpSize();
}

const QPixmap &CheckBox::currentPixmap() const
{
    const bool disabled = !isEnabled();
    const bool focused  = m_focused || m_hovered;

    if (m_half_checked) {
        if (disabled) return m_half_disabled.bmp();
        if (focused)  return m_half_focused.bmp();
        return m_half.bmp();
    }
    if (m_checked) {
        if (disabled) return m_on_disabled.bmp();
        if (focused)  return m_on_focused.bmp();
        return m_on.bmp();
    }
    if (disabled) return m_off_disabled.bmp();
    if (focused)  return m_off_focused.bmp();
    return m_off.bmp();
}

void CheckBox::update_bitmap() { update(); }

void CheckBox::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.drawPixmap(QPoint(0, 0), currentPixmap());
}

void CheckBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange)
        update();
    QAbstractButton::changeEvent(event);
}

void CheckBox::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    update();
    QAbstractButton::enterEvent(event);
}

void CheckBox::leaveEvent(QEvent *event)
{
    m_hovered = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void CheckBox::focusInEvent(QFocusEvent *event)
{
    m_focused = true;
    update();
    QAbstractButton::focusInEvent(event);
}

void CheckBox::focusOutEvent(QFocusEvent *event)
{
    m_focused = false;
    update();
    QAbstractButton::focusOutEvent(event);
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        event->accept();
    else
        QAbstractButton::mousePressEvent(event);
}

void CheckBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        m_half_checked = false;
        m_checked      = !m_checked;
        update();
        emit toggled(m_checked);
        event->accept();
    } else {
        QAbstractButton::mouseReleaseEvent(event);
    }
}

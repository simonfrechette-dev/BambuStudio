#include "SideButton.hpp"
#include "Label.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QMouseEvent>

SideButton::SideButton(QWidget *parent, const QString &text,
                        const QString &iconName, long /*style*/, int iconSize)
    : QWidget(parent)
    , state_handler(this)
    , m_text(text)
    , text_orientation(HO_Center)
    , text_margin(15)
{
    if (!iconName.isEmpty())
        icon = ScalableBitmap(this, iconName.toStdString(), iconSize > 0 ? iconSize : 20);
    setFont(Label::Body_14);
    setAutoFillBackground(false);
    measureSize();
}

void SideButton::SetCornerRadius(double r) { radius = r; update(); }
void SideButton::SetCornerEnable(const std::vector<bool> &e) { radius_enable = e; }
void SideButton::SetTextLayout(EHorizontalOrientation orient, int margin)
{
    text_orientation = orient;
    text_margin      = margin;
    update();
}
void SideButton::SetLayoutStyle(int s) { layout_style = s; }
void SideButton::setText(const QString &label) { m_text = label; measureSize(); update(); }
void SideButton::setForegroundColour(QColor c) { text_color.append(c, StateColor::Normal); update(); }
void SideButton::setBackgroundColour(QColor c) { background_color.append(c, StateColor::Normal); update(); }
bool SideButton::SetBottomColour(QColor c) { bottom_color = c; update(); return true; }
void SideButton::SetMinSize(const QSize &size) { minSize = size; setMinimumSize(size); }
void SideButton::SetBorderColor(const StateColor &c) { border_color = c; update(); }
void SideButton::SetForegroundColor(const StateColor &c) { text_color = c; update(); }
void SideButton::SetBackgroundColor(const StateColor &c) { background_color = c; update(); }
bool SideButton::Enable(bool e) { setEnabled(e); update(); return true; }
void SideButton::Rescale() { measureSize(); update(); }
void SideButton::SetExtraSize(const QSize &s) { extra_size = s; measureSize(); }
void SideButton::SetIconOffset(int o) { icon_offset = o; update(); }

QSize SideButton::sizeHint() const { return minimumSizeHint(); }

void SideButton::measureSize()
{
    QFontMetrics fm(font());
    textSize = fm.boundingRect(m_text).size();
    QSize sz = textSize + QSize(text_margin * 2, 8) + extra_size;
    if (icon.bmp().IsOk())
        sz.setWidth(sz.width() + icon.GetBmpSize().width() + 4);
    minSize = sz;
    setMinimumSize(sz);
}

void SideButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int states = state_handler.states();
    const QRect rc = rect();

    // Background
    if (background_color.count() > 0) {
        const QColor bg = background_color.colorForStates(states);
        p.setBrush(bg);
        p.setPen(Qt::NoPen);
        if (radius > 0)
            p.drawRoundedRect(rc, radius, radius);
        else
            p.drawRect(rc);
    }

    // Bottom colour accent
    if (bottom_color.isValid()) {
        p.setBrush(bottom_color);
        p.setPen(Qt::NoPen);
        p.drawRect(QRect(0, rc.height() - 3, rc.width(), 3));
    }

    // Border
    if (border_color.count() > 0) {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(border_color.colorForStates(states), 1));
        if (radius > 0)
            p.drawRoundedRect(rc, radius, radius);
        else
            p.drawRect(rc);
    }

    // Icon
    int x = text_margin;
    if (icon.bmp().IsOk()) {
        const QSize isz = icon.GetBmpSize();
        int iy = (rc.height() - isz.height()) / 2;
        p.drawPixmap(QPoint(x + icon_offset, iy), icon.bmp());
        x += isz.width() + 4;
    }

    // Text
    QColor tc = text_color.count() > 0 ? text_color.colorForStates(states) : Qt::black;
    p.setPen(tc);
    p.setFont(font());

    int tx;
    switch (text_orientation) {
    case HO_Left:   tx = x; break;
    case HO_Right:  tx = rc.width() - textSize.width() - text_margin; break;
    default:        tx = (rc.width() - textSize.width()) / 2; break;
    }
    p.drawText(QPoint(tx, (rc.height() + textSize.height()) / 2 - QFontMetrics(font()).descent()), m_text);
}

void SideButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pressedDown = true;
        update();
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void SideButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && pressedDown) {
        pressedDown = false;
        update();
        if (rect().contains(event->pos()))
            emit clicked();
        event->accept();
    } else {
        pressedDown = false;
        QWidget::mouseReleaseEvent(event);
    }
}

void SideButton::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) update();
    QWidget::changeEvent(event);
}

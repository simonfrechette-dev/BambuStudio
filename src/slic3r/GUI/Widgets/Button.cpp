#include "Button.hpp"
#include "Label.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QToolTip>
#include <algorithm>

// ---------------------------------------------------------------------------
Button::Button()
    : paddingSize(10, 8)
{
    background_color = StateColor(
        std::make_pair(QColor(0xF0F0F1), (int)StateColor::Disabled),
        std::make_pair(QColor(0x37EE7C), (int)(StateColor::Hovered | StateColor::Checked)),
        std::make_pair(QColor(0x00AE42), (int)StateColor::Checked),
        std::make_pair(QColor(Qt::lightGray), (int)StateColor::Hovered),
        std::make_pair(QColor(Qt::white),     (int)StateColor::Normal));
    text_color = StateColor(
        std::make_pair(QColor(Qt::lightGray), (int)StateColor::Disabled),
        std::make_pair(QColor(Qt::black),     (int)StateColor::Normal));
}

Button::Button(QWidget *parent, const QString &text, const QString &icon,
               int iconSize, Qt::WindowFlags flags)
    : Button()
{
    init(parent, text, icon, iconSize, flags);
}

bool Button::init(QWidget *parent, const QString &text, const QString &icon,
                  int iconSize, Qt::WindowFlags flags)
{
    StaticBox::init(parent, flags);
    state_handler.attach({&text_color});
    state_handler.update_binds();
    QWidget::setFont(Label::Body_14);
    m_text = text;
    if (!icon.isEmpty())
        active_icon = ScalableBitmap(this, icon.toStdString(), iconSize > 0 ? iconSize : 20);
    measureSize();
    if (canFocus)
        setFocusPolicy(Qt::StrongFocus);
    else
        setFocusPolicy(Qt::NoFocus);
    return true;
}

// ---------------------------------------------------------------------------
void Button::setText(const QString &label)
{
    if (label == m_text) return;
    m_text = label;
    measureSize();
    update();
}

bool Button::setFont(const QFont &font)
{
    QWidget::setFont(font);
    measureSize();
    update();
    return true;
}

void Button::SetIcon(const QString &icon)
{
    auto tmp = ScalableBitmap(this, icon.toStdString(), active_icon.px_cnt());
    if (!icon.isEmpty()) {
        if (!tmp.bmp().IsSameAs(active_icon.bmp())) {
            active_icon = tmp;
            update();
        }
    } else {
        active_icon = ScalableBitmap();
        update();
    }
}

void Button::SetInactiveIcon(const QString &icon)
{
    if (!icon.isEmpty())
        inactive_icon = ScalableBitmap(this, icon.toStdString(), active_icon.px_cnt());
    else
        inactive_icon = ScalableBitmap();
    update();
}

void Button::setMinimumSize(const QSize &sz)
{
    minSize = sz;
    measureSize();
}

void Button::setMaximumSize(const QSize &sz)
{
    QWidget::setMaximumSize(sz);
    measureSize();
}

void Button::SetPaddingSize(const QSize &sz)
{
    paddingSize = sz;
    measureSize();
}

void Button::SetTextColor(StateColor const &color)
{
    text_color = color;
    state_handler.update_binds();
    update();
}

void Button::SetTextColorNormal(QColor const &color)
{
    text_color.setColorForStates(color, 0);
    update();
}

void Button::setEnabled(bool enable)
{
    QWidget::setEnabled(enable);
    update();
}

void Button::SetCanFocus(bool cf)
{
    canFocus = cf;
    setFocusPolicy(cf ? Qt::StrongFocus : Qt::NoFocus);
}

void Button::SetValue(bool state)
{
    if (GetValue() == state) return;
    state_handler.set_state(state ? StateHandler::Checked : 0, StateHandler::Checked);
}

bool Button::GetValue() const { return state_handler.states() & StateHandler::Checked; }

void Button::SetCenter(bool c) { isCenter = c; update(); }

void Button::SetVertical(bool v)
{
    vertical = v;
    measureSize();
}

void Button::Rescale()
{
    // msw_rescale is a no-op on Qt (DPR handled automatically), just remeasure.
    measureSize();
    update();
}

// ---------------------------------------------------------------------------
// Size

QSize Button::sizeHint() const { return QWidget::minimumSizeHint(); }

void Button::measureSize()
{
    QFontMetrics fm(font());
    textRect = fm.boundingRect(m_text);

    QSize szContent = textRect.size();
    if (active_icon.bmp().IsOk()) {
        QSize szIcon = active_icon.GetBmpSize();
        if (szContent.height() > 0) {
            if (vertical) szContent.setHeight(szContent.height() + 5);
            else          szContent.setWidth(szContent.width() + 5);
        }
        if (vertical) {
            szContent.setHeight(szContent.height() + szIcon.height());
            if (szIcon.width() > szContent.width()) szContent.setWidth(szIcon.width());
        } else {
            szContent.setWidth(szContent.width() + szIcon.width());
            if (szIcon.height() > szContent.height()) szContent.setHeight(szIcon.height());
        }
    }

    QSize sz = szContent + QSize(paddingSize.width() * 2, paddingSize.height() * 2);
    if (minSize.height() > 0)
        sz.setHeight(minSize.height());

    const int maxW = maximumWidth();
    if (maxW > 0 && sz.width() > maxW) {
        sz.setWidth(maxW);
        if (toolTip().isEmpty())
            setToolTip(m_text);
    }

    if (minSize.width() > sz.width())
        QWidget::setMinimumSize(minSize);
    else
        QWidget::setMinimumSize(sz);
}

// ---------------------------------------------------------------------------
// Painting

void Button::doRender(QPainter &painter)
{
    // Draw base (border + background) via StaticBox.
    StaticBox::doRender(painter);

    if (m_left_corner_white || m_right_corner_white)
        renderWhiteCorners(painter);

    const int    states  = state_handler.states();
    const QSize  sz      = size();
    const int    spacing = 5;

    ScalableBitmap icon = (m_selected || (states & (int)StateColor::Hovered))
                              ? active_icon : inactive_icon;

    QSize padding  = paddingSize;
    QSize textSz   = textRect.size();

    // --- text wrapping for vertical layout ---
    QString drawText = m_text;
    if (vertical && textSz.width() + padding.width() * 2 > sz.width()) {
        // Simple elide; full wrapping helper is in Label and will be wired later.
        QFontMetrics fm(font());
        drawText = fm.elidedText(m_text, Qt::ElideRight, sz.width() - padding.width() * 2);
        textSz   = fm.boundingRect(drawText).size();
    }

    QSize szContent = textSz;
    if (icon.bmp().IsOk()) {
        QSize szIcon = icon.GetBmpSize();
        if (szContent.height() > 0) {
            if (vertical) szContent.setHeight(szContent.height() + spacing);
            else          szContent.setWidth(szContent.width() + spacing);
        }
        if (vertical) {
            szContent.setHeight(szContent.height() + szIcon.height());
            if (szIcon.width() > szContent.width()) szContent.setWidth(szIcon.width());
        } else {
            szContent.setWidth(szContent.width() + szIcon.width());
            if (szIcon.height() > szContent.height()) szContent.setHeight(szIcon.height());
        }
        if (szContent.width() > sz.width()) {
            int d = std::min(padding.width(), (szContent.width() - sz.width()) / 2);
            padding.setWidth(padding.width() - d);
            szContent.setWidth(szContent.width() - d);
        }
    }

    // Content rect (centered or top-left)
    QRect rcContent(QPoint(0, 0), sz);
    if (isCenter) {
        QSize offset = (sz - szContent) / 2;
        if (offset.width()  < 0) offset.setWidth(0);
        if (offset.height() < 0) offset.setHeight(0);
        rcContent.adjust(offset.width(), offset.height(),
                         -offset.width(), -offset.height());
    }

    QPoint pt = rcContent.topLeft();

    if (icon.bmp().IsOk()) {
        QSize szIcon = icon.GetBmpSize();
        QPoint iconPt = pt;
        if (vertical)
            iconPt.setX(pt.x() + (rcContent.width() - szIcon.width()) / 2);
        else
            iconPt.setY(pt.y() + (rcContent.height() - szIcon.height()) / 2);
        painter.drawPixmap(iconPt, icon.bmp());
        if (vertical) {
            pt.setY(pt.y() + szIcon.height() + spacing);
            pt.setX(rcContent.x());
        } else {
            pt.setX(pt.x() + szIcon.width() + spacing);
            pt.setY(rcContent.y());
        }
    }

    if (!drawText.isEmpty()) {
        QFontMetrics fm(font());
        if (!vertical) {
            if (pt.x() + textSz.width() > sz.width())
                drawText = fm.elidedText(drawText, Qt::ElideRight, sz.width() - pt.x());
            pt.setY(pt.y() + (rcContent.height() - textSz.height()) / 2);
        } else {
            pt.setX(pt.x() + (rcContent.width() - textSz.width()) / 2);
        }
        painter.setPen(text_color.colorForStates(states));
        painter.drawText(pt, drawText);
    }
}

void Button::renderWhiteCorners(QPainter &painter)
{
    const int   r            = static_cast<int>(radius);
    const QSize sz           = size();
    const int   states       = state_handler.states();
    QColor      parentBg     = StaticBox::GetParentBackgroundColor(parentWidget());
    QColor      bg           = background_color.colorForStates(states);

    painter.setRenderHint(QPainter::Antialiasing);

    auto drawCorners = [&]() {
        painter.setPen(Qt::NoPen);
        painter.setBrush(parentBg);
        if (m_left_corner_white) {
            painter.drawRect(0, 0, r, r);
            painter.drawRect(0, sz.height() - r, r, r);
            painter.setBrush(bg);
            painter.drawRoundedRect(0, 0, r * 2, r * 2, r, r);
            painter.drawRoundedRect(0, sz.height() - r * 2, r * 2, r * 2, r, r);
            painter.setBrush(parentBg);
        }
        if (m_right_corner_white) {
            painter.drawRect(sz.width() - r, 0, r, r);
            painter.drawRect(sz.width() - r, sz.height() - r, r, r);
            painter.setBrush(bg);
            painter.drawRoundedRect(sz.width() - r * 2, 0, r * 2, r * 2, r, r);
            painter.drawRoundedRect(sz.width() - r * 2, sz.height() - r * 2, r * 2, r * 2, r, r);
        }
    };
    drawCorners();
}

// ---------------------------------------------------------------------------
// Events

void Button::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }
    pressedDown = true;
    if (canFocus) setFocus();
    event->accept();
    update();
}

void Button::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) { event->ignore(); return; }
    if (pressedDown) {
        pressedDown = false;
        if (rect().contains(event->pos()))
            emit clicked();
    }
    event->accept();
    update();
}

void Button::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return
        || event->key() == Qt::Key_Enter) {
        pressedDown = true;
        event->accept();
        update();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Button::keyReleaseEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return
         || event->key() == Qt::Key_Enter) && pressedDown) {
        pressedDown = false;
        emit clicked();
        event->accept();
        update();
        return;
    }
    QWidget::keyReleaseEvent(event);
}

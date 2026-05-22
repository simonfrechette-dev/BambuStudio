#include "ImageSwitchButton.hpp"
#include "Label.hpp"

#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QEnterEvent>

// ============================================================
// ImageSwitchButton
// ============================================================

ImageSwitchButton::ImageSwitchButton(QWidget *parent,
                                     ScalableBitmap &img_on,
                                     ScalableBitmap &img_off,
                                     long /*style*/)
    : StaticBox(parent)
    , m_on(img_on)
    , m_off(img_off)
    , m_on_off(false)
{
    setAutoFillBackground(false);
    measureSize();
}

void ImageSwitchButton::SetLabels(const QString &lbl_on, const QString &lbl_off)
{
    labels[0] = lbl_on;
    labels[1] = lbl_off;
    measureSize();
    update();
}

void ImageSwitchButton::SetImages(ScalableBitmap &img_on, ScalableBitmap &img_off)
{
    m_on  = img_on;
    m_off = img_off;
    measureSize();
    update();
}

void ImageSwitchButton::SetTextColor(const StateColor &color)
{
    text_color = color;
    update();
}

void ImageSwitchButton::SetValue(bool value)
{
    m_on_off = value;
    update();
}

void ImageSwitchButton::SetPadding(int p) { m_padding = p; }
void ImageSwitchButton::Rescale() { measureSize(); update(); }

void ImageSwitchButton::measureSize()
{
    const ScalableBitmap &cur = m_on_off ? m_on : m_off;
    QSize sz = cur.bmp().IsOk() ? cur.GetBmpSize() : QSize(16, 16);

    if (!labels[0].isEmpty()) {
        QFontMetrics fm(Label::Body_14);
        const QSize ts = fm.boundingRect(labels[m_on_off ? 0 : 1]).size();
        textSize = ts;
        sz.setWidth(sz.width() + m_padding + ts.width());
        if (ts.height() > sz.height()) sz.setHeight(ts.height());
    }
    minSize = sz;
    setMinimumSize(sz + QSize(8, 4));
}

void ImageSwitchButton::doRender(QPainter &painter)
{
    StaticBox::doRender(painter);

    const ScalableBitmap &bmp = m_on_off ? m_on : m_off;
    const QRect rc = rect();
    int x = (rc.width() - minSize.width()) / 2;
    const int y0 = (rc.height() - minSize.height()) / 2;

    if (bmp.bmp().IsOk()) {
        painter.drawPixmap(QPoint(x, y0 + (minSize.height() - bmp.GetBmpSize().height()) / 2), bmp.bmp());
        x += bmp.GetBmpSize().width() + m_padding;
    }
    if (!labels[0].isEmpty()) {
        painter.setFont(Label::Body_14);
        const int states = state_handler.states() | (hover ? StateColor::Hovered : 0);
        painter.setPen(text_color.colorForStates(states));
        painter.drawText(QPoint(x, y0 + (minSize.height() + textSize.height()) / 2 - QFontMetrics(Label::Body_14).descent()),
                         labels[m_on_off ? 0 : 1]);
    }
}

void ImageSwitchButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pressedDown = true;
        event->accept();
    } else {
        StaticBox::mousePressEvent(event);
    }
}

void ImageSwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && pressedDown &&
        rect().contains(event->pos())) {
        pressedDown = false;
        SetValue(!m_on_off);
        emit toggled(m_on_off);
        event->accept();
    } else {
        pressedDown = false;
        StaticBox::mouseReleaseEvent(event);
    }
}

void ImageSwitchButton::enterEvent(QEnterEvent *event)
{
    hover = true;
    update();
    StaticBox::enterEvent(event);
}

void ImageSwitchButton::leaveEvent(QEvent *event)
{
    hover = false;
    update();
    StaticBox::leaveEvent(event);
}

// ============================================================
// FanSwitchButton
// ============================================================

FanSwitchButton::FanSwitchButton(QWidget *parent,
                                  ScalableBitmap &img_on,
                                  ScalableBitmap &img_off,
                                  long /*style*/)
    : StaticBox(parent)
    , m_on(img_on)
    , m_off(img_off)
{
    measureSize();
}

void FanSwitchButton::SetLabels(const QString &lbl_on, const QString &lbl_off)
{
    labels[0] = lbl_on; labels[1] = lbl_off;
    measureSize(); update();
}
void FanSwitchButton::SetImages(ScalableBitmap &on, ScalableBitmap &off)
{
    m_on = on; m_off = off; measureSize(); update();
}
void FanSwitchButton::SetTextColor(const StateColor &c) { text_color = c; update(); }
void FanSwitchButton::SetValue(bool v) { m_on_off = v; update(); emit toggled(v); }
void FanSwitchButton::SetPadding(int p) { m_padding = p; }
void FanSwitchButton::Rescale() { measureSize(); update(); }
void FanSwitchButton::setFanValue(int val) { m_speed = val; update(); }
void FanSwitchButton::UseTextFan() { setText(QStringLiteral("fan")); }
void FanSwitchButton::UseTextAirCondition() { setText(QStringLiteral("a/c")); }
void FanSwitchButton::setText(const QString &text) { m_text = text; update(); }

void FanSwitchButton::measureSize()
{
    const QSize sz = m_on.bmp().IsOk() ? m_on.GetBmpSize() : QSize(16, 16);
    minSize = sz + QSize(8, 4);
    setMinimumSize(minSize);
}

void FanSwitchButton::doRender(QPainter &painter)
{
    StaticBox::doRender(painter);
    const ScalableBitmap &bmp = m_on_off ? m_on : m_off;
    if (bmp.bmp().IsOk()) {
        const QRect rc = rect();
        int x = (rc.width()  - bmp.GetBmpSize().width())  / 2;
        int y = (rc.height() - bmp.GetBmpSize().height()) / 2;
        painter.drawPixmap(QPoint(x, y), bmp.bmp());
    }
}

void FanSwitchButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { pressedDown = true; event->accept(); }
    else StaticBox::mousePressEvent(event);
}

void FanSwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && pressedDown &&
        rect().contains(event->pos())) {
        pressedDown = false;
        SetValue(!m_on_off);
        event->accept();
    } else {
        pressedDown = false;
        StaticBox::mouseReleaseEvent(event);
    }
}

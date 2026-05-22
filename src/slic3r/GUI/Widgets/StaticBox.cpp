#include "StaticBox.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

StaticBox::StaticBox(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , state_handler(this)
{
    border_color = StateColor(
        std::make_pair(QColor(0xF0F0F1), (int) StateColor::Disabled),
        std::make_pair(QColor("#CECECE"), (int) StateColor::Normal));
    setAttribute(Qt::WA_StyledBackground, false);
}

bool StaticBox::init(QWidget *parent, Qt::WindowFlags f)
{
    // Called by subclasses that cannot pass parent to the QWidget constructor.
    setParent(parent);
    setWindowFlags(f);
    state_handler.attach({&border_color, &background_color, &background_color2});
    state_handler.update_binds();
    setAutoFillBackground(false);
    return true;
}

void StaticBox::SetCornerRadius(double radius)
{
    this->radius = radius;
    update();
}

void StaticBox::SetBorderWidth(int width)
{
    border_width = width;
    update();
}

void StaticBox::SetBorderColor(StateColor const &color)
{
    if (border_color != color) {
        border_color = color;
        state_handler.update_binds();
        update();
    }
}

void StaticBox::SetBorderColorNormal(QColor const &color)
{
    border_color.setColorForStates(color, 0);
    update();
}

void StaticBox::SetBorderStyle(Qt::PenStyle style)
{
    border_style = style;
    update();
}

void StaticBox::SetBackgroundColor(StateColor const &color)
{
    background_color = color;
    state_handler.update_binds();
    update();
}

void StaticBox::SetBackgroundColorNormal(QColor const &color)
{
    background_color.setColorForStates(color, 0);
    update();
}

void StaticBox::SetBackgroundColor2(StateColor const &color)
{
    background_color2 = color;
    state_handler.update_binds();
    update();
}

QColor StaticBox::GetParentBackgroundColor(QWidget *parent)
{
    if (auto *box = qobject_cast<StaticBox *>(parent)) {
        if (box->background_color.count() > 0) {
            if (box->background_color2.count() == 0)
                return box->background_color.defaultColor();
            QColor s = box->background_color.defaultColor();
            QColor e = box->background_color2.defaultColor();
            return QColor((s.red()   + e.red())   / 2,
                          (s.green() + e.green()) / 2,
                          (s.blue()  + e.blue())  / 2);
        }
    }
    if (parent)
        return parent->palette().color(QPalette::Window);
    return Qt::white;
}

void StaticBox::ShowBadge(bool show)
{
    if (show && badge.name() != "badge") {
        badge = ScalableBitmap(this, "badge", 18);
        update();
    } else if (!show && !badge.name().empty()) {
        badge = ScalableBitmap{};
        update();
    }
}

void StaticBox::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    render(painter);
}

void StaticBox::render(QPainter &painter)
{
    doRender(painter);
}

void StaticBox::doRender(QPainter &painter)
{
    const QSize  sz     = size();
    const int    states = state_handler.states();

    if (background_color2.count() == 0) {
        if ((border_width && border_color.count() > 0) || background_color.count() > 0) {
            QRectF rc(0, 0, sz.width(), sz.height());

            // Inset rect so the pen stroke stays inside the widget boundary.
            if (border_width && border_color.count() > 0) {
                const double half = border_width / 2.0;
                rc.adjust(half, half, -half, -half);
                painter.setPen(QPen(border_color.colorForStates(states),
                                    border_width,
                                    border_style));
            } else {
                painter.setPen(QPen(background_color.colorForStates(states)));
            }

            if (background_color.count() > 0)
                painter.setBrush(background_color.colorForStates(states));
            else
                painter.setBrush(palette().color(QPalette::Window));

            if (radius <= 0) {
                painter.drawRect(rc);
            } else {
                painter.drawRoundedRect(rc, radius - border_width, radius - border_width);
            }
        }
    } else {
        // Vertical linear gradient
        QColor start = background_color.colorForStates(states);
        QColor stop  = background_color2.colorForStates(states);
        QLinearGradient grad(0, 0, 0, sz.height());
        grad.setColorAt(0.0, start);
        grad.setColorAt(1.0, stop);
        painter.setPen(Qt::NoPen);
        painter.setBrush(grad);
        painter.drawRect(QRect(QPoint(0, 0), sz));
    }

    if (badge.bmp().IsOk()) {
        const QSize bs = badge.bmp().GetScaledSize();
        painter.drawPixmap(sz.width() - bs.width(), 0, badge.bmp());
    }
}

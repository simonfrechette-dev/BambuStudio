#include "StaticGroup.hpp"
#include <QPainter>
#include <QStyleOptionGroupBox>
#include <QStyle>

StaticGroup::StaticGroup(QWidget *parent, int /*id*/)
    : QGroupBox(parent)
    , badge(this, "notification_dots", 20)
    , borderColor_(Qt::transparent)
{
}

void StaticGroup::ShowBadge(bool show)
{
    showBadge_ = show;
    update();
}

void StaticGroup::SetBorderColor(const QColor &color)
{
    borderColor_ = color;
    update();
}

void StaticGroup::setVisible(bool show)
{
    QGroupBox::setVisible(show);
}

void StaticGroup::paintEvent(QPaintEvent *event)
{
    QGroupBox::paintEvent(event);
    if (showBadge_ && badge.bmp().IsOk()) {
        QPainter p(this);
        const QSize bsz = badge.GetBmpSize();
        // Draw badge at top-right of the group box title area
        p.drawPixmap(width() - bsz.width() - 4, 2, badge.bmp());
    }
    if (borderColor_.isValid() && borderColor_ != Qt::transparent) {
        QPainter p(this);
        p.setPen(QPen(borderColor_, 1));
        p.drawRect(0, 0, width() - 1, height() - 1);
    }
}

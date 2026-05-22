#include "StaticLine.hpp"
#include "Label.hpp"
#include "StateColor.hpp"

#include <QPainter>
#include <QFontMetrics>

StaticLine::StaticLine(QWidget *parent, bool vertical,
                       const QString &label, const QString &icon)
    : QWidget(parent)
    , lineColor(QColor("#EEEEEE"))
    , m_vertical(vertical)
    , m_text(label)
{
    setAutoFillBackground(false);
    setFont(Label::Body_14);
    SetIcon(icon);
    measureSize();
}

void StaticLine::setText(const QString &label)
{
    m_text = label;
    measureSize();
    update();
}

void StaticLine::SetIcon(const QString &iconName)
{
    icon = iconName.isEmpty() ? ScalableBitmap()
                              : ScalableBitmap(this, iconName.toStdString(), 18);
    measureSize();
    update();
}

void StaticLine::SetLineColour(QColor color)
{
    lineColor = color;
    update();
}

void StaticLine::Rescale()
{
    measureSize();
    update();
}

QSize StaticLine::sizeHint() const { return minimumSizeHint(); }

void StaticLine::measureSize()
{
    QFontMetrics fm(font());
    QSize textSz = fm.boundingRect(m_text).size();
    QSize szContent = textSz;
    if (icon.bmp().IsOk()) {
        QSize szIcon = icon.GetBmpSize();
        if (szContent.height() > 0)
            szContent.setWidth(szContent.width() + 5);
        szContent.setWidth(szContent.width() + szIcon.width());
        if (szIcon.height() > szContent.height())
            szContent.setHeight(szIcon.height());
    }
    if (m_vertical)
        szContent = QSize(std::max(szContent.width(), 1), szContent.height() + 10);
    else
        szContent = QSize(szContent.width() + 10, std::max(szContent.height(), 1));
    setMinimumSize(szContent);
}

void StaticLine::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    render(painter);
}

void StaticLine::render(QPainter &painter)
{
    const QSize sz = size();
    QFontMetrics fm(font());
    QSize textSz = fm.boundingRect(m_text).size();

    painter.setPen(lineColor);
    int x = 0, y = 0;
    if (icon.bmp().IsOk()) {
        const QSize szIcon = icon.GetBmpSize();
        painter.drawPixmap(QPoint(0, (sz.height() - szIcon.height()) / 2), icon.bmp());
        x += szIcon.width() + 5;
    }
    if (!m_text.isEmpty()) {
        painter.setPen(Qt::black);
        painter.drawText(QPoint(x, (sz.height() + textSz.height()) / 2 - fm.descent()), m_text);
        x += textSz.width() + 5;
    }
    painter.setPen(lineColor);
    if (m_vertical)
        painter.drawLine(sz.width() / 2, y, sz.width() / 2, sz.height());
    else
        painter.drawLine(x, sz.height() / 2, sz.width(), sz.height() / 2);
}

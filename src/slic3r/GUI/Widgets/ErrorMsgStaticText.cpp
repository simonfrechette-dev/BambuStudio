#include "ErrorMsgStaticText.hpp"
#include <QPainter>
#include <QFontMetrics>

ErrorMsgStaticText::ErrorMsgStaticText(QWidget *parent, int /*id*/,
                                        const QPoint &pos, const QSize &size)
    : QWidget(parent)
{
    if (!pos.isNull())  move(pos);
    if (!size.isEmpty()) setFixedSize(size);
}

void ErrorMsgStaticText::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QFontMetrics fm(font());

    // Simple word-wrapped text render
    const QStringList words = m_msg.split(' ');
    int x = 0, y = fm.ascent();
    const int lineH = fm.height();
    QString line;

    auto flushLine = [&]() {
        if (!line.isEmpty()) {
            p.drawText(QPoint(0, y), line);
            y += lineH;
            line.clear();
            x = 0;
        }
    };

    for (const QString &w : words) {
        const int ww = fm.horizontalAdvance(w + ' ');
        if (x + ww > width() && !line.isEmpty()) {
            flushLine();
        }
        line += w + ' ';
        x    += ww;
        if (y > height()) break;
    }
    flushLine();
}

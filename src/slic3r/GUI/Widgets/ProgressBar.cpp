#include "ProgressBar.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

ProgressBar::ProgressBar(QWidget *parent, int maxVal, bool showNumber)
    : QWidget(parent)
    , m_shownumber(showNumber)
    , m_max(maxVal)
{
    setMinimumHeight(14);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void ProgressBar::ShowNumber(bool shown)
{
    m_shownumber = shown;
    update();
}

void ProgressBar::Disable(const QString &text)
{
    m_disable      = true;
    m_disable_text = text;
    update();
}

void ProgressBar::SetValue(int step)
{
    m_step       = std::clamp(step, 0, m_max);
    m_proportion = m_max > 0 ? double(m_step) / m_max : 0.0;
    update();
}

void ProgressBar::Reset()
{
    m_step       = 0;
    m_proportion = 0.0;
    m_disable    = false;
    m_disable_text.clear();
    update();
}

void ProgressBar::SetProgress(int step) { SetValue(step); }

void ProgressBar::SetRadius(double r)
{
    m_radius = r;
    update();
}

void ProgressBar::SetProgressForedColour(QColor c)
{
    m_progress_colour = c;
    update();
}

void ProgressBar::SetProgressBackgroundColour(QColor c)
{
    m_progress_background_colour = c;
    update();
}

QSize ProgressBar::sizeHint() const
{
    return QSize(200, std::max(14, qRound(m_radius * 2)));
}

void ProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    doRender(p);
}

void ProgressBar::doRender(QPainter &p)
{
    const QRectF rc = rect();
    const double r  = m_radius;

    // Background
    p.setBrush(m_progress_background_colour);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rc, r, r);

    // Filled portion
    if (m_proportion > 0.0) {
        QRectF filled = rc;
        filled.setWidth(rc.width() * m_proportion);
        const QColor &fc = m_disable ? m_progress_colour_disable : m_progress_colour;
        p.setBrush(fc);
        p.drawRoundedRect(filled, r, r);
    }

    // Disable text overlay
    if (m_disable && !m_disable_text.isEmpty()) {
        p.setPen(Qt::white);
        p.drawText(rc, Qt::AlignCenter, m_disable_text);
    } else if (m_shownumber) {
        p.setPen(Qt::white);
        p.drawText(rc, Qt::AlignCenter,
                   QString("%1%").arg(qRound(m_proportion * 100)));
    }
}

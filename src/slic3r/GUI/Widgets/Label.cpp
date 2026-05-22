#include "Label.hpp"
#include "StaticBox.hpp"

#include "libslic3r/Utils.hpp"
#include "libslic3r/AppConfig.hpp"

#include <QFontMetrics>
#include <QFontDatabase>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>

// ---------------------------------------------------------------------------
// Static font catalogue

QFont Label::Head_48;
QFont Label::Head_32;
QFont Label::Head_24;
QFont Label::Head_20;
QFont Label::Head_18;
QFont Label::Head_16;
QFont Label::Head_15;
QFont Label::Head_14;
QFont Label::Head_13;
QFont Label::Head_12;
QFont Label::Head_11;
QFont Label::Head_10;

QFont Label::Body_16;
QFont Label::Body_15;
QFont Label::Body_14;
QFont Label::Body_13;
QFont Label::Body_12;
QFont Label::Body_11;
QFont Label::Body_10;
QFont Label::Body_9;
QFont Label::Body_8;

QFont Label::sysFont(int size, bool bold, std::string lang_code)
{
#ifndef __APPLE__
    size = size * 4 / 5;
#endif
    QString face;
    if (lang_code == "ja")
        face = QStringLiteral("Source Han Sans JP Normal");
    else if (lang_code == "ko")
        face = QStringLiteral("NanumGothic");
    else
        face = QStringLiteral("HarmonyOS Sans SC");

    QFont font(face, size);
    font.setBold(bold);
    if (!QFontDatabase::families().contains(face, Qt::CaseInsensitive)) {
        font = QApplication::font();
        font.setBold(bold);
        font.setPointSize(size);
    }
    return font;
}

void Label::initSysFont(std::string lang_code, bool load_font_resource)
{
#ifdef __linux__
    if (load_font_resource) {
        const std::string &rdir = Slic3r::resources_dir();
        QFontDatabase::addApplicationFont(
            QString::fromStdString(rdir + "/fonts/HarmonyOS_Sans_SC_Bold.ttf"));
        QFontDatabase::addApplicationFont(
            QString::fromStdString(rdir + "/fonts/HarmonyOS_Sans_SC_Regular.ttf"));
    }
#else
    (void)load_font_resource;
#endif
    Head_48 = sysFont(48, true,  lang_code);
    Head_32 = sysFont(32, true,  lang_code);
    Head_24 = sysFont(24, true,  lang_code);
    Head_20 = sysFont(20, true,  lang_code);
    Head_18 = sysFont(18, true,  lang_code);
    Head_16 = sysFont(16, true,  lang_code);
    Head_15 = sysFont(15, true,  lang_code);
    Head_14 = sysFont(14, true,  lang_code);
    Head_13 = sysFont(13, true,  lang_code);
    Head_12 = sysFont(12, true,  lang_code);
    Head_11 = sysFont(11, true,  lang_code);
    Head_10 = sysFont(10, true,  lang_code);

    Body_16 = sysFont(16, false, lang_code);
    Body_15 = sysFont(15, false, lang_code);
    Body_14 = sysFont(14, false, lang_code);
    Body_13 = sysFont(13, false, lang_code);
    Body_12 = sysFont(12, false, lang_code);
    Body_11 = sysFont(11, false, lang_code);
    Body_10 = sysFont(10, false, lang_code);
    Body_9  = sysFont(9,  false, lang_code);
    Body_8  = sysFont(8,  false, lang_code);
}

// ---------------------------------------------------------------------------
// Word-wrap helper (replaces wxTextWrapper2 + wxLabelWrapper2)

QSize Label::split_lines(const QFontMetrics &fm, int maxW,
                          const QString &text, QString &out, int maxCount)
{
    out.clear();
    const QStringList paragraphs = text.split('\n');
    int lineCount = 0;
    bool first = true;

    for (const QString &para : paragraphs) {
        if (!first) out += '\n';
        first = false;

        if (maxCount > 0 && lineCount >= maxCount - 1) {
            out += fm.elidedText(para, Qt::ElideRight, maxW);
            ++lineCount;
            break;
        }

        QString remaining = para;
        while (!remaining.isEmpty()) {
            if (maxCount > 0 && lineCount >= maxCount - 1) {
                out += fm.elidedText(remaining, Qt::ElideRight, maxW);
                ++lineCount;
                remaining.clear();
                break;
            }
            // Find how many characters fit.
            int lo = 0, hi = remaining.size();
            while (lo < hi) {
                int mid = (lo + hi + 1) / 2;
                if (fm.horizontalAdvance(remaining.left(mid)) <= maxW)
                    lo = mid;
                else
                    hi = mid - 1;
            }
            if (lo == 0) lo = 1; // always advance at least one char
            if (lo == remaining.size()) {
                out += remaining;
                remaining.clear();
            } else {
                // Break at last space or CJK boundary.
                int breakAt = lo;
                for (int i = lo - 1; i > 0; --i) {
                    QChar c = remaining[i];
                    if (c == ' ') { breakAt = i; break; }
                    if (c.unicode() > 0x4E00) { breakAt = i + 1; break; }
                }
                out += remaining.left(breakAt);
                remaining = remaining.mid(breakAt).trimmed();
                if (!remaining.isEmpty()) {
                    out += '\n';
                    ++lineCount;
                }
            }
        }
    }
    return fm.boundingRect(QRect(0, 0, maxW, 9999),
                           Qt::TextWordWrap, out).size();
}

// ---------------------------------------------------------------------------
// Constructors

Label::Label(QWidget *parent, const QString &text, long style, const QSize &sz)
    : Label(parent, Body_14, text, style, sz)
{}

Label::Label(QWidget *parent, const QFont &font, const QString &text,
             long style, const QSize &sz)
    : QLabel(parent)
    , m_font(font)
    , m_color(QColor(0x26, 0x2E, 0x30))  // #262E30
    , m_rawText(text)
    , m_style(style)
{
    setFont(font);
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, StaticBox::GetParentBackgroundColor(parent));
    setPalette(pal);
    setForegroundRole(QPalette::WindowText);

    QPalette p2 = palette();
    p2.setColor(QPalette::WindowText, m_color);
    setPalette(p2);

    if (!sz.isEmpty())
        setFixedSize(sz);

    if (style & LB_PROPAGATE_MOUSE_EVENT) {
        // handled in mousePressEvent / mouseReleaseEvent overrides
    }
    if (style & LB_AUTO_WRAP) {
        setWordWrap(true);
    }

    applyStyle();
    QLabel::setText(m_rawText);
}

// ---------------------------------------------------------------------------

void Label::setText(const QString &label)
{
    if (m_rawText == label) return;
    m_rawText = label;
    rewrap();
}

void Label::setStyleFlags(long style)
{
    if (style == m_style) return;
    m_style = style;
    applyStyle();
}

void Label::applyStyle()
{
    if (m_style & LB_HYPERLINK) {
        m_color = palette().color(QPalette::WindowText);
        QFont f = m_font;
        f.setUnderline(true);
        QWidget::setFont(f);
        QPalette p = palette();
        p.setColor(QPalette::WindowText, QColor("#00AE42"));
        setPalette(p);
        setCursor(Qt::PointingHandCursor);
    } else {
        QPalette p = palette();
        p.setColor(QPalette::WindowText, m_color);
        setPalette(p);
        QWidget::setFont(m_font);
        setCursor(Qt::ArrowCursor);
    }
    rewrap();
}

void Label::Wrap(int width)
{
    if (m_style & LB_AUTO_WRAP) {
        setMaximumWidth(width);
        setWordWrap(true);
        QLabel::setText(m_rawText);
    }
}

void Label::rewrap()
{
    if (m_inRewrap) return;
    m_inRewrap = true;
    if ((m_style & LB_AUTO_WRAP) && width() > 0)
        Wrap(width());
    else
        QLabel::setText(m_rawText);
    m_inRewrap = false;
}

QSize Label::sizeHint() const
{
    QSize s = QLabel::sizeHint();
#ifdef _WIN32
    if (s.width() > 0) s.setWidth(s.width() + 4);
#endif
    return s;
}

void Label::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    if (m_style & LB_AUTO_WRAP)
        rewrap();
}

void Label::mousePressEvent(QMouseEvent *event)
{
    if (m_style & LB_PROPAGATE_MOUSE_EVENT)
        QApplication::sendEvent(parentWidget(), event);
    else
        QLabel::mousePressEvent(event);
}

void Label::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_style & LB_PROPAGATE_MOUSE_EVENT)
        QApplication::sendEvent(parentWidget(), event);
    else
        QLabel::mouseReleaseEvent(event);
}

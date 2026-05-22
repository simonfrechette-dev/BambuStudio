#include "BBLStatusBarSend.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QIcon>
#include <QCursor>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "libslic3r/Utils.hpp"

namespace {
// Simple QObject-based event filter that calls a callback on mouse press.
class ClickFilter : public QObject
{
public:
    ClickFilter(QObject *parent, std::function<void()> fn)
        : QObject(parent), m_fn(std::move(fn)) {}

    bool eventFilter(QObject *, QEvent *e) override
    {
        if (e->type() == QEvent::MouseButtonPress) {
            m_fn();
            return true;
        }
        return false;
    }
private:
    std::function<void()> m_fn;
};
} // anonymous namespace

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "libslic3r/Utils.hpp"

namespace Slic3r {

BBLStatusBarSend::BBLStatusBarSend(QWidget *parent, int /*id*/)
    : m_self{new QWidget(parent)}
    , m_sizer{new QHBoxLayout}
    , m_sizer_eline{new QHBoxLayout}
    , m_sizer_status_text{new QHBoxLayout}
    , block_left{nullptr}
    , block_right{nullptr}
{
    m_self->setStyleSheet("background-color: white;");

    // Progress bar
    m_prog = new QProgressBar(m_self);
    m_prog->setRange(0, 100);
    m_prog->setValue(0);
    m_prog->setFixedHeight(6);
    m_prog->setMinimumWidth(300);
    m_prog->setTextVisible(false);

    // Status text
    m_status_text = new QLabel(m_self);
    m_status_text->setStyleSheet("color: #6b6b6b;");
    m_status_text->setFont(Label::Body_13);
    m_status_text->setMaximumWidth(360);
    m_status_text->setMaximumHeight(40);
    m_status_text->setWordWrap(true);

    // Percent text
    m_stext_percent = new QLabel(m_self);
    m_stext_percent->setStyleSheet("color: #6b6b6b;");
    m_stext_percent->setFont(Label::Body_13);

    // Cancel button
    m_cancelbutton = new Button(m_self, _L("Cancel"));
    m_cancelbutton->setFixedSize(58, 22);
    QObject::connect(m_cancelbutton, &Button::clicked, m_self, [this]() { cancel(); });

    // Error info: link label + arrow icon
    m_link_show_error = new Label(m_self, _L("Check the reason"));
    m_link_show_error->setStyleSheet("color: #6b6b6b;");
    m_link_show_error->setFont(Label::Head_13);
    m_link_show_error->setCursor(Qt::PointingHandCursor);

    // Load SVG icons for the expand/collapse arrow
    std::string res = Slic3r::resources_dir();
    m_bitmap_show_error_close =
        QIcon(QString::fromStdString(res + "/images/link_more_error_close.svg"))
            .pixmap(7, 7);
    m_bitmap_show_error_open =
        QIcon(QString::fromStdString(res + "/images/link_more_error_open.svg"))
            .pixmap(7, 7);

    m_static_bitmap_show_error = new QLabel(m_self);
    m_static_bitmap_show_error->setPixmap(m_bitmap_show_error_open);
    m_static_bitmap_show_error->setFixedSize(7, 7);
    m_static_bitmap_show_error->setCursor(Qt::PointingHandCursor);

    // Toggle error info on click
    auto toggle_error = [this]() {
        m_show_error_info_state = !m_show_error_info_state;
        m_static_bitmap_show_error->setPixmap(
            m_show_error_info_state ? m_bitmap_show_error_close
                                    : m_bitmap_show_error_open);
        if (m_self->parentWidget())
            QApplication::postEvent(m_self->parentWidget(),
                                    new QEvent(EVT_SHOW_ERROR_INFO_SEND));
    };
    auto *filter = new ClickFilter(m_self, toggle_error);
    m_link_show_error->installEventFilter(filter);
    m_static_bitmap_show_error->installEventFilter(filter);

    m_link_show_error->hide();
    m_static_bitmap_show_error->hide();

    // m_sizer_status_text: link label + arrow icon
    m_sizer_status_text->setContentsMargins(0, 0, 0, 0);
    m_sizer_status_text->setSpacing(2);
    m_sizer_status_text->addWidget(m_link_show_error);
    m_sizer_status_text->addWidget(m_static_bitmap_show_error,
                                   0, Qt::AlignVCenter);

    // m_sizer_eline: progress + percent + error_link + stretch + cancel
    m_sizer_eline->setContentsMargins(0, 0, 0, 0);
    m_sizer_eline->setSpacing(4);
    m_sizer_eline->addWidget(m_prog, 1);
    m_sizer_eline->addWidget(m_stext_percent, 0, Qt::AlignVCenter);
    m_sizer_eline->addLayout(m_sizer_status_text, 0);
    m_sizer_eline->addStretch(1);
    m_sizer_eline->addWidget(m_cancelbutton, 0, Qt::AlignVCenter);

    // Body: stretch + status_text + bottom bar + stretch
    auto *m_sizer_body = new QVBoxLayout;
    m_sizer_body->setContentsMargins(0, 0, 0, 0);
    m_sizer_body->setSpacing(4);
    m_sizer_body->addStretch(1);
    m_sizer_body->addWidget(m_status_text);
    m_sizer_body->addLayout(m_sizer_eline);
    m_sizer_body->addStretch(1);

    // Outer HBox: body fills all space
    m_sizer->setContentsMargins(10, 0, 10, 0);
    m_sizer->addLayout(m_sizer_body, 1);

    m_self->setLayout(m_sizer);
}



void BBLStatusBarSend::set_prog_block() {}

int BBLStatusBarSend::get_progress() const
{
    return m_prog->value();
}

void BBLStatusBarSend::set_progress(int val)
{
    if (val < 0) return;

    if (!m_prog->isVisible()) {
        m_prog->show();
        m_cancelbutton->show();
    }
    m_prog->setValue(val);
    set_percent_text(QString("%1%").arg(val));
}

int BBLStatusBarSend::get_range() const
{
    return m_prog->maximum();
}

void BBLStatusBarSend::set_range(int val)
{
    if (val != m_prog->maximum())
        m_prog->setRange(0, val);
}

void BBLStatusBarSend::clear_percent()
{
    m_cancelbutton->hide();
}

void BBLStatusBarSend::show_error_info(const QString &msg, int /*code*/,
                                       const QString & /*description*/,
                                       const QString & /*extra*/)
{
    set_status_text(msg);
    m_prog->hide();
    m_stext_percent->hide();
    m_link_show_error->show();
    m_static_bitmap_show_error->show();
    m_cancelbutton->show();

    if (m_self->parentWidget())
        QApplication::postEvent(m_self->parentWidget(),
                                new QEvent(EVT_SHOW_ERROR_FAIL_SEND));
}

void BBLStatusBarSend::show_progress(bool show)
{
    if (show)
        m_prog->show();
    else
        m_prog->hide();
}

void BBLStatusBarSend::start_busy(int /*rate*/)
{
    m_busy = true;
    show_progress(true);
    show_cancel_button();
}

void BBLStatusBarSend::stop_busy()
{
    show_progress(false);
    hide_cancel_button();
    m_prog->setValue(0);
    m_busy = false;
}

void BBLStatusBarSend::set_cancel_callback_fina(BBLStatusBarSend::CancelFn ccb)
{
    m_cancel_cb_fina = ccb;
    if (ccb)
        m_cancelbutton->show();
    else
        m_cancelbutton->hide();
}

void BBLStatusBarSend::set_cancel_callback(BBLStatusBarSend::CancelFn /*ccb*/)
{
    // intentional no-op, matches original wx behaviour
}

QWidget *BBLStatusBarSend::get_panel()
{
    return m_self;
}

bool BBLStatusBarSend::is_english_text(QString str)
{
    std::regex reg("^[0-9a-zA-Z]+$");
    std::smatch m;
    std::string pattern_special = "{}[]<>~!@#$%^&*(),.?/ :";
    for (int i = 0; i < str.length(); i++) {
        std::string ch = str.mid(i, 1).toStdString();
        if (!std::regex_match(ch, m, reg)) {
            if (pattern_special.find(ch) == std::string::npos)
                return false;
        }
    }
    return true;
}

bool BBLStatusBarSend::format_text(QLabel *dc, int width, const QString &text,
                                   QString &multiline_text)
{
    multiline_text = text;
    if (width > 0 && dc->fontMetrics().horizontalAdvance(text) > width) {
        int start = 0;
        while (true) {
            int idx = -1;
            for (int i = start; i < multiline_text.length(); i++) {
                if (multiline_text[i] == ' ') {
                    QString sub = multiline_text.mid(start, i - start);
                    if (dc->fontMetrics().horizontalAdvance(sub) < width)
                        idx = i;
                    else {
                        if (idx == -1) idx = i;
                        break;
                    }
                }
            }
            if (idx == -1) break;
            multiline_text[idx] = '\n';
            start = idx + 1;
            if (dc->fontMetrics().horizontalAdvance(multiline_text.mid(start)) < width)
                break;
        }
        return true;
    }
    return false;
}

void BBLStatusBarSend::set_status_text(const QString &txt)
{
    m_status_text->setText(txt);
    m_status_text->setWordWrap(true);
}

void BBLStatusBarSend::set_percent_text(const QString &txt)
{
    m_stext_percent->setText(txt);
}

void BBLStatusBarSend::set_status_text(const std::string &txt)
{
    set_status_text(QString::fromStdString(txt));
}

void BBLStatusBarSend::set_status_text(const char *txt)
{
    set_status_text(QString::fromUtf8(txt));
    if (m_self->parentWidget())
        m_self->parentWidget()->updateGeometry();
}

void BBLStatusBarSend::msw_rescale()
{
    m_cancelbutton->setFixedSize(56, 24);
}

QString BBLStatusBarSend::get_status_text() const
{
    return m_status_text->text();
}

bool BBLStatusBarSend::update_status(QString &msg, bool &was_cancel, int percent,
                                     bool /*yield*/)
{
    set_status_text(msg);
    if (percent >= 0)
        set_progress(percent);
    was_cancel = m_was_cancelled;
    return true;
}

void BBLStatusBarSend::reset()
{
    m_link_show_error->hide();
    m_static_bitmap_show_error->hide();
    m_prog->show();
    m_stext_percent->show();
    m_cancelbutton->setEnabled(true);
    m_cancelbutton->show();
    m_was_cancelled = false;

    set_status_text(QString());
    set_progress(0);
    set_percent_text("0%");
}

void BBLStatusBarSend::show_cancel_button()
{
    m_cancelbutton->show();
}

void BBLStatusBarSend::hide_cancel_button()
{
    m_cancelbutton->hide();
}

void BBLStatusBarSend::change_button_label(QString name)
{
    m_cancelbutton->setText(name);
}

void BBLStatusBarSend::disable_cancel_button()
{
    m_cancelbutton->setEnabled(false);
}

void BBLStatusBarSend::enable_cancel_button()
{
    m_cancelbutton->setEnabled(true);
}

void BBLStatusBarSend::cancel()
{
    m_was_cancelled = true;
    if (m_cancel_cb_fina) m_cancel_cb_fina();
}

void BBLStatusBarSend::set_object_info(const QString & /*txt*/) {}
void BBLStatusBarSend::set_slice_info(const QString & /*txt*/) {}
void BBLStatusBarSend::show_slice_info(bool /*show*/) {}
bool BBLStatusBarSend::is_slice_info_shown() { return false; }
void BBLStatusBarSend::set_font(const QFont &font) { m_self->setFont(font); }

} // namespace Slic3r

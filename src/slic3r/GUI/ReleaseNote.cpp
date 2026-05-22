#include "ReleaseNote.hpp"
#include "I18N.hpp"
#include "wxExtensions.hpp"
#include "GUI_App.hpp"
#include "Plater.hpp"
#include "libslic3r/AppConfig.hpp"
#include "Widgets/TextInput.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QFrame>
#include <QCheckBox>
#include <QFont>
#include <QTimer>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

// ============================================================================
// ReleaseNoteDialog
// ============================================================================

ReleaseNoteDialog::ReleaseNoteDialog(Plater * /*plater*/)
    : DPIDialog(nullptr)
{
    setWindowTitle(_L("Release Note"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(560, 480);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    m_text_up_info = new Label(this, Label::Head_16);
    m_text_up_info->setWordWrap(true);
    vbox->addWidget(m_text_up_info);

    // Scroll area for release note content
    m_vebview_release_note = new QScrollArea(this);
    m_vebview_release_note->setWidgetResizable(true);
    m_vebview_release_note->setFrameShape(QFrame::NoFrame);
    auto *content_w = new QWidget;
    auto *content_layout = new QVBoxLayout(content_w);
    content_layout->setContentsMargins(4, 4, 4, 4);
    auto *note_text = new QTextBrowser(content_w);
    note_text->setOpenExternalLinks(true);
    content_layout->addWidget(note_text);
    m_vebview_release_note->setWidget(content_w);
    vbox->addWidget(m_vebview_release_note, 1);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *ok_btn = new Button(this, _L("OK"));
    ok_btn->SetValue(true);
    connect(ok_btn, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(ok_btn);
    vbox->addLayout(btn_row);

    adjustSize();
}

ReleaseNoteDialog::~ReleaseNoteDialog() = default;

void ReleaseNoteDialog::update_release_note(QString release_note, std::string version)
{
    if (m_text_up_info)
        m_text_up_info->setText(_L("New version:") + " " + QString::fromStdString(version));
    if (m_vebview_release_note) {
        auto *tb = m_vebview_release_note->findChild<QTextBrowser *>();
        if (tb) tb->setPlainText(release_note);
    }
}

void ReleaseNoteDialog::on_dpi_changed(const QRect &) {}

// ============================================================================
// UpdatePluginDialog
// ============================================================================

UpdatePluginDialog::UpdatePluginDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Update plugin"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(480, 360);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    m_text_up_info = new Label(this, Label::Head_16);
    m_text_up_info->setWordWrap(true);
    m_text_up_info->setText(_L("A new plugin update is available."));
    vbox->addWidget(m_text_up_info);

    operation_tips = new Label(this, Label::Body_12);
    operation_tips->setWordWrap(true);
    vbox->addWidget(operation_tips);

    m_vebview_release_note = new QScrollArea(this);
    m_vebview_release_note->setWidgetResizable(true);
    m_vebview_release_note->setFrameShape(QFrame::NoFrame);
    auto *inner = new QWidget;
    new QVBoxLayout(inner);
    m_vebview_release_note->setWidget(inner);
    vbox->addWidget(m_vebview_release_note, 1);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *cancel = new Button(this, _L("Later"));
    connect(cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(cancel);
    auto *ok = new Button(this, _L("Update"));
    ok->SetValue(true);
    connect(ok, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(ok);
    vbox->addLayout(btn_row);

    adjustSize();
}

UpdatePluginDialog::~UpdatePluginDialog() = default;
void UpdatePluginDialog::update_info(std::string /*json_path*/) {}
void UpdatePluginDialog::on_dpi_changed(const QRect &) {}

// ============================================================================
// UpdateVersionDialog
// ============================================================================

UpdateVersionDialog::UpdateVersionDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("New version available"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(560, 480);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    // Brand area (dark header)
    m_brand = new QLabel(this);
    m_brand->setStyleSheet(
        "background-color: #1F2637; color: white; padding: 12px; font-size: 16px; font-weight: bold;");
    m_brand->setText("Bambu Studio");
    vbox->addWidget(m_brand);

    m_text_up_info = new Label(this, Label::Head_16);
    m_text_up_info->setWordWrap(true);
    m_text_up_info->setText(_L("A new version of BambuStudio is available."));
    vbox->addWidget(m_text_up_info);

    // Stacked widget: webview page vs scrollable text page
    m_simplebook_release_note = new QStackedWidget(this);

    // Page 0: scrollable text
    auto *scroll_page = new QWidget;
    auto *scroll_vbox = new QVBoxLayout(scroll_page);
    m_scrollwindows_release_note = new QScrollArea(scroll_page);
    m_scrollwindows_release_note->setWidgetResizable(true);
    m_scrollwindows_release_note->setFrameShape(QFrame::NoFrame);
    auto *notes_inner = new QWidget;
    sizer_text_release_note = new QVBoxLayout(notes_inner);
    m_staticText_release_note = new Label(notes_inner, Label::Body_12);
    m_staticText_release_note->setWordWrap(true);
    sizer_text_release_note->addWidget(m_staticText_release_note);
    sizer_text_release_note->addStretch(1);
    m_scrollwindows_release_note->setWidget(notes_inner);
    scroll_vbox->addWidget(m_scrollwindows_release_note, 1);
    m_simplebook_release_note->addWidget(scroll_page);

    // Page 1: fallback empty webview placeholder
    m_vebview_release_note = new QWidget;
    m_simplebook_release_note->addWidget(m_vebview_release_note);

    m_simplebook_release_note->setCurrentIndex(0);
    vbox->addWidget(m_simplebook_release_note, 1);

    // Browser link row
    auto *browser_row = new QHBoxLayout;
    m_bitmap_open_in_browser = new QLabel(this);
    m_link_open_in_browser   = new QLabel(this);
    m_link_open_in_browser->setText(
        "<a href='https://bambulab.com/en/download'>" + _L("Open in browser") + "</a>");
    m_link_open_in_browser->setOpenExternalLinks(true);
    browser_row->addWidget(m_bitmap_open_in_browser);
    browser_row->addWidget(m_link_open_in_browser);
    browser_row->addStretch(1);
    vbox->addLayout(browser_row);

    // Buttons
    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_button_skip_version = new Button(this, _L("Skip this version"));
    connect(m_button_skip_version, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_skip_version);

    m_button_cancel = new Button(this, _L("Remind me later"));
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_cancel);

    m_button_download = new Button(this, _L("Download"));
    m_button_download->SetValue(true);
    connect(m_button_download, &Button::clicked, this, [this] {
        if (!url_line.empty())
            QDesktopServices::openUrl(QUrl(QString::fromStdString(url_line)));
        accept();
    });
    btn_row->addWidget(m_button_download);

    vbox->addLayout(btn_row);
    adjustSize();
}

UpdateVersionDialog::~UpdateVersionDialog() = default;

QWidget *UpdateVersionDialog::CreateTipView(QWidget *) { return nullptr; }
void UpdateVersionDialog::OnLoaded(QEvent &) {}
void UpdateVersionDialog::OnTitleChanged(QEvent &) {}
void UpdateVersionDialog::OnError(QEvent &) {}
bool UpdateVersionDialog::ShowReleaseNote(std::string content)
{
    if (m_staticText_release_note)
        m_staticText_release_note->setText(QString::fromStdString(content));
    return true;
}
void UpdateVersionDialog::RunScript(std::string) {}
void UpdateVersionDialog::on_dpi_changed(const QRect &) {}

void UpdateVersionDialog::update_version_info(QString release_note, QString version)
{
    if (m_text_up_info)
        m_text_up_info->setText(_L("New version:") + " " + version);
    if (m_staticText_release_note)
        m_staticText_release_note->setText(release_note);
}

std::vector<std::string> UpdateVersionDialog::splitWithStl(std::string str, std::string pattern)
{
    std::vector<std::string> result;
    std::string::size_type pos;
    str += pattern;
    while ((pos = str.find(pattern)) != std::string::npos) {
        result.push_back(str.substr(0, pos));
        str = str.substr(pos + pattern.size());
    }
    return result;
}

// ============================================================================
// SecondaryCheckDialog  (DPIFrame — floating frame, not modal dialog)
// ============================================================================

SecondaryCheckDialog::SecondaryCheckDialog(QWidget *parent, int, const QString &title,
                                            ButtonStyle btn_style,
                                            const QPoint &pos, const QSize &,
                                            long, bool show_check)
    : DPIFrame(parent)
    , event_parent(parent)
    , m_button_style(btn_style)
    , not_show_again(show_check)
{
    setWindowTitle(title.isEmpty() ? _L("Confirm") : title);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white; border: 1px solid #aaa;");

    btn_bg_green = StateColor(
        std::pair<QColor,int>(QColor(27,136,68), StateColor::Pressed),
        std::pair<QColor,int>(QColor(61,203,115), StateColor::Hovered),
        std::pair<QColor,int>(QColor(0,174,66), StateColor::Normal));
    btn_bg_white = StateColor(
        std::pair<QColor,int>(QColor(220,220,220), StateColor::Pressed),
        std::pair<QColor,int>(QColor(240,240,240), StateColor::Hovered),
        std::pair<QColor,int>(QColor(255,255,255), StateColor::Normal));

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 16, 16, 12);
    outer->setSpacing(10);

    m_sizer_main = outer;

    // Scrollable message area
    m_vebview_release_note = new QScrollArea(this);
    m_vebview_release_note->setWidgetResizable(true);
    m_vebview_release_note->setFrameShape(QFrame::NoFrame);
    auto *inner = new QWidget;
    auto *inner_vbox = new QVBoxLayout(inner);
    m_staticText_release_note = new Label(inner, Label::Body_14);
    m_staticText_release_note->setWordWrap(true);
    inner_vbox->addWidget(m_staticText_release_note);
    inner_vbox->addStretch(1);
    m_vebview_release_note->setWidget(inner);
    outer->addWidget(m_vebview_release_note, 1);

    // "Don't show again" checkbox
    if (not_show_again) {
        m_show_again_checkbox = new QCheckBox(_L("Don't show again"), this);
        outer->addWidget(m_show_again_checkbox);
    } else {
        m_show_again_checkbox = nullptr;
    }

    // Buttons
    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    auto add_btn = [&](Button *&var, const QString &text, bool primary = false) {
        var = new Button(this, text);
        if (primary) var->SetValue(true);
        btn_row->addWidget(var);
    };

    switch (m_button_style) {
    case ONLY_CONFIRM:
        add_btn(m_button_ok, _L("Confirm"), true);
        break;
    case CONFIRM_AND_CANCEL:
        add_btn(m_button_cancel, _L("Cancel"));
        add_btn(m_button_ok, _L("Confirm"), true);
        break;
    case CONFIRM_AND_DONE:
        add_btn(m_button_fn, _L("Done"));
        add_btn(m_button_ok, _L("Confirm"), true);
        break;
    case CONFIRM_AND_RETRY:
        add_btn(m_button_retry, _L("Retry"));
        add_btn(m_button_ok, _L("Confirm"), true);
        break;
    case CONFIRM_AND_RESUME:
        add_btn(m_button_resume, _L("Resume"));
        add_btn(m_button_ok, _L("Confirm"), true);
        break;
    case DONE_AND_RETRY:
        add_btn(m_button_retry, _L("Retry"));
        add_btn(m_button_fn, _L("Done"), true);
        break;
    default:
        add_btn(m_button_ok, _L("OK"), true);
        break;
    }

    outer->addLayout(btn_row);

    if (m_button_ok)     connect(m_button_ok,     &Button::clicked, this, [this]{ close(); });
    if (m_button_cancel) connect(m_button_cancel,  &Button::clicked, this, [this]{ close(); });
    if (m_button_retry)  connect(m_button_retry,   &Button::clicked, this, [this]{ close(); });
    if (m_button_fn)     connect(m_button_fn,      &Button::clicked, this, [this]{ close(); });
    if (m_button_resume) connect(m_button_resume,  &Button::clicked, this, [this]{ close(); });

    if (!pos.isNull()) move(pos);
    adjustSize();
    setFixedHeight(sizeHint().height());
}

SecondaryCheckDialog::~SecondaryCheckDialog() = default;

void SecondaryCheckDialog::update_text(QString text)
{
    if (m_staticText_release_note)
        m_staticText_release_note->setText(text);
}

void SecondaryCheckDialog::on_show() { show(); }
void SecondaryCheckDialog::on_hide() { hide(); }

void SecondaryCheckDialog::update_btn_label(QString ok_btn_text, QString cancel_btn_text)
{
    if (m_button_ok)     m_button_ok->setText(ok_btn_text);
    if (m_button_cancel) m_button_cancel->setText(cancel_btn_text);
}

void SecondaryCheckDialog::update_title_style(QString title, ButtonStyle style, QWidget *parent)
{
    setWindowTitle(title);
    // TODO: re-arrange buttons if style changes
}

void SecondaryCheckDialog::post_event(QEvent &&) {}
void SecondaryCheckDialog::rescale() {}
void SecondaryCheckDialog::on_dpi_changed(const QRect &) {}
void SecondaryCheckDialog::msw_rescale() {}

// ============================================================================
// PrintErrorDialog  (DPIFrame)
// ============================================================================

PrintErrorDialog::PrintErrorDialog(QWidget *parent, int, const QString &title,
                                    const QPoint &pos, const QSize &, long)
    : DPIFrame(parent)
    , event_parent(parent)
{
    setWindowTitle(title.isEmpty() ? _L("Print Error") : title);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white; border: 1px solid #e00;");

    btn_bg_white = StateColor(
        std::pair<QColor,int>(QColor(220,220,220), StateColor::Pressed),
        std::pair<QColor,int>(QColor(240,240,240), StateColor::Hovered),
        std::pair<QColor,int>(QColor(255,255,255), StateColor::Normal));

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 16, 16, 12);
    outer->setSpacing(8);
    m_sizer_main = outer;

    // Error image
    m_error_prompt_pic_static = new QLabel(this);
    m_error_prompt_pic_static->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_error_prompt_pic_static);

    // Error code
    m_staticText_error_code = new Label(this, Label::Body_12);
    m_staticText_error_code->setStyleSheet("color: #E00;");
    m_staticText_error_code->setWordWrap(true);
    outer->addWidget(m_staticText_error_code);

    // Error text (scroll area)
    m_vebview_release_note = new QScrollArea(this);
    m_vebview_release_note->setWidgetResizable(true);
    m_vebview_release_note->setFrameShape(QFrame::NoFrame);
    auto *inner = new QWidget;
    auto *inner_vbox = new QVBoxLayout(inner);
    m_staticText_release_note = new Label(inner, Label::Body_14);
    m_staticText_release_note->setWordWrap(true);
    inner_vbox->addWidget(m_staticText_release_note);
    m_vebview_release_note->setWidget(inner);
    outer->addWidget(m_vebview_release_note, 1);

    // Button row
    m_sizer_button = new QHBoxLayout;
    m_sizer_button->addStretch(1);
    outer->addLayout(m_sizer_button);

    if (!pos.isNull()) move(pos);
    adjustSize();
}

PrintErrorDialog::~PrintErrorDialog() = default;

void PrintErrorDialog::update_text_image(const QString &text, const QString &error_code,
                                          const QString &/*image_url*/)
{
    if (m_staticText_release_note) m_staticText_release_note->setText(text);
    if (m_staticText_error_code)   m_staticText_error_code->setText(error_code);
}

void PrintErrorDialog::on_show() { show(); }
void PrintErrorDialog::on_hide() { hide(); }

void PrintErrorDialog::update_title_style(QString title, std::vector<int> /*style*/,
                                           QWidget * /*parent*/)
{
    setWindowTitle(title);
}

void PrintErrorDialog::post_event(QEvent &)  {}
void PrintErrorDialog::post_event(QEvent &&) {}
void PrintErrorDialog::rescale() {}
void PrintErrorDialog::on_dpi_changed(const QRect &) {}
void PrintErrorDialog::msw_rescale() {}

void PrintErrorDialog::init_button(PrintErrorButton style, QString button_text)
{
    if (m_used_button.end() != std::find(m_used_button.begin(), m_used_button.end(), (int)style))
        return;
    m_used_button.push_back((int)style);

    auto *btn = new Button(this, button_text);
    m_button_list[(int)style] = btn;
    m_sizer_button->addWidget(btn);
}

void PrintErrorDialog::init_button_list() {}
void PrintErrorDialog::on_webrequest_state(QEvent &) {}

// ============================================================================
// ConfirmBeforeSendDialog
// ============================================================================

ConfirmBeforeSendDialog::ConfirmBeforeSendDialog(QWidget *parent, int,
                                                  const QString &title,
                                                  ButtonStyle btn_style,
                                                  const QPoint &pos, const QSize &,
                                                  long, bool show_check)
    : DPIDialog(parent)
    , not_show_again(show_check)
{
    setWindowTitle(title.isEmpty() ? _L("Confirm") : title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(420, 300);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 16, 16, 12);
    outer->setSpacing(10);
    m_sizer_main = outer;

    // Scrollable message
    m_vebview_release_note = new QScrollArea(this);
    m_vebview_release_note->setWidgetResizable(true);
    m_vebview_release_note->setFrameShape(QFrame::NoFrame);
    auto *inner = new QWidget;
    auto *inner_vbox = new QVBoxLayout(inner);
    m_staticText_release_note = new Label(inner, Label::Body_14);
    m_staticText_release_note->setWordWrap(true);
    inner_vbox->addWidget(m_staticText_release_note);
    inner_vbox->addStretch(1);
    m_vebview_release_note->setWidget(inner);
    outer->addWidget(m_vebview_release_note, 1);

    if (not_show_again) {
        m_show_again_checkbox = new QCheckBox(_L("Don't show again"), this);
        outer->addWidget(m_show_again_checkbox);
    } else {
        m_show_again_checkbox = nullptr;
    }

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_button_update_nozzle = new Button(this, _L("Update nozzle"));
    m_button_update_nozzle->hide();
    btn_row->addWidget(m_button_update_nozzle);

    if (btn_style == CONFIRM_AND_CANCEL) {
        m_button_cancel = new Button(this, _L("Cancel"));
        connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
        btn_row->addWidget(m_button_cancel);
    } else {
        m_button_cancel = nullptr;
    }

    m_button_ok = new Button(this, _L("Confirm"));
    m_button_ok->SetValue(true);
    connect(m_button_ok, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_button_ok);

    outer->addLayout(btn_row);
    if (!pos.isNull()) move(pos);
    adjustSize();
}

ConfirmBeforeSendDialog::~ConfirmBeforeSendDialog() = default;

void ConfirmBeforeSendDialog::update_text(QString text)
{
    if (m_staticText_release_note)
        m_staticText_release_note->setText(text);
}

void ConfirmBeforeSendDialog::update_text(std::vector<ConfirmBeforeSendInfo> texts,
                                           bool enable_warning_clr)
{
    QString combined;
    for (const auto &info : texts) {
        if (enable_warning_clr && info.level == ConfirmBeforeSendInfo::Warning)
            combined += "<span style='color:#E08000;'>" + info.text + "</span><br>";
        else
            combined += info.text + "<br>";
    }
    if (m_staticText_release_note)
        m_staticText_release_note->setText(combined.trimmed());
}

void ConfirmBeforeSendDialog::on_show() { show(); }
void ConfirmBeforeSendDialog::on_hide() { hide(); }

void ConfirmBeforeSendDialog::update_btn_label(QString ok_text, QString cancel_text)
{
    if (m_button_ok)     m_button_ok->setText(ok_text);
    if (m_button_cancel) m_button_cancel->setText(cancel_text);
}

void ConfirmBeforeSendDialog::rescale() {}
void ConfirmBeforeSendDialog::on_dpi_changed(const QRect &) {}

void ConfirmBeforeSendDialog::show_update_nozzle_button(bool show)
{
    if (m_button_update_nozzle) m_button_update_nozzle->setVisible(show);
}

void ConfirmBeforeSendDialog::hide_button_ok()
{
    if (m_button_ok) m_button_ok->hide();
}

void ConfirmBeforeSendDialog::edit_cancel_button_txt(const QString &txt, bool switch_green)
{
    if (m_button_cancel) {
        m_button_cancel->setText(txt);
        if (switch_green) m_button_cancel->SetValue(true);
    }
}

void ConfirmBeforeSendDialog::disable_button_ok()
{
    if (m_button_ok) m_button_ok->setEnabled(false);
}

void ConfirmBeforeSendDialog::enable_button_ok()
{
    if (m_button_ok) m_button_ok->setEnabled(true);
}

QString ConfirmBeforeSendDialog::format_text(QString str, int /*warp*/)
{
    return str;
}

// ============================================================================
// InputIpAddressDialog
// ============================================================================

InputIpAddressDialog::InputIpAddressDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Add printer"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(480, 400);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    // Steps
    auto *steps_row = new QHBoxLayout;
    m_img_step1 = new QLabel("1", this);
    m_img_step2 = new QLabel("2", this);
    m_img_step3 = new QLabel("3", this);
    m_step_icon_panel3 = new QWidget(this);
    for (auto *s : {m_img_step1, m_img_step2, m_img_step3}) {
        s->setAlignment(Qt::AlignCenter);
        s->setFixedSize(24, 24);
        s->setStyleSheet("background-color: #00AF42; color: white; border-radius: 12px;");
        steps_row->addWidget(s);
    }
    steps_row->addStretch(1);
    vbox->addLayout(steps_row);

    // Tips
    m_tip0 = new Label(this, Label::Body_12);
    m_tip0->setText(_L("Enter the IP address of your Bambu Lab printer."));
    m_tip0->setWordWrap(true);
    vbox->addWidget(m_tip0);

    m_tip1 = new Label(this, Label::Body_12);
    m_tip2 = new Label(this, Label::Body_12);
    m_tip3 = new Label(this, Label::Body_12);
    m_tip4 = new Label(this, Label::Body_12);
    vbox->addWidget(m_tip1);
    vbox->addWidget(m_tip2);
    vbox->addWidget(m_tip3);
    vbox->addWidget(m_tip4);

    // Input fields
    m_tips_ip = new Label(this, Label::Body_12);
    m_tips_ip->setText(_L("IP Address"));
    vbox->addWidget(m_tips_ip);

    m_input_ip = new TextInput(this);
    vbox->addWidget(m_input_ip);

    m_tips_access_code = new Label(this, Label::Body_12);
    m_tips_access_code->setText(_L("Access Code"));
    vbox->addWidget(m_tips_access_code);

    m_input_access_code = new TextInput(this);
    vbox->addWidget(m_input_access_code);

    // Test result messages
    m_test_right_msg = new Label(this, Label::Body_12);
    m_test_right_msg->setStyleSheet("color: #00AF42;");
    m_test_right_msg->hide();
    vbox->addWidget(m_test_right_msg);

    m_test_wrong_msg = new Label(this, Label::Body_12);
    m_test_wrong_msg->setStyleSheet("color: #E00;");
    m_test_wrong_msg->hide();
    vbox->addWidget(m_test_wrong_msg);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    m_button_ok = new Button(this, _L("Confirm"));
    m_button_ok->SetValue(true);
    connect(m_button_ok, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_button_ok);
    vbox->addLayout(btn_row);

    adjustSize();
}

InputIpAddressDialog::~InputIpAddressDialog() = default;

void InputIpAddressDialog::switch_input_panel(int /*index*/) {}
void InputIpAddressDialog::on_cancel() { reject(); }
void InputIpAddressDialog::update_title(QString title) { setWindowTitle(title); }
void InputIpAddressDialog::set_machine_obj(MachineObject *obj) { m_obj = obj; }
void InputIpAddressDialog::update_test_msg(QString msg, bool connected)
{
    if (connected) {
        if (m_test_right_msg) { m_test_right_msg->setText(msg); m_test_right_msg->show(); }
        if (m_test_wrong_msg) m_test_wrong_msg->hide();
    } else {
        if (m_test_wrong_msg) { m_test_wrong_msg->setText(msg); m_test_wrong_msg->show(); }
        if (m_test_right_msg) m_test_right_msg->hide();
    }
}
bool InputIpAddressDialog::isIp(std::string ipstr)
{
    // Very basic IP check
    int dots = 0;
    for (char c : ipstr) {
        if (c == '.') ++dots;
        else if (!std::isdigit(c)) return false;
    }
    return dots == 3;
}
void InputIpAddressDialog::check_ip_address_failed(int /*result*/) {}
void InputIpAddressDialog::on_check_ip_address_failed(QEvent &) {}
void InputIpAddressDialog::on_ok(QMouseEvent &) { accept(); }
void InputIpAddressDialog::on_send_retry() {}
void InputIpAddressDialog::update_test_msg_event(QEvent &) {}
void InputIpAddressDialog::post_update_test_msg(std::weak_ptr<InputIpAddressDialog>,
                                                  QString, bool) {}
void InputIpAddressDialog::workerThreadFunc(std::string, std::string,
                                             std::string, std::string) {}
void InputIpAddressDialog::OnTimer(QTimerEvent &) {}
void InputIpAddressDialog::on_text(QEvent &) {}
void InputIpAddressDialog::on_dpi_changed(const QRect &) {}

// ============================================================================
// SendFailedConfirm
// ============================================================================

SendFailedConfirm::SendFailedConfirm(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Send failed"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Failed to send the print job to the printer.\nPlease check the connection and try again."));
    lbl->setWordWrap(true);
    vbox->addWidget(lbl);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *ok_btn = new Button(this, _L("OK"));
    ok_btn->SetValue(true);
    connect(ok_btn, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(ok_btn);
    vbox->addLayout(btn_row);

    adjustSize();
}

void SendFailedConfirm::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI

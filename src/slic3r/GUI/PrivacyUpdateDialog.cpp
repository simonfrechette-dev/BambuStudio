#include "PrivacyUpdateDialog.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QTextBrowser>

namespace Slic3r { namespace GUI {

PrivacyUpdateDialog::PrivacyUpdateDialog(QWidget *parent, int, const QString &title,
                                          ButtonStyle btn_style,
                                          const QPoint &pos, const QSize &, long)
    : DPIDialog(parent)
{
    setWindowTitle(title.isEmpty() ? _L("Privacy Policy") : title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(560, 480);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 16, 16, 12);
    outer->setSpacing(10);
    m_sizer_main = outer;

    m_staticText_release_note = new Label(this, Label::Body_14);
    m_staticText_release_note->setWordWrap(true);
    outer->addWidget(m_staticText_release_note);

    // Scrollable webview/text area
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *text_inner = new QTextBrowser;
    text_inner->setOpenExternalLinks(true);
    scroll->setWidget(text_inner);
    m_vebview_release_note = scroll;
    outer->addWidget(scroll, 1);

    // Buttons
    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

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

PrivacyUpdateDialog::~PrivacyUpdateDialog() = default;

QWidget *PrivacyUpdateDialog::CreateTipView(QWidget *) { return nullptr; }
void PrivacyUpdateDialog::OnNavigating(QEvent &) {}

bool PrivacyUpdateDialog::ShowReleaseNote(std::string content)
{
    if (auto *scroll = qobject_cast<QScrollArea *>(m_vebview_release_note)) {
        if (auto *tb = qobject_cast<QTextBrowser *>(scroll->widget()))
            tb->setPlainText(QString::fromStdString(content));
    }
    return true;
}

void PrivacyUpdateDialog::RunScript(std::string) {}

void PrivacyUpdateDialog::on_show() { show(); }
void PrivacyUpdateDialog::on_hide() { hide(); }

void PrivacyUpdateDialog::update_btn_label(QString ok_text, QString cancel_text)
{
    if (m_button_ok)     m_button_ok->setText(ok_text);
    if (m_button_cancel) m_button_cancel->setText(cancel_text);
}

void PrivacyUpdateDialog::rescale() {}
void PrivacyUpdateDialog::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI

// Qt6 port of MsgDialog.cpp
#include "MsgDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QDesktopServices>
#include <QUrl>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTextEdit>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// MsgDialog — base
// ---------------------------------------------------------------------------

MsgDialog::MsgDialog(QWidget *parent,
                     const QString &title,
                     const QString &headline,
                     long           style,
                     const QPixmap &bitmap,
                     const QString &forward_str)
    : DPIDialog(parent)
    , m_forward_str(forward_str)
{
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    boldfont = font();
    boldfont.setBold(true);

    // Outer vertical layout
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(BORDER, BORDER, BORDER, BORDER);
    outer->setSpacing(VERT_SPACING);

    // Top row: logo + content area
    auto *top_row = new QHBoxLayout;
    top_row->setSpacing(LOGO_GAP);
    outer->addLayout(top_row);

    // Logo label (left) — use actual BambuStudio SVG icons
    logo = new QLabel(this);
    logo->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    logo->setFixedSize(LOGO_SPACING, LOGO_SPACING);
    if (!bitmap.isNull()) {
        logo->setPixmap(bitmap.scaled(LOGO_SPACING, LOGO_SPACING, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        std::string icon_name = "info";
        if (style & MsgICON_ERROR)
            icon_name = "error";
        else if (style & MsgICON_WARN)
            icon_name = "dev_warning";
        else if (style & MsgICON_QUESTION)
            icon_name = "question";
        ScalableBitmap sb(this, icon_name, LOGO_SPACING);
        if (!sb.bmp().isNull())
            logo->setPixmap(sb.bmp());
    }
    top_row->addWidget(logo, 0, Qt::AlignTop);

    // Right-side content column
    auto *right_col = new QVBoxLayout;
    right_col->setSpacing(VERT_SPACING / 2);
    top_row->addLayout(right_col, 1);

    // Headline
    if (!headline.isEmpty()) {
        auto *hl = new QLabel(headline, this);
        hl->setFont(boldfont);
        hl->setWordWrap(true);
        right_col->addWidget(hl);
    }

    // content_sizer: subclasses add widgets here
    content_sizer = right_col;

    // Horizontal separator
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    outer->addWidget(sep);

    // DSA sizer row (hidden initially)
    m_dsa_sizer = new QHBoxLayout;
    outer->addLayout(m_dsa_sizer);

    // Button row
    btn_sizer = new QHBoxLayout;
    btn_sizer->addStretch(1);
    btn_sizer->setSpacing(BTN_SPACING);
    outer->addLayout(btn_sizer);

    apply_style(style);
}

MsgDialog::~MsgDialog() = default;

void MsgDialog::on_dpi_changed(const QRect &) {}

Button *MsgDialog::add_button(int btn_id, bool set_focus, const QString &label)
{
    QString lbl = label;
    if (lbl.isEmpty()) {
        switch (btn_id) {
        case MsgID_OK:     lbl = _L("OK");     break;
        case MsgID_CANCEL: lbl = _L("Cancel"); break;
        case MsgID_YES:    lbl = _L("Yes");    break;
        case MsgID_NO:     lbl = _L("No");     break;
        case MsgID_APPLY:  lbl = _L("Apply");  break;
        case MsgID_CLOSE:  lbl = _L("Close");  break;
        case MsgID_HELP:   lbl = _L("Help");   break;
        default:           lbl = _L("OK");     break;
        }
    }

    auto *btn = new Button(this, "");
    btn->setText(lbl);

    // Primary buttons (OK/Yes/Apply) get green accent style; secondary stay white
    bool is_primary = (btn_id == MsgID_OK || btn_id == MsgID_YES || btn_id == MsgID_APPLY);
    btn->setMinimumSize(QSize(58, 24));
    if (is_primary)
        btn->SetValue(true);

    // Wire accept / reject
    connect(btn, &Button::clicked, this, [this, btn_id]() {
        switch (btn_id) {
        case MsgID_OK:
        case MsgID_YES:
        case MsgID_APPLY:
        case MsgID_CLOSE:
            done(btn_id);
            break;
        case MsgID_CANCEL:
        case MsgID_NO:
        default:
            done(btn_id);
            break;
        }
    });

    btn_sizer->addWidget(btn);
    if (set_focus)
        btn->setFocus();

    auto *mb = new MsgButton;
    mb->id = QString::number(btn_id);
    auto *bd = new ButtonData;
    bd->button = btn;
    bd->type = ButtonSizeNormal;
    mb->buttondata = bd;
    m_buttons.insert(mb->id, mb);

    return btn;
}

Button *MsgDialog::get_button(int btn_id)
{
    QString key = QString::number(btn_id);
    auto it = m_buttons.find(key);
    if (it != m_buttons.end())
        return it.value()->buttondata->button;
    return nullptr;
}

void MsgDialog::apply_style(long style)
{
    if (style & MsgYES_NO) {
        add_button(MsgID_NO,  false);
        add_button(MsgID_YES, true);
    } else if (style & MsgOK) {
        add_button(MsgID_OK, true);
        if (style & MsgCANCEL)
            add_button(MsgID_CANCEL, false);
    } else if (style & MsgCANCEL) {
        add_button(MsgID_CANCEL, false);
    }
}

void MsgDialog::finalize()
{
    setMinimumWidth(CONTENT_WIDTH + LOGO_SPACING + LOGO_GAP + 2 * BORDER);
    adjustSize();
}

void MsgDialog::show_dsa_button(const QString &title)
{
    if (!m_dsa_sizer)
        return;
    m_checkbox_dsa = new CheckBox(this);
    m_checkbox_dsa->setChecked(false);
    auto *lbl = new QLabel(title.isEmpty() ? _L("Don't show again") : title, this);
    m_dsa_sizer->addWidget(m_checkbox_dsa);
    m_dsa_sizer->addWidget(lbl, 1);
}

bool MsgDialog::get_checkbox_state()
{
    return m_checkbox_dsa ? m_checkbox_dsa->isChecked() : false;
}

void MsgDialog::SetButtonLabel(int btn_id, const QString &label, bool set_focus)
{
    if (Button *btn = get_button(btn_id)) {
        btn->setText(label);
        if (set_focus)
            btn->setFocus();
    }
}

// ---------------------------------------------------------------------------
// ErrorDialog
// ---------------------------------------------------------------------------

ErrorDialog::ErrorDialog(QWidget *parent, const QString &temp_msg, bool courier_font)
    : MsgDialog(parent, _L("Error"), _L("Error"), MsgOK | MsgICON_ERROR)
    , msg(temp_msg)
{
    auto *label = new QLabel(msg, this);
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    if (courier_font) {
        QFont f("Courier New", label->font().pointSize());
        f.setFixedPitch(true);
        label->setFont(f);
    }
    content_sizer->addWidget(label);
    finalize();
}

// ---------------------------------------------------------------------------
// WarningDialog
// ---------------------------------------------------------------------------

WarningDialog::WarningDialog(QWidget *parent,
                             const QString &message,
                             const QString &caption,
                             long           style)
    : MsgDialog(parent,
                caption.isEmpty() ? _L("Warning") : caption,
                caption.isEmpty() ? _L("Warning") : caption,
                (style == 0 ? MsgOK : style) | MsgICON_WARN)
{
    auto *label = new QLabel(message, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);
    finalize();
}

// ---------------------------------------------------------------------------
// PostProcessScriptDialog
// ---------------------------------------------------------------------------

PostProcessScriptDialog::PostProcessScriptDialog(QWidget *parent,
                                                 const QString &message,
                                                 const QString &script_content)
    : MsgDialog(parent, _L("Post-processing Script"), _L("Post-processing Script"), MsgOK | MsgCANCEL | MsgICON_WARN)
{
    auto *label = new QLabel(message, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);

    m_toggle_details = add_button(MsgID_HELP, false, _L("Show script"));
    m_script_text = new QLineEdit(script_content, this);
    m_script_text->setReadOnly(true);
    m_script_text->hide();
    content_sizer->addWidget(m_script_text);

    connect(m_toggle_details, &Button::clicked, this, [this]() {
        m_details_expanded = !m_details_expanded;
        m_script_text->setVisible(m_details_expanded);
        m_toggle_details->setText(m_details_expanded ? _L("Hide script") : _L("Show script"));
        adjustSize();
    });
    finalize();
}

// ---------------------------------------------------------------------------
// MessageDialog
// ---------------------------------------------------------------------------

MessageDialog::MessageDialog(QWidget *parent,
                             const QString &message,
                             const QString &caption,
                             long           style,
                             const QString &forward_str,
                             const QString &link_text,
                             std::function<void(const QString &)> link_callback)
    : MsgDialog(parent,
                caption.isEmpty() ? _L("Info") : caption,
                caption.isEmpty() ? _L("Info") : caption,
                style == 0 ? MsgOK : style,
                QPixmap(),
                forward_str)
{
    auto *label = new QLabel(message, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);

    if (!link_text.isEmpty()) {
        auto *link = new QLabel(QString("<a href='link'>%1</a>").arg(link_text), this);
        link->setOpenExternalLinks(false);
        if (link_callback) {
            connect(link, &QLabel::linkActivated, this, [link_callback, link_text](const QString &) {
                link_callback(link_text);
            });
        }
        content_sizer->addWidget(link);
    }
    finalize();
}

MessageDialog::MessageDialog(QWidget *parent,
                             const QString &message,
                             const QString &caption,
                             long           style,
                             const QString &forward_str,
                             const QString &link_text,
                             std::function<void(const QString &)> link_callback,
                             bool           /*is_marked_msg*/)
    : MessageDialog(parent, message, caption, style, forward_str, link_text, link_callback)
{}

// ---------------------------------------------------------------------------
// RichMessageDialog
// ---------------------------------------------------------------------------

RichMessageDialog::RichMessageDialog(QWidget *parent,
                                     const QString &message,
                                     const QString &caption,
                                     long           style)
    : MsgDialog(parent,
                caption.isEmpty() ? _L("Info") : caption,
                caption.isEmpty() ? _L("Info") : caption,
                style == 0 ? MsgOK : style)
{
    auto *label = new QLabel(message, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);
    finalize();
}

int RichMessageDialog::exec()
{
    // If checkbox was configured, add it now before showing
    if (!m_checkBoxText.isEmpty() && !m_checkBox) {
        m_checkBox = new QCheckBox(m_checkBoxText, this);
        m_checkBox->setChecked(m_checkBoxValue);
        layout()->addWidget(m_checkBox);
    }
    int r = MsgDialog::exec();
    if (m_checkBox)
        m_checkBoxValue = m_checkBox->isChecked();
    return r;
}

bool RichMessageDialog::IsCheckBoxChecked() const
{
    return m_checkBox ? m_checkBox->isChecked() : m_checkBoxValue;
}

// ---------------------------------------------------------------------------
// InfoDialog
// ---------------------------------------------------------------------------

InfoDialog::InfoDialog(QWidget *parent,
                       const QString &title,
                       const QString &msg_text,
                       bool           /*is_marked*/,
                       long           style)
    : MsgDialog(parent,
                title,
                title,
                (style == 0 ? MsgOK : style) | MsgICON_INFO)
    , msg(msg_text)
{
    auto *label = new QLabel(msg, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);
    finalize();
}

// ---------------------------------------------------------------------------
// DownloadDialog
// ---------------------------------------------------------------------------

DownloadDialog::DownloadDialog(QWidget *parent,
                               const QString &title,
                               const QString &msg_text,
                               bool           /*is_marked*/,
                               long           style)
    : MsgDialog(parent,
                title,
                title,
                (style == 0 ? MsgOK : style) | MsgICON_INFO)
    , msg(msg_text)
{
    auto *label = new QLabel(msg, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);
    finalize();
}

void DownloadDialog::SetExtendedMessage(const QString &extendedMessage)
{
    auto *label = new QLabel(extendedMessage, this);
    label->setWordWrap(true);
    content_sizer->addWidget(label);
    adjustSize();
}

// ---------------------------------------------------------------------------
// DeleteConfirmDialog
// ---------------------------------------------------------------------------

DeleteConfirmDialog::DeleteConfirmDialog(QWidget *parent, const QString &title, const QString &msg_text)
    : DPIDialog(parent)
    , msg(msg_text)
{
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 20, 20, 20);
    outer->setSpacing(15);

    m_msg_text = new QLabel(msg, this);
    m_msg_text->setWordWrap(true);
    outer->addWidget(m_msg_text);

    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    outer->addWidget(sep);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_cancel_btn = new Button(this, "");
    m_cancel_btn->setText(_L("Cancel"));
    connect(m_cancel_btn, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_cancel_btn);

    m_del_btn = new Button(this, "");
    m_del_btn->setText(_L("Delete"));
    connect(m_del_btn, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_del_btn);

    outer->addLayout(btn_row);
    adjustSize();
}

DeleteConfirmDialog::~DeleteConfirmDialog() = default;

void DeleteConfirmDialog::on_dpi_changed(const QRect &) {}

// ---------------------------------------------------------------------------
// Newer3mfVersionDialog
// ---------------------------------------------------------------------------

Newer3mfVersionDialog::Newer3mfVersionDialog(QWidget *parent,
                                             const Semver *file_version,
                                             const Semver *cloud_version,
                                             const QString &new_keys)
    : DPIDialog(parent)
    , m_file_version(file_version)
    , m_cloud_version(cloud_version)
    , m_new_keys(new_keys)
{
    setWindowTitle(_L("Newer 3MF version"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *outer = new QVBoxLayout(this);
    outer->addLayout(get_msg_sizer());
    outer->addLayout(get_btn_sizer());
    adjustSize();
}

QBoxLayout *Newer3mfVersionDialog::get_msg_sizer()
{
    auto *vbox = new QVBoxLayout;
    QString txt;
    if (m_file_version && m_cloud_version) {
        txt = _L("The 3MF file was created by a newer version of BambuStudio. "
                 "Some features may not be available.");
    } else {
        txt = _L("The 3MF file format version is newer than the current version of BambuStudio supports.");
    }
    if (!m_new_keys.isEmpty()) {
        txt += "\n\n" + _L("New features:") + "\n" + m_new_keys;
    }
    m_msg_text = new QLabel(txt, this);
    m_msg_text->setWordWrap(true);
    vbox->addWidget(m_msg_text);
    return vbox;
}

QBoxLayout *Newer3mfVersionDialog::get_btn_sizer()
{
    auto *hbox = new QHBoxLayout;
    hbox->addStretch(1);

    m_later_btn = new Button(this, "");
    m_later_btn->setText(_L("Later"));
    connect(m_later_btn, &Button::clicked, this, &QDialog::reject);
    hbox->addWidget(m_later_btn);

    m_update_btn = new Button(this, "");
    m_update_btn->setText(_L("Update"));
    connect(m_update_btn, &Button::clicked, this, &QDialog::accept);
    hbox->addWidget(m_update_btn);

    return hbox;
}

// ---------------------------------------------------------------------------
// NetworkErrorDialog
// ---------------------------------------------------------------------------

NetworkErrorDialog::NetworkErrorDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Network Error"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 20, 20, 20);
    outer->setSpacing(10);

    m_text_basic = new QLabel(_L("A network error occurred. Please check your internet connection."), this);
    m_text_basic->setWordWrap(true);
    outer->addWidget(m_text_basic);

    m_text_proposal = new QLabel(_L("Please try again later."), this);
    m_text_proposal->setWordWrap(true);
    outer->addWidget(m_text_proposal);

    m_link_server_state = new QLabel(
        QString("<a href='https://status.bambulab.com'>%1</a>").arg(_L("Check server status")), this);
    m_link_server_state->setOpenExternalLinks(true);
    outer->addWidget(m_link_server_state);

    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    outer->addWidget(sep);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    auto *chk = new CheckBox(this);
    chk->setChecked(false);
    auto *chk_lbl = new QLabel(_L("Don't show again"), this);
    btn_row->addWidget(chk);
    btn_row->addWidget(chk_lbl);
    btn_row->addStretch(1);

    m_button_confirm = new Button(this, "");
    m_button_confirm->setText(_L("OK"));
    connect(m_button_confirm, &Button::clicked, this, [this, chk]() {
        m_show_again = !chk->isChecked();
        accept();
    });
    btn_row->addWidget(m_button_confirm);

    outer->addLayout(btn_row);
    adjustSize();
}

// ---------------------------------------------------------------------------
// FilamentWarningDialog
// ---------------------------------------------------------------------------

FilamentWarningDialog::FilamentWarningDialog(QWidget *parent,
                                             const QString &title,
                                             std::vector<FilamentWarningInfo> infos)
    : MsgDialog(parent, title, title, MsgOK | MsgICON_WARN)
    , m_messages(std::move(infos))
{
    BuildContent();
    finalize();
}

void FilamentWarningDialog::BuildContent()
{
    for (const auto &info : m_messages) {
        if (!info.info_msg.isEmpty()) {
            auto *label = new QLabel(info.info_msg, this);
            label->setWordWrap(true);
            content_sizer->addWidget(label);
        }
        if (!info.wiki_url.isEmpty()) {
            auto *link = new QLabel(
                QString("<a href='%1'>%2</a>").arg(info.wiki_url).arg(_L("Learn more")), this);
            link->setOpenExternalLinks(true);
            content_sizer->addWidget(link);
        }
    }
}

}} // namespace Slic3r::GUI

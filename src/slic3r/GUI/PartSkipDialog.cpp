// Phantom forward-declaration must precede ALL headers.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "PartSkipDialog.hpp"
#include "I18N.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

// ============================================================================
// PartSkipConfirmDialog
// ============================================================================

PartSkipConfirmDialog::PartSkipConfirmDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Skip parts confirmation"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    m_msg_label = new Label(this, Label::Head_16);
    m_msg_label->setWordWrap(true);
    vbox->addWidget(m_msg_label);

    m_tip_label = new Label(this, Label::Body_12);
    m_tip_label->setWordWrap(true);
    vbox->addWidget(m_tip_label);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *cancel = new Button(this, _L("Cancel"));
    connect(cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(cancel);
    m_apply_button = new Button(this, _L("Confirm"));
    m_apply_button->SetValue(true);
    connect(m_apply_button, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_apply_button);
    vbox->addLayout(btn_row);

    adjustSize();
}

PartSkipConfirmDialog::~PartSkipConfirmDialog() = default;
void PartSkipConfirmDialog::on_dpi_changed(const QRect &) {}
Button *PartSkipConfirmDialog::GetConfirmButton() { return m_apply_button; }
void PartSkipConfirmDialog::SetMsgLabel(QString msg) { if (m_msg_label) m_msg_label->setText(msg); }
void PartSkipConfirmDialog::SetTipLabel(QString msg) { if (m_tip_label) m_tip_label->setText(msg); }
bool PartSkipConfirmDialog::Show(bool show)
{
    setVisible(show);
    return true;
}

// ============================================================================
// PartSkipDialog
// ============================================================================

PartSkipDialog::PartSkipDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Skip parts"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(520, 480);

    m_sizer = new QVBoxLayout(this);
    m_sizer->setContentsMargins(0, 0, 0, 0);

    m_simplebook = new QStackedWidget(this);
    m_sizer->addWidget(m_simplebook, 1);

    // Page 1: loading state
    m_book_first_panel = new QWidget;
    m_book_first_sizer = new QVBoxLayout(m_book_first_panel);
    m_loading_label = new Label(m_book_first_panel, Label::Body_14);
    m_loading_label->setText(_L("Loading..."));
    m_loading_label->setAlignment(Qt::AlignCenter);
    m_book_first_sizer->addStretch(1);
    m_book_first_sizer->addWidget(m_loading_label, 0, Qt::AlignCenter);
    m_book_first_sizer->addStretch(1);
    m_simplebook->addWidget(m_book_first_panel);

    // Page 2: retry state
    m_book_second_panel = new QWidget;
    m_book_second_sizer = new QVBoxLayout(m_book_second_panel);
    m_retry_label = new Label(m_book_second_panel, Label::Body_14);
    m_retry_label->setText(_L("Failed to load. Please retry."));
    m_retry_label->setAlignment(Qt::AlignCenter);
    m_book_second_sizer->addStretch(1);
    m_book_second_sizer->addWidget(m_retry_label, 0, Qt::AlignCenter);
    m_second_retry_btn = new Button(m_book_second_panel, _L("Retry"));
    m_second_retry_btn->SetValue(true);
    m_book_second_sizer->addWidget(m_second_retry_btn, 0, Qt::AlignCenter);
    m_book_second_sizer->addStretch(1);
    m_simplebook->addWidget(m_book_second_panel);

    // Page 3: main canvas/list
    m_book_third_panel = new QWidget;
    m_dlg_sizer = new QVBoxLayout(m_book_third_panel);

    m_dlg_content_sizer = new QVBoxLayout;
    m_dlg_sizer->addLayout(m_dlg_content_sizer, 1);

    m_dlg_placeholder = new QWidget(m_book_third_panel);
    m_dlg_content_sizer->addWidget(m_dlg_placeholder, 1);

    m_dlg_btn_sizer = new QHBoxLayout;
    auto *close_btn = new Button(m_book_third_panel, _L("Close"));
    connect(close_btn, &Button::clicked, this, &QDialog::reject);
    m_apply_btn = new Button(m_book_third_panel, _L("Apply"));
    m_apply_btn->SetValue(true);
    connect(m_apply_btn, &Button::clicked, this, &QDialog::accept);
    m_dlg_btn_sizer->addStretch(1);
    m_dlg_btn_sizer->addWidget(close_btn);
    m_dlg_btn_sizer->addWidget(m_apply_btn);
    m_dlg_sizer->addLayout(m_dlg_btn_sizer);

    m_simplebook->addWidget(m_book_third_panel);
    m_simplebook->setCurrentIndex(0);

    adjustSize();
}

PartSkipDialog::~PartSkipDialog() = default;
void PartSkipDialog::on_dpi_changed(const QRect &) {}
bool PartSkipDialog::Show(bool show) { setVisible(show); return true; }
void PartSkipDialog::UpdatePartsStateFromPrinter(MachineObject *obj_) { m_obj = obj_; }
void PartSkipDialog::SetSimplebookPage(int page) { if (m_simplebook) m_simplebook->setCurrentIndex(page); }
void PartSkipDialog::InitSchedule(MachineObject *obj_) { m_obj = obj_; }
void PartSkipDialog::InitDialogUI() {}
int  PartSkipDialog::GetAllSkippedPartsNum() { return 0; }

}} // namespace Slic3r::GUI

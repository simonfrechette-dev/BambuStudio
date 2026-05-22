#include "BindDialog.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "Plater.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/StaticBox.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <QStackedWidget>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

// ============================================================================
// PingCodeBindDialog
// ============================================================================

PingCodeBindDialog::PingCodeBindDialog(Plater *plater)
    : DPIDialog(plater ? static_cast<QWidget *>(plater) : nullptr)
{
    setWindowTitle(_L("Bind via ping code"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(480, 400);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(24, 20, 24, 16);
    vbox->setSpacing(12);

    m_status_text = new Label(this, Label::Body_14);
    m_status_text->setAlignment(Qt::AlignCenter);
    vbox->addWidget(m_status_text);

    m_text_input_title = new QLabel(_L("Enter the 6-digit ping code:"), this);
    vbox->addWidget(m_text_input_title);

    auto *code_row = new QHBoxLayout;
    for (int i = 0; i < PING_CODE_LENGTH; ++i) {
        m_text_input_single_code[i] = new TextInput(this);
        m_text_input_single_code[i]->setFixedSize(48, 48);
        code_row->addWidget(m_text_input_single_code[i]);
    }
    vbox->addLayout(code_row);

    m_sw_bind_failed_info = new QScrollArea(this);
    m_sw_bind_failed_info->setWidgetResizable(true);
    m_sw_bind_failed_info->setMaximumHeight(120);
    m_sw_bind_failed_info->hide();
    m_bind_failed_info = new Label(this, Label::Body_12);
    m_bind_failed_info->setStyleSheet("color: red;");
    m_bind_failed_info->setWordWrap(true);
    m_sw_bind_failed_info->setWidget(m_bind_failed_info);
    vbox->addWidget(m_sw_bind_failed_info);

    m_link_show_ping_code_wiki = new QLabel(this);
    m_link_show_ping_code_wiki->setText(
        "<a href='" + m_ping_code_wiki + "'>" + _L("How to find ping code?") + "</a>");
    vbox->addWidget(m_link_show_ping_code_wiki);

    vbox->addStretch(1);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    m_button_cancel = new Button(this, _L("Cancel"));
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_cancel);
    m_button_bind = new Button(this, _L("Bind"));
    m_button_bind->SetValue(true);
    connect(m_button_bind, &Button::clicked, this, [this] {
        BOOST_LOG_TRIVIAL(info) << "PingCodeBindDialog: bind attempted (stub)";
        accept();
    });
    btn_row->addWidget(m_button_bind);
    vbox->addLayout(btn_row);

    m_button_close = new Button(this, _L("Close"));
    m_button_close->hide();
    connect(m_button_close, &Button::clicked, this, &QDialog::reject);

    adjustSize();
}

PingCodeBindDialog::~PingCodeBindDialog() = default;
void PingCodeBindDialog::on_key_input(QKeyEvent &) {}
void PingCodeBindDialog::on_text_changed(QEvent &) {}
void PingCodeBindDialog::on_key_backspace(QKeyEvent &) {}
void PingCodeBindDialog::on_cancel(QEvent &) { reject(); }
void PingCodeBindDialog::on_bind_printer(QEvent &) {}
void PingCodeBindDialog::on_dpi_changed(const QRect &) {}

// ============================================================================
// BindMachineDialog
// ============================================================================

BindMachineDialog::BindMachineDialog(Plater *plater)
    : DPIDialog(plater ? static_cast<QWidget *>(plater) : nullptr)
{
    setWindowTitle(_L("Bind machine"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(480, 360);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(24, 20, 24, 16);
    vbox->setSpacing(12);

    auto *top_row = new QHBoxLayout;
    m_panel_left = new StaticBox(this);
    m_panel_right = new StaticBox(this);
    top_row->addWidget(m_panel_left);
    top_row->addWidget(m_panel_right);
    vbox->addLayout(top_row);

    auto *left_vbox = new QVBoxLayout(m_panel_left);
    m_printer_img = new QLabel(m_panel_left);
    m_printer_img->setAlignment(Qt::AlignCenter);
    m_printer_name = new QLabel(m_panel_left);
    m_printer_name->setAlignment(Qt::AlignCenter);
    left_vbox->addWidget(m_printer_img);
    left_vbox->addWidget(m_printer_name);

    auto *right_vbox = new QVBoxLayout(m_panel_right);
    m_avatar = new QLabel(m_panel_right);
    m_avatar->setAlignment(Qt::AlignCenter);
    m_user_name = new QLabel(m_panel_right);
    m_user_name->setAlignment(Qt::AlignCenter);
    right_vbox->addWidget(m_avatar);
    right_vbox->addWidget(m_user_name);

    m_status_text = new QLabel(this);
    m_status_text->setAlignment(Qt::AlignCenter);
    vbox->addWidget(m_status_text);

    m_sw_bind_failed_info = new QScrollArea(this);
    m_sw_bind_failed_info->setWidgetResizable(true);
    m_sw_bind_failed_info->setMaximumHeight(120);
    m_sw_bind_failed_info->hide();
    m_bind_failed_info = new Label(this, Label::Body_12);
    m_bind_failed_info->setStyleSheet("color: red;");
    m_bind_failed_info->setWordWrap(true);
    m_sw_bind_failed_info->setWidget(m_bind_failed_info);
    vbox->addWidget(m_sw_bind_failed_info);

    vbox->addStretch(1);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    m_button_cancel = new Button(this, _L("Cancel"));
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_cancel);
    m_button_bind = new Button(this, _L("Bind"));
    m_button_bind->SetValue(true);
    connect(m_button_bind, &Button::clicked, this, [this] {
        BOOST_LOG_TRIVIAL(info) << "BindMachineDialog: bind attempted (stub)";
    });
    btn_row->addWidget(m_button_bind);
    vbox->addLayout(btn_row);

    adjustSize();
}

BindMachineDialog::~BindMachineDialog() = default;
void BindMachineDialog::show_bind_failed_info(bool show, int, QString desc, QString extra)
{
    if (m_sw_bind_failed_info) m_sw_bind_failed_info->setVisible(show);
    if (m_bind_failed_info && show)
        m_bind_failed_info->setText(desc + (extra.isEmpty() ? QString() : (" (" + extra + ")")));
}
void BindMachineDialog::on_cancel(QEvent &) { reject(); }
void BindMachineDialog::on_bind_fail(QEvent &) { show_bind_failed_info(true, -1); }
void BindMachineDialog::on_update_message(QEvent &) {}
void BindMachineDialog::on_bind_success(QEvent &) { accept(); }
void BindMachineDialog::on_bind_printer(QEvent &) {}
void BindMachineDialog::on_dpi_changed(const QRect &) {}
void BindMachineDialog::update_machine_info(MachineObject *info)
{
    m_machine_info = info;
    if (!info) return;
    if (m_printer_name) m_printer_name->setText(QString::fromStdString(info->get_dev_name()));
}
void BindMachineDialog::on_show(QShowEvent &) {}
void BindMachineDialog::on_close(QCloseEvent &ev) { QDialog::closeEvent(&ev); }
void BindMachineDialog::on_destroy() {}
QString BindMachineDialog::get_print_error(QString str) { return str; }

// ============================================================================
// UnBindMachineDialog
// ============================================================================

UnBindMachineDialog::UnBindMachineDialog(Plater *plater)
    : DPIDialog(plater ? static_cast<QWidget *>(plater) : nullptr)
{
    setWindowTitle(_L("Unbind machine"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(400, 280);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(24, 20, 24, 16);
    vbox->setSpacing(12);

    auto *info_row = new QHBoxLayout;
    m_printer_img = new QLabel(this);
    m_printer_img->setAlignment(Qt::AlignCenter);
    m_printer_img->setFixedSize(64, 64);
    info_row->addWidget(m_printer_img);
    auto *info_col = new QVBoxLayout;
    m_printer_name = new QLabel(this);
    m_user_name = new QLabel(this);
    info_col->addWidget(m_printer_name);
    info_col->addWidget(m_user_name);
    info_row->addLayout(info_col, 1);
    vbox->addLayout(info_row);

    m_status_text = new QLabel(this);
    m_status_text->setAlignment(Qt::AlignCenter);
    vbox->addWidget(m_status_text);

    m_avatar = new QLabel(this);
    vbox->addWidget(m_avatar, 0, Qt::AlignCenter);

    vbox->addStretch(1);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    m_button_cancel = new Button(this, _L("Cancel"));
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_cancel);
    m_button_unbind = new Button(this, _L("Unbind"));
    m_button_unbind->setStyleSheet("background-color: #D32F2F; color: white;");
    connect(m_button_unbind, &Button::clicked, this, [this] {
        BOOST_LOG_TRIVIAL(info) << "UnBindMachineDialog: unbind attempted (stub)";
        accept();
    });
    btn_row->addWidget(m_button_unbind);
    vbox->addLayout(btn_row);

    adjustSize();
}

UnBindMachineDialog::~UnBindMachineDialog() = default;
void UnBindMachineDialog::on_cancel(QEvent &) { reject(); }
void UnBindMachineDialog::on_unbind_printer(QEvent &) {}
void UnBindMachineDialog::on_dpi_changed(const QRect &) {}
void UnBindMachineDialog::on_show(QShowEvent &) {}

}} // namespace Slic3r::GUI

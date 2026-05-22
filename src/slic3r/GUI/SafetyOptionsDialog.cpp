// SafetyOptionsDialog.cpp — Qt6 port
// Phantom forward-declaration must precede ALL headers (see PrintOptionsDialog.cpp).
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "SafetyOptionsDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/SwitchButton.hpp"
#include "Widgets/StaticLine.hpp"

namespace Slic3r { namespace GUI {

SafetyOptionsDialog::SafetyOptionsDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Safety Options"));
    setMinimumWidth(460);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 20);
    main_layout->setSpacing(0);

    // Title
    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Safety Options"));
    main_layout->addWidget(title);
    main_layout->addSpacing(12);

    // Scrollable settings area
    m_scrollwindow = new QScrollArea(this);
    m_scrollwindow->setFrameShape(QFrame::NoFrame);
    m_scrollwindow->setWidgetResizable(true);

    auto *scroll_content = new QWidget(m_scrollwindow);
    auto *scroll_layout  = create_settings_group(scroll_content);
    scroll_content->setLayout(scroll_layout);
    m_scrollwindow->setWidget(scroll_content);

    main_layout->addWidget(m_scrollwindow, 1);
    main_layout->addSpacing(12);

    // Close button
    auto *btn_layout = new QHBoxLayout();
    btn_layout->addStretch(1);
    auto *btn_close = new Button(this, _L("Close"));
    btn_close->SetValue(true);
    btn_layout->addWidget(btn_close);
    main_layout->addLayout(btn_layout);

    connect(btn_close, &Button::clicked, this, &QDialog::accept);

    // Idle heating toast timer
    connect(&m_idel_heating_toast_timer, &QTimer::timeout, this, [this]() {
        if (m_idel_heating_toast) m_idel_heating_toast->hide();
    });
}

SafetyOptionsDialog::~SafetyOptionsDialog() {}

QBoxLayout *SafetyOptionsDialog::create_settings_group(QWidget *parent)
{
    auto *layout = new QVBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // Open door detection row
    auto *door_row = new QHBoxLayout();

    m_cb_open_door = new CheckBox(parent);
    door_row->addWidget(m_cb_open_door);

    auto *door_col = new QVBoxLayout();
    m_text_open_door = new Label(parent, Label::Body_14);
    m_text_open_door->setText(_L("Detect open door during printing"));
    m_text_open_door->setWordWrap(true);
    door_col->addWidget(m_text_open_door);

    // SwitchBoard for open door options (multi-option switch)
    m_open_door_switch_board = new SwitchBoard(parent);
    door_col->addWidget(m_open_door_switch_board);
    door_row->addLayout(door_col);
    layout->addLayout(door_row);

    // Separator
    auto *sep1 = new StaticLine(parent);
    layout->addWidget(sep1);

    // Idle heating protection row
    auto *heat_row = new QHBoxLayout();

    m_cb_idel_heating_protection = new CheckBox(parent);
    heat_row->addWidget(m_cb_idel_heating_protection);

    auto *heat_col = new QVBoxLayout();
    m_text_idel_heating_protection = new Label(parent, Label::Body_14);
    m_text_idel_heating_protection->setText(_L("Idle heating protection"));
    m_text_idel_heating_protection->setWordWrap(true);
    heat_col->addWidget(m_text_idel_heating_protection);

    m_text_idel_heating_protection_caption = new Label(parent, Label::Body_12);
    m_text_idel_heating_protection_caption->setText(
        _L("Automatically reduces nozzle temperature when the printer is idle."));
    m_text_idel_heating_protection_caption->setWordWrap(true);
    heat_col->addWidget(m_text_idel_heating_protection_caption);

    // Container for the idle heating controls (can be hidden)
    m_idel_heating_container = new QWidget(parent);
    auto *idel_layout = new QVBoxLayout(m_idel_heating_container);
    idel_layout->setContentsMargins(0, 0, 0, 0);
    // (additional controls go here if needed)
    heat_col->addWidget(m_idel_heating_container);

    heat_row->addLayout(heat_col);
    layout->addLayout(heat_row);

    // Toast for idle heating unavailable
    m_idel_heating_toast = new QWidget(parent);
    m_idel_heating_toast->setStyleSheet(
        "background: #333333; color: white; border-radius: 4px; padding: 6px;");
    auto *toast_layout = new QHBoxLayout(m_idel_heating_toast);
    auto *toast_label  = new QLabel(_L("Idle heating protection is not available on this printer."),
                                    m_idel_heating_toast);
    toast_label->setStyleSheet("color: white;");
    toast_layout->addWidget(toast_label);
    m_idel_heating_toast->hide();
    layout->addWidget(m_idel_heating_toast);

    layout->addStretch(1);
    return layout;
}

void SafetyOptionsDialog::update_options(MachineObject *obj_)
{
    if (!obj_) return;
    obj = obj_;
    updateOpenDoorCheck(obj_);
    updateIdelHeatingProtect(obj_);
}

void SafetyOptionsDialog::update_machine_obj(MachineObject *obj_)
{
    obj = obj_;
}

void SafetyOptionsDialog::setVisible(bool show)
{
    DPIDialog::setVisible(show);
}

void SafetyOptionsDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

void SafetyOptionsDialog::updateOpenDoorCheck(MachineObject * /*obj*/)
{
    // stub — to be implemented with actual MachineObject capability check
}

void SafetyOptionsDialog::updateIdelHeatingProtect(MachineObject * /*obj*/)
{
    // stub — to be implemented with actual MachineObject capability check
}

void SafetyOptionsDialog::show_idel_heating_toast(const QString &text)
{
    if (!m_idel_heating_toast) return;
    // Update toast text if present
    auto *lbl = m_idel_heating_toast->findChild<QLabel *>();
    if (lbl) lbl->setText(text);
    m_idel_heating_toast->show();
    m_idel_heating_toast_timer.start(3000);
}

}} // namespace Slic3r::GUI

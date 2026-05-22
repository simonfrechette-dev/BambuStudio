// Phantom forward-declaration must precede ALL headers so that MachineObject*
// inside namespace Slic3r::GUI resolves to Slic3r::GUI::MachineObject (phantom),
// matching the mangled names expected by StatusPanel.cpp.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "PrintOptionsDialog.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "Widgets/StaticLine.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QTimer>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

// ============================================================================
// PrinterPartsDialog
// ============================================================================

PrinterPartsDialog::PrinterPartsDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Printer parts"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(400, 300);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    // Single nozzle panel
    single_panel = new QWidget(this);
    auto *single_vbox = new QVBoxLayout(single_panel);
    single_vbox->setContentsMargins(0, 0, 0, 0);

    auto make_row = [&](QWidget *parent_w, const QString &label_text,
                         ComboBox *&combo) -> QHBoxLayout * {
        auto *row = new QHBoxLayout;
        auto *lbl = new Label(parent_w, Label::Body_14);
        lbl->setText(label_text);
        row->addWidget(lbl);
        combo = new ComboBox(parent_w);
        row->addWidget(combo, 1);
        return row;
    };

    single_vbox->addLayout(make_row(single_panel, _L("Nozzle type:"), nozzle_type_checkbox));
    single_vbox->addLayout(make_row(single_panel, _L("Nozzle diameter:"), nozzle_diameter_checkbox));
    nozzle_flow_type_label = new Label(single_panel, Label::Body_14);
    nozzle_flow_type_label->setText(_L("Flow type:"));
    single_vbox->addWidget(nozzle_flow_type_label);
    single_vbox->addLayout(make_row(single_panel, _L("Flow type:"), nozzle_flow_type_checkbox));

    change_nozzle_tips = new Label(single_panel, Label::Body_12);
    change_nozzle_tips->setWordWrap(true);
    single_vbox->addWidget(change_nozzle_tips);

    m_wiki_link = new Label(single_panel, Label::Body_12);
    m_wiki_link->setText("<a href='https://wiki.bambulab.com'>" + _L("Wiki") + "</a>");
    single_vbox->addWidget(m_wiki_link);

    m_single_update_nozzle_button = new Button(single_panel, _L("Update nozzle info"));
    m_single_update_nozzle_button->SetValue(true);
    single_vbox->addWidget(m_single_update_nozzle_button);

    vbox->addWidget(single_panel);

    // Multi nozzle panel (hidden by default)
    multiple_panel = new QWidget(this);
    multiple_panel->hide();
    vbox->addWidget(multiple_panel);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *cancel = new Button(this, _L("Cancel"));
    connect(cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(cancel);
    m_multiple_update_nozzle_button = new Button(this, _L("Confirm"));
    m_multiple_update_nozzle_button->SetValue(true);
    connect(m_multiple_update_nozzle_button, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_multiple_update_nozzle_button);
    vbox->addLayout(btn_row);

    adjustSize();
}

PrinterPartsDialog::~PrinterPartsDialog() = default;
void PrinterPartsDialog::on_dpi_changed(const QRect &) {}
void PrinterPartsDialog::update_machine_obj(MachineObject *obj_) { obj = obj_; }
void PrinterPartsDialog::setVisible(bool show) { QDialog::setVisible(show); }
void PrinterPartsDialog::UpdateNozzleInfo() {}
void PrinterPartsDialog::EnableEditing(bool) {}
void PrinterPartsDialog::OnWikiClicked(QMouseEvent &) {}
void PrinterPartsDialog::OnNozzleRefresh(QEvent &) {}

QString PrinterPartsDialog::GetString(NozzleType t) const
{
    switch (t) {
    case NozzleType::ntStainlessSteel: return _L("Stainless steel");
    case NozzleType::ntHardenedSteel:  return _L("Hardened steel");
    default: return _L("Unknown");
    }
}
QString PrinterPartsDialog::GetString(NozzleFlowType t) const
{
    switch (t) {
    case Slic3r::S_FLOW:  return _L("Standard flow");
    case Slic3r::H_FLOW:  return _L("High flow");
    default: return _L("Unknown");
    }
}

// ============================================================================
// PrintOptionToast
// ============================================================================

PrintOptionToast::PrintOptionToast(QWidget *parent, const QString &text)
    : QWidget(parent, Qt::ToolTip)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #323232; color: white; border-radius: 4px; padding: 6px 12px;");
    auto *lbl = new QLabel(text, this);
    lbl->setStyleSheet("color: white;");
    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(8, 4, 8, 4);
    vbox->addWidget(lbl);
    adjustSize();
}

// ============================================================================
// PrintOptionsDialog
// ============================================================================

PrintOptionsDialog::PrintOptionsDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Print options"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumWidth(360);

    m_print_option_timer = new QTimer(this);
    m_print_option_timer->setSingleShot(true);

    m_scrollwindow = new QScrollArea(this);
    m_scrollwindow->setWidgetResizable(true);
    m_scrollwindow->setFrameShape(QFrame::NoFrame);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(m_scrollwindow);

    auto *content = new QWidget;
    m_scrollwindow->setWidget(content);

    auto *vbox = new QVBoxLayout(content);
    vbox->setContentsMargins(16, 12, 16, 12);
    vbox->setSpacing(6);

    // Helper to create a labeled checkbox row
    auto make_check_row = [&](CheckBox *&cb, Label *&lbl, const QString &text,
                               Label *&caption_lbl, const QString &caption = QString()) {
        auto *row = new QHBoxLayout;
        cb = new CheckBox(content);
        row->addWidget(cb);
        lbl = new Label(content, Label::Body_14);
        lbl->setText(text);
        row->addWidget(lbl);
        row->addStretch(1);
        vbox->addLayout(row);
        if (!caption.isEmpty()) {
            caption_lbl = new Label(content, Label::Body_12);
            caption_lbl->setText(caption);
            caption_lbl->setStyleSheet("color: #888; margin-left: 22px;");
            caption_lbl->setWordWrap(true);
            vbox->addWidget(caption_lbl);
        } else {
            caption_lbl = nullptr;
        }
    };

    // First layer inspection
    make_check_row(m_cb_first_layer, text_first_layer, _L("First layer inspection"),
                   text_first_layer,
                   _L("AI will inspect the first layer to detect issues."));

    line1 = new StaticLine(content); vbox->addWidget(line1);

    // AI monitoring
    auto *ai_hdr_row = new QHBoxLayout;
    text_ai_detections = new Label(content, Label::Head_14);
    text_ai_detections->setText(_L("AI monitoring"));
    ai_hdr_row->addWidget(text_ai_detections);
    ai_hdr_row->addStretch(1);
    vbox->addLayout(ai_hdr_row);

    text_ai_detections_caption = new Label(content, Label::Body_12);
    text_ai_detections_caption->setWordWrap(true);
    text_ai_detections_caption->setStyleSheet("color: #888;");
    text_ai_detections_caption->setText(_L("Monitor print quality with the camera."));
    vbox->addWidget(text_ai_detections_caption);

    ai_refine_panel = new QWidget(content);
    auto *ai_vbox = new QVBoxLayout(ai_refine_panel);
    ai_vbox->setContentsMargins(0, 0, 0, 0);

    Label *dummy_cap;
    make_check_row(m_cb_ai_monitoring, text_ai_detections, _L("Enable AI monitoring"), dummy_cap);

    make_check_row(m_cb_spaghetti_detection, text_ai_detections, _L("Spaghetti detection"), dummy_cap);
    make_check_row(m_cb_purgechutepileup_detection, text_ai_detections, _L("Purge chute pile-up detection"), dummy_cap);
    make_check_row(m_cb_nozzleclumping_detection, text_ai_detections, _L("Nozzle clumping detection"), dummy_cap);
    make_check_row(m_cb_airprinting_detection, text_ai_detections, _L("Air printing detection"), dummy_cap);

    text_non_visual_airprinting_detection = new Label(content, Label::Body_12);
    text_non_visual_airprinting_detection->setWordWrap(true);
    vbox->addWidget(text_non_visual_airprinting_detection);
    make_check_row(m_cb_non_visual_airprinting_detection, text_ai_detections,
                   _L("Non-visual air printing detection"), dummy_cap);

    vbox->addWidget(ai_refine_panel);

    line2 = new StaticLine(content); vbox->addWidget(line2);
    make_check_row(m_cb_plate_mark, text_first_layer, _L("Build plate mark"), dummy_cap);
    make_check_row(m_cb_auto_recovery, text_first_layer, _L("Auto-recovery from step loss"), dummy_cap);
    make_check_row(m_cb_save_remote_print_file_to_storage, text_first_layer,
                   _L("Save print file to storage"), dummy_cap);

    line3 = new StaticLine(content); vbox->addWidget(line3);
    make_check_row(m_cb_sup_sound, text_first_layer, _L("Aux fan"), dummy_cap);
    make_check_row(m_cb_filament_tangle, text_first_layer, _L("Filament tangle detection"), dummy_cap);
    make_check_row(m_cb_nozzle_blob, text_first_layer, _L("Nozzle blob detection"), dummy_cap);

    line4 = new StaticLine(content); vbox->addWidget(line4);
    make_check_row(m_cb_open_door, text_first_layer, _L("Print halt when door open"), dummy_cap);
    make_check_row(m_cb_purify_air_at_print_end, text_purify_air_context,
                   _L("Purify air at print end"), text_purify_air_context);

    line5 = new StaticLine(content); vbox->addWidget(line5);
    make_check_row(m_cb_fod_check, text_first_layer, _L("Foreign object detection (FOD)"), dummy_cap);
    make_check_row(m_cb_displacement_detection, text_first_layer,
                   _L("Displacement detection"), dummy_cap);

    vbox->addStretch(1);
    adjustSize();
}

PrintOptionsDialog::~PrintOptionsDialog() = default;
void PrintOptionsDialog::on_dpi_changed(const QRect &) {}
void PrintOptionsDialog::update_ai_monitor_status() {}
void PrintOptionsDialog::update_spaghetti_detection_status() {}
void PrintOptionsDialog::update_purgechutepileup_detection_status() {}
void PrintOptionsDialog::update_nozzleclumping_detection_status() {}
void PrintOptionsDialog::update_airprinting_detection_status() {}
void PrintOptionsDialog::update_purify_air_at_print_end(MachineObject *) {}
void PrintOptionsDialog::show_print_option_toast(const QString &text)
{
    if (m_print_option_toast) { delete m_print_option_toast; m_print_option_toast = nullptr; }
    m_print_option_toast = new PrintOptionToast(this, text);
    // Position near top-right of dialog
    m_print_option_toast->move(rect().right() - m_print_option_toast->width() - 8,
                               rect().top() + 8);
    m_print_option_toast->show();
    m_print_option_timer->start(3000);
    connect(m_print_option_timer, &QTimer::timeout, this, [this] {
        if (m_print_option_toast) { m_print_option_toast->hide(); }
    });
}
void PrintOptionsDialog::purify_air_bind_toast() {}
void PrintOptionsDialog::update_options(MachineObject *obj_) { obj = obj_; }
void PrintOptionsDialog::update_machine_obj(MachineObject *obj_) { obj = obj_; }
void PrintOptionsDialog::setVisible(bool show) { QDialog::setVisible(show); }
QString PrintOptionsDialog::sensitivity_level_to_label_string(AiMonitorSensitivityLevel level)
{
    switch (level) {
    case LOW:    return _L("Low");
    case MEDIUM: return _L("Medium");
    case HIGH:   return _L("High");
    default:     return {};
    }
}
std::string PrintOptionsDialog::sensitivity_level_to_msg_string(AiMonitorSensitivityLevel level)
{
    switch (level) {
    case LOW:    return "low";
    case MEDIUM: return "medium";
    case HIGH:   return "high";
    default:     return {};
    }
}
void PrintOptionsDialog::set_ai_monitor_sensitivity(QEvent &) {}
void PrintOptionsDialog::set_spaghetti_detection_sensitivity(QEvent &) {}
void PrintOptionsDialog::set_purgechutepileup_detection_sensitivity(QEvent &) {}
void PrintOptionsDialog::set_nozzleclumping_detection_sensitivity(QEvent &) {}
void PrintOptionsDialog::set_airprinting_detection_sensitivity(QEvent &) {}
void PrintOptionsDialog::UpdateOptionSavePrintFileToStorage(MachineObject *) {}
void PrintOptionsDialog::UpdateOptionOpenDoorCheck(MachineObject *) {}
void PrintOptionsDialog::UpdateOptionSnapshot(MachineObject *) {}

}} // namespace Slic3r::GUI

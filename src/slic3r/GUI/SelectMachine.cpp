#include "SelectMachine.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "Plater.hpp"
#include "PrePrintChecker.hpp"
#include "DeviceManager.hpp"
#include "Widgets/StaticBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "wxExtensions.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QTimerEvent>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

// Static member definition
std::vector<QString>     SelectMachineDialog::MACHINE_BED_TYPE_STRING;
std::vector<std::string> SelectMachineDialog::MachineBedTypeString;

void SelectMachineDialog::init_machine_bed_types()
{
    MACHINE_BED_TYPE_STRING = {_L("Cool plate"), _L("Engineering plate"),
                               _L("High temperature plate"), _L("Textured PEI plate")};
    MachineBedTypeString    = {"cool_plate", "eng_plate", "hot_plate", "textured_pei_plate"};
}

// ============================================================================
// PrintOptionItem
// ============================================================================

PrintOptionItem::PrintOptionItem(QWidget *parent, std::vector<POItem> ops, std::string param)
    : QWidget(parent), m_param(std::move(param))
{
    m_ops = std::move(ops);
}

void PrintOptionItem::setValue(std::string value)
{
    selected_key = std::move(value);
    m_old_selected_key = selected_key;
    update();
}

void PrintOptionItem::msw_rescale() {}
void PrintOptionItem::OnPaint(QPaintEvent &ev)  { QPainter dc(this); render(dc); QWidget::paintEvent(&ev); }
void PrintOptionItem::render(QPainter &dc) { dc.fillRect(rect(), QColor("#F0F0F0")); }
void PrintOptionItem::on_left_down(QMouseEvent &) {}
void PrintOptionItem::doRender(QPainter &dc) { render(dc); }

// ============================================================================
// PrintOption
// ============================================================================

PrintOption::PrintOption(QWidget *parent, QString title, QString tips,
                          std::vector<POItem> ops, std::string param)
    : QWidget(parent), m_full_title(std::move(title)), m_param(std::move(param)),
      m_ops(std::move(ops))
{
    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(4);

    m_printoption_title = new Label(this, Label::Body_14);
    m_printoption_title->setText(m_full_title);
    vbox->addWidget(m_printoption_title);

    if (!tips.isEmpty()) {
        m_printoption_tips = new ScalableButton(this, 0, "enable_question_hover");
        m_printoption_tips->setToolTip(tips);
        vbox->addWidget(m_printoption_tips);
    }

    m_printoption_item = new PrintOptionItem(this, m_ops, m_param);
    vbox->addWidget(m_printoption_item);
}

void PrintOption::enable(bool en)  { setEnabled(en); if (m_printoption_item) m_printoption_item->enable(en); }
void PrintOption::setValue(std::string v)    { if (m_printoption_item) m_printoption_item->setValue(v); }
void PrintOption::restoreValue()             { if (m_printoption_item) m_printoption_item->restoreValue(); }
std::string PrintOption::getValue()          { return m_printoption_item ? m_printoption_item->getValue() : ""; }
int  PrintOption::getValueInt()              { return m_printoption_item ? (int)m_ops.size() : -1; }
bool PrintOption::contain_opt(const std::string &opt) const
{
    for (auto &i : m_ops) if (i.key == opt) return true;
    return false;
}
void PrintOption::update_options(std::vector<POItem> ops, const QString &tips)
{
    m_ops = ops;
    if (m_printoption_item) m_printoption_item->update_options(ops);
    if (m_printoption_tips) m_printoption_tips->setToolTip(tips);
}
void PrintOption::update_tooltip(const QString &tips)       { if (m_printoption_tips) m_printoption_tips->setToolTip(tips); }
void PrintOption::update_title_display()                    { if (m_printoption_title) m_printoption_title->setText(m_full_title); }
void PrintOption::update_tooltip_options_area(const QString &) {}
void PrintOption::insert_extra_widget(QWidget *w)
{
    if (auto *l = qobject_cast<QVBoxLayout *>(layout())) l->insertWidget(1, w);
}
void PrintOption::msw_rescale()  {}
void PrintOption::OnPaint(QPaintEvent &ev)  { QWidget::paintEvent(&ev); }
void PrintOption::render(QPainter &) {}
void PrintOption::doRender(QPainter &) {}

// ============================================================================
// ThumbnailPanel
// ============================================================================

ThumbnailPanel::ThumbnailPanel(QWidget *parent, int, const QPoint &pos, const QSize &size)
    : QWidget(parent)
{
    if (!pos.isNull()) move(pos);
    if (!size.isEmpty()) resize(size);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #F0F0F0;");
}

ThumbnailPanel::~ThumbnailPanel() = default;

void ThumbnailPanel::OnPaint(QPaintEvent &ev)    { QPainter dc(this); render(dc); QWidget::paintEvent(&ev); }
void ThumbnailPanel::PaintBackground(QPainter &) {}
void ThumbnailPanel::OnEraseBackground(QEvent &) {}
void ThumbnailPanel::set_thumbnail(QImage &img)
{
    m_bitmap = QPixmap::fromImage(img);
    if (m_staticbitmap) m_staticbitmap->setPixmap(m_bitmap);
    update();
}
void ThumbnailPanel::render(QPainter &dc)
{
    if (!m_bitmap.isNull())
        dc.drawPixmap(rect(), m_bitmap);
    else
        dc.fillRect(rect(), QColor("#F0F0F0"));
}

// ============================================================================
// SendModeSwitchButton
// ============================================================================

SendModeSwitchButton::SendModeSwitchButton(QWidget *parent, QString /*mode*/, bool sel)
    : QWidget(parent), is_selected(sel)
{
    setFixedHeight(32);
    setCursor(Qt::PointingHandCursor);
}

void SendModeSwitchButton::msw_rescale()       {}
void SendModeSwitchButton::setSelected(bool v) { is_selected = v; update(); }
void SendModeSwitchButton::OnPaint(QPaintEvent &ev) { QPainter dc(this); render(dc); QWidget::paintEvent(&ev); }
void SendModeSwitchButton::render(QPainter &dc)
{
    dc.fillRect(rect(), is_selected ? QColor("#1F8FEB") : QColor("#E8E8E8"));
}
void SendModeSwitchButton::on_left_down(QMouseEvent &) { setSelected(!is_selected); }
void SendModeSwitchButton::doRender(QPainter &dc)      { render(dc); }

// ============================================================================
// PrinterInfoBox
// ============================================================================

PrinterInfoBox::PrinterInfoBox(QWidget *parent, SelectMachineDialog *select_dialog)
    : StaticBox(parent), m_select_dialog(select_dialog)
{
    Create();
}

void PrinterInfoBox::Create()
{
    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->setSpacing(6);

    auto *title_row = new QHBoxLayout;
    m_stext_printer_title = new Label(this, Label::Body_14);
    m_stext_printer_title->setText(_L("Printer:"));
    title_row->addWidget(m_stext_printer_title);
    title_row->addStretch(1);
    m_button_question = new ScalableButton(this, 0, "enable_question_hover");
    title_row->addWidget(m_button_question);
    vbox->addLayout(title_row);

    auto *combo_row = new QHBoxLayout;
    m_comboBox_printer = new ComboBox(this);
    combo_row->addWidget(m_comboBox_printer, 1);
    m_button_refresh = new ScalableButton(this, 0, "refresh");
    combo_row->addWidget(m_button_refresh);
    vbox->addLayout(combo_row);

    m_text_bed_type = new Label(this, Label::Body_12);
    m_text_bed_type->setStyleSheet("color: #888;");
    vbox->addWidget(m_text_bed_type);
}

void PrinterInfoBox::UpdatePlate(const std::string &plate_name)
{
    if (m_text_bed_type)
        m_text_bed_type->setText(QString::fromStdString(plate_name));
}

void PrinterInfoBox::SetPrinters(const std::vector<MachineObject *> &)
{
    // stub — MachineObject is incomplete in GUI namespace due to AMSItem.hpp forward declaration
}

void PrinterInfoBox::EnableEditing(bool en)       { if (m_comboBox_printer) m_comboBox_printer->setEnabled(en); }
void PrinterInfoBox::EnableRefreshButton(bool en) { if (m_button_refresh) m_button_refresh->setEnabled(en); }
void PrinterInfoBox::SetDefault(bool)             {}
void PrinterInfoBox::OnBtnQuestionClicked(QEvent &) {}

// ============================================================================
// NozzleStatePanel
// ============================================================================

NozzleStatePanel::NozzleStatePanel(QWidget *parent)
    : QWidget(parent)
{
    m_sizer = new QVBoxLayout(this);
}

void NozzleStatePanel::UpdateInfoBy(Plater *, MachineObject *) {}
void NozzleStatePanel::UpdateInfo(const ExtruderNozzleInfos &, const ExtruderNozzleInfos &) {}
void NozzleStatePanel::UpdateGui()          {}
void NozzleStatePanel::UpdateLabelColour() {}

// ============================================================================
// SelectMachineDialog
// ============================================================================

SelectMachineDialog::SelectMachineDialog(Plater *plater)
    : DPIDialog(plater ? static_cast<QWidget *>(plater) : nullptr)
{
    setWindowTitle(_L("Send/Print File"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(800, 600);

    m_simplebook = new QStackedWidget(this);
    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(m_simplebook);

    // Prepare panel
    m_panel_prepare = new QWidget;
    auto *prep_vbox = new QVBoxLayout(m_panel_prepare);
    prep_vbox->setContentsMargins(16, 12, 16, 12);
    m_basic_panel = new QWidget(m_panel_prepare);
    prep_vbox->addWidget(m_basic_panel, 1);
    m_simplebook->addWidget(m_panel_prepare);

    // Sending panel
    m_panel_sending = new QWidget;
    auto *send_vbox = new QVBoxLayout(m_panel_sending);
    m_statictext_ams_msg = new PrinterMsgPanel(m_panel_sending, this);
    send_vbox->addWidget(m_statictext_ams_msg, 0, Qt::AlignCenter);
    m_simplebook->addWidget(m_panel_sending);

    // Finish panel
    m_panel_finish = new QWidget;
    auto *fin_vbox = new QVBoxLayout(m_panel_finish);
    m_statictext_finish = new QLabel(_L("Print task sent successfully."), m_panel_finish);
    m_statictext_finish->setAlignment(Qt::AlignCenter);
    fin_vbox->addWidget(m_statictext_finish);
    m_simplebook->addWidget(m_panel_finish);

    m_simplebook->setCurrentIndex(0);

    init_machine_bed_types();
    adjustSize();
}

SelectMachineDialog::~SelectMachineDialog() = default;

void SelectMachineDialog::on_dpi_changed(const QRect &) {}
bool SelectMachineDialog::Show(bool show)    { setVisible(show); return true; }

void SelectMachineDialog::init_bind()        {}
void SelectMachineDialog::init_timer()       {}

void SelectMachineDialog::show_print_failed_info(bool, int, QString, QString) {}
void SelectMachineDialog::check_fcous_state(QWidget *) {}
void SelectMachineDialog::popup_filament_backup() {}
void SelectMachineDialog::update_select_layout(MachineObject *) {}
void SelectMachineDialog::prepare_mode(bool) {}
void SelectMachineDialog::sending_mode()     {}
void SelectMachineDialog::finish_mode()      {}
void SelectMachineDialog::sync_ams_mapping_result(const std::vector<FilamentInfo> &) {}
void SelectMachineDialog::prepare(int)       {}
void SelectMachineDialog::show_status(PrintDialogStatus, std::vector<QString>, QString, prePrintInfoStyle) {}
void SelectMachineDialog::sys_color_changed() {}
void SelectMachineDialog::reset_timeout()    {}
void SelectMachineDialog::update_user_printer() {}
void SelectMachineDialog::reset_ams_material() {}
void SelectMachineDialog::update_show_status(MachineObject *) {}

bool SelectMachineDialog::CheckErrorRackStatus(MachineObject *)               { return true; }
bool SelectMachineDialog::CheckErrorExtruderNozzleWithSlicing(MachineObject *) { return true; }
bool SelectMachineDialog::CheckErrorSyncNozzleMappingResultV1(MachineObject *) { return true; }
bool SelectMachineDialog::CheckErrorSyncNozzleMappingResultV0(MachineObject *) { return true; }
bool SelectMachineDialog::CheckErrorDynamicSwitchNozzle(MachineObject *)       { return true; }

void SelectMachineDialog::UpdateStatusCheckWarning_ExtensionTool(MachineObject *) {}
void SelectMachineDialog::update_best_pos_dialog(QEvent &)   {}
void SelectMachineDialog::update_ams_check(MachineObject *)  {}
void SelectMachineDialog::update_filament_change_count()     {}
void SelectMachineDialog::update_printer_combobox(QEvent &)  {}
void SelectMachineDialog::update_page_turn_state(bool)       {}
void SelectMachineDialog::update_user_machine_list()         {}
void SelectMachineDialog::update_print_status_msg()          {}
void SelectMachineDialog::update_print_error_info(int, std::string, std::string) {}
void SelectMachineDialog::update_timelapse_folder_btn_icon() {}
void SelectMachineDialog::update_ams_backup(MachineObject *) {}
void SelectMachineDialog::update_material_item_pos(MachineObject *) {}
void SelectMachineDialog::update_scroll_area_size()          {}
void SelectMachineDialog::update_option_opts(MachineObject *) {}
void SelectMachineDialog::update_options_layout()            {}
void SelectMachineDialog::update_option_dynamic_state(MachineObject *) {}

bool SelectMachineDialog::can_support_pa_auto_cali()            { return false; }
bool SelectMachineDialog::is_same_printer_model()               { return false; }
bool SelectMachineDialog::is_blocking_printing(MachineObject *)  { return false; }
bool SelectMachineDialog::is_nozzle_hrc_matched(const NozzleType &, const std::string &) const { return true; }
bool SelectMachineDialog::check_sdcard_for_timelpase(MachineObject *) { return true; }

void SelectMachineDialog::show_timelapse_folder_popup()        {}
void SelectMachineDialog::check_timelapse_storage_warning(MachineObject *) {}
void SelectMachineDialog::start_timelapse_storage_check(MachineObject *) {}
void SelectMachineDialog::on_timelapse_storage_check_timer(QTimerEvent &) {}
void SelectMachineDialog::on_timelapse_storage_check_result()  {}
void SelectMachineDialog::show_timelapse_storage_dialog(MachineObject *) {}
void SelectMachineDialog::navigate_to_timelapse_page()         {}

bool SelectMachineDialog::is_timeout()     { return false; }
int  SelectMachineDialog::update_print_required_data(Slic3r::DynamicPrintConfig, Slic3r::Model,
                                                      Slic3r::PlateDataPtrs, std::string, std::string) { return 0; }
bool SelectMachineDialog::do_ams_mapping(MachineObject *, bool)  { return false; }
bool SelectMachineDialog::get_ams_mapping_result(std::string &, std::string &, std::string &) const { return false; }
bool SelectMachineDialog::build_nozzles_info(std::string &)      { return false; }
bool SelectMachineDialog::can_hybrid_mapping(MachineObject *) const { return false; }
void SelectMachineDialog::auto_supply_with_ext(std::vector<DevAmsTray>) {}
bool SelectMachineDialog::is_ams_drying(MachineObject *)           { return false; }
bool SelectMachineDialog::is_selected_ams_drying(MachineObject *)  { return false; }
void SelectMachineDialog::EnableEditing(bool)                {}
bool SelectMachineDialog::is_at_suggested_pos(MachineObject *, int) const { return false; }
bool SelectMachineDialog::is_used_filament(int) const        { return false; }
void SelectMachineDialog::clear_nozzle_mapping()             {}
bool SelectMachineDialog::slicing_with_fila_switch() const   { return false; }
bool SelectMachineDialog::use_dynamic_nozzle_map() const     { return false; }
void SelectMachineDialog::load_option_vals(MachineObject *)  {}
void SelectMachineDialog::save_option_vals()                 {}
void SelectMachineDialog::save_option_vals(MachineObject *)  {}
void SelectMachineDialog::check_tpu_aero_flow_cali(MachineObject *) {}
void SelectMachineDialog::clear_material_infos()             {}
void SelectMachineDialog::on_material_item_clicked(MaterialItem *, const std::vector<pPresetFilaInfo> &,
                                                    int, QMouseEvent &) {}
std::optional<FilamentInfo> SelectMachineDialog::get_slicing_filament_info(int) const { return {}; }
std::optional<FilamentInfo> SelectMachineDialog::get_mapped_filament_info(int)  const { return {}; }
void SelectMachineDialog::on_flow_pa_caliation_option_changed(QEvent &) {}
void SelectMachineDialog::on_nozzle_offset_option_changed(QEvent &) {}
void SelectMachineDialog::on_pa_value_switch_changed(QEvent &) {}
int  SelectMachineDialog::get_print_task_total_extruder_count() const { return 1; }
bool SelectMachineDialog::is_enable_external_change_assist(std::vector<FilamentInfo> &) { return false; }
void SelectMachineDialog::refresh_save_time(MachineObject *)  {}
bool SelectMachineDialog::has_bowden_extuder(MachineObject *) { return false; }

// format_steel_name declared in header
QString SelectMachineDialog::format_steel_name(NozzleType type)
{
    switch (type) {
    case NozzleType::ntHardenedSteel:  return _L("Hardened steel");
    case NozzleType::ntStainlessSteel: return _L("Stainless steel");
    default: return {};
    }
}

// Free function declared in header
void print_ams_mapping_result(std::vector<FilamentInfo> &) {}

}} // namespace Slic3r::GUI

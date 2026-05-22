// SyncAmsInfoDialog.cpp — Qt6 port (stub implementations)
#include "SyncAmsInfoDialog.hpp"

#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QGridLayout>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/StaticBox.hpp"
#include "Widgets/ComboBox.hpp"

#include "CapsuleButton.hpp"
#include "wxExtensions.hpp"
#include "SelectMachine.hpp"
#include "BaseTransparentDPIFrame.hpp"
#include "DeviceCore/DevExtruderSystem.h"

namespace Slic3r { namespace GUI {

// ─── SyncAmsInfoDialog ────────────────────────────────────────────────────────

SyncAmsInfoDialog::SyncAmsInfoDialog(QWidget *parent, SyncInfo &info)
    : DPIDialog(parent)
    , m_input_info(info)
    , m_basic_panel(nullptr)
{
    setWindowTitle(_L("Sync AMS Info"));
    setMinimumSize(640, 520);

    auto *main_layout = new QVBoxLayout(this);
    m_sizer_main = main_layout;
    m_sizer_this = main_layout;
    main_layout->setContentsMargins(20, 16, 20, 16);
    main_layout->setSpacing(8);

    // Mode switcher row
    auto *mode_row = new QHBoxLayout();
    m_colormap_btn = new CapsuleButton(this, _L("Color Map"), true);
    mode_row->addWidget(m_colormap_btn);

    m_override_btn = new CapsuleButton(this, _L("Override"), false);
    mode_row->addWidget(m_override_btn);
    mode_row->addStretch(1);
    main_layout->addLayout(mode_row);

    // Scrollable area
    m_scrolledWindow = new QScrollArea(this);
    m_scrolledWindow->setFrameShape(QFrame::NoFrame);
    m_scrolledWindow->setWidgetResizable(true);

    auto *scroll_content = new QWidget(m_scrolledWindow);
    auto *scroll_layout  = new QVBoxLayout(scroll_content);
    scroll_layout->setContentsMargins(0, 0, 0, 0);

    m_thumbnailPanel = new ThumbnailPanel(scroll_content);
    m_sizer_thumbnail = new QVBoxLayout();
    m_sizer_thumbnail->addWidget(m_thumbnailPanel);
    scroll_layout->addLayout(m_sizer_thumbnail);

    m_basic_panel = new QWidget(scroll_content);
    m_basicl_sizer = new QVBoxLayout(m_basic_panel);
    scroll_layout->addWidget(m_basic_panel);

    m_scrolledWindow->setWidget(scroll_content);
    main_layout->addWidget(m_scrolledWindow, 1);

    m_statictext_ams_msg = new Label(this, Label::Body_12);
    m_statictext_ams_msg->setWordWrap(true);
    main_layout->addWidget(m_statictext_ams_msg);

    // Footer buttons
    auto *btn_row = new QHBoxLayout();
    btn_row->addStretch(1);

    m_button_cancel = new Button(this, _L("Cancel"));
    btn_row->addWidget(m_button_cancel);

    m_button_ok = new Button(this, _L("OK"));
    m_button_ok->SetValue(true);
    btn_row->addWidget(m_button_ok);
    main_layout->addLayout(btn_row);

    connect(m_button_ok,     &Button::clicked, this, [this]() { deal_ok(); });
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);

    if (info.use_dialog_pos) move(info.dialog_pos);
}

SyncAmsInfoDialog::~SyncAmsInfoDialog() {}

void SyncAmsInfoDialog::set_info(SyncInfo &info) { m_input_info = info; }
void SyncAmsInfoDialog::on_dpi_changed(const QRect & /*r*/) {}

void SyncAmsInfoDialog::setVisible(bool show) { DPIDialog::setVisible(show); }
void SyncAmsInfoDialog::updata_ui_data_after_connected_printer() {}
void SyncAmsInfoDialog::set_default(bool /*hide_some*/) {}
void SyncAmsInfoDialog::update_select_layout(MachineObject * /*obj*/) {}
void SyncAmsInfoDialog::set_default_normal(const ThumbnailData & /*data*/) {}
bool SyncAmsInfoDialog::is_must_finish_slice_then_connected_printer() { return false; }
void SyncAmsInfoDialog::hide_no_use_controls() {}
void SyncAmsInfoDialog::show_sizer(QLayout *sizer, bool show)
{
    if (!sizer) return;
    for (int i = 0; i < sizer->count(); ++i) {
        auto *item = sizer->itemAt(i);
        if (item && item->widget()) item->widget()->setVisible(show);
    }
}
void SyncAmsInfoDialog::deal_ok() { accept(); }
bool SyncAmsInfoDialog::get_is_double_extruder() { return false; }
bool SyncAmsInfoDialog::is_dirty_filament() { return false; }
bool SyncAmsInfoDialog::is_need_show() { return true; }

void SyncAmsInfoDialog::check_empty_project() {}
void SyncAmsInfoDialog::reinit_dialog() {}
void SyncAmsInfoDialog::init_bind() {}
void SyncAmsInfoDialog::init_timer() {}
void SyncAmsInfoDialog::show_print_failed_info(bool /*show*/, int /*code*/,
    QString /*description*/, QString /*extra*/) {}
void SyncAmsInfoDialog::check_fcous_state(QWidget * /*window*/) {}
void SyncAmsInfoDialog::popup_filament_backup() {}
void SyncAmsInfoDialog::prepare_mode(bool /*refresh_button*/) {}
void SyncAmsInfoDialog::finish_mode() {}
void SyncAmsInfoDialog::sync_ams_mapping_result(std::vector<FilamentInfo> & /*result*/) {}
void SyncAmsInfoDialog::prepare(int /*print_plate_idx*/) {}
void SyncAmsInfoDialog::show_status(PrintDialogStatus /*status*/, std::vector<QString> /*params*/) {}
void SyncAmsInfoDialog::reset_timeout() {}
void SyncAmsInfoDialog::update_user_printer() {}
void SyncAmsInfoDialog::reset_ams_material() {}
void SyncAmsInfoDialog::reset_all_ams_info() {}
void SyncAmsInfoDialog::reset_one_ams_material(const std::string & /*index_str*/, bool /*reset_to_first*/) {}
void SyncAmsInfoDialog::update_show_status() {}
void SyncAmsInfoDialog::update_printer_combobox(QEvent & /*event*/) {}
void SyncAmsInfoDialog::on_cancel(QCloseEvent & /*event*/) {}
void SyncAmsInfoDialog::show_errors(QString & /*info*/) {}
void SyncAmsInfoDialog::Enable_Auto_Refill(bool /*enable*/) {}
void SyncAmsInfoDialog::on_refresh(QEvent & /*event*/) {}
void SyncAmsInfoDialog::on_set_finish_mapping(QEvent & /*evt*/) {}
void SyncAmsInfoDialog::on_print_job_cancel(QEvent & /*evt*/) {}
void SyncAmsInfoDialog::reset_and_sync_ams_list() {}
void SyncAmsInfoDialog::generate_override_fix_ams_list() {}
void SyncAmsInfoDialog::clone_thumbnail_data() {}
void SyncAmsInfoDialog::record_edge_pixels_data() {}
QColor SyncAmsInfoDialog::adjust_color_for_render(const QColor &color) { return color; }
void SyncAmsInfoDialog::final_deal_edge_pixels_data(ThumbnailData & /*data*/) {}
void SyncAmsInfoDialog::updata_thumbnail_data_after_connected_printer() {}
void SyncAmsInfoDialog::show_ams_controls(bool /*flag*/) {}
void SyncAmsInfoDialog::show_advanced_settings(bool /*flag*/, bool /*update_layout*/) {}
void SyncAmsInfoDialog::update_thumbnail_data_accord_plate_index(bool /*allow_clone_ams_color*/) {}
void SyncAmsInfoDialog::update_final_thumbnail_data() {}
void SyncAmsInfoDialog::unify_deal_thumbnail_data(ThumbnailData & /*input_data*/,
    ThumbnailData & /*no_light_data*/, bool /*allow_clone_ams_color*/) {}
void SyncAmsInfoDialog::change_default_normal(int /*old_filament_id*/, QColor /*temp_ams_color*/) {}
void SyncAmsInfoDialog::on_timer(QTimerEvent & /*event*/) {}
void SyncAmsInfoDialog::update_user_machine_list() {}
void SyncAmsInfoDialog::update_lan_machine_list() {}
void SyncAmsInfoDialog::stripWhiteSpace(std::string & /*str*/) {}
void SyncAmsInfoDialog::update_ams_status_msg(QString /*msg*/, bool /*is_warning*/) {}
void SyncAmsInfoDialog::update_priner_status_msg(QString /*msg*/, bool /*is_warning*/) {}
void SyncAmsInfoDialog::update_print_status_msg(QString /*msg*/, bool /*is_warning*/, bool /*is_printer*/) {}
void SyncAmsInfoDialog::update_print_error_info(int /*code*/, std::string /*msg*/, std::string /*extra*/) {}
bool SyncAmsInfoDialog::has_timelapse_warning() { return false; }
void SyncAmsInfoDialog::update_timelapse_enable_status() {}
bool SyncAmsInfoDialog::is_same_printer_model() { return true; }
bool SyncAmsInfoDialog::is_blocking_printing(MachineObject * /*obj_*/) { return false; }
bool SyncAmsInfoDialog::is_same_nozzle_diameters(NozzleType & /*tag_nozzle_type*/, float & /*nozzle_diameter*/) { return true; }
bool SyncAmsInfoDialog::is_same_nozzle_type(std::string & /*filament_type*/, NozzleType & /*tag_nozzle_type*/) { return true; }
bool SyncAmsInfoDialog::is_timeout() { return false; }
int  SyncAmsInfoDialog::update_print_required_data(Slic3r::DynamicPrintConfig /*config*/,
    Slic3r::Model /*model*/, Slic3r::PlateDataPtrs /*plate_data_list*/,
    std::string /*file_name*/, std::string /*file_path*/) { return 0; }
bool SyncAmsInfoDialog::has_selector(MachineObject * /*obj_*/) const { return false; }
bool SyncAmsInfoDialog::do_ams_mapping(MachineObject * /*obj_*/) { return true; }
void SyncAmsInfoDialog::deal_only_exist_ext_spool(MachineObject * /*obj_*/) {}
void SyncAmsInfoDialog::show_thumbnail_page() {}
bool SyncAmsInfoDialog::get_ams_mapping_result(std::string & /*mapping_array_str*/,
    std::string & /*mapping_array_str2*/, std::string & /*ams_mapping_info*/) { return false; }
bool SyncAmsInfoDialog::build_nozzles_info(std::string & /*nozzles_info*/) { return false; }
void SyncAmsInfoDialog::auto_supply_with_ext(std::vector<DevAmsTray> /*slots*/) {}
bool SyncAmsInfoDialog::is_nozzle_type_match(DevExtderSystem /*data*/, QString & /*error_message*/) const { return true; }
int  SyncAmsInfoDialog::convert_filament_map_nozzle_id_to_task_nozzle_id(int nozzle_id) { return nozzle_id; }
QString SyncAmsInfoDialog::format_text(QString &m_msg) { return m_msg; }
std::vector<std::string> SyncAmsInfoDialog::sort_string(std::vector<std::string> strArray)
{
    std::sort(strArray.begin(), strArray.end());
    return strArray;
}

QBoxLayout *SyncAmsInfoDialog::create_sizer_thumbnail(QPushButton * /*image_button*/, bool /*left*/)
{ return new QVBoxLayout(); }
void SyncAmsInfoDialog::update_when_change_plate(int /*idx*/) {}
void SyncAmsInfoDialog::update_when_change_map_mode(int /*idx*/) {}
void SyncAmsInfoDialog::update_plate_combox() {}
void SyncAmsInfoDialog::update_map_when_change_map_mode() {}
QColor SyncAmsInfoDialog::decode_ams_color(const std::string & /*color*/) { return QColor(255, 255, 255); }
void SyncAmsInfoDialog::update_when_change_map_mode(QEvent & /*e*/) {}
void SyncAmsInfoDialog::update_panel_status(PageType /*page*/) {}
void SyncAmsInfoDialog::show_color_panel(bool /*flag*/, bool /*update_layout*/) {}
void SyncAmsInfoDialog::update_more_setting(bool /*layout*/, bool /*from_more_seting_text*/) {}
void SyncAmsInfoDialog::add_two_image_control() {}
void SyncAmsInfoDialog::to_next_plate(QEvent & /*event*/) {}
void SyncAmsInfoDialog::to_previous_plate(QEvent & /*event*/) {}
void SyncAmsInfoDialog::update_swipe_button_state() {}
void SyncAmsInfoDialog::updata_ui_when_priner_not_same() {}
void SyncAmsInfoDialog::init_bitmaps() {}

// ─── SyncNozzleAndAmsDialog ───────────────────────────────────────────────────

SyncNozzleAndAmsDialog::SyncNozzleAndAmsDialog(InputInfo &input_info)
    : BaseTransparentDPIFrame(nullptr, 360, input_info.dialog_pos, 120,
                              _L("Sync Nozzle & AMS"), _L("OK"), _L("Cancel"))
    , m_input_info(input_info)
{}

SyncNozzleAndAmsDialog::~SyncNozzleAndAmsDialog() {}

void SyncNozzleAndAmsDialog::deal_ok()     { BaseTransparentDPIFrame::deal_ok(); }
void SyncNozzleAndAmsDialog::deal_cancel() { BaseTransparentDPIFrame::deal_cancel(); }
void SyncNozzleAndAmsDialog::update_info(InputInfo &info)
{
    m_input_info = info;
    move(info.dialog_pos);
}

// ─── FinishSyncAmsDialog ──────────────────────────────────────────────────────

FinishSyncAmsDialog::FinishSyncAmsDialog(InputInfo &input_info)
    : BaseTransparentDPIFrame(nullptr, 360, input_info.dialog_pos, 120,
                              _L("Sync Complete"), _L("OK"))
    , m_input_info(input_info)
{}

FinishSyncAmsDialog::~FinishSyncAmsDialog() {}

void FinishSyncAmsDialog::deal_ok()
{
    BaseTransparentDPIFrame::deal_ok();
}

void FinishSyncAmsDialog::update_info(InputInfo &info)
{
    m_input_info = info;
    move(info.dialog_pos);
}

// ─── ExtruderWarningDialog ────────────────────────────────────────────────────

ExtruderWarningDialog::ExtruderWarningDialog(InputInfo &input_info,
                                              std::string icon_name)
    : BaseTransparentDPIFrame(nullptr, 360, input_info.dialog_pos, 120,
                              _L("Extruder Warning"), _L("OK"), QString(),
                              DisappearanceMode::None, icon_name)
    , m_input_info(input_info)
{}

ExtruderWarningDialog::~ExtruderWarningDialog() {}

void ExtruderWarningDialog::deal_ok()
{
    BaseTransparentDPIFrame::deal_ok();
}

void ExtruderWarningDialog::update_info(InputInfo &info)
{
    m_input_info = info;
    move(info.dialog_pos);
}

}} // namespace Slic3r::GUI

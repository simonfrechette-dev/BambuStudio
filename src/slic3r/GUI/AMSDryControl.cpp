// AMSDryControl.cpp — stubbed for Qt6 port (wx UI removed, logic preserved in stubs)
#include "AMSDryControl.hpp"
#include "DeviceCore/DevFilaSystem.h"
#include "GUI_App.hpp"
#include "I18N.hpp"

#include "slic3r/GUI/DeviceCore/DevExtruderSystem.h"
#include "slic3r/GUI/DeviceCore/DevUpgrade.h"
#include "slic3r/GUI/DeviceCore/DevManager.h"
#include "slic3r/GUI/MsgDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QCheckBox>
#include <QLineEdit>
#include <QStackedWidget>
#include <QTimer>
#include <QPainter>
#include <QCloseEvent>
#include <QShowEvent>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// FilamentItemPanel
// ---------------------------------------------------------------------------

FilamentItemPanel::FilamentItemPanel(QWidget* parent, const QString& text,
                                     const std::string& icon_name, int id)
    : QWidget(parent)
    , m_icon_bitmap(new QLabel(this))
    , m_text_label(nullptr)
    , m_target_size(0)
{
    (void)text; (void)icon_name; (void)id;
}

void FilamentItemPanel::SetText(const QString& text) { (void)text; }
void FilamentItemPanel::SetIcon(const std::string& icon_name) { (void)icon_name; }
void FilamentItemPanel::msw_rescale() {}
void FilamentItemPanel::OnPaint(QPaintEvent *event) { (void)event; QPainter p(this); }
void FilamentItemPanel::OnSize(QResizeEvent *event) { (void)event; }

// ---------------------------------------------------------------------------
// AMSFilamentPanel
// ---------------------------------------------------------------------------

AMSFilamentPanel::AMSFilamentPanel(QWidget* parent, const QString& ams_name, int id)
    : QWidget(parent)
    , m_filament_sizer(nullptr)
    , m_ams_name_label(nullptr)
    , m_border_radius(0)
    , m_filament_container(nullptr)
{
    (void)ams_name; (void)id;
}

void AMSFilamentPanel::AddFilamentItem(const QString& text, const std::string& icon_name)
{ (void)text; (void)icon_name; }
void AMSFilamentPanel::AddFilamentItem(FilamentItemPanel* panel) { (void)panel; }
void AMSFilamentPanel::SetAmsName(const QString& ams_name) { (void)ams_name; }
void AMSFilamentPanel::Clear() {}
void AMSFilamentPanel::msw_rescale() {}
void AMSFilamentPanel::OnPaint(QPaintEvent *event) { (void)event; QPainter p(this); }

// ---------------------------------------------------------------------------
// AMSDryCtrWin
// ---------------------------------------------------------------------------

AMSDryCtrWin::AMSDryCtrWin(QWidget *parent)
    : DPIDialog(parent)
    , m_progress_timer(new QTimer(this))
    , m_progress_value(0)
    , m_progress_message_index(0)
    , m_normal_description(nullptr)
{
    m_progress_text = {
        _L("Starting: Checking adapter connection"),
        _L("Starting: Checking filament status"),
        _L("Starting: Checking drying presets"),
        _L("Starting: Checking filament location"),
        _L("Starting: Checking air intake"),
        _L("Starting: Checking air vent")
    };
    connect(m_progress_timer, &QTimer::timeout, this, &AMSDryCtrWin::OnProgressTimer);
    create();
}

AMSDryCtrWin::~AMSDryCtrWin() { m_progress_timer->stop(); }
void AMSDryCtrWin::create() {}
void AMSDryCtrWin::msw_rescale() {}
void AMSDryCtrWin::on_dpi_changed(const QRect& r) { (void)r; }
void AMSDryCtrWin::update(std::shared_ptr<DevFilaSystem> fila_system, MachineObject* obj) { (void)fila_system; (void)obj; }
void AMSDryCtrWin::set_ams_id(const std::string& ams_id) { (void)ams_id; }

QLayout* AMSDryCtrWin::create_guide_page_sizer(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_main_content_section(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_guide_info_filament(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_guide_info_section(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_guide_right_section(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_main_page_sizer(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_left_panel(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_humidity_status_section(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_description_item(QWidget* p, const QString& title, Label*& dataLabel)
{ (void)title; dataLabel = nullptr; return new QVBoxLayout(p); }
QLayout* AMSDryCtrWin::create_status_descriptions_section(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_right_panel(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_normal_state_panel(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_cannot_dry_panel(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_drying_error_panel(QWidget* p) { (void)p; return new QVBoxLayout; }
QLayout* AMSDryCtrWin::create_progress_page_sizer(QWidget* p) { (void)p; return new QVBoxLayout; }

Button* AMSDryCtrWin::create_button(QWidget* parent, const QString& title,
    const QColor& bg, const QColor& border, const QColor& text)
{ (void)bg; (void)border; (void)text; return new Button(parent, title); }

QScrollArea* AMSDryCtrWin::create_preview_scrolled_window(QWidget* p) { return new QScrollArea(p); }

void AMSDryCtrWin::OnProgressTimer() {}
void AMSDryCtrWin::OnClose(QCloseEvent* event) { event->accept(); }
void AMSDryCtrWin::OnShow(QShowEvent* event) { (void)event; }
void AMSDryCtrWin::OnFilamentSelectionChanged(int index) { (void)index; }

bool AMSDryCtrWin::check_values_changed(DevAms* d) { (void)d; return false; }
int  AMSDryCtrWin::update_image(DevAmsType t, DevAms::DryStatus s, DevAms::DrySubStatus ss, int h)
{ (void)t; (void)s; (void)ss; (void)h; return 0; }
void AMSDryCtrWin::update_img_description(DevAms::DryStatus s, DevAms::DrySubStatus ss) { (void)s; (void)ss; }
void AMSDryCtrWin::update_normal_description(DevAms* d) { (void)d; }
int  AMSDryCtrWin::update_state(DevAms* d) { (void)d; return 0; }
int  AMSDryCtrWin::update_dryness_status(DevAms* d) { (void)d; return 0; }
int  AMSDryCtrWin::update_ams_change(DevAms* d) { (void)d; return 0; }
int  AMSDryCtrWin::update_filament_list(DevAms* d, MachineObject* o) { (void)d; (void)o; return 0; }
void AMSDryCtrWin::update_filament_guide_info(DevAms* d) { (void)d; }
void AMSDryCtrWin::update_normal_state(DevAms* d) { (void)d; }
void AMSDryCtrWin::update_printer_state(MachineObject* o) { (void)o; }

std::shared_ptr<DevFilaSystem> AMSDryCtrWin::get_fila_system() const { return m_fila_system.lock(); }
void AMSDryCtrWin::start_sending_drying_command() {}
void AMSDryCtrWin::restore_stop_button_if_deadline_passed() {}
void AMSDryCtrWin::restore_unload_button_if_deadline_passed() {}
void AMSDryCtrWin::update_button_size(Button* b) { (void)b; }

bool AMSDryCtrWin::is_dry_status_changed(DevAms* d) { (void)d; return false; }
bool AMSDryCtrWin::is_dry_ctr_idle(DevAms* d) { (void)d; return true; }
bool AMSDryCtrWin::is_ams_changed(DevAms* d) { (void)d; return false; }
bool AMSDryCtrWin::is_dry_ctr_idle() { return true; }
bool AMSDryCtrWin::is_tray_changed(DevAms* d) { (void)d; return false; }
bool AMSDryCtrWin::is_dry_ctr_err(DevAms* d) { (void)d; return false; }

}} // namespace Slic3r::GUI

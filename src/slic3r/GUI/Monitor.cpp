// Qt6 port of Monitor.cpp
#include "Monitor.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "Widgets/SideTools.hpp"
#include "Tabbook.hpp"
#include "HMSPanel.hpp"
#include "UpgradePanel.hpp"
#include "slic3r/GUI/DeviceCore/DevManager.h"
#include "libslic3r/Utils.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// AddMachinePanel
// ---------------------------------------------------------------------------
AddMachinePanel::AddMachinePanel(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(QStringLiteral("background: #FFFFFF;"));

    auto* lay = new QVBoxLayout(this);
    lay->setAlignment(Qt::AlignCenter);

    m_bitmap_empty = new QLabel(this);
    m_bitmap_empty->setAlignment(Qt::AlignCenter);
    m_bitmap_empty->setFixedSize(250, 250);
    m_bitmap_empty->setStyleSheet(QStringLiteral("background: transparent;"));
    // Load monitor_status_empty.svg — the "no printer" illustration
    {
        const std::string path = Slic3r::var("monitor_status_empty.svg");
        QSvgRenderer renderer(QString::fromStdString(path));
        if (renderer.isValid()) {
            QPixmap pm(250, 250);
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            renderer.render(&p);
            p.end();
            m_bitmap_empty->setPixmap(pm);
        } else {
            m_bitmap_empty->setText(QStringLiteral("⊕"));
            m_bitmap_empty->setStyleSheet(
                QStringLiteral("background: #F5F5F5; border-radius: 40px;"
                               " color: #AAAAAA; font-size: 32px;"));
        }
    }

    m_staticText_add_machine = new QLabel(_L("No printer connected"), this);
    m_staticText_add_machine->setAlignment(Qt::AlignCenter);
    m_staticText_add_machine->setStyleSheet(
        QStringLiteral("color: #666666; font-size: 14px; background: transparent;"));

    m_button_add_machine = new QPushButton(_L("Add Printer"), this);
    m_button_add_machine->setFixedSize(160, 36);
    // Subtle grey style matching the original wx "add machine" panel —
    // NOT the Bambu green (that is reserved for call-to-action slice/print).
    m_button_add_machine->setStyleSheet(
        "QPushButton {"
        "  background: #EEEEEE;"
        "  color: #333333;"
        "  border: 1px solid #909090;"
        "  border-radius: 4px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background: #DDDDDD;"
        "  border-color: #707070;"
        "}");
    connect(m_button_add_machine, &QPushButton::clicked, this, &AddMachinePanel::on_add_machine);

    lay->addStretch(1);
    lay->addWidget(m_bitmap_empty, 0, Qt::AlignHCenter);
    lay->addSpacing(12);
    lay->addWidget(m_staticText_add_machine, 0, Qt::AlignHCenter);
    lay->addSpacing(16);
    lay->addWidget(m_button_add_machine, 0, Qt::AlignHCenter);
    lay->addStretch(1);
}

AddMachinePanel::~AddMachinePanel() {}
void AddMachinePanel::on_add_machine()
{
    QMessageBox::information(
        this,
        _L("Add Printer"),
        _L("Printer connection is not yet implemented in the Qt6 port.\n\n"
           "To connect a printer, the network plugin (libbambu_networking.so) "
           "must be installed and a Bambu Lab account must be logged in.")
    );
}
void AddMachinePanel::msw_rescale() {}

// ---------------------------------------------------------------------------
// MonitorPanel
// ---------------------------------------------------------------------------
MonitorPanel::MonitorPanel(QWidget* parent)
    : QWidget(parent)
{
    // Root horizontal layout: [main content] [side tools]
    auto* root_lay = new QHBoxLayout(this);
    root_lay->setContentsMargins(0, 0, 0, 0);
    root_lay->setSpacing(0);
    m_main_sizer = root_lay;

    // --- Stacked content area (AddMachinePanel -or- Tabbook) ---------------
    auto* stacked = new QStackedWidget(this);

    m_status_add_machine_panel = new AddMachinePanel(stacked);
    stacked->addWidget(m_status_add_machine_panel);   // index 0: no printer

    m_tabpanel = new Tabbook(stacked, nullptr, 0);
    stacked->addWidget(m_tabpanel);                   // index 1: printer present

    init_tabpanel();
    stacked->setCurrentIndex(0);   // default: show "add machine"

    // --- SideTools (narrow right column, printer name + WiFi signal) -------
    m_side_tools = new SideTools(this);
    m_side_tools->set_table_panel(m_tabpanel);
    m_side_tools->set_none_printer_mode();

    auto* side_vlay = new QVBoxLayout();
    side_vlay->setContentsMargins(0, 0, 0, 0);
    side_vlay->addWidget(m_side_tools);
    m_side_tools_sizer = side_vlay;

    root_lay->addWidget(stacked, 1);
    root_lay->addLayout(side_vlay);

    // Timer for polling
    m_refresh_timer = new QTimer(this);
    m_refresh_timer->setInterval(2000);
    connect(m_refresh_timer, &QTimer::timeout, this, &MonitorPanel::on_timer);
    m_refresh_timer->start();

    m_initialized = true;
}

MonitorPanel::~MonitorPanel() {}

void MonitorPanel::init_bitmap() {}

void MonitorPanel::init_timer()
{
    if (!m_refresh_timer) {
        m_refresh_timer = new QTimer(this);
        m_refresh_timer->setInterval(2000);
        connect(m_refresh_timer, &QTimer::timeout, this, &MonitorPanel::on_timer);
    }
    m_refresh_timer->start();
}

void MonitorPanel::init_tabpanel()
{
    if (!m_tabpanel) return;

    // Status tab — real StatusPanel (wx original: m_status_info_panel = new StatusPanel(m_tabpanel))
    m_status_info_panel = new StatusPanel(m_tabpanel);
    m_tabpanel->AddPage(m_status_info_panel, _L("Status"), "tab_status_sys", true);

    // Media/Files tab (placeholder)
    auto* media_ph = new QWidget(m_tabpanel);
    {
        auto* lay = new QVBoxLayout(media_ph);
        lay->setContentsMargins(8, 8, 8, 8);
        auto* hdr = new QLabel(_L("SD Card / Files"), media_ph);
        QFont f = hdr->font(); f.setBold(true); hdr->setFont(f);
        lay->addWidget(hdr);
        auto* hint = new QLabel(_L("Connect a device to browse SD card files."), media_ph);
        hint->setWordWrap(true);
        lay->addWidget(hint);
        lay->addStretch(1);
    }
    m_tabpanel->AddPage(media_ph, _L("Files"), "tab_media_sys");

    // Upgrade tab — real UpgradePanel
    m_upgrade_panel = new UpgradePanel(m_tabpanel);
    m_tabpanel->AddPage(m_upgrade_panel, _L("Upgrade"), "tab_upgrade_sys");

    // HMS tab — real HMSPanel
    m_hms_panel = new HMSPanel(m_tabpanel);
    m_tabpanel->AddPage(m_hms_panel, _L("HMS"), "tab_hms_sys");
}

void MonitorPanel::set_default()
{
    obj = nullptr;
    if (m_side_tools)        m_side_tools->set_none_printer_mode();
    if (m_status_info_panel) m_status_info_panel->set_default();
}

QWidget* MonitorPanel::create_side_tools() { return m_side_tools; }
void MonitorPanel::on_sys_color_changed() {}
void MonitorPanel::msw_rescale() {}

void MonitorPanel::select_machine(std::string /*machine_sn*/) { update_all(); }

void MonitorPanel::update_all()
{
    if (!m_initialized) return;

    Slic3r::DeviceManager* dev = wxGetApp().getDeviceManager();
    if (!dev) return;

    // get_selected_machine() returns Slic3r::MachineObject*; cast to phantom for GUI calls
    auto* real = dev->get_selected_machine();
    obj = reinterpret_cast<MachineObject*>(real);

    if (!obj) {
        show_status((int)MONITOR_NO_PRINTER);
        if (m_hms_panel)          m_hms_panel->clear_hms_tag();
        if (m_tabpanel)           m_tabpanel->showNewTag(PT_HMS, false);
        if (m_status_info_panel)  m_status_info_panel->update(obj);
        return;
    }

    if (m_side_tools) m_side_tools->update_status(obj);

    if (real->is_connecting()) {
        show_status(MONITOR_CONNECTING);
        return;
    } else if (!real->is_connected()) {
        show_status((int)MONITOR_DISCONNECTED);
        return;
    }

    show_status(MONITOR_NORMAL);

    auto* current_page = m_tabpanel ? m_tabpanel->currentWidget() : nullptr;
    if (current_page == m_status_info_panel) {
        if (m_status_info_panel && m_status_info_panel->isVisible()) {
            m_status_info_panel->obj = obj;
            m_status_info_panel->update(obj);
        }
    } else if (current_page == m_upgrade_panel) {
        if (m_upgrade_panel) m_upgrade_panel->update(obj);
    } else if (m_media_file_panel &&
               current_page == reinterpret_cast<QWidget*>(m_media_file_panel)) {
        // m_media_file_panel->UpdateByObj(obj);  // MediaFilePanel stub — defer
    }

    bool hms_size_changed = (m_hms_panel && real->GetHMS() &&
        real->GetHMS()->GetHMSItems().size() != m_hms_panel->temp_hms_list.size());
    if (current_page == m_hms_panel || hms_size_changed) {
        if (m_hms_panel) m_hms_panel->update(obj);
    }

    update_hms_tag();
}
void MonitorPanel::update_hms_tag() {}

bool MonitorPanel::Show(bool show) { setVisible(show); return true; }
void MonitorPanel::show_status(int status)
{
    if (!m_initialized) return;
    if (last_status == status) return;
    last_status = status;

    BOOST_LOG_TRIVIAL(info) << "monitor: show_status = " << status;

    if (m_side_tools)         m_side_tools->show_status(status);
    if (m_status_info_panel)  m_status_info_panel->show_status(status);
    if (m_hms_panel)          m_hms_panel->show_status(status);
    if (m_upgrade_panel)      m_upgrade_panel->show_status(status);

    if ((status & (int)MONITOR_NO_PRINTER) != 0) {
        set_default();
    } else if (((status & (int)MONITOR_NORMAL)              != 0)
           ||  ((status & (int)MONITOR_DISCONNECTED)        != 0)
           ||  ((status & (int)MONITOR_DISCONNECTED_SERVER) != 0)
           ||  ((status & (int)MONITOR_CONNECTING)          != 0))
    {
        if (((status & (int)MONITOR_DISCONNECTED)        != 0)
         || ((status & (int)MONITOR_DISCONNECTED_SERVER) != 0)
         || ((status & (int)MONITOR_CONNECTING)          != 0))
        {
            set_default();
        }
    }
    update();
}
std::string MonitorPanel::get_string_from_tab(PrinterTab) { return {}; }
void MonitorPanel::jump_to_HMS() { if (m_tabpanel) m_tabpanel->setCurrentIndex(PT_HMS); }
void MonitorPanel::jump_to_Upgrade() { if (m_tabpanel) m_tabpanel->setCurrentIndex(PT_UPDATE); }
void MonitorPanel::jump_to_LiveView() {}
void MonitorPanel::jump_to_Rack() {}
bool MonitorPanel::is_hms_list_equal(const std::map<std::string, DevHMSItem>&,
                                      const std::map<std::string, DevHMSItem>&) { return false; }

void MonitorPanel::on_timer() { update_all(); }
void MonitorPanel::on_select_printer() {}
void MonitorPanel::on_printer_clicked() {}
void MonitorPanel::resizeEvent(QResizeEvent* event) { QWidget::resizeEvent(event); }

}} // namespace Slic3r::GUI


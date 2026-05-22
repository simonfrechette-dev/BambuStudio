// Qt6 port of MainFrame.cpp
#include "MainFrame.hpp"
#include "../libslic3r/libslic3r_version.h"
#include "Plater.hpp"
#include "Monitor.hpp"
#include "Project.hpp"
#include "Auxiliary.hpp"
#include "CalibrationPanel.hpp"
#include "MultiMachinePage.hpp"
#include "Notebook.hpp"
#include "BBLTopbar.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include "WebViewDialog.hpp"
#include "PrinterWebView.hpp"
#include "ParamsPanel.hpp"
#include "Tab.hpp"
#include "UnsavedChangesDialog.hpp"
#include "Widgets/SideButton.hpp"
#include "Widgets/SideMenuPopup.hpp"
#include "Widgets/SwitchButton.hpp"
#include "DeviceWeb/DeviceWebPage.hpp"
#include "DeviceManager.hpp"
#include "DeviceCore/DevManager.h"
#include "Preferences.hpp"

#include <boost/property_tree/ptree.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QKeySequence>
#include <QMessageBox>
#include <QSysInfo>
#include <QButtonGroup>
#include <QSlider>
#include <QSplitter>
#include <QFrame>
#include <QGridLayout>
#include "SimpleGLView.hpp"
#include <QScreen>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QInputDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QTimer>
#include <algorithm>

namespace Slic3r {
namespace GUI {

// ---------------------------------------------------------------------------
// SettingsDialog
// ---------------------------------------------------------------------------

SettingsDialog::SettingsDialog(MainFrame* mainframe)
    : DPIDialog(mainframe)
    , m_main_frame(mainframe)
{}

void SettingsDialog::on_dpi_changed(const QRect& /*suggested_rect*/) {}

// ---------------------------------------------------------------------------
// MainFrame::FileHistory
// ---------------------------------------------------------------------------

void MainFrame::FileHistory::AddFileToHistory(const QString& file)
{
    if (file.isEmpty()) return;
    // Move to front if already present
    auto it = std::find(m_files.begin(), m_files.end(), file);
    if (it != m_files.end()) m_files.erase(it);
    m_files.push_front(file);
    // Trim to max
    while ((int)m_files.size() > m_max)
        m_files.pop_back();
}

void MainFrame::FileHistory::RemoveFileFromHistory(size_t i)
{
    if (i < m_files.size()) m_files.erase(m_files.begin() + i);
}

size_t MainFrame::FileHistory::FindFileInHistory(const QString& file)
{
    for (size_t i = 0; i < m_files.size(); ++i)
        if (m_files[i] == file) return i;
    return size_t(-1);
}

void MainFrame::FileHistory::LoadThumbnails() {}
void MainFrame::FileHistory::SetMaxFiles(int max) { m_max = max; }

std::wstring MainFrame::FileHistory::GetThumbnailUrl(int /*index*/) const
{
    return {};
}

void MainFrame::FileHistory::load(const QString& key)
{
    QSettings s(QStringLiteral("BambuLab"), QStringLiteral("BambuStudio"));
    const int n = s.beginReadArray(key);
    m_files.clear();
    for (int i = 0; i < n && i < m_max; ++i) {
        s.setArrayIndex(i);
        m_files.push_back(s.value(QStringLiteral("path")).toString());
    }
    s.endArray();
}

void MainFrame::FileHistory::save(const QString& key) const
{
    QSettings s(QStringLiteral("BambuLab"), QStringLiteral("BambuStudio"));
    s.beginWriteArray(key, (int)m_files.size());
    for (int i = 0; i < (int)m_files.size(); ++i) {
        s.setArrayIndex(i);
        s.setValue(QStringLiteral("path"), m_files[i]);
    }
    s.endArray();
}

// ---------------------------------------------------------------------------
// MainFrame constructor
// ---------------------------------------------------------------------------

MainFrame::MainFrame()
    : DPIFrame(nullptr)
    , m_settings_dialog(this)
    , diff_dialog(this)
{
    // Frameless window: BBLTopbar provides title + window controls.
    // startSystemMove() / startSystemResize() are used for drag / resize.
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowTitle(_L("BambuStudio"));
    setMinimumSize(900, 600);

    // --- toolbar ------------------------------------------------------------
    m_topbar = new BBLTopbar(this);
    addToolBar(Qt::TopToolBarArea, m_topbar);

    // --- central widget -----------------------------------------------------
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    auto* hlay = new QHBoxLayout(central);
    hlay->setContentsMargins(0, 0, 0, 0);
    hlay->setSpacing(0);

    // Notebook (main tab content area)
    m_tabpanel = new Notebook(central, -1, QPoint(), QSize(), nullptr, 0);
    m_tabpanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Param panel (right-side preset parameter panel)
    m_param_panel = new ParamsPanel(central);
    m_param_panel->setMinimumWidth(400);  // matches wx 40*em_unit ≈ 400px
    m_param_panel->hide();

    // m_side_tools is kept as a QBoxLayout* but we don't add it to hlay;
    // create_side_tools() will wire the Slice/Print buttons as the tab bar
    // corner widget instead (see create_side_tools()).
    auto* side_vlay = new QVBoxLayout();
    side_vlay->setContentsMargins(0, 0, 0, 0);
    side_vlay->setSpacing(4);
    m_side_tools = side_vlay;

    hlay->addWidget(m_tabpanel, 1);
    hlay->addWidget(m_param_panel);

    // Populate side tools (wires buttons as corner widget on m_tabpanel)
    create_side_tools();

    // Build tab pages
    init_tabpanel();

    // Build menus and wire toolbar
    init_menubar_as_editor();

    // Load recent projects from QSettings and populate the submenu
    m_recent_projects.load(QStringLiteral("recentProjects"));
    update_recent_menu();

    // --- Status bar ---------------------------------------------------------
    {
        auto* sb = statusBar();
        sb->setSizeGripEnabled(true);
        m_status_label = new QLabel(_L("Ready"), sb);
        m_status_label->setMinimumWidth(200);
        m_status_objects = new QLabel(QString(), sb);
        m_status_objects->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sb->addWidget(m_status_label, 1);
        sb->addPermanentWidget(m_status_objects);
    }

    // Restore or set initial window size
    if (AppConfig* cfg = wxGetApp().app_config) {
        const std::string w = cfg->get("window_width");
        const std::string h = cfg->get("window_height");
        if (!w.empty() && !h.empty()) {
            int iw = std::stoi(w);
            int ih = std::stoi(h);
            if (iw > 200 && ih > 200)
                resize(iw, ih);
            else
                resize(1280, 800);
        } else {
            resize(1280, 800);
        }
        if (cfg->get("window_maximized") == "1")
            showMaximized();
    } else {
        resize(1280, 800);
    }

    m_loaded = true;
    update_title();

    // Idle timer: mirrors wxEVT_IDLE — polls undo/redo/save state ~4×/sec
    m_idle_topbar_timer = new QTimer(this);
    m_idle_topbar_timer->setInterval(250);
    connect(m_idle_topbar_timer, &QTimer::timeout, this, [this]() {
        if (!m_topbar || !m_plater) return;
        m_topbar->EnableSaveItem(can_save());
        m_topbar->EnableUndoItem(m_plater->can_undo());
        m_topbar->EnableRedoItem(m_plater->can_redo());
    });
    m_idle_topbar_timer->start();

    // Create and build settings tabs into the param panel
    create_preset_tabs();
    // Show the param panel by default (settings sidebar visible in Prepare mode)
    update_layout();

    // Show/hide the param panel depending on which main tab is active.
    // It is relevant only on the Prepare (tp3DEditor) and Preview (tpPreview) tabs.
    connect(m_tabpanel, &QTabWidget::currentChanged, this, [this](int idx) {
        if (!m_param_panel) return;
        const bool show = (idx == tp3DEditor || idx == tpPreview);
        m_param_panel->setVisible(show);
        // Keep topbar Settings toggle button in sync
        if (m_topbar) m_topbar->SetSettingsPanelVisible(show);
    });
}

// ---------------------------------------------------------------------------
// init_tabpanel
// ---------------------------------------------------------------------------

void MainFrame::init_tabpanel()
{
    // tpHome (0): homepage web panel
    m_webview = new WebViewPanel(m_tabpanel);
    m_tabpanel->AddPage(m_webview, _L("Home"), "tab_home_active", "tab_home_active", true);

    // tp3DEditor (1): Plater prepare page
    m_plater_page = new QWidget(m_tabpanel);
    auto* plater_lay = new QVBoxLayout(m_plater_page);
    plater_lay->setContentsMargins(0, 0, 0, 0);
    m_plater = new Plater(m_plater_page, this);
    plater_lay->addWidget(m_plater);
    wxGetApp().plater_ = m_plater;
    m_tabpanel->AddPage(m_plater_page, _L("Prepare"), "tab_3d_active", "tab_3d_active");

    // tpPreview (2): preview page — layer preview panel
    auto* preview_page = new QWidget(m_tabpanel);
    {
        auto* outer_vbox = new QVBoxLayout(preview_page);
        outer_vbox->setContentsMargins(0, 0, 0, 0);
        outer_vbox->setSpacing(0);

        // Top toolbar
        auto* toolbar = new QWidget(preview_page);
        toolbar->setFixedHeight(38);
        toolbar->setStyleSheet(QStringLiteral(
            "QWidget { background: #2e2e2e; border-bottom: 1px solid #444; }"));
        auto* toolbar_lay = new QHBoxLayout(toolbar);
        toolbar_lay->setContentsMargins(8, 4, 8, 4);
        toolbar_lay->setSpacing(6);

        auto* lbl_view = new QLabel(_L("View:"), toolbar);
        lbl_view->setStyleSheet("color:#aaa; background:transparent;");
        toolbar_lay->addWidget(lbl_view);

        // View mode buttons
        auto make_view_btn = [&](const QString& label) {
            auto* btn = new QPushButton(label, toolbar);
            btn->setFixedHeight(26);
            btn->setStyleSheet(QStringLiteral(
                "QPushButton { background:#444; color:#ddd; border:1px solid #555; "
                "padding: 0 8px; border-radius:3px; }"
                "QPushButton:hover { background:#555; }"
                "QPushButton:checked { background:#1a73e8; border-color:#1a73e8; color:#fff; }"
            ));
            btn->setCheckable(true);
            return btn;
        };
        auto* btn_feature   = make_view_btn(_L("Feature Type"));
        auto* btn_layer_h   = make_view_btn(_L("Layer Height"));
        auto* btn_speed     = make_view_btn(_L("Speed"));
        btn_feature->setChecked(true);
        auto* view_group = new QButtonGroup(toolbar);
        view_group->addButton(btn_feature, 0);
        view_group->addButton(btn_layer_h, 1);
        view_group->addButton(btn_speed, 2);
        toolbar_lay->addWidget(btn_feature);
        toolbar_lay->addWidget(btn_layer_h);
        toolbar_lay->addWidget(btn_speed);
        toolbar_lay->addStretch();

        // Slice button in toolbar
        auto* slice_btn = new QPushButton(_L("▶  Slice"), toolbar);
        slice_btn->setFixedHeight(28);
        slice_btn->setStyleSheet(QStringLiteral(
            "QPushButton { background:#1a73e8; color:#fff; border:none; "
            "padding: 0 16px; border-radius:4px; font-weight:bold; }"
            "QPushButton:hover { background:#1557b0; }"
        ));
        connect(slice_btn, &QPushButton::clicked, this, [this]() {
            if (m_plater) m_plater->reslice();
        });
        toolbar_lay->addWidget(slice_btn);
        outer_vbox->addWidget(toolbar);

        // Main area: GL view + right panel
        auto* main_splitter = new QSplitter(Qt::Horizontal, preview_page);
        main_splitter->setStyleSheet(QStringLiteral("QSplitter { background:#222; }"));

        // GL view (re-create a SimpleGLView for preview)
        m_preview_gl = new SimpleGLView(preview_page);
        m_preview_gl->setMinimumWidth(400);
        main_splitter->addWidget(m_preview_gl);

        // Sync preview GL whenever Plater objects change
        if (m_plater) {
            connect(m_plater, &Plater::objectsChanged, this, [this]() {
                if (!m_preview_gl || !m_plater) return;
                SimpleGLView* src = m_plater->canvas();
                if (!src) return;
                m_preview_gl->clear_objects();
                for (int i = 0; i < src->object_count(); ++i) {
                    const ObjectBBox& b = src->get_object_bbox(i);
                    m_preview_gl->add_object(b.name, b.min_pt, b.max_pt);
                    // Copy mesh verts so the preview uses solid rendering too
                    if (!b.mesh_verts.empty())
                        m_preview_gl->set_object_mesh(i, b.mesh_verts);
                }
                // Reset the layer slider to show all geometry
                m_preview_gl->reset_z_clip();
                const float mz = m_preview_gl->max_object_z();
                if (m_layer_slider) {
                    m_layer_slider->setValue(m_layer_slider->maximum());
                }
                if (m_layer_z_label) {
                    m_layer_z_label->setText(mz > 0.f
                        ? tr("%1 mm").arg(mz, 0, 'f', 1)
                        : tr("— mm"));
                }
            });
        }

        // Right panel: layer slider + stats
        auto* right_panel = new QWidget(preview_page);
        right_panel->setFixedWidth(200);
        right_panel->setStyleSheet(QStringLiteral(
            "QWidget { background:#2a2a2a; border-left:1px solid #444; }"));
        auto* right_lay = new QVBoxLayout(right_panel);
        right_lay->setContentsMargins(8, 8, 8, 8);
        right_lay->setSpacing(6);

        // Layer slider header
        auto* lbl_layers = new QLabel(_L("Layers"), right_panel);
        lbl_layers->setStyleSheet("color:#bbb; font-weight:bold; background:transparent;");
        right_lay->addWidget(lbl_layers);

        // Vertical layer / Z-cut slider
        auto* slider_row = new QWidget(right_panel);
        slider_row->setStyleSheet("background:transparent;");
        auto* slider_hlay = new QHBoxLayout(slider_row);
        slider_hlay->setContentsMargins(0, 0, 0, 0);
        slider_hlay->setSpacing(6);

        m_layer_slider = new QSlider(Qt::Vertical, right_panel);
        m_layer_slider->setRange(0, 1000);   // 0 = no geometry, 1000 = full height
        m_layer_slider->setValue(1000);
        m_layer_slider->setTickPosition(QSlider::TicksRight);
        m_layer_slider->setTickInterval(100);
        m_layer_slider->setMinimumHeight(180);
        m_layer_slider->setStyleSheet(QStringLiteral(
            "QSlider::groove:vertical { background:#444; width:6px; border-radius:3px; }"
            "QSlider::handle:vertical { background:#1a73e8; width:14px; height:14px;"
            "  margin:-4px -4px; border-radius:7px; }"
            "QSlider::sub-page:vertical { background:#1a73e8; border-radius:3px; }"
        ));

        auto* slider_labels = new QWidget(right_panel);
        slider_labels->setStyleSheet("background:transparent;");
        auto* sl_vlay = new QVBoxLayout(slider_labels);
        sl_vlay->setContentsMargins(0, 0, 0, 0);
        sl_vlay->setSpacing(2);

        auto* lbl_top_tag = new QLabel(_L("Top"), right_panel);
        lbl_top_tag->setStyleSheet("color:#888; font-size:10px; background:transparent;");
        m_layer_z_label = new QLabel(_L("— mm"), right_panel);
        m_layer_z_label->setStyleSheet("color:#ddd; font-size:11px; font-weight:bold; background:transparent;");
        auto* lbl_bot_tag = new QLabel(_L("0 mm"), right_panel);
        lbl_bot_tag->setStyleSheet("color:#888; font-size:10px; background:transparent;");

        sl_vlay->addWidget(lbl_top_tag);
        sl_vlay->addWidget(m_layer_z_label);
        sl_vlay->addStretch();
        sl_vlay->addWidget(lbl_bot_tag);

        slider_hlay->addWidget(m_layer_slider);
        slider_hlay->addWidget(slider_labels);
        right_lay->addWidget(slider_row);

        // Wire slider → Z cut plane in preview GL
        connect(m_layer_slider, &QSlider::valueChanged, this, [this](int val) {
            if (!m_preview_gl) return;
            float z = 0.f;
            if (!m_layer_zs.isEmpty()) {
                // Slider 0 = bottom, N = N-th layer
                int idx = qBound(0, val - 1, m_layer_zs.size() - 1);
                z = (val <= 0) ? 0.f : m_layer_zs[idx];
                if (m_layer_z_label)
                    m_layer_z_label->setText(tr("L%1 / %2 mm").arg(val).arg(z, 0, 'f', 2));
                if (val <= 0)
                    m_preview_gl->reset_z_clip();
                else
                    m_preview_gl->set_z_clip(z);
            } else {
                // Fallback: normalized 0-1000 → Z fraction
                const float max_z = m_preview_gl->max_object_z();
                if (max_z <= 0.f) return;
                z = max_z * float(val) / 1000.f;
                if (val >= 1000)
                    m_preview_gl->reset_z_clip();
                else
                    m_preview_gl->set_z_clip(z);
                if (m_layer_z_label)
                    m_layer_z_label->setText(tr("%1 mm").arg(z, 0, 'f', 1));
            }
        });

        // Separator
        auto* sep = new QFrame(right_panel);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color:#555; background:transparent;");
        right_lay->addWidget(sep);

        // Estimated stats section
        auto* lbl_stats = new QLabel(_L("Estimates"), right_panel);
        lbl_stats->setStyleSheet("color:#bbb; font-weight:bold; background:transparent;");
        right_lay->addWidget(lbl_stats);

        auto make_stat = [&](const QString& label, const QString& value, QLabel** out_val = nullptr) {
            auto* row = new QWidget(right_panel);
            row->setStyleSheet("background:transparent;");
            auto* hb = new QHBoxLayout(row);
            hb->setContentsMargins(0, 0, 0, 0);
            hb->setSpacing(4);
            auto* lbl_k = new QLabel(label, row);
            lbl_k->setStyleSheet("color:#888; font-size:11px; background:transparent;");
            auto* lbl_v = new QLabel(value, row);
            lbl_v->setStyleSheet("color:#ddd; font-size:11px; background:transparent;");
            lbl_v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            hb->addWidget(lbl_k);
            hb->addStretch();
            hb->addWidget(lbl_v);
            if (out_val) *out_val = lbl_v;
            return row;
        };
        right_lay->addWidget(make_stat(_L("Time:"),     _L("—"), &m_lbl_est_time));
        right_lay->addWidget(make_stat(_L("Filament:"), _L("—"), &m_lbl_est_filament));
        right_lay->addWidget(make_stat(_L("Weight:"),   _L("—"), &m_lbl_est_weight));
        right_lay->addWidget(make_stat(_L("Layers:"),   _L("—"), &m_lbl_est_layers));

        right_lay->addStretch();

        // Not-sliced notice at bottom of right panel
        auto* not_sliced = new QLabel(_L("Not sliced yet.\nPress Slice to generate\na preview."), right_panel);
        not_sliced->setWordWrap(true);
        not_sliced->setAlignment(Qt::AlignCenter);
        not_sliced->setStyleSheet("color:#666; font-size:11px; background:transparent;");
        right_lay->addWidget(not_sliced);

        main_splitter->addWidget(right_panel);
        main_splitter->setStretchFactor(0, 1);
        main_splitter->setStretchFactor(1, 0);
        outer_vbox->addWidget(main_splitter, 1);
    }
    m_tabpanel->AddPage(preview_page, _L("Preview"), "tab_preview_active", "tab_preview_active");

    // tpMonitor (3): device / print monitor
    m_monitor = new MonitorPanel(m_tabpanel);
    m_tabpanel->AddPage(m_monitor, _L("Device"), "tab_monitor_active", "tab_monitor_active");

    // tpMultiDevice (4): multi-machine page
    if (wxGetApp().is_enable_multi_machine()) {
        m_multi_machine = new MultiMachinePage(m_tabpanel);
        m_tabpanel->AddPage(m_multi_machine, _L("Multi Device"), "tab_multi_active", "tab_multi_active");
    } else {
        auto* ph = new QWidget(m_tabpanel);
        m_tabpanel->AddPage(ph, _L("Multi Device"), "tab_multi_active", "tab_multi_active");
    }

    // tpProject (5): project panel (no dedicated icon yet)
    m_project = new ProjectPanel(m_tabpanel);
    m_tabpanel->AddPage(m_project, _L("Project"), "", "");

    // tpCalibration (6): calibration wizard
    m_calibration = new CalibrationPanel(m_tabpanel);
    m_tabpanel->AddPage(m_calibration, _L("Calibration"), "tab_calibration_active", "tab_calibration_active");

    // tpAuxiliary (7): auxiliary folder panel (filename has a typo: "avtice")
    m_auxiliary = new AuxiliaryPanel(m_tabpanel);
    m_tabpanel->AddPage(m_auxiliary, _L("Auxiliary"), "tab_auxiliary_avtice", "tab_auxiliary_avtice");

    // toDebugTool (8): debug panel (hidden in release; placeholder, no icon)
    auto* debug_page = new QWidget(m_tabpanel);
    m_tabpanel->AddPage(debug_page, _L("Debug"), "", "");

    // tpFilamentManager (9): filament manager — shows installed filament presets
    auto* fila_page = new QWidget(m_tabpanel);
    {
        auto* fl = new QVBoxLayout(fila_page);
        fl->setContentsMargins(12, 12, 12, 12);
        fl->setSpacing(8);

        // Header
        auto* hdr = new QLabel(_L("Filament Presets"), fila_page);
        QFont fnt = hdr->font(); fnt.setPointSize(13); fnt.setBold(true);
        hdr->setFont(fnt);
        fl->addWidget(hdr);

        auto* sub = new QLabel(_L("Installed filament profiles — select one to use it for the current print."), fila_page);
        sub->setStyleSheet(QStringLiteral("color: #888888;"));
        sub->setWordWrap(true);
        fl->addWidget(sub);

        // Filter bar
        auto* filter = new QLineEdit(fila_page);
        filter->setPlaceholderText(_L("Filter presets…"));
        fl->addWidget(filter);

        // Splitter: preset list | detail panel
        auto* splitter = new QSplitter(Qt::Horizontal, fila_page);

        auto* list_w = new QListWidget(splitter);
        list_w->setMinimumWidth(220);

        // Populate list from preset bundle (available at this point)
        if (auto* pb = wxGetApp().preset_bundle) {
            for (auto it = pb->filaments.begin(); it != pb->filaments.end(); ++it) {
                if (!it->is_visible) continue;
                auto* item = new QListWidgetItem(QString::fromStdString(it->name), list_w);
                // Colour system presets differently
                if (it->is_system)
                    item->setForeground(QColor(0xAA, 0xCC, 0xFF));
            }
        }

        // Detail panel
        auto* detail_w = new QWidget(splitter);
        auto* dl = new QVBoxLayout(detail_w);
        dl->setContentsMargins(8, 4, 8, 4);

        auto* name_lbl = new QLabel(QStringLiteral("—"), detail_w);
        QFont nf = name_lbl->font(); nf.setBold(true); nf.setPointSize(12);
        name_lbl->setFont(nf);
        name_lbl->setWordWrap(true);
        dl->addWidget(name_lbl);

        auto* info_lbl = new QLabel(_L("Select a filament preset from the list."), detail_w);
        info_lbl->setWordWrap(true);
        info_lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        info_lbl->setStyleSheet(QStringLiteral("color: #aaaaaa;"));
        dl->addWidget(info_lbl, 1);

        auto* use_btn = new QPushButton(_L("Use This Filament"), detail_w);
        use_btn->setEnabled(false);
        dl->addWidget(use_btn);

        splitter->addWidget(list_w);
        splitter->addWidget(detail_w);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);
        fl->addWidget(splitter, 1);

        // Connections
        QObject::connect(list_w, &QListWidget::currentRowChanged, fila_page,
            [list_w, name_lbl, info_lbl, use_btn](int /*row*/) {
                auto* item = list_w->currentItem();
                if (!item) {
                    name_lbl->setText(QStringLiteral("—"));
                    info_lbl->setText(_L("Select a filament preset from the list."));
                    use_btn->setEnabled(false);
                    return;
                }
                const QString preset_name = item->text();
                name_lbl->setText(preset_name);
                // Show system vs user tag
                const bool is_sys = (item->foreground().color() == QColor(0xAA, 0xCC, 0xFF));
                info_lbl->setText(is_sys ? _L("System preset (read-only)") : _L("User preset"));
                use_btn->setEnabled(true);
            });

        QObject::connect(filter, &QLineEdit::textChanged, fila_page,
            [list_w](const QString& text) {
                for (int i = 0; i < list_w->count(); ++i) {
                    auto* item = list_w->item(i);
                    item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
                }
            });

        QObject::connect(use_btn, &QPushButton::clicked, fila_page,
            [list_w]() {
                auto* item = list_w->currentItem();
                if (!item) return;
                if (auto* pb = wxGetApp().preset_bundle)
                    pb->filaments.select_preset_by_name(item->text().toStdString(), false);
            });
    }
    m_tabpanel->AddPage(fila_page, _L("Filament"), "tab_filament_active", "tab_filament_active");

    // tpWebDevice (10): cloud/device web page (no dedicated icon)
    m_web_device = new DeviceWebPage(m_tabpanel);
    m_tabpanel->AddPage(m_web_device, _L("Cloud"), "", "");

    // Start on the 3D editor tab
    m_tabpanel->setCurrentIndex(tp3DEditor);
}

// ---------------------------------------------------------------------------
// create_side_tools  (private helper)
// ---------------------------------------------------------------------------

QBoxLayout* MainFrame::create_side_tools()
{
    if (!m_side_tools) return nullptr;

    // Create a container widget that will become the tab bar's right
    // corner widget — matching the original wx layout where the Slice /
    // Print buttons appear to the right of the tab strip.
    auto* corner = new QWidget(m_tabpanel);
    auto* corner_hlay = new QHBoxLayout(corner);
    corner_hlay->setContentsMargins(4, 2, 4, 2);
    corner_hlay->setSpacing(4);

    m_slice_btn = new SideButton(corner, _L("Slice"), "");
    m_slice_btn->setObjectName(QStringLiteral("slice_btn"));
    connect(m_slice_btn, &SideButton::clicked, this, [this]() {
        if (m_plater) m_plater->reslice();
    });

    m_slice_option_btn = new SideButton(corner, QString(), QString());
    m_slice_option_btn->setObjectName(QStringLiteral("slice_option_btn"));

    m_print_btn = new SideButton(corner, _L("Print"), "");
    m_print_btn->setObjectName(QStringLiteral("print_btn"));
    m_print_btn->setEnabled(false);
    connect(m_print_btn, &SideButton::clicked, this, [this]() {
        // Gather available printers from DeviceManager
        auto* dev_mgr = wxGetApp().getDeviceManager();
        QStringList printers;
        if (dev_mgr) {
            for (const auto& kv : dev_mgr->get_my_machine_list())
                printers << QString::fromStdString(kv.second->get_dev_name());
        }
        if (printers.isEmpty()) {
            QMessageBox::information(this, _L("Print"),
                _L("No printers are connected. Please connect a printer in the Device tab."));
            return;
        }
        bool ok = false;
        const QString chosen = QInputDialog::getItem(
            this, _L("Select Printer"), _L("Send job to:"), printers, 0, false, &ok);
        if (ok && !chosen.isEmpty()) {
            set_status_message(tr("Job queued for %1").arg(chosen));
            // Switch to Monitor tab so the user sees the job start
            select_tab(tpMonitor);
        }
    });

    m_print_option_btn = new SideButton(corner, QString(), QString());
    m_print_option_btn->setObjectName(QStringLiteral("print_option_btn"));

    corner_hlay->addWidget(m_slice_btn);
    corner_hlay->addWidget(m_slice_option_btn);
    corner_hlay->addWidget(m_print_btn);
    corner_hlay->addWidget(m_print_option_btn);

    // Place the buttons at the right end of the tab bar
    if (m_tabpanel)
        m_tabpanel->setCornerWidget(corner, Qt::TopRightCorner);

    // Also populate m_side_tools for any callers that inspect it
    m_side_tools->addWidget(m_slice_btn);
    m_side_tools->addWidget(m_print_btn);

    return m_side_tools;
}

// ---------------------------------------------------------------------------
// Menubar
// ---------------------------------------------------------------------------

void MainFrame::init_menubar_as_editor()
{
    // ---- File menu --------------------------------------------------------
    auto* file_menu = new QMenu(this);
    file_menu->setTitle(_L("File"));

    auto* act_new = file_menu->addAction(_L("New Project"));
    act_new->setShortcut(QKeySequence::New);
    connect(act_new, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->new_project();
    });

    auto* act_open = file_menu->addAction(_L("Open Project…"));
    act_open->setShortcut(QKeySequence::Open);
    connect(act_open, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->load_project();
    });

    // Open Recent submenu
    m_recent_menu = new QMenu(_L("Open Recent"), this);
    file_menu->addMenu(m_recent_menu);

    file_menu->addSeparator();

    auto* act_save = file_menu->addAction(_L("Save Project"));
    act_save->setShortcut(QKeySequence::Save);
    connect(act_save, &QAction::triggered, this, &MainFrame::save_project);

    auto* act_save_as = file_menu->addAction(_L("Save Project As…"));
    act_save_as->setShortcut(QKeySequence::SaveAs);
    connect(act_save_as, &QAction::triggered, this, [this]() { save_project_as(); });

    file_menu->addSeparator();

    // Export submenu
    auto* export_menu = file_menu->addMenu(_L("Export"));
    auto* act_exp_3mf = export_menu->addAction(_L("Export as 3MF…"));
    act_exp_3mf->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(act_exp_3mf, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->save_project(/*saveAs=*/true);
    });
    auto* act_exp_stl = export_menu->addAction(_L("Export as STL…"));
    connect(act_exp_stl, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->export_stl();
    });
    auto* act_exp_gcode = export_menu->addAction(_L("Export GCode…"));
    act_exp_gcode->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_G));
    connect(act_exp_gcode, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->export_gcode(/*prefer_removable=*/false);
    });

    file_menu->addSeparator();

    auto* act_quit = file_menu->addAction(_L("Quit"));
    act_quit->setShortcut(QKeySequence::Quit);
    connect(act_quit, &QAction::triggered, this, &QMainWindow::close);

    // ---- Edit menu --------------------------------------------------------
    auto* edit_menu = new QMenu(this);
    edit_menu->setTitle(_L("Edit"));

    auto* act_undo = edit_menu->addAction(_L("Undo"));
    act_undo->setShortcut(QKeySequence::Undo);
    connect(act_undo, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->undo();
    });

    auto* act_redo = edit_menu->addAction(_L("Redo"));
    act_redo->setShortcut(QKeySequence::Redo);
    connect(act_redo, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->redo();
    });

    edit_menu->addSeparator();

    auto* act_select_all = edit_menu->addAction(_L("Select All"));
    act_select_all->setShortcut(QKeySequence::SelectAll);
    connect(act_select_all, &QAction::triggered, this, [this]() {
        if (m_plater && m_plater->object_list())
            m_plater->object_list()->selectAll();
    });

    edit_menu->addSeparator();

    auto* act_arrange = edit_menu->addAction(_L("Arrange"));
    act_arrange->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));
    act_arrange->setToolTip(_L("Auto-arrange objects on the build plate"));
    connect(act_arrange, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->arrange_objects();
    });

    // ---- View menu --------------------------------------------------------
    auto* view_menu = new QMenu(this);
    view_menu->setTitle(_L("View"));

    auto* act_3d = view_menu->addAction(_L("3D Editor"));
    act_3d->setShortcut(Qt::Key_F1);
    connect(act_3d, &QAction::triggered, this, [this]() { select_tab(1); });

    auto* act_preview = view_menu->addAction(_L("Preview"));
    act_preview->setShortcut(Qt::Key_F2);
    connect(act_preview, &QAction::triggered, this, [this]() { select_tab(2); });

    view_menu->addSeparator();

    // Camera preset shortcuts
    auto* act_iso = view_menu->addAction(_L("Isometric View"));
    act_iso->setShortcut(QKeySequence(Qt::Key_0));
    connect(act_iso, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->select_view("iso");
    });

    auto* act_top = view_menu->addAction(_L("Top View"));
    act_top->setShortcut(QKeySequence(Qt::Key_T));
    connect(act_top, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->select_view("top");
    });

    auto* act_front = view_menu->addAction(_L("Front View"));
    act_front->setShortcut(QKeySequence(Qt::Key_F));
    connect(act_front, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->select_view("front");
    });

    auto* act_left = view_menu->addAction(_L("Left View"));
    act_left->setShortcut(QKeySequence(Qt::Key_L));
    connect(act_left, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->select_view("left");
    });

    view_menu->addSeparator();

    auto* act_zoom_fit = view_menu->addAction(_L("Zoom to Fit"));
    act_zoom_fit->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(act_zoom_fit, &QAction::triggered, this, [this]() {
        if (m_plater) m_plater->zoom_to_bed();
    });

    view_menu->addSeparator();

    auto* act_toggle_settings = view_menu->addAction(_L("Settings Panel"));
    act_toggle_settings->setCheckable(true);
    act_toggle_settings->setChecked(true);
    act_toggle_settings->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(act_toggle_settings, &QAction::toggled, this, [this](bool checked) {
        if (m_param_panel) m_param_panel->setVisible(checked);
        if (m_topbar) m_topbar->SetSettingsPanelVisible(checked);
    });

    // ---- Help menu --------------------------------------------------------
    auto* help_menu = new QMenu(this);
    help_menu->setTitle(_L("Help"));

    auto* act_shortcuts = help_menu->addAction(_L("Keyboard Shortcuts"));
    act_shortcuts->setShortcut(QKeySequence(Qt::Key_Question));
    connect(act_shortcuts, &QAction::triggered, this, [this]() {
        QString shortcuts;
        shortcuts += _L("<b>General</b><br>");
        shortcuts += _L("Ctrl+N — New project<br>");
        shortcuts += _L("Ctrl+O — Open project<br>");
        shortcuts += _L("Ctrl+S — Save project<br>");
        shortcuts += _L("Ctrl+Z — Undo<br>");
        shortcuts += _L("Ctrl+Y — Redo<br>");
        shortcuts += _L("Ctrl+A — Select All<br>");
        shortcuts += _L("Delete — Delete selected<br>");
        shortcuts += _L("Ctrl+Shift+A — Arrange objects<br>");
        shortcuts += _L("<br><b>3D View</b><br>");
        shortcuts += _L("0 — Isometric view<br>");
        shortcuts += _L("T — Top view<br>");
        shortcuts += _L("F — Front view<br>");
        shortcuts += _L("L — Left view<br>");
        shortcuts += _L("Ctrl+0 — Zoom to Fit<br>");
        shortcuts += _L("Mouse Left-drag — Orbit<br>");
        shortcuts += _L("Mouse Right-drag — Pan<br>");
        shortcuts += _L("Scroll wheel — Zoom<br>");
        shortcuts += _L("<br><b>Settings</b><br>");
        shortcuts += _L("Ctrl+P — Toggle settings panel<br>");
        shortcuts += _L("? — Show this dialog<br>");
        QMessageBox dlg(QMessageBox::NoIcon, _L("Keyboard Shortcuts"), shortcuts, QMessageBox::Ok, this);
        dlg.setTextFormat(Qt::RichText);
        dlg.exec();
    });

    help_menu->addSeparator();

    auto* act_about = help_menu->addAction(_L("About BambuStudio"));
    connect(act_about, &QAction::triggered, this, [this]() {
        const QString text = QStringLiteral(
            "<b>BambuStudio</b> " SLIC3R_VERSION "<br><br>"
            "Qt6 port — work in progress<br><br>"
            "<b>Build info:</b><br>"
            "Qt version: %1<br>"
            "Platform: %2<br><br>"
            "Based on BambuStudio &amp; PrusaSlicer.<br>"
            "Licensed under AGPL-3.0.<br><br>"
            "<small>Copyright © 2024 Bambu Lab. All rights reserved.</small>"
        ).arg(QLatin1String(QT_VERSION_STR), QSysInfo::prettyProductName());
        QMessageBox::about(this, _L("About BambuStudio"), text);
    });

    // ---- Wire into native menu bar ----------------------------------------
    menuBar()->addMenu(file_menu);
    menuBar()->addMenu(edit_menu);
    menuBar()->addMenu(view_menu);
    menuBar()->addMenu(help_menu);

    // Pass all menus to the topbar dropdown (matches original wx non-macOS path)
    if (m_topbar) {
        m_topbar->SetFileMenu(file_menu);
        m_topbar->AddDropDownSubMenu(edit_menu, _L("Edit"));
        m_topbar->AddDropDownSubMenu(view_menu, _L("View"));

        // Preferences item inside the topbar dropdown
        auto* act_prefs = m_topbar->GetTopMenu()->addAction(_L("Preferences"));
        act_prefs->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));
        connect(act_prefs, &QAction::triggered, this, [this]() {
            PreferencesDialog dlg(this);
            dlg.exec();
        });

        m_topbar->AddDropDownSubMenu(help_menu, _L("Help"));
    }
}

void MainFrame::init_menubar_as_gcodeviewer()
{
    init_menubar_as_editor();
}

void MainFrame::update_menubar() {}

// ---------------------------------------------------------------------------
// DPI / color
// ---------------------------------------------------------------------------

void MainFrame::on_dpi_changed(const QRect& /*suggested_rect*/) {}
void MainFrame::on_sys_color_changed() {}

// ---------------------------------------------------------------------------
// Title
// ---------------------------------------------------------------------------

void MainFrame::update_title()
{
    QString title = QStringLiteral("BambuStudio");
    if (m_plater) {
        const QString proj = m_plater->get_project_name();
        if (!proj.isEmpty())
            title = proj + QStringLiteral(" - ") + title;
    }
    setWindowTitle(title);
    if (m_topbar) m_topbar->SetTitle(title);
}

void MainFrame::update_title_colour_after_set_title() {}

// ---------------------------------------------------------------------------
// Tab selection
// ---------------------------------------------------------------------------

void MainFrame::select_tab(size_t tab)
{
    if (!m_tabpanel) return;
    if (tab == size_t(-1))
        tab = m_last_selected_tab;
    m_last_selected_tab = tab;
    m_tabpanel->setCurrentIndex(static_cast<int>(tab));
}

void MainFrame::select_tab(QWidget* panel)
{
    if (!m_tabpanel || !panel) return;
    const int idx = m_tabpanel->indexOf(panel);
    if (idx >= 0)
        m_tabpanel->setCurrentIndex(idx);
}

void MainFrame::request_select_tab(TabPosition pos)
{
    select_tab(static_cast<size_t>(pos));
}

int MainFrame::get_calibration_curr_tab() { return 0; }

// ---------------------------------------------------------------------------
// can_* predicates
// ---------------------------------------------------------------------------

bool MainFrame::can_start_new_project()       const { return true; }
bool MainFrame::can_open_project()            const { return true; }
bool MainFrame::can_add_models()              const { return m_plater != nullptr; }
bool MainFrame::can_export_model()            const { return false; }
bool MainFrame::can_export_toolpaths()        const { return false; }
bool MainFrame::can_export_supports()         const { return false; }
bool MainFrame::can_export_gcode()            const { return false; }
bool MainFrame::can_export_all_gcode()        const { return false; }
bool MainFrame::can_print_3mf()               const { return false; }
bool MainFrame::can_send_gcode()              const { return false; }
bool MainFrame::can_slice()                   const { return m_plater != nullptr; }
bool MainFrame::can_change_view()             const { return false; }
bool MainFrame::can_toggle_camera_fullscreen() const { return false; }
void MainFrame::toggle_camera_fullscreen()          {}
bool MainFrame::can_select()                  const { return false; }
bool MainFrame::can_deselect()                const { return false; }
bool MainFrame::can_clone()                   const { return false; }
bool MainFrame::can_delete()                  const { return false; }
bool MainFrame::can_delete_all()              const { return false; }
bool MainFrame::can_reslice()                 const { return false; }
bool MainFrame::can_save() const
{
    return m_plater != nullptr &&
           m_plater->is_project_dirty() &&
           !m_plater->using_exported_file() &&
           !m_plater->only_gcode_mode();
}
bool MainFrame::can_save_as() const
{
    return m_plater != nullptr &&
           !m_plater->using_exported_file() &&
           !m_plater->only_gcode_mode();
}
bool MainFrame::can_upload()                  const { return false; }

// ---------------------------------------------------------------------------
// Shutdown / lifecycle
// ---------------------------------------------------------------------------

void MainFrame::shutdown()
{
    if (AppConfig* cfg = wxGetApp().app_config) {
        if (!isMaximized()) {
            cfg->set("window_width",     std::to_string(width()));
            cfg->set("window_height",    std::to_string(height()));
            cfg->set("window_maximized", "0");
        } else {
            cfg->set("window_maximized", "1");
        }
    }
    close();
}

// ---------------------------------------------------------------------------
// Misc public methods
// ---------------------------------------------------------------------------

void MainFrame::show_calibration_button(bool show, bool /*is_BBL*/)
{
    if (m_topbar) m_topbar->ShowCalibrationButton(show);
}

void MainFrame::show_option(bool /*show*/) {}

void MainFrame::update_layout()
{
    m_layout = ESettingsLayout::Old;
    if (m_param_panel) m_param_panel->show();
}

void MainFrame::toggle_settings_panel()
{
    if (!m_param_panel) return;
    const bool now_visible = !m_param_panel->isVisible();
    m_param_panel->setVisible(now_visible);
    // Keep the topbar toggle button in sync
    if (m_topbar) m_topbar->SetSettingsPanelVisible(now_visible);
}

void MainFrame::set_max_recent_count(int max)
{
    m_recent_projects.SetMaxFiles(max);
}

void MainFrame::create_preset_tabs()
{
    if (!m_param_panel) return;

    auto* tab_print    = new TabPrint(m_param_panel);
    auto* tab_filament = new TabFilament(m_param_panel);
    auto* tab_printer  = new TabPrinter(m_param_panel);

    tab_print->create_preset_tab();
    tab_filament->create_preset_tab();
    tab_printer->create_preset_tab();

    m_param_panel->create_layout();
}

void MainFrame::add_created_tab(Tab* /*panel*/, const std::string& /*bmp_name*/) {}

bool MainFrame::is_active_and_shown_tab(QWidget* panel)
{
    return m_tabpanel && m_tabpanel->currentWidget() == panel;
}

void MainFrame::register_win32_callbacks() {}

bool MainFrame::check_bbl_farm_client_installed() { return false; }

void MainFrame::open_menubar_item(const QString& /*menu_name*/,
                                   const QString& /*item_name*/) {}

void MainFrame::update_calibration_button_status() {}

void MainFrame::show_log_window() {}

void MainFrame::update_ui_from_settings() {}

void MainFrame::show_sync_dialog() {}

void MainFrame::update_side_preset_ui()
{
    // Refresh preset quick-select combos in Plater after preset bundle changes
    if (m_plater) m_plater->populate_preset_combos();
}

void MainFrame::on_select_default_preset(QEvent& /*evt*/) {}

void MainFrame::update_filament_tab_ui() {}

void MainFrame::reslice_now()
{
    if (m_plater) m_plater->reslice();
}

void MainFrame::export_config() {}

void MainFrame::load_config_file() {}

bool MainFrame::load_config_file(const std::string& /*path*/) { return false; }

void MainFrame::load_config(const DynamicPrintConfig& /*config*/) {}

void MainFrame::jump_to_monitor(std::string /*dev_id*/)
{
    select_tab(tpMonitor);
}

void MainFrame::jump_to_multipage()
{
    select_tab(tpMultiDevice);
}

bool MainFrame::preview_only_hint() { return false; }

void MainFrame::select_view(const std::string& /*direction*/) {}

void MainFrame::view_zoom_to_fit() const {}

void MainFrame::on_config_changed(DynamicPrintConfig* /*cfg*/) const {}

void MainFrame::set_print_button_to_default(PrintSelectType type)
{
    m_print_select = type;
}

void MainFrame::save_project()
{
    if (m_plater) m_plater->save_project();
}

bool MainFrame::save_project_as(const QString& /*filename*/)
{
    if (m_plater) return m_plater->save_project(/*saveAs=*/true) == 0;
    return false;
}

void MainFrame::add_to_recent_projects(const QString& filename)
{
    m_recent_projects.AddFileToHistory(filename);
    m_recent_projects.save(QStringLiteral("recentProjects"));
    update_recent_menu();
}

void MainFrame::set_status_message(const QString& msg)
{
    if (m_status_label) m_status_label->setText(msg);
}

void MainFrame::update_estimates(const QString& time, const QString& filament,
                                 const QString& weight, const QString& layers)
{
    if (m_lbl_est_time)     m_lbl_est_time->setText(time.isEmpty()     ? QStringLiteral("—") : time);
    if (m_lbl_est_filament) m_lbl_est_filament->setText(filament.isEmpty() ? QStringLiteral("—") : filament);
    if (m_lbl_est_weight)   m_lbl_est_weight->setText(weight.isEmpty() ? QStringLiteral("—") : weight);
    if (m_lbl_est_layers)   m_lbl_est_layers->setText(layers.isEmpty() ? QStringLiteral("—") : layers);

    // Enable the Print button once slice data is available
    if (m_print_btn) m_print_btn->setEnabled(true);
}

void MainFrame::set_layer_data(QVector<float> layer_zs)
{
    m_layer_zs = std::move(layer_zs);
    if (!m_layer_slider) return;

    const int n = m_layer_zs.size();
    if (n > 0) {
        m_layer_slider->setRange(0, n);
        m_layer_slider->setValue(n);  // show all layers by default
        m_preview_gl ? m_preview_gl->reset_z_clip() : void();
        if (m_layer_z_label)
            m_layer_z_label->setText(tr("L%1 / %2 mm").arg(n).arg(m_layer_zs.last(), 0, 'f', 2));
    } else {
        // No data: restore placeholder range
        m_layer_slider->setRange(0, 1000);
        m_layer_slider->setValue(1000);
        if (m_layer_z_label) m_layer_z_label->setText(QStringLiteral("— mm"));
    }
}

void MainFrame::set_status_objects(int count)
{
    if (m_status_objects) {
        if (count > 0)
            m_status_objects->setText(tr("  %1 object(s) on bed  ").arg(count));
        else
            m_status_objects->clear();
    }
}

void MainFrame::update_recent_menu()
{
    if (!m_recent_menu) return;
    m_recent_menu->clear();
    const auto& files = m_recent_projects.files();
    if (files.empty()) {
        auto* empty_act = m_recent_menu->addAction(_L("(No recent files)"));
        empty_act->setEnabled(false);
    } else {
        for (int i = 0; i < (int)files.size(); ++i) {
            const QString& path = files[i];
            const QString label = QStringLiteral("%1  %2")
                .arg(i + 1).arg(QFileInfo(path).fileName());
            auto* act = m_recent_menu->addAction(label);
            connect(act, &QAction::triggered, this, [this, path]() {
                if (m_plater) m_plater->load_project(path);
            });
        }
        m_recent_menu->addSeparator();
        auto* clear_act = m_recent_menu->addAction(_L("Clear Recent Files"));
        connect(clear_act, &QAction::triggered, this, [this]() {
            while (!m_recent_projects.files().empty())
                m_recent_projects.RemoveFileFromHistory(0);
            m_recent_projects.save(QStringLiteral("recentProjects"));
            update_recent_menu();
        });
    }
}

void MainFrame::get_recent_projects(boost::property_tree::wptree& /*tree*/,
                                    int /*images*/) {}

void MainFrame::open_recent_project(size_t /*file_id*/, QString const& filename)
{
    if (m_plater) m_plater->load_project(filename);
}

void MainFrame::remove_recent_project(size_t file_id,
                                       const QString& /*filename*/)
{
    m_recent_projects.RemoveFileFromHistory(file_id);
}

void MainFrame::technology_changed() {}

void MainFrame::load_url(QString /*url*/) {}
void MainFrame::load_printer_url(QString /*url*/) {}
void MainFrame::load_printer_url() {}

bool MainFrame::is_printer_view() const { return false; }

void MainFrame::refresh_plugin_tips() {}
void MainFrame::RunScript(QString /*js*/) {}
void MainFrame::RunScriptLeft(QString /*js*/) {}
void MainFrame::show_device(bool /*bBBLPrinter*/) {}

void MainFrame::update_side_button_style() {}

void MainFrame::update_slice_print_status(SlicePrintEventType /*event*/,
                                          bool can_slice, bool can_print)
{
    m_slice_enable = can_slice;
    m_print_enable = can_print;
    if (m_slice_btn) m_slice_btn->setEnabled(can_slice);
    if (m_print_btn) m_print_btn->setEnabled(can_print);
}

bool MainFrame::get_enable_slice_status() { return m_slice_enable; }
bool MainFrame::get_enable_print_status() { return m_print_enable; }

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void MainFrame::on_presets_changed(QEvent& /*evt*/) {}
void MainFrame::on_value_changed(QEvent& /*evt*/) {}

std::string MainFrame::get_base_name(const QString& full_name,
                                     const char* extension) const
{
    std::string s = full_name.toStdString();
    size_t pos = s.rfind('/');
    if (pos == std::string::npos) pos = s.rfind('\\');
    std::string base = (pos == std::string::npos) ? s : s.substr(pos + 1);
    if (extension) {
        size_t dot = base.rfind('.');
        if (dot != std::string::npos) base = base.substr(0, dot);
        base += extension;
    }
    return base;
}

std::string MainFrame::get_dir_name(const QString& full_name) const
{
    std::string s = full_name.toStdString();
    size_t pos = s.rfind('/');
    if (pos == std::string::npos) pos = s.rfind('\\');
    return (pos == std::string::npos) ? "" : s.substr(0, pos);
}

} // namespace GUI
} // namespace Slic3r

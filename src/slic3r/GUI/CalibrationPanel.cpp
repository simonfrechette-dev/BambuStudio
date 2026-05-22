// Qt6 partial port for CalibrationPanel.cpp
#include "CalibrationPanel.hpp"
#include "libslic3r/Calib.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QMessageBox>

namespace Slic3r { namespace GUI {

// ── SelectMObjectPopup stubs ─────────────────────────────────────────────────
SelectMObjectPopup::SelectMObjectPopup(QWidget* parent)
    : PopupWindow(parent) {}
SelectMObjectPopup::~SelectMObjectPopup() {}
void SelectMObjectPopup::Popup(QWidget* /*focus*/) {}
void SelectMObjectPopup::OnDismiss() {}
bool SelectMObjectPopup::ProcessLeftDown(QMouseEvent& /*event*/) { return false; }
void SelectMObjectPopup::setVisible(bool visible) { PopupWindow::setVisible(visible); }
void SelectMObjectPopup::update_machine_list(QEvent& /*event*/) {}

// ── Calibration type metadata ─────────────────────────────────────────────────
struct CalibEntry {
    CalibMode   mode;
    const char* name;
    const char* description;
};

static const CalibEntry k_calib_entries[] = {
    { CalibMode::Calib_PA_Line,       "Pressure Advance — Line",
      "Print a single line pattern varying PA from start to end value.\n"
      "Measure the best PA value by finding the sharpest corner transition." },
    { CalibMode::Calib_PA_Pattern,    "Pressure Advance — Pattern",
      "Print a corner-based PA pattern for easy visual inspection.\n"
      "Works well on direct-drive printers with fast print speeds." },
    { CalibMode::Calib_PA_Tower,      "Pressure Advance — Tower",
      "Print a tower that changes PA value per layer.\n"
      "Best for Bowden-style printers where PA variation is more noticeable." },
    { CalibMode::Calib_Auto_PA_Line,  "Pressure Advance — Auto (Line)",
      "Automatically calibrate PA by scanning the printed line pattern.\n"
      "Requires a Bambu printer with built-in lidar or camera scan capability." },
    { CalibMode::Calib_Flow_Rate,     "Flow Rate",
      "Print a test object varying extrusion multiplier from start to end.\n"
      "Find the ideal flow ratio where surfaces are smooth and consistent." },
    { CalibMode::Calib_Temp_Tower,    "Temperature Tower",
      "Print a tower changing hotend temperature per section.\n"
      "Find the optimal print temperature for bridging, stringing, and strength." },
    { CalibMode::Calib_Vol_speed_Tower, "Volumetric Speed Tower",
      "Print a tower varying volumetric flow rate (mm³/s).\n"
      "Determine the maximum reliable extrusion speed for your hotend/filament combo." },
    { CalibMode::Calib_VFA_Tower,     "VFA (Vertical Fine Artifact) Tower",
      "Print a tower to identify ringing/ghosting by varying print speed.\n"
      "Use with input shaper tuning for best surface quality." },
    { CalibMode::Calib_Retraction_tower, "Retraction Tower",
      "Print a tower varying retraction distance or speed per section.\n"
      "Find the optimal retraction to minimise stringing and ooze." },
};

// ── CalibrationPanel ──────────────────────────────────────────────────────────
CalibrationPanel::CalibrationPanel(QWidget* parent, int /*id*/,
                                   const QPoint& /*pos*/, const QSize& /*size*/,
                                   long /*style*/)
    : QWidget(parent), m_mobjectlist_popup(this)
{
    // Null-init arrays
    for (int i = 0; i < CALI_MODE_COUNT; ++i)
        m_cali_panels[i] = nullptr;

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // ── Header ────────────────────────────────────────────────────────────────
    auto* hdr = new QLabel(tr("Calibration"), this);
    hdr->setAlignment(Qt::AlignCenter);
    QFont f = hdr->font();
    f.setPointSize(13);
    f.setBold(true);
    hdr->setFont(f);
    root->addWidget(hdr);

    auto* sub = new QLabel(tr("Select a calibration type to begin."), this);
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet(QStringLiteral("color: #888888;"));
    root->addWidget(sub);

    // ── Splitter: list | description ─────────────────────────────────────────
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    root->addWidget(splitter, 1);

    // List
    auto* list_widget = new QListWidget(splitter);
    for (const auto& e : k_calib_entries) {
        list_widget->addItem(QString::fromUtf8(e.name));
    }
    list_widget->setMinimumWidth(200);
    splitter->addWidget(list_widget);

    // Description pane
    auto* right_pane = new QWidget(splitter);
    auto* right_lay  = new QVBoxLayout(right_pane);
    right_lay->setContentsMargins(8, 8, 8, 8);

    auto* desc_title = new QLabel(tr("Description"), right_pane);
    desc_title->setStyleSheet(QStringLiteral("font-weight: bold;"));
    right_lay->addWidget(desc_title);

    auto* desc_lbl = new QLabel(tr("Select a calibration type from the list."), right_pane);
    desc_lbl->setWordWrap(true);
    desc_lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    desc_lbl->setStyleSheet(QStringLiteral("color: #cccccc;"));
    right_lay->addWidget(desc_lbl, 1);

    auto* start_btn = new QPushButton(tr("Start Calibration"), right_pane);
    start_btn->setEnabled(false);
    start_btn->setFixedHeight(32);
    right_lay->addWidget(start_btn);
    splitter->addWidget(right_pane);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    // ── Connections ───────────────────────────────────────────────────────────
    connect(list_widget, &QListWidget::currentRowChanged, this,
            [desc_lbl, start_btn](int row) {
        if (row < 0 || row >= int(sizeof(k_calib_entries)/sizeof(k_calib_entries[0]))) {
            desc_lbl->setText(tr("Select a calibration type from the list."));
            start_btn->setEnabled(false);
            return;
        }
        desc_lbl->setText(QString::fromUtf8(k_calib_entries[row].description));
        start_btn->setEnabled(true);
    });

    connect(start_btn, &QPushButton::clicked, this, [list_widget, this]() {
        const int row = list_widget->currentRow();
        if (row < 0) return;
        QMessageBox::information(this, tr("Calibration"),
            tr("Starting calibration: %1\n\nFull calibration wizard is pending port.")
                .arg(list_widget->currentItem()->text()));
    });
}

CalibrationPanel::~CalibrationPanel() {}

// ── Stub implementations for declared virtual/override methods ────────────────
void CalibrationPanel::update_print_error_info(int /*code*/, std::string /*msg*/, std::string /*extra*/) {}
void CalibrationPanel::update_all() {}
void CalibrationPanel::show_status(int /*status*/) {}
bool CalibrationPanel::Show(bool show) { setVisible(show); return true; }
void CalibrationPanel::on_printer_clicked(QMouseEvent& /*event*/) {}
void CalibrationPanel::set_default() {}
void CalibrationPanel::msw_rescale() {}
void CalibrationPanel::on_sys_color_changed() {}
void CalibrationPanel::init_tabpanel() {}
void CalibrationPanel::init_timer() {}
void CalibrationPanel::on_timer(QTimerEvent& /*event*/) {}

} } // Slic3r::GUI

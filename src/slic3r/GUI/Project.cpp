// Qt6 port for Project.cpp — shows live project summary
#include "Project.hpp"
#include "GUI_App.hpp"
#include "Plater.hpp"
#include "libslic3r/PresetBundle.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QShowEvent>

namespace Slic3r {
namespace GUI {

// ── helper: a two-column info row ──────────────────────────────────────────
static void add_info_row(QGridLayout* grid, int row,
                         const QString& key, QLabel** value_out,
                         QWidget* parent)
{
    auto* k = new QLabel(key, parent);
    k->setStyleSheet(QStringLiteral("color: #888888;"));
    auto* v = new QLabel(QStringLiteral("—"), parent);
    v->setStyleSheet(QStringLiteral("color: #dddddd;"));
    v->setWordWrap(true);
    grid->addWidget(k, row, 0, Qt::AlignTop | Qt::AlignRight);
    grid->addWidget(v, row, 1, Qt::AlignTop | Qt::AlignLeft);
    if (value_out) *value_out = v;
}

ProjectPanel::ProjectPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 16, 20, 16);
    root->setSpacing(12);

    // ── Header ────────────────────────────────────────────────────────────────
    auto* hdr = new QLabel(tr("Project Summary"), this);
    QFont f = hdr->font();
    f.setPointSize(13);
    f.setBold(true);
    hdr->setFont(f);
    root->addWidget(hdr);

    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet(QStringLiteral("color: #555555;"));
    root->addWidget(sep);

    // ── Info grid ─────────────────────────────────────────────────────────────
    auto* grid = new QGridLayout();
    grid->setColumnMinimumWidth(0, 140);
    grid->setColumnStretch(1, 1);
    grid->setVerticalSpacing(6);

    add_info_row(grid, 0, tr("Project name:"),      &m_lbl_name,     this);
    add_info_row(grid, 1, tr("File path:"),         &m_lbl_path,     this);
    add_info_row(grid, 2, tr("Objects loaded:"),    &m_lbl_objects,  this);
    add_info_row(grid, 3, tr("Print profile:"),     &m_lbl_print,    this);
    add_info_row(grid, 4, tr("Filament profile:"),  &m_lbl_filament, this);
    add_info_row(grid, 5, tr("Printer profile:"),   &m_lbl_printer,  this);

    root->addLayout(grid);

    // ── Refresh button ────────────────────────────────────────────────────────
    auto* btn_row = new QHBoxLayout();
    auto* refresh_btn = new QPushButton(tr("Refresh"), this);
    refresh_btn->setFixedWidth(100);
    btn_row->addStretch(1);
    btn_row->addWidget(refresh_btn);
    root->addLayout(btn_row);

    root->addStretch(1);

    connect(refresh_btn, &QPushButton::clicked, this, &ProjectPanel::refresh_info);

    // Initialise from whatever is loaded now
    refresh_info();
}

void ProjectPanel::refresh_info()
{
    // Project name + path
    Plater* plater = wxGetApp().plater();
    if (plater) {
        const QString name = plater->get_project_name();
        m_lbl_name->setText(name.isEmpty() ? tr("(untitled)") : name);
        const QString path = plater->get_project_filename();
        m_lbl_path->setText(path.isEmpty() ? tr("(not saved)") : path);

        // Object count via Model
        const int n = static_cast<int>(plater->model().objects.size());
        m_lbl_objects->setText(QString::number(n));
    } else {
        m_lbl_name->setText(tr("(unavailable)"));
        m_lbl_path->setText(QStringLiteral("—"));
        m_lbl_objects->setText(QStringLiteral("0"));
    }

    // Preset names
    if (auto* pb = wxGetApp().preset_bundle) {
        m_lbl_print->setText(   QString::fromStdString(pb->prints.get_selected_preset().name));
        m_lbl_filament->setText(QString::fromStdString(pb->filaments.get_selected_preset().name));
        m_lbl_printer->setText( QString::fromStdString(pb->printers.get_selected_preset().name));
    }
}

void ProjectPanel::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    refresh_info();
}

void ProjectPanel::msw_rescale() {}
void ProjectPanel::update_model_data() { refresh_info(); }
void ProjectPanel::clear_model_info()
{
    m_lbl_name->setText(QStringLiteral("—"));
    m_lbl_path->setText(QStringLiteral("—"));
    m_lbl_objects->setText(QStringLiteral("0"));
}

bool ProjectPanel::isVisible() const { return QWidget::isVisible(); }
void ProjectPanel::OnScriptMessage(const QString& /*msg*/) {}
void ProjectPanel::RunScript(const std::string& /*content*/) {}
bool ProjectPanel::is_editing_page() const { return false; }

std::map<std::string, std::vector<nlohmann::json>> ProjectPanel::Reload(const QString& /*aux_path*/)
{
    return {};
}

std::string ProjectPanel::formatBytes(unsigned long bytes)
{
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024*1024) return std::to_string(bytes/1024) + " KB";
    return std::to_string(bytes/(1024*1024)) + " MB";
}

std::string ProjectPanel::get_model_id(std::string design_id)
{
    auto it = m_model_id_map.find(design_id);
    return it != m_model_id_map.end() ? it->second : std::string{};
}

QString ProjectPanel::to_base64(const std::string& /*path*/)
{
    return {};
}

void ProjectPanel::save_project() {}

}} // namespace Slic3r::GUI

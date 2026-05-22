// Phase 4: Qt stub implementation of AuxiliaryPanel
// Original wx implementation backed up to Auxiliary.cpp.wx-backup

#include "Auxiliary.hpp"
#include "GUI_App.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

namespace Slic3r {
namespace GUI {

AuxiliaryPanel::AuxiliaryPanel(QWidget* parent)
    : QWidget(parent)
{
    setAcceptDrops(true);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(6);

    // Header
    auto* hdr = new QLabel(tr("Auxiliary Files"), this);
    hdr->setStyleSheet(QStringLiteral("font-weight:bold; font-size:14px;"));
    lay->addWidget(hdr);

    // Toolbar: Import + Open Folder buttons
    auto* toolbar = new QWidget(this);
    auto* tblay = new QHBoxLayout(toolbar);
    tblay->setContentsMargins(0, 0, 0, 0);
    tblay->setSpacing(4);
    auto* btn_import = new QPushButton(tr("Import…"), toolbar);
    btn_import->setFixedHeight(26);
    auto* btn_open = new QPushButton(tr("Open Folder"), toolbar);
    btn_open->setFixedHeight(26);
    tblay->addWidget(btn_import);
    tblay->addWidget(btn_open);
    tblay->addStretch();
    lay->addWidget(toolbar);

    // File list
    m_file_list = new QListWidget(this);
    m_file_list->setAlternatingRowColors(true);
    m_file_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    lay->addWidget(m_file_list, 1);

    // Drop hint label (shown when empty)
    m_hint_lbl = new QLabel(tr("Drop files here or use Import…\nto add auxiliary files."), this);
    m_hint_lbl->setAlignment(Qt::AlignCenter);
    m_hint_lbl->setStyleSheet(QStringLiteral("color:#888;"));
    m_hint_lbl->setWordWrap(true);
    lay->addWidget(m_hint_lbl);

    // Connections
    connect(btn_import, &QPushButton::clicked, this, [this]() {
        if (m_root_dir.isEmpty()) return;
        const QStringList files = QFileDialog::getOpenFileNames(
            this, tr("Import Auxiliary Files"), QDir::homePath(),
            tr("All Files (*)"));
        for (const QString& src : files) {
            const QString dst = m_root_dir + "/" + QFileInfo(src).fileName();
            if (QFile::copy(src, dst))
                add_file_item(dst);
        }
        update_hint_visibility();
    });

    connect(btn_open, &QPushButton::clicked, this, [this]() {
        if (!m_root_dir.isEmpty())
            QDesktopServices::openUrl(QUrl::fromLocalFile(m_root_dir));
    });

    connect(m_file_list, &QListWidget::itemDoubleClicked, this, [](QListWidgetItem* item) {
        if (item) QDesktopServices::openUrl(QUrl::fromLocalFile(item->data(Qt::UserRole).toString()));
    });

    update_hint_visibility();
}

void AuxiliaryPanel::add_file_item(const QString& path)
{
    auto* item = new QListWidgetItem(QFileInfo(path).fileName(), m_file_list);
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
}

void AuxiliaryPanel::update_hint_visibility()
{
    const bool empty = (m_file_list->count() == 0);
    m_file_list->setVisible(!empty);
    m_hint_lbl->setVisible(empty);
}

void AuxiliaryPanel::Reload(const QString& aux_path)
{
    m_root_dir = aux_path;
    m_file_list->clear();
    if (!aux_path.isEmpty()) {
        QDir dir(aux_path);
        if (dir.exists()) {
            for (const QFileInfo& fi : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot))
                add_file_item(fi.absoluteFilePath());
        }
    }
    update_hint_visibility();
}

void AuxiliaryPanel::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void AuxiliaryPanel::dropEvent(QDropEvent* event)
{
    if (m_root_dir.isEmpty()) return;
    for (const QUrl& url : event->mimeData()->urls()) {
        const QString src = url.toLocalFile();
        if (src.isEmpty()) continue;
        const QString dst = m_root_dir + "/" + QFileInfo(src).fileName();
        if (QFile::copy(src, dst))
            add_file_item(dst);
    }
    update_hint_visibility();
}

void AuxiliaryPanel::init_bitmap() {}
void AuxiliaryPanel::init_tabpanel() {}

void AuxiliaryPanel::Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest)
{
    (void)src; (void)separator; (void)dest;
}

void AuxiliaryPanel::msw_rescale() {}
void AuxiliaryPanel::on_size() {}
bool AuxiliaryPanel::isVisible() const { return QWidget::isVisible(); }

void AuxiliaryPanel::init_auxiliary() {}
void AuxiliaryPanel::create_folder(const QString& name) { (void)name; }

std::string AuxiliaryPanel::replaceSpace(std::string s, std::string ts, std::string ns)
{
    (void)ts; (void)ns;
    return s;
}

void AuxiliaryPanel::update_all_panel() {}
void AuxiliaryPanel::update_all_cover() {}

}} // namespace Slic3r::GUI


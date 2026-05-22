// Qt6 placeholder for UpgradePanel.cpp
#include "UpgradePanel.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

namespace Slic3r {
namespace GUI {

UpgradePanel::UpgradePanel(QWidget *parent, int /*id*/, const QPoint& /*pos*/,
                            const QSize& size, long /*style*/)
    : QWidget(parent)
{
    if (!size.isEmpty()) resize(size);

    auto* vlay = new QVBoxLayout(this);
    vlay->setContentsMargins(8, 8, 8, 8);
    vlay->setSpacing(4);

    auto* hdr = new QLabel(tr("Firmware Upgrade"), this);
    QFont f = hdr->font();
    f.setBold(true);
    hdr->setFont(f);
    vlay->addWidget(hdr);

    auto* hint = new QLabel(tr("Connect a device to check for firmware updates."), this);
    hint->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    hint->setWordWrap(true);
    vlay->addWidget(hint);
    vlay->addStretch(1);

    m_scrolledWindow = nullptr;
    m_machine_list_sizer = nullptr;
}

UpgradePanel::~UpgradePanel() {}

void UpgradePanel::clean_push_upgrade_panel() {}
void UpgradePanel::msw_rescale() {}
void UpgradePanel::setVisible(bool show) { QWidget::setVisible(show); }
void UpgradePanel::refresh_version_and_firmware(MachineObject* /*obj*/) {}
void UpgradePanel::update(MachineObject* /*obj*/) {}
void UpgradePanel::show_status(int /*status*/) {}
void UpgradePanel::on_sys_color_changed() {}

} // namespace GUI
} // namespace Slic3r

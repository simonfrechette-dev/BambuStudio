// Qt6 placeholder for HMSPanel.cpp
#include "HMSPanel.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

namespace Slic3r {
namespace GUI {

HMSPanel::HMSPanel(QWidget *parent, int /*id*/, const QPoint& /*pos*/,
                   const QSize& size, long /*style*/)
    : QWidget(parent)
{
    if (!size.isEmpty()) resize(size);

    auto* vlay = new QVBoxLayout(this);
    vlay->setContentsMargins(8, 8, 8, 8);
    vlay->setSpacing(4);

    auto* hdr = new QLabel(tr("HMS — Health Management System"), this);
    QFont f = hdr->font();
    f.setBold(true);
    hdr->setFont(f);
    vlay->addWidget(hdr);

    auto* hint = new QLabel(tr("No active HMS notifications."), this);
    hint->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    hint->setWordWrap(true);
    vlay->addWidget(hint);
    vlay->addStretch(1);

    m_scrolledWindow = nullptr;
}

HMSPanel::~HMSPanel() {}

void HMSPanel::setVisible(bool show) { QWidget::setVisible(show); }
void HMSPanel::update(MachineObject* /*obj*/) {}
void HMSPanel::show_status(int status) { last_status = status; }
void HMSPanel::clear_hms_tag() {}
void HMSPanel::append_hms_panel(const std::string& /*dev_id*/, DevHMSItem& /*item*/) {}
void HMSPanel::delete_hms_panels() {}

// HMSNotifyItem
HMSNotifyItem::HMSNotifyItem(const std::string& /*dev_id*/, QWidget* parent,
                              DevHMSItem& item)
    : QWidget(parent)
    , m_hms_item(item)
{}
HMSNotifyItem::~HMSNotifyItem() {}

} // namespace GUI
} // namespace Slic3r

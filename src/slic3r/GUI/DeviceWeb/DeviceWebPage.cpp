// Qt6 stub for DeviceWeb/DeviceWebPage.cpp
#include "DeviceWebPage.hpp"
#include <QVBoxLayout>
#include <QLabel>

namespace Slic3r { namespace GUI {

DeviceWebPage::DeviceWebPage(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(12, 12, 12, 12);
    auto* lbl = new QLabel(tr("Cloud / Device Web"), this);
    lbl->setAlignment(Qt::AlignCenter);
    QFont f = lbl->font();
    f.setPointSize(14);
    lbl->setFont(f);
    lay->addWidget(lbl);
    auto* hint = new QLabel(tr("Cloud device management \u2014 pending QWebEngineView port."), this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(QStringLiteral("color: #888888;"));
    lay->addWidget(hint);
    lay->addStretch(1);
}

DeviceWebPage::~DeviceWebPage() {}

} } // Slic3r::GUI

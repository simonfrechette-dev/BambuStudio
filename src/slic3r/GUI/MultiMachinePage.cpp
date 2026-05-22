// Qt6 stub for MultiMachinePage.cpp
#include "MultiMachinePage.hpp"
#include <QVBoxLayout>
#include <QLabel>

namespace Slic3r { namespace GUI {

MultiMachinePage::MultiMachinePage(QWidget* parent, int /*id*/,
                                   const QPoint& /*pos*/, const QSize& /*size*/,
                                   long /*style*/)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(12, 12, 12, 12);
    auto* lbl = new QLabel(tr("Multi-Device"), this);
    lbl->setAlignment(Qt::AlignCenter);
    QFont f = lbl->font();
    f.setPointSize(14);
    lbl->setFont(f);
    lay->addWidget(lbl);
    auto* hint = new QLabel(tr("Multi-printer management \u2014 pending port."), this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(QStringLiteral("color: #888888;"));
    lay->addWidget(hint);
    lay->addStretch(1);
}

MultiMachinePage::~MultiMachinePage() {}

} } // Slic3r::GUI

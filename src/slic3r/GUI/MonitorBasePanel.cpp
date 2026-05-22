// Qt6 port stub for MonitorBasePanel.cpp
#include "MonitorBasePanel.h"

namespace Slic3r {
namespace GUI {

MonitorBasePanel::MonitorBasePanel(QWidget* parent, const QSize& size)
    : QWidget(parent)
{
    if (!size.isEmpty()) setMinimumSize(size);
}

VideoMonitoringBasePanel::VideoMonitoringBasePanel(QWidget* parent, const QSize& size)
    : QWidget(parent)
{
    if (!size.isEmpty()) setMinimumSize(size);
}

TaskListBasePanel::TaskListBasePanel(QWidget* parent, const QSize& size)
    : QWidget(parent)
{
    if (!size.isEmpty()) setMinimumSize(size);
}

} // namespace GUI
} // namespace Slic3r

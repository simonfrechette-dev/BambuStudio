// Qt6 stub for MonitorPage.cpp
#include "MonitorPage.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Slic3r {
namespace GUI {

MonitorPage::MonitorPage(QWidget* parent)
    : QWidget(parent)
    , m_main_sizer(nullptr)
    , m_content_sizer(nullptr)
{
}

MonitorPage::~MonitorPage() {}

} // namespace GUI
} // namespace Slic3r

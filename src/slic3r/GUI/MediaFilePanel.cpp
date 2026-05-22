// Qt6 stub for MediaFilePanel.cpp
#include "MediaFilePanel.h"

namespace Slic3r {
namespace GUI {

MediaFilePanel::MediaFilePanel(QWidget* parent) : QWidget(parent) {}
MediaFilePanel::~MediaFilePanel() {}
void MediaFilePanel::modeChanged(QEvent&) {}

MediaFileFrame::MediaFileFrame(QWidget* parent) : DPIFrame(parent) {}
void MediaFileFrame::on_dpi_changed(const QRect&) {}

} // namespace GUI
} // namespace Slic3r

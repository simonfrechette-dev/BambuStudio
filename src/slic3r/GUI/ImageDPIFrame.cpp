// Qt6 stub for ImageDPIFrame.cpp
#include "ImageDPIFrame.hpp"
#include <QTimer>

namespace Slic3r { namespace GUI {

ImageDPIFrame::ImageDPIFrame()
    : DPIFrame(nullptr)
{
    m_refresh_timer = new QTimer(this);
    connect(m_refresh_timer, &QTimer::timeout, this, &ImageDPIFrame::on_timer);
}

ImageDPIFrame::~ImageDPIFrame() {}

void ImageDPIFrame::on_dpi_changed(const QRect& /*suggested_rect*/) {}
void ImageDPIFrame::on_timer() {}

}} // namespace Slic3r::GUI



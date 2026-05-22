// Phase 3 TODO: Qt QPainter implementation of ImageGrid
// Original wx implementation backed up to ImageGrid.cpp.wx-backup

#include "ImageGrid.h"
#include "Printer/PrinterFileSystem.h"
#include <boost/log/trivial.hpp>

namespace Slic3r {
namespace GUI {

ImageGrid::ImageGrid(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    connect(&m_timer, &QTimer::timeout, this, [this]() { update(); });
}

void ImageGrid::SetFileSystem(boost::shared_ptr<PrinterFileSystem> file_sys)
{
    m_file_sys = file_sys;
    update();
}

void ImageGrid::SetStatus(const QPixmap& icon, const QString& msg)
{
    m_status_msg = msg;
    update();
}

void ImageGrid::SetFileType(int type, std::string const& storage)
{
    if (m_file_sys) {
        // TODO: implement
    }
    update();
}

void ImageGrid::SetGroupMode(int mode)
{
    if (m_file_sys) {
        // TODO: implement
    }
    update();
}

void ImageGrid::SetSelecting(bool selecting)
{
    m_selecting = selecting;
    update();
}

void ImageGrid::SetAllSelecting(bool selecting)
{
    if (m_file_sys) {
        // TODO: implement
    }
    update();
}

void ImageGrid::DoActionOnSelection(int action)
{
    if (m_file_sys) {
        // TODO: implement
    }
}

void ImageGrid::ShowDownload(bool show)
{
    m_show_download = show;
    update();
}

void ImageGrid::Rescale()
{
    update();
}

void ImageGrid::paintEvent(QPaintEvent* event)
{
    // Phase 3 TODO: custom QPainter rendering
    Q_UNUSED(event)
}

void ImageGrid::mousePressEvent(QMouseEvent* event)
{
    m_pressed = true;
    QWidget::mousePressEvent(event);
}

void ImageGrid::mouseReleaseEvent(QMouseEvent* event)
{
    m_pressed = false;
    QWidget::mouseReleaseEvent(event);
}

void ImageGrid::mouseMoveEvent(QMouseEvent* event)
{
    m_hovered = true;
    QWidget::mouseMoveEvent(event);
}

void ImageGrid::leaveEvent(QEvent* event)
{
    m_hovered = false;
    QWidget::leaveEvent(event);
}

void ImageGrid::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
}

void ImageGrid::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

} // namespace GUI
} // namespace Slic3r

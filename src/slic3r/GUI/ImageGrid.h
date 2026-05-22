#ifndef ImageGrid_h
#define ImageGrid_h

// Qt port of ImageGrid — Phase 3 TODO: full QPainter implementation

#include <QWidget>
#include <QTimer>
#include <boost/shared_ptr.hpp>
#include "Widgets/StateColor.hpp"

class Button;
class Label;
class PrinterFileSystem;

namespace Slic3r {
class MachineObject;
namespace GUI {

class ImageGrid : public QWidget
{
    Q_OBJECT
public:
    explicit ImageGrid(QWidget *parent = nullptr);
    ~ImageGrid() override = default;

    void SetFileSystem(boost::shared_ptr<PrinterFileSystem> file_sys);
    void SetStatus(const QPixmap &icon, const QString &msg);
    boost::shared_ptr<PrinterFileSystem> GetFileSystem() { return m_file_sys; }
    void SetFileType(int type, std::string const &storage);
    void SetGroupMode(int mode);
    void SetSelecting(bool selecting);
    void SetAllSelecting(bool selecting);
    bool IsSelecting() { return m_selecting; }
    void DoActionOnSelection(int action);
    void ShowDownload(bool show);
    void Rescale();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    boost::shared_ptr<PrinterFileSystem> m_file_sys;
    QString  m_status_msg;
    bool     m_selecting    = false;
    bool     m_show_download = false;
    bool     m_pressed      = false;
    bool     m_hovered      = false;
    int      m_scroll_offset = 0;
    int      m_hit_type     = 0; // HitType
    size_t   m_hit_item     = size_t(-1);
    QTimer   m_timer;
};

}} // namespace Slic3r::GUI

#endif /* ImageGrid_h */

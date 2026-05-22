#ifndef slic3r_GUI_Utils_hpp_
#define slic3r_GUI_Utils_hpp_

// Qt port of GUI_Utils.hpp
// DPIDialog / DPIFrame are now QDialog / QMainWindow with an on_dpi_changed hook.

#include <memory>
#include <string>
#include <ostream>
#include <functional>
#include <deque>
#include <array>

#include <boost/optional.hpp>
#include <boost/log/trivial.hpp>

#include <QDialog>
#include <QMainWindow>
#include <QWidget>
#include <QRect>
#include <QFont>
#include <QFileDialog>
#include <QString>
#include <QMetaObject>
#include <QEvent>

#include "../libslic3r/libslic3r_version.h"
#include "../libslic3r/Utils.hpp"

namespace Slic3r {
namespace GUI {

// ---------------------------------------------------------------------------
// Utility: color helpers
// ---------------------------------------------------------------------------

inline int hex_to_int(const char c)
{
    return (c >= '0' && c <= '9') ? int(c - '0') :
           (c >= 'A' && c <= 'F') ? int(c - 'A') + 10 :
           (c >= 'a' && c <= 'f') ? int(c - 'a') + 10 : -1;
}

static std::array<float, 4> decode_color_to_float_array(const std::string color)
{
    std::array<float, 4> ret = {0, 0, 0, 1.0f};
    const char *c = color.data() + 1;
    if (color.size() == 7 && color.front() == '#') {
        for (size_t j = 0; j < 3; ++j) {
            int d1 = hex_to_int(*c++), d2 = hex_to_int(*c++);
            if (d1 == -1 || d2 == -1) break;
            ret[j] = float(d1 * 16 + d2) / 255.0f;
        }
    } else if (color.size() == 9 && color.front() == '#') {
        for (size_t j = 0; j < 4; ++j) {
            int d1 = hex_to_int(*c++), d2 = hex_to_int(*c++);
            if (d1 == -1 || d2 == -1) break;
            ret[j] = float(d1 * 16 + d2) / 255.0f;
        }
    }
    return ret;
}

inline int hex_digit_to_int(const char c)
{
    return (c >= '0' && c <= '9') ? int(c - '0') :
           (c >= 'A' && c <= 'F') ? int(c - 'A') + 10 :
           (c >= 'a' && c <= 'f') ? int(c - 'a') + 10 : -1;
}

// ---------------------------------------------------------------------------
// DPI helpers
// ---------------------------------------------------------------------------

enum { DPI_DEFAULT = 96 };

inline int get_dpi_for_window(const QWidget *w)
{
    if (!w) return DPI_DEFAULT;
    return qRound(w->devicePixelRatioF() * DPI_DEFAULT);
}

inline QFont get_default_font_for_dpi(const QWidget *, int) { return QFont(); }
inline QFont get_default_font(const QWidget *w) { return get_default_font_for_dpi(w, get_dpi_for_window(w)); }

bool check_dark_mode();
void update_dark_config();

CopyFileResult copy_file_gui(const std::string &from, const std::string &to, std::string &error_message, const bool with_check = false);

// ---------------------------------------------------------------------------
// DPIDialog
// ---------------------------------------------------------------------------

extern std::deque<QDialog *> dialogStack;

class DPIDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DPIDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Dialog)
        : QDialog(parent, flags) {}

    DPIDialog(QWidget *parent, int, const QString &title,
              const QPoint & = QPoint(), const QSize &size = QSize(), long = 0)
        : QDialog(parent)
    {
        setWindowTitle(title);
        if (size.isValid()) resize(size);
    }

    int exec() override
    {
        dialogStack.push_front(this);
        int r = QDialog::exec();
        if (!dialogStack.empty() && dialogStack.front() == this)
            dialogStack.pop_front();
        return r;
    }

    virtual void on_dpi_changed(const QRect &) {}
    virtual void on_sys_color_changed() {}

    int    em_unit()      const { return qRound(10 * devicePixelRatioF()); }
    float  scale_factor() const { return static_cast<float>(devicePixelRatioF()); }
    QFont  normal_font()  const { return font(); }

protected:
    void changeEvent(QEvent *e) override
    {
        QDialog::changeEvent(e);
        if (e->type() == QEvent::FontChange || e->type() == QEvent::StyleChange)
            on_dpi_changed(geometry());
    }
};

// ---------------------------------------------------------------------------
// DPIFrame
// ---------------------------------------------------------------------------

class DPIFrame : public QMainWindow
{
    Q_OBJECT
public:
    explicit DPIFrame(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Window)
        : QMainWindow(parent, flags) {}

    DPIFrame(QWidget *parent, int, const QString &title,
             const QPoint & = QPoint(), const QSize &size = QSize(), long = 0)
        : QMainWindow(parent)
    {
        setWindowTitle(title);
        if (size.isValid()) resize(size);
    }

    virtual void on_dpi_changed(const QRect &) {}
    virtual void on_sys_color_changed() {}

    int    em_unit()      const { return qRound(10 * devicePixelRatioF()); }
    float  scale_factor() const { return static_cast<float>(devicePixelRatioF()); }
    QFont  normal_font()  const { return font(); }

protected:
    void changeEvent(QEvent *e) override
    {
        QMainWindow::changeEvent(e);
        if (e->type() == QEvent::FontChange || e->type() == QEvent::StyleChange)
            on_dpi_changed(geometry());
    }
};

// ---------------------------------------------------------------------------
// EventGuard — RAII wrapper for QMetaObject::Connection
// ---------------------------------------------------------------------------

class EventGuard
{
public:
    EventGuard() = default;
    EventGuard(const EventGuard &) = delete;
    EventGuard(EventGuard &&o) noexcept : m_conn(o.m_conn) { o.m_conn = {}; }
    explicit EventGuard(QMetaObject::Connection c) : m_conn(c) {}
    ~EventGuard() { unbind(); }

    EventGuard &operator=(const EventGuard &) = delete;
    EventGuard &operator=(EventGuard &&o) noexcept
    {
        unbind(); m_conn = o.m_conn; o.m_conn = {}; return *this;
    }

    void unbind() { if (m_conn) { QObject::disconnect(m_conn); m_conn = {}; } }
    explicit operator bool() const { return static_cast<bool>(m_conn); }

private:
    QMetaObject::Connection m_conn;
};

// ---------------------------------------------------------------------------
// CheckboxFileDialog stub
// ---------------------------------------------------------------------------

class CheckboxFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    CheckboxFileDialog(QWidget *parent,
                       const QString &, bool,
                       const QString &caption  = QString(),
                       const QString &directory = QString(),
                       const QString &filter    = QString())
        : QFileDialog(parent, caption, directory, filter) {}
    bool get_checkbox_value() const { return false; }
};

// ---------------------------------------------------------------------------
// WindowMetrics
// ---------------------------------------------------------------------------

class WindowMetrics
{
    QRect rect;
    bool  maximized{false};
    WindowMetrics() = default;
public:
    static WindowMetrics from_window(QWidget *window);
    static boost::optional<WindowMetrics> deserialize(const std::string &str);

    const QRect &get_rect()     const { return rect; }
    bool          get_maximized() const { return maximized; }

    void sanitize_for_display(const QRect &screen_rect);
    void center_for_display(const QRect &screen_rect);
    std::string serialize() const;
};

std::ostream &operator<<(std::ostream &os, const WindowMetrics &metrics);

// ---------------------------------------------------------------------------
// TaskTimer / KeyAutoRepeatFilter
// ---------------------------------------------------------------------------

class TaskTimer
{
    std::chrono::milliseconds start_timer;
    std::string               task_name;
public:
    TaskTimer(std::string task_name);
    ~TaskTimer();
};

class KeyAutoRepeatFilter
{
    size_t m_count{0};
public:
    void increase_count() { ++m_count; }
    void reset_count()    { m_count = 0; }
    bool is_first() const { return m_count == 0; }
};

// ---------------------------------------------------------------------------
// WikiPanel stub
// ---------------------------------------------------------------------------

class WikiPanel : public QWidget
{
    Q_OBJECT
public:
    WikiPanel(QWidget *parent,
              const QString & = QStringLiteral("Wiki"),
              const QString & = QString(),
              const std::string & = "https://wiki.bambulab.com")
        : QWidget(parent) {}
    void SetWikiText(const QString &) {}
    void SetWikiUrl(const std::string &) {}
    void SetTooltip(const QString &) {}
    void msw_rescale() {}
};

// ---------------------------------------------------------------------------
// Macros and constants
// ---------------------------------------------------------------------------

#define ICON_SINGLE_SIZE 16
#define ICON_SIZE        QSize(16, 16)
#define _3MF_COVER_SIZE              QSize(240, 240)
#define PRINTER_THUMBNAIL_SMALL_SIZE QSize(252, 188)
#define PRINTER_THUMBNAIL_MIDDLE_SIZE QSize(680, 680)
#define GERNERATE_IMAGE_RESIZE        0
#define GERNERATE_IMAGE_CROP_VERTICAL 1

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_Utils_hpp_

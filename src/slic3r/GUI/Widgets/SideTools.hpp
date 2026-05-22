#ifndef slic3r_GUI_SIDETOOLS_hpp_
#define slic3r_GUI_SIDETOOLS_hpp_

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <QString>
#include <QLabel>
#include "Button.hpp"
#include "Label.hpp"
#include "../QtExtensions.hpp"

// Colour helpers (replaces preprocessor macros)
inline constexpr QColor SIDE_TOOLS_GREY900() { return QColor(38, 46, 48); }
inline constexpr QColor SIDE_TOOLS_GREY600() { return QColor(144, 144, 144); }
inline constexpr QColor SIDE_TOOLS_GREY400() { return QColor(206, 206, 206); }
inline constexpr QColor SIDE_TOOLS_BRAND()   { return QColor(0, 174, 66); }
inline constexpr QColor SIDE_TOOLS_LIGHT_GREEN() { return QColor(219, 253, 231); }

enum WifiSignal { NONE, WEAK, MIDDLE, STRONG, WIRED };

enum MonitorStatus {
    MONITOR_UNKNOWN              = 0,
    MONITOR_NORMAL               = 1 << 1,
    MONITOR_NO_PRINTER           = 1 << 2,
    MONITOR_DISCONNECTED         = 1 << 3,
    MONITOR_DISCONNECTED_SERVER  = 1 << 4,
    MONITOR_CONNECTING           = 1 << 5,
};

#define SIDE_TOOL_CLICK_INTERVAL 20

namespace Slic3r { namespace GUI {

class MachineObject;

class SideToolsPanel : public QWidget
{
    Q_OBJECT
public:
    SideToolsPanel(QWidget *parent = nullptr);
    ~SideToolsPanel() override;

    void set_none_printer_mode();
    void set_current_printer_name(const std::string &dev_name);
    void set_current_printer_signal(WifiSignal sign);
    void start_interval();
    bool is_in_interval() const;
    void msw_rescale();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_interval_timeout();
    void stop_interval();

private:
    void doRender(QPainter &p);

    WifiSignal m_wifi_type{WifiSignal::NONE};
    QString    m_dev_name;
    bool       m_hover{false};
    bool       m_click{false};
    bool       m_none_printer{true};
    int        last_printer_signal = 0;

    ScalableBitmap m_printing_img;
    ScalableBitmap m_arrow_img;
    ScalableBitmap m_none_printing_img;
    ScalableBitmap m_none_arrow_img;
    ScalableBitmap m_none_add_img;
    ScalableBitmap m_wifi_none_img;
    ScalableBitmap m_wifi_weak_img;
    ScalableBitmap m_wifi_middle_img;
    ScalableBitmap m_wifi_strong_img;
    ScalableBitmap m_network_wired_img;

    QTimer *m_interval_timer{nullptr};
    bool    m_is_in_interval{false};
};

class SideTools : public QWidget
{
    Q_OBJECT
public:
    SideTools(QWidget *parent = nullptr);
    ~SideTools() override;

    void set_table_panel(QWidget *tb);
    void msw_rescale();
    bool is_in_interval() const;
    void set_current_printer_name(const std::string &dev_name);
    void set_current_printer_signal(WifiSignal sign);
    void set_none_printer_mode();
    void start_interval();
    void update_status(MachineObject *obj);
    void update_connect_err_info(int code, const QString &desc, const QString &info);
    void show_status(int status);

    SideToolsPanel *get_panel() { return m_side_tools; }

private:
    SideToolsPanel  *m_side_tools{nullptr};
    QWidget         *m_tabpanel{nullptr};
    Label           *m_st_txt_error_code{nullptr};
    Label           *m_st_txt_error_desc{nullptr};
    Label           *m_st_txt_error_extra{nullptr};
    QWidget         *m_side_error_panel{nullptr};
    Button          *m_connection_info{nullptr};
    QLabel          *m_hyperlink{nullptr};
    ScalableBitmap   m_more_err_open;
    ScalableBitmap   m_more_err_close;
    bool             m_more_err_state{false};
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_SIDETOOLS_hpp_

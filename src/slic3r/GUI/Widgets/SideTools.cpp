#include "SideTools.hpp"
#include "Label.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QTimer>

namespace Slic3r { namespace GUI {

// ============================================================
// SideToolsPanel
// ============================================================

SideToolsPanel::SideToolsPanel(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(56);
    setCursor(Qt::PointingHandCursor);

    m_printing_img       = ScalableBitmap(this, "monitor_printer",      24);
    m_arrow_img          = ScalableBitmap(this, "monitor_arrow",         16);
    m_none_printing_img  = ScalableBitmap(this, "monitor_none_printer",  24);
    m_none_arrow_img     = ScalableBitmap(this, "monitor_none_arrow",    16);
    m_none_add_img       = ScalableBitmap(this, "monitor_none_add",      24);
    m_wifi_none_img      = ScalableBitmap(this, "monitor_signal_no",     16);
    m_wifi_weak_img      = ScalableBitmap(this, "monitor_signal_weak",   16);
    m_wifi_middle_img    = ScalableBitmap(this, "monitor_signal_middle", 16);
    m_wifi_strong_img    = ScalableBitmap(this, "monitor_signal_strong", 16);
    m_network_wired_img  = ScalableBitmap(this, "monitor_network_wired", 16);
}

SideToolsPanel::~SideToolsPanel() = default;

void SideToolsPanel::set_none_printer_mode()   { m_none_printer = true; update(); }
void SideToolsPanel::set_current_printer_name(const std::string &name)
{
    m_dev_name = QString::fromStdString(name);
    m_none_printer = name.empty();
    update();
}
void SideToolsPanel::set_current_printer_signal(WifiSignal sign)
{
    m_wifi_type = sign; update();
}

void SideToolsPanel::start_interval()
{
    m_is_in_interval = true;
    if (!m_interval_timer) {
        m_interval_timer = new QTimer(this);
        m_interval_timer->setSingleShot(true);
        connect(m_interval_timer, &QTimer::timeout, this, &SideToolsPanel::stop_interval);
    }
    m_interval_timer->start(SIDE_TOOL_CLICK_INTERVAL * 1000);
}

void SideToolsPanel::stop_interval()      { m_is_in_interval = false; }
bool SideToolsPanel::is_in_interval() const { return m_is_in_interval; }
void SideToolsPanel::msw_rescale() { update(); }

void SideToolsPanel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    doRender(p);
    QWidget::paintEvent(event);
}

void SideToolsPanel::doRender(QPainter &p)
{
    const QRect rc = rect();

    // Background
    p.setBrush(m_hover ? QColor(240, 250, 240) : Qt::white);
    p.setPen(Qt::NoPen);
    p.drawRect(rc);

    // Printer icon
    const ScalableBitmap &printerBmp = m_none_printer ? m_none_printing_img : m_printing_img;
    const int iy = (rc.height() - 24) / 2;
    int x = 12;
    if (printerBmp.bmp().IsOk()) {
        p.drawPixmap(QPoint(x, iy), printerBmp.bmp());
        x += 24 + 8;
    }

    // Name text
    p.setFont(Label::Body_13);
    p.setPen(m_none_printer ? QColor(144, 144, 144) : QColor(38, 46, 48));
    const QString displayName = m_none_printer ? QStringLiteral("No printer") : m_dev_name;
    p.drawText(QRect(x, 0, rc.width() - x - 40, rc.height()),
               Qt::AlignVCenter | Qt::AlignLeft, displayName);

    // Wifi signal icon
    ScalableBitmap *wifiBmp = nullptr;
    switch (m_wifi_type) {
    case WEAK:   wifiBmp = &m_wifi_weak_img;   break;
    case MIDDLE: wifiBmp = &m_wifi_middle_img; break;
    case STRONG: wifiBmp = &m_wifi_strong_img; break;
    case WIRED:  wifiBmp = &m_network_wired_img; break;
    default:     wifiBmp = &m_wifi_none_img;   break;
    }
    if (wifiBmp && wifiBmp->bmp().IsOk()) {
        const QSize wsz = wifiBmp->GetBmpSize();
        p.drawPixmap(QPoint(rc.right() - wsz.width() - 20, (rc.height() - wsz.height()) / 2),
                     wifiBmp->bmp());
    }

    // Arrow
    const ScalableBitmap &arrowBmp = m_none_printer ? m_none_arrow_img : m_arrow_img;
    if (arrowBmp.bmp().IsOk()) {
        const QSize asz = arrowBmp.GetBmpSize();
        p.drawPixmap(QPoint(rc.right() - asz.width() - 4, (rc.height() - asz.height()) / 2),
                     arrowBmp.bmp());
    }
}

void SideToolsPanel::enterEvent(QEnterEvent *e) { m_hover = true; update(); QWidget::enterEvent(e); }
void SideToolsPanel::leaveEvent(QEvent *e)      { m_hover = false; update(); QWidget::leaveEvent(e); }
void SideToolsPanel::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) { m_click = true; e->accept(); }
    else QWidget::mousePressEvent(e);
}
void SideToolsPanel::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && m_click && rect().contains(e->pos())) {
        m_click = false;
        if (!m_is_in_interval) emit clicked();
        e->accept();
    } else {
        m_click = false;
        QWidget::mouseReleaseEvent(e);
    }
}

// ============================================================
// SideTools
// ============================================================

SideTools::SideTools(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_side_tools = new SideToolsPanel(this);
    layout->addWidget(m_side_tools);

    m_side_error_panel = new QWidget(this);
    m_side_error_panel->hide();
    auto *errLayout = new QVBoxLayout(m_side_error_panel);
    errLayout->setContentsMargins(8, 4, 8, 4);

    m_st_txt_error_code  = new Label(m_side_error_panel, QString{});
    m_st_txt_error_desc  = new Label(m_side_error_panel, QString{});
    m_st_txt_error_extra = new Label(m_side_error_panel, QString{});
    errLayout->addWidget(m_st_txt_error_code);
    errLayout->addWidget(m_st_txt_error_desc);
    errLayout->addWidget(m_st_txt_error_extra);

    layout->addWidget(m_side_error_panel);
    setLayout(layout);
}

SideTools::~SideTools() = default;

void SideTools::set_table_panel(QWidget *tb)   { m_tabpanel = tb; }
void SideTools::msw_rescale()                  { if (m_side_tools) m_side_tools->msw_rescale(); }
bool SideTools::is_in_interval() const         { return m_side_tools && m_side_tools->is_in_interval(); }
void SideTools::set_current_printer_name(const std::string &n) { if (m_side_tools) m_side_tools->set_current_printer_name(n); }
void SideTools::set_current_printer_signal(WifiSignal s)       { if (m_side_tools) m_side_tools->set_current_printer_signal(s); }
void SideTools::set_none_printer_mode()                        { if (m_side_tools) m_side_tools->set_none_printer_mode(); }
void SideTools::start_interval()                               { if (m_side_tools) m_side_tools->start_interval(); }

void SideTools::update_status(MachineObject * /*obj*/) { /* stubbed — real impl needs DeviceManager */ }

void SideTools::update_connect_err_info(int code, const QString &desc, const QString &info)
{
    if (m_st_txt_error_code)  m_st_txt_error_code->setText(QString::number(code));
    if (m_st_txt_error_desc)  m_st_txt_error_desc->setText(desc);
    if (m_st_txt_error_extra) m_st_txt_error_extra->setText(info);
}

void SideTools::show_status(int status)
{
    const bool hasError = (status & MONITOR_DISCONNECTED) || (status & MONITOR_DISCONNECTED_SERVER);
    if (m_side_error_panel) m_side_error_panel->setVisible(hasError);
}

void SideToolsPanel::on_interval_timeout() {}

}} // namespace Slic3r::GUI

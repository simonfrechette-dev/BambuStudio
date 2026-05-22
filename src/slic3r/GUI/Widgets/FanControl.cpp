#include "FanControl.hpp"
#include "Label.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>

namespace Slic3r { namespace GUI {

// ============================================================
// Fan
// ============================================================
Fan::Fan(QWidget *parent) : QWidget(parent)
{
    setFixedSize(80, 80);
    m_bitmap_bk = ScalableBitmap(this, "fan_dash_bk", 60);
    for (int i = 0; i <= 10; ++i)
        m_bitmap_scales.push_back(ScalableBitmap(this, "fan_scale_" + std::to_string(i), 12));
}
void Fan::set_fan_speeds(int g)  { m_current_speeds = g; update(); }
void Fan::msw_rescale()          { update(); }
void Fan::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_bitmap_bk.bmp().IsOk())
        p.drawPixmap(QPoint(0, 0), m_bitmap_bk.bmp().scaled(size()));
    // Draw speed indicator text
    p.setPen(DRAW_HEAD_TEXT_COLOUR());
    p.setFont(Label::Body_12);
    p.drawText(rect(), Qt::AlignCenter, QString::number(m_current_speeds) + "%");
}

// ============================================================
// FanOperate
// ============================================================
FanOperate::FanOperate(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(28);
    m_bitmap_add      = ScalableBitmap(this, "fan_control_add",      16);
    m_bitmap_decrease = ScalableBitmap(this, "fan_control_decrease", 16);
}
void FanOperate::set_fan_speeds(int g) { m_current_speeds = g; update(); }
bool FanOperate::check_printing_state() { return true; }
void FanOperate::add_fan_speeds()
{
    m_current_speeds = std::min(m_current_speeds + 5, m_max_speeds);
    emit fanSpeedChanged(m_current_speeds);
    update();
}
void FanOperate::decrease_fan_speeds()
{
    m_current_speeds = std::max(m_current_speeds - 5, m_min_speeds);
    emit fanSpeedChanged(m_current_speeds);
    update();
}
void FanOperate::set_machine_obj(MachineObject *obj) { m_obj = obj; }
void FanOperate::msw_rescale() { update(); }
void FanOperate::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const QRect rc = rect();
    if (m_bitmap_decrease.bmp().IsOk())
        p.drawPixmap(QPoint(4, (rc.height() - 16) / 2), m_bitmap_decrease.bmp());
    p.setFont(Label::Body_12);
    p.setPen(DRAW_HEAD_TEXT_COLOUR());
    p.drawText(QRect(24, 0, rc.width() - 48, rc.height()), Qt::AlignCenter, QString::number(m_current_speeds));
    if (m_bitmap_add.bmp().IsOk())
        p.drawPixmap(QPoint(rc.width() - 20, (rc.height() - 16) / 2), m_bitmap_add.bmp());
}
void FanOperate::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->pos().x() < 24)        decrease_fan_speeds();
        else if (event->pos().x() > width() - 24) add_fan_speeds();
    }
    QWidget::mousePressEvent(event);
}

// ============================================================
// FanControlNew
// ============================================================
FanControlNew::FanControlNew(QWidget *parent, const AirDuctData &fan_data,
                              int mode_id, int part_id)
    : QWidget(parent)
    , m_fan_data(fan_data)
    , m_mode_id(mode_id)
    , m_part_id(part_id)
{
    auto *layout = new QHBoxLayout(this);
    m_static_name = new QLabel(this);
    layout->addWidget(m_static_name);
    m_fan_operate = new FanOperate(this);
    layout->addWidget(m_fan_operate);
    setLayout(layout);
}
void FanControlNew::command_control_fan() {}
bool FanControlNew::check_printing_state() { return true; }
void FanControlNew::set_machine_obj(MachineObject *obj) { m_obj = obj; }
void FanControlNew::set_name(const QString &name) { if (m_static_name) m_static_name->setText(name); }
void FanControlNew::set_fan_speed(int g) { m_current_speed = g; if (m_fan_operate) m_fan_operate->set_fan_speeds(g); }
void FanControlNew::set_fan_speed_percent(int speed) { set_fan_speed(speed); }
void FanControlNew::set_fan_switch(bool s) { m_switch_fan = s; update(); }
void FanControlNew::post_event() {}
void FanControlNew::on_swith_fan(bool on) { set_fan_switch(on); }
void FanControlNew::update_mode() { update(); }
void FanControlNew::msw_rescale() { update(); }

// ============================================================
// FanControlNewSwitchPanel
// ============================================================
FanControlNewSwitchPanel::FanControlNewSwitchPanel(QWidget *parent,
                                                    const QString &title,
                                                    const QString & /*tips*/,
                                                    bool on)
    : QWidget(parent)
    , switch_state_on(on)
{
    auto *layout = new QHBoxLayout(this);
    auto *lbl = new QLabel(title, this);
    layout->addWidget(lbl);
    m_switch_btn = new QLabel(this);
    layout->addWidget(m_switch_btn);
    setLayout(layout);
    m_bitmap_toggle_off = new ScalableBitmap(this, "toggle_off", 20);
    m_bitmap_toggle_on  = new ScalableBitmap(this, "toggle_on",  20);
    SetSwitchOn(on);
}
void FanControlNewSwitchPanel::SetSwitchOn(bool on)
{
    switch_state_on = on;
    if (m_switch_btn) {
        const ScalableBitmap &bmp = on ? *m_bitmap_toggle_on : *m_bitmap_toggle_off;
        if (bmp.bmp().IsOk()) m_switch_btn->setPixmap(bmp.bmp());
    }
}
void FanControlNewSwitchPanel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        SetSwitchOn(!switch_state_on);
        emit switchChanged(switch_state_on);
    }
    QWidget::mousePressEvent(event);
}

// ============================================================
// FanControlPopupNew
// ============================================================
FanControlPopupNew::FanControlPopupNew(QWidget *parent, MachineObject *obj,
                                        const AirDuctData &data)
    : QDialog(parent)
    , m_obj(obj)
    , m_data(data)
{
    setWindowTitle(tr("Fan Control"));
    auto *layout = new QVBoxLayout(this);
    m_mode_text = new Label(this, tr("Fan Control"));
    layout->addWidget(m_mode_text);
    setLayout(layout);
    CreateDuct();
}
void FanControlPopupNew::update_fan_data(MachineObject * /*obj*/) {}
void FanControlPopupNew::msw_rescale() {}
void FanControlPopupNew::init_names(MachineObject *) {}
void FanControlPopupNew::CreateDuct() {}
void FanControlPopupNew::UpdateParts() {}
void FanControlPopupNew::UpdatePartSubMode() {}
void FanControlPopupNew::update_fan_data(const AirDuctData &d) { m_data = d; }
void FanControlPopupNew::update_fan_data(AIR_FUN /*id*/, int /*speed*/) {}
void FanControlPopupNew::command_control_air_duct(int /*mode_id*/, int /*submode*/) {}

}} // namespace Slic3r::GUI

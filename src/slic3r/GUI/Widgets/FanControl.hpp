#ifndef slic3r_GUI_FANCONTROL_hpp_
#define slic3r_GUI_FANCONTROL_hpp_

#include "../QtExtensions.hpp"
#include "StaticBox.hpp"
#include "StepCtrl.hpp"
#include "Button.hpp"
#include "PopupWindow.hpp"
#include "Label.hpp"
#include "../DeviceCore/DevFan.h"

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QColor>
#include <QTimer>
#include <unordered_map>
#include <map>

// DevFan.h is clean C++, so AirDuctData / AIR_DUCT / AIR_FUN / AIR_DOOR are available.

namespace Slic3r { namespace GUI {

class MachineObject;
class SendModeSwitchButton;

// Colour helpers
inline constexpr QColor DRAW_TEXT_COLOUR()      { return QColor(0x89, 0x89, 0x89); }
inline constexpr QColor DRAW_HEAD_TEXT_COLOUR() { return QColor(0x26, 0x2e, 0x30); }

enum FanControlType {
    PART_FAN    = 0,
    AUX_FAN,
    EXHAUST_FAN,
    FILTER_FAN,
    CHAMBER_FAN,
    TOP_FAN
};

struct RotateOffSet { float rotate; QPoint offset; };

// ============================================================
// Fan — animated fan wheel display
// ============================================================
class Fan : public QWidget
{
    Q_OBJECT
public:
    Fan(QWidget *parent = nullptr);
    void set_fan_speeds(int g);
    void msw_rescale();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_current_speeds = 0;
    std::vector<RotateOffSet> m_rotate_offsets;
    std::vector<ScalableBitmap> m_bitmap_scales;
    ScalableBitmap m_bitmap_bk;
};

// ============================================================
// FanOperate — +/- speed controls
// ============================================================
class FanOperate : public QWidget
{
    Q_OBJECT
public:
    FanOperate(QWidget *parent = nullptr);
    void set_fan_speeds(int g);
    bool check_printing_state();
    void add_fan_speeds();
    void decrease_fan_speeds();
    void set_machine_obj(MachineObject *obj);
    void msw_rescale();

signals:
    void fanSpeedChanged(int speed);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int            m_current_speeds = 0;
    int            m_target_speed   = 0;
    int            m_min_speeds     = 0;
    int            m_max_speeds     = 255;
    ScalableBitmap m_bitmap_add;
    ScalableBitmap m_bitmap_decrease;
    MachineObject *m_obj = nullptr;
};

// ============================================================
// FanControlNew — single fan row with name, speed, and switch
// ============================================================
class FanControlNew : public QWidget
{
    Q_OBJECT
public:
    FanControlNew(QWidget *parent, const AirDuctData &fan_data, int mode_id,
                  int part_id);

    void update_obj_state(bool stat)             { m_update_already = stat; }
    void update_fan_data(const AirDuctData &data){ m_fan_data = data; }
    void command_control_fan();
    bool check_printing_state();
    void set_machine_obj(MachineObject *obj);
    void set_name(const QString &name);
    void set_mode_id(int id) { m_mode_id = id; }
    void set_part_id(int id) { m_part_id = id; }
    void set_fan_speed(int g);
    void set_fan_speed_percent(int speed);
    void set_fan_switch(bool s);
    void post_event();
    void on_swith_fan(bool on);
    void update_mode();
    void msw_rescale();

protected:
    MachineObject *m_obj = nullptr;
    QLabel        *m_static_name = nullptr;
    int            m_fan_id  = 0;
    bool           m_switch_fan     = false;
    bool           m_update_already = false;
    int            m_current_speed  = 0;
    int            m_part_id        = 0;
    int            m_mode_id        = 0;
    int            m_show_mode      = 0;
    AirDuctData    m_fan_data;
    bool           m_new_protocol   = false;
    ScalableBitmap *m_bitmap_fan         = nullptr;
    ScalableBitmap *m_bitmap_toggle_off  = nullptr;
    ScalableBitmap *m_bitmap_toggle_on   = nullptr;
    FanOperate     *m_fan_operate        = nullptr;
    QLabel         *m_static_status_name = nullptr;
};

// ============================================================
// FanControlNewSwitchPanel — on/off toggle row
// ============================================================
class FanControlNewSwitchPanel : public QWidget
{
    Q_OBJECT
public:
    FanControlNewSwitchPanel(QWidget *parent, const QString &title,
                              const QString &tips, bool on = true);
    bool IsSwitchOn() const { return switch_state_on; }
    void SetSwitchOn(bool on);

signals:
    void switchChanged(bool on);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool           switch_state_on    = false;
    QLabel        *m_switch_btn       = nullptr;
    ScalableBitmap *m_bitmap_toggle_off = nullptr;
    ScalableBitmap *m_bitmap_toggle_on  = nullptr;
};

// ============================================================
// FanControlPopupNew — full fan-control dialog
// ============================================================
class FanControlPopupNew : public QDialog
{
    Q_OBJECT
public:
    FanControlPopupNew(QWidget *parent, MachineObject *obj, const AirDuctData &data);

    void update_fan_data(MachineObject *obj);
    void msw_rescale();

private:
    void init_names(MachineObject *obj);
    void CreateDuct();
    void UpdateParts();
    void UpdatePartSubMode();
    void update_fan_data(const AirDuctData &data);
    void update_fan_data(AIR_FUN id, int speed);
    void command_control_air_duct(int mode_id, int submode = -1);

    std::map<int, FanControlNew *>         m_fan_control_list;
    std::unordered_map<int, SendModeSwitchButton *> m_mode_switch_btns;
    FanControlNewSwitchPanel              *m_cooling_filter_switch_panel = nullptr;
    Label                                 *m_mode_text = nullptr;
    QWidget                               *m_sub_mode_panel = nullptr;
    MachineObject                         *m_obj = nullptr;
    AirDuctData                            m_data;
    int                                    m_air_duct_time_out = 0;
    int                                    m_fan_set_time_out  = 0;
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_FANCONTROL_hpp_

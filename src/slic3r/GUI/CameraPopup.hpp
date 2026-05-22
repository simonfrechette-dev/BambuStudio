#ifndef slic3r_CameraPopup_hpp_
#define slic3r_CameraPopup_hpp_

// Phase 4 TODO: Qt port of CameraPopup

#include <QWidget>
#include <chrono>
#include "GUI.hpp"

class MachineObject;

namespace Slic3r {
namespace GUI {

// Event IDs replacing wxDECLARE_EVENT
static constexpr int EVT_VCAMERA_SWITCH    = 10100;
static constexpr int EVT_SDCARD_ABSENT_HINT = 10101;

class CameraPopup : public QWidget
{
    Q_OBJECT
public:
    explicit CameraPopup(QWidget* parent = nullptr);
    ~CameraPopup() override = default;

    void Popup(QWidget* focus = nullptr);
    bool OnDismiss();
    void sync_vcamera_state();
    void sync_local_rtsp_url();
    void on_timer(bool bTimerTriggered);
    void OnClose();
    virtual bool ProcessLeftDown(QMouseEvent* evt);

    static bool IsRecordEnable(MachineObject* obj_);

    MachineObject* obj = nullptr;
};


class CameraItem : public QWidget
{
    Q_OBJECT
public:
    explicit CameraItem(QWidget* parent = nullptr, const QString& name = QString(), const QString& img = QString());
    ~CameraItem() override = default;

    void OnEnter(QEvent* event);
    void OnLeave(QEvent* event);
    void OnSelected();
    bool IsSelected() { return m_selected; }
    void SetSelected(bool selected) { m_selected = selected; }
    QString GetName() { return m_name; }

private:
    QString m_name;
    bool    m_selected = false;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_CameraPopup_hpp_

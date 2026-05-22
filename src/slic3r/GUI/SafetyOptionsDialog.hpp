#ifndef slic3r_GUI_SafetyOptionsDialog_hpp_
#define slic3r_GUI_SafetyOptionsDialog_hpp_
#include <QWidget>
#include <QString>


#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "DeviceManager.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/StaticLine.hpp"
#include "Widgets/ComboBox.hpp"

// Previous definitions
class SwitchBoard;

namespace Slic3r { namespace GUI {

class SafetyOptionsDialog : public DPIDialog
{
protected:
    // settings
    QScrollArea* m_scrollwindow;

    CheckBox*    m_cb_open_door;
    CheckBox*    m_cb_idel_heating_protection;
    Label*       m_text_open_door;
    Label*       m_text_idel_heating_protection;
    Label*       m_text_idel_heating_protection_caption;
    SwitchBoard* m_open_door_switch_board;
    QWidget*    m_idel_heating_container { nullptr };

    // toast for idle heating unavailable
    QWidget *m_idel_heating_toast{nullptr};
    QTimer      m_idel_heating_toast_timer;
    bool         m_idel_protect_unavailable { false };

    QBoxLayout* create_settings_group(QWidget* parent);
    bool print_halt = false;

public:
    SafetyOptionsDialog(QWidget* parent);
    ~SafetyOptionsDialog();
    void on_dpi_changed(const QRect &suggested_rect) override;

    MachineObject *obj { nullptr };

    void             update_options(MachineObject *obj_);
    void             update_machine_obj(MachineObject *obj_);
    void setVisible(bool show) override;

private:
    void updateOpenDoorCheck(MachineObject *obj);
    void updateIdelHeatingProtect(MachineObject *obj);
    void show_idel_heating_toast(const QString &text);
};

}} // namespace Slic3r::GUI

#endif
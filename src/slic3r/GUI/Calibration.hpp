#ifndef slic3r_GUI_Calibration_hpp_
#define slic3r_GUI_Calibration_hpp_
#include <QWidget>
#include <QString>


#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "DeviceManager.hpp"
#include "Plater.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/StepCtrl.hpp"
#include "Widgets/CheckBox.hpp"

namespace Slic3r { namespace GUI {

class CalibrationDialog : public DPIDialog
{
private:
    std::map<std::string, ::CheckBox*> m_checkbox_list;

    QWidget* select_xcam_cali { nullptr };
    QWidget* select_bed_leveling { nullptr };
    QWidget* select_vibration { nullptr };
    QWidget* select_motor_noise { nullptr };
    QWidget* select_nozzle_cali{ nullptr };
    QWidget* select_heatbed_cali{ nullptr };
    QWidget* select_clumppos_cali{ nullptr };
    QWidget* create_check_option(QString title, QWidget *parent, QString tooltip, std::string param);

public:
    CalibrationDialog(Plater *plater = nullptr);
    ~CalibrationDialog();
    void on_dpi_changed(const QRect &suggested_rect) override;

    StepIndicator *m_calibration_flow;
    Button *       m_calibration_btn;
    MachineObject *m_obj = nullptr;

    std::vector<int> last_stage_list_info; 
    int              m_state{0};
    void             update_cali(MachineObject *obj);
    bool             is_stage_list_info_changed(MachineObject *obj);
    void             on_start_calibration(QMouseEvent &event);
    void             update_machine_obj(MachineObject *obj);
    void setVisible(bool show) override;
};

}} // namespace Slic3r::GUI

#endif

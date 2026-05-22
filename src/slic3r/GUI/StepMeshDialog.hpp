#ifndef _STEP_MESH_DIALOG_H_
#define _STEP_MESH_DIALOG_H_

#include <future>
#include <thread>
#include "GUI_App.hpp"
#include "GUI_Utils.hpp"
#include "libslic3r/Format/STEP.hpp"
#include "Widgets/Button.hpp"
class Button;

class StepMeshDialog : public Slic3r::GUI::DPIDialog
{
public:
    StepMeshDialog(QWidget* parent, Slic3r::Step& file, double linear_init, double angle_init);
    ~StepMeshDialog() override;
    void on_dpi_changed(const QRect& suggested_rect) override;
    inline double get_linear_defletion() {
        bool ok; double value = m_linear_last.toDouble(&ok);
        return ok ? value : m_last_linear;
    }
    inline double get_angle_defletion() {
        bool ok; double value = m_angle_last.toDouble(&ok);
        return ok ? value : m_last_angle;
    }
    inline bool get_split_compound_value() {
        return m_split_compound_checkbox->isChecked();
    }
private:
    Slic3r::Step& m_file;
    Button* m_button_ok = nullptr;
    Button* m_button_cancel = nullptr;
    QCheckBox* m_checkbox = nullptr;
    QCheckBox* m_split_compound_checkbox = nullptr;
    QString m_linear_last;
    QString m_angle_last;
    QLabel* mesh_face_number_text;
    double m_last_linear = 0.003;
    double m_last_angle = 0.5;
    unsigned int m_mesh_number = 0;
    boost::thread* m_task {nullptr};
    bool validate_number_range(const QString& value, double min, double max);
    void update_mesh_number_text();
    void on_task_done(QEvent& event);
    void stop_task();
};

#endif  // _STEP_MESH_DIALOG_H_

#ifndef slic3r_GUI_CalibrationWizardSavePage_hpp_
#define slic3r_GUI_CalibrationWizardSavePage_hpp_

#include "CalibrationWizardPage.hpp"
#include "Widgets/TextInput.hpp"

namespace Slic3r { namespace GUI {

enum CaliSaveStyle {
    CALI_SAVE_P1P_STYLE = 0,
    CALI_SAVE_X1_STYLE,
};


class CalibrationCommonSavePage : public CalibrationWizardPage
{
public:
    CalibrationCommonSavePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);

protected:
    QBoxLayout* m_top_sizer;
};

class PAColumnDataPanel : QWidget {
public:
    PAColumnDataPanel(
        QWidget* parent,
        bool is_failed,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    bool is_failed() { return m_is_failed; }
    int get_col_idx() { return m_col_idx; }
    QString get_k_str();
    QString get_n_str();
    QString get_name();
    void set_data(QString k_str, QString n_str, QString name);

private:
    QBoxLayout* m_top_sizer;
    TextInput* m_k_value_input;
    TextInput* m_n_value_input;
    ComboBox* m_comboBox_tray_name;
    int m_col_idx;
    bool m_is_failed;
};

class CaliSavePresetValuePanel : public QWidget
{
protected:
    QBoxLayout* m_top_sizer;
    CaliPagePicture* m_picture_panel;
    Label* m_value_title;
    Label* m_save_name_title;
    ::TextInput* m_input_value;
    ::TextInput* m_input_name;


public:
    CaliSavePresetValuePanel(
        QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_panel(QWidget* parent);

    void set_img(const std::string& bmp_name_in);
    void set_value_title(const QString& title);
    void set_save_name_title(const QString& title);
    void get_value(double& value);
    void get_save_name(std::string& name);
    void set_save_name(const std::string& name);
    void msw_rescale();
};


class CaliPASaveAutoPanel : public QWidget
{
public:
    CaliPASaveAutoPanel(
        QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_panel(QWidget* parent);

    void set_machine_obj(MachineObject* obj) { m_obj = obj; }

    std::vector<std::pair<int, std::string>> default_naming(std::vector<std::pair<int, std::string>> preset_names);
    void sync_cali_result(const std::vector<PACalibResult>& cali_result, const std::vector<PACalibResult>& history_result);
    void save_to_result_from_widgets(QWidget* window, bool* out_is_valid, QString* out_msg);
    bool get_result(std::vector<PACalibResult>& out_result);
    bool is_all_failed() { return m_is_all_failed; }

protected:
    void sync_cali_result_for_multi_extruder(const std::vector<PACalibResult> &cali_result, const std::vector<PACalibResult> &history_result);

protected:
    QBoxLayout* m_top_sizer;
    QWidget* m_complete_text_panel;
    QWidget* m_part_failed_panel;
    QWidget*    m_grid_panel{ nullptr };
    QWidget*    m_multi_extruder_grid_panel{ nullptr };
    std::vector<PACalibResult> m_calib_results;
    std::vector<PACalibResult> m_history_results;
    bool m_is_all_failed{ true };
    MachineObject* m_obj{ nullptr };
};

class CaliPASaveManualPanel : public QWidget
{
public:
    CaliPASaveManualPanel(
        QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);
    void create_panel(QWidget* parent);
    void set_save_img();
    void set_pa_cali_method(ManualPaCaliMethod method);

    void set_machine_obj(MachineObject* obj) { m_obj = obj; }

    void set_default_name(const QString& name);

    bool get_result(PACalibResult& out_result);

    virtual void setVisible(bool show) override;

    void msw_rescale();

protected:
    QBoxLayout* m_top_sizer;
    Label *          m_complete_text;
    CaliPagePicture* m_picture_panel;
    ::TextInput* m_save_name_input;
    ::TextInput* m_k_val;
    ::TextInput* m_n_val;

    MachineObject* m_obj{ nullptr };
};

class CaliPASaveP1PPanel : public QWidget
{
public:
    CaliPASaveP1PPanel(
        QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);
    void create_panel(QWidget* parent);
    void set_save_img();
    void set_pa_cali_method(ManualPaCaliMethod method);

    bool get_result(float* out_k, float* out_n);

    virtual void setVisible(bool show) override;

    void msw_rescale();

protected:
    QBoxLayout* m_top_sizer;
    Label *          m_complete_text;
    CaliPagePicture* m_picture_panel;
    ::TextInput* m_k_val;
    ::TextInput* m_n_val;
};

class CalibrationPASavePage : public CalibrationCommonSavePage
{
public:
    CalibrationPASavePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);

    void create_page(QWidget* parent);
    void set_cali_method(CalibrationMethod method) override;
    // sync widget value from obj cali result
    void sync_cali_result(MachineObject* obj);
    bool get_auto_result(std::vector<PACalibResult>& result) { return m_auto_panel->get_result(result); }
    bool is_all_failed() { return m_auto_panel->is_all_failed(); }
    bool get_manual_result(PACalibResult& result) { return m_manual_panel->get_result(result); }
    bool get_p1p_result(float* k, float* n) { return m_p1p_panel->get_result(k, n); }

    void show_panels(CalibrationMethod method, const PrinterSeries printer_ser);

    void on_device_connected(MachineObject* obj);

    void update(MachineObject* obj) override;

    virtual void setVisible(bool show) override;

    void msw_rescale() override;

protected:
    CaliPageStepGuide*  m_step_panel { nullptr };
    CaliPASaveAutoPanel*  m_auto_panel { nullptr };
    CaliPASaveManualPanel* m_manual_panel { nullptr };
    CaliPASaveP1PPanel* m_p1p_panel{ nullptr };
    PAPageHelpPanel* m_help_panel{ nullptr };

    CaliSaveStyle m_save_style;
};

class CalibrationFlowX1SavePage : public CalibrationCommonSavePage
{
public:
    CalibrationFlowX1SavePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);

    void create_page(QWidget* parent);

    // sync widget value from cali flow rate result
    void sync_cali_result(MachineObject* obj, const std::vector<FlowRatioCalibResult>& cali_result);
    void save_to_result_from_widgets(QWidget* window, bool* out_is_valid, QString* out_msg);
    bool get_result(std::vector<std::pair<QString, float>>& out_results);
    bool is_all_failed() { return m_is_all_failed; }

    virtual void setVisible(bool show) override;

    void msw_rescale() override;

protected:
    CaliPageStepGuide* m_step_panel{ nullptr };
    QWidget* m_complete_text_panel;
    QWidget* m_part_failed_panel;
    QWidget*           m_grid_panel{ nullptr };
    std::map<int, std::pair<QString, float>> m_save_results; // map<tray_id, <name, flow ratio>>
    bool m_is_all_failed{ true };
};

class CalibrationFlowCoarseSavePage : public CalibrationCommonSavePage
{
public:
    CalibrationFlowCoarseSavePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);

    void create_page(QWidget* parent);
    void set_save_img();

    void set_default_options(const QString &name);

    bool is_skip_fine_calibration();

    void set_curr_flow_ratio(float value);

    bool get_result(float* out_value, QString* out_name);

    virtual void setVisible(bool show) override;

    void update_print_error_info(int code, const std::string& msg, const std::string& extra) { m_sending_panel->update_print_error_info(code, msg, extra); }

    void on_cali_start_job();

    void on_cali_finished_job();

    void on_cali_cancel_job();

    std::shared_ptr<ProgressIndicator> get_sending_progress_bar() {
        return m_sending_panel->get_sending_progress_bar();
    }

    void msw_rescale() override;

protected:
    CaliPageStepGuide* m_step_panel{ nullptr };
    CaliPagePicture*   m_picture_panel;
    ComboBox*          m_optimal_block_coarse;
    TextInput*         m_save_name_input;

    Label* m_coarse_calc_result_text;
    CheckBox* m_checkBox_skip_calibration;

    bool m_skip_fine_calibration = false;
    float m_curr_flow_ratio;
    float m_coarse_flow_ratio;

    CaliPageSendingPanel* m_sending_panel{ nullptr };
};

class CalibrationFlowFineSavePage : public CalibrationCommonSavePage
{
public:
    CalibrationFlowFineSavePage(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0);

    void create_page(QWidget* parent);
    void set_save_img();

    void set_default_options(const QString &name);

    void set_curr_flow_ratio(float value);

    bool get_result(float* out_value, QString* out_name);

    virtual void setVisible(bool show) override;

    void msw_rescale() override;

protected:
    CaliPageStepGuide* m_step_panel{ nullptr };
    CaliPagePicture*   m_picture_panel;
    ComboBox*          m_optimal_block_fine;
    TextInput*         m_save_name_input;

    Label* m_fine_calc_result_text;

    float m_curr_flow_ratio;
    float m_fine_flow_ratio;
};

class CalibrationMaxVolumetricSpeedSavePage : public CalibrationCommonSavePage
{
public:
    CalibrationMaxVolumetricSpeedSavePage(QWidget *parent, int id = -1,
        const QPoint &pos = QPoint(), const QSize &size = QSize(), long style = 0);

    void create_page(QWidget *parent);
    void set_save_img();

    bool get_save_result(double &value, std::string &name);

    void set_prest_name(const std::string &name) { m_save_preset_panel->set_save_name(name); };

    virtual void setVisible(bool show) override;

protected:
    CaliPageStepGuide *m_step_panel{nullptr};
    CaliSavePresetValuePanel *m_save_preset_panel;
};


}} // namespace Slic3r::GUI

#endif
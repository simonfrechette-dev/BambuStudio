#ifndef slic3r_GUI_HelioReleaseNote_hpp_
#define slic3r_GUI_HelioReleaseNote_hpp_
#include "Widgets/SwitchButton.hpp"
#include <QWidget>
#include <QString>

#include <limits>

#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "HelioDragon.hpp"
#include "libslic3r/ExtrusionEntity.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/LinkLabel.hpp"
#include "Widgets/ScrolledWindow.hpp"


namespace Slic3r { namespace GUI {



class HelioStatementDialog : public DPIDialog
{
private:
    Label *m_title{nullptr};
    Button *m_button_confirm{nullptr};
    Button *m_button_cancel{nullptr};

    int current_page{ 0 }; // 0 = legal terms page, 1 = PAT page
    std::shared_ptr<int> shared_ptr{nullptr};

    QWidget* page_legal_panel{ nullptr };
    QWidget* page_pat_panel{ nullptr };

    // Accordion sections
    QWidget* terms_section_panel{ nullptr };
    QWidget* terms_content_panel{ nullptr };
    QWidget* privacy_section_panel{ nullptr };
    QWidget* privacy_content_panel{ nullptr };
    QScrollArea* m_scroll_panel{ nullptr };
    bool terms_expanded{ true };
    bool privacy_expanded{ true };

    // Checkbox for agreement
    ::CheckBox* m_agree_checkbox{ nullptr };

    Label* pat_err_label{ nullptr };
    TextInput* helio_input_pat{ nullptr };
    QLabel* helio_pat_refresh{ nullptr };
    QLabel* helio_pat_eview{ nullptr };
    QLabel* helio_pat_dview{ nullptr };
    QLabel* helio_pat_copy{ nullptr };
    Button* copy_pat_button{ nullptr };
    
    int m_original_tooltip_delay{500};

public:
    HelioStatementDialog(QWidget *parent = nullptr);
    ~HelioStatementDialog();

    void on_dpi_changed(const QRect &suggested_rect) override;
    void show_err_info(std::string type);
    void show_pat_option(std::string opt);
    void show_legal_page();
    void show_pat_page();
    void request_pat();
    void on_confirm(QMouseEvent& e);
    void report_consent_install();
    void open_url(std::string type);
    void create_legal_page();
    void create_pat_page();
    void toggle_terms_section();
    void toggle_privacy_section();
    void update_confirm_button_state();
    void refresh_checkbox_visual();

    void OnLoaded(QEvent& event);
    void OnTitleChanged(QEvent& event);
    void OnError(QEvent& event);
};

class HelioRemainUsageTime : public QWidget
{
public:
    Label* label_click_to_use{ nullptr };
    Label* label_click_to_buy{ nullptr };
    HelioRemainUsageTime(QWidget *parent = nullptr, QString label = QString());

public:
    void UpdateRemainTime(int remain_time);
    void UpdateHelpTips(int type);

private:
    void Create(QString label);
   
private:
    int    m_remain_usage_time = 0;
    Label* m_label_remain_usage_time;
};

// Theme colors for HelioInputDialog
struct HelioInputDialogTheme {
    QColor bg;           // Main background
    QColor card;         // Card background
    QColor card2;        // Slightly darker card (for inputs)
    QColor border;       // Card border
    QColor text;         // Primary text
    QColor muted;        // Secondary/muted text
    QColor purple;       // Purple accent (simulation)
    QColor blue;         // Blue accent (optimization)
};

class HelioCheckBadgePanel;

class HelioInputDialog : public DPIDialog
{
private:
    bool use_advanced_settings{false};
    bool only_advanced_settings{false};
    bool is_no_chamber{false};
    bool show_temp_input{false};

    // Mode card panels (replacing toggle buttons)
    QWidget* simulation_card_panel{nullptr};
    QWidget* optimization_card_panel{nullptr};
    Label* simulation_card_title{nullptr};
    Label* simulation_card_subtitle{nullptr};
    Label* optimization_card_title{nullptr};
    Label* optimization_card_subtitle{nullptr};
    QLabel* simulation_mode_icon{nullptr};
    QLabel* optimization_mode_icon{nullptr};
    QPixmap simulation_icon_color;
    QPixmap simulation_icon_gray;
    QPixmap optimization_icon_color;
    QPixmap optimization_icon_gray;

    // Keep toggle button pointers for compatibility
    CustomToggleButton* togglebutton_simulate{nullptr};
    CustomToggleButton* togglebutton_optimize{nullptr};

    QWidget* last_tid_panel{nullptr};
    Label*   last_tid_label{nullptr};

    std::map<std::string, TextInput*> m_input_items;
    std::map<std::string, ComboBox*> m_combo_items;
    Button* m_button_confirm{nullptr};
    QString m_lastValidValue = QString();

    std::string m_material_id;
    std::vector<HelioQuery::PrintPriorityOption> m_print_priority_options;
    std::vector<HelioQuery::PrintPriorityOption> m_available_print_priority_options;
    bool m_print_priority_loading{false};
    bool m_using_fallback_print_priority{false};  // Track if using hard-coded fallback options (old method)

    QWidget* panel_simulation{nullptr};
    QWidget* panel_pay_optimization{nullptr};
    QWidget* panel_optimization{nullptr};
    QWidget* panel_velocity_volumetric{nullptr};

    // Card wrapper panels
    QWidget* card_simulation{nullptr};
    QWidget* card_account_status{nullptr};
    QWidget* card_environment{nullptr};
    QWidget* card_optimization_settings{nullptr};

    QWidget* advanced_settings_link{nullptr};
    LinkLabel* buy_now_link{nullptr};
    Button* buy_now_button{nullptr};
    LinkLabel* helio_wiki_link{nullptr};

    int current_action{-1}; //0-simulation 1-optimization
    int support_optimization{0}; //-1-no 0-yes
    int remaining_optimization_times{0};

    QLabel* advanced_options_icon{nullptr};
    QWidget* panel_advanced_option{nullptr};

    std::shared_ptr<int> shared_ptr{nullptr};

    Label* m_label_subscription{nullptr};
    Label* m_label_monthly_quota{nullptr};
    Label* m_label_addons{nullptr};
    bool m_free_trial_eligible{false};
    bool m_is_free_trial_active{false};
    bool m_is_free_trial_claimed{false};
    
    // Theme helper
    HelioInputDialogTheme get_theme() const;
    
    int m_original_tooltip_delay{500};
public:
    HelioInputDialog(QWidget *parent = nullptr, const std::string& material_id = "");
    ~HelioInputDialog();

public:
    int get_action() const { return current_action; }

    HelioQuery::SimulationInput get_simulation_input(bool& ok);
    HelioQuery::OptimizationInput get_optimization_input(bool& ok);
    
    // Force "Slicer default" limits mode (disables "Helio default" option)
    void set_force_slicer_default(bool force);

private:
    QBoxLayout* create_input_item(QWidget* parent, std::string key, QString name, QString unit,
                                  const std::vector<std::shared_ptr<TextInputValChecker>>& checkers);
    QBoxLayout* create_combo_item(QWidget* parent, std::string key,  QString name, std::map<int, QString> combolist, int def, int width = 120);
    QBoxLayout* create_input_optimize_layers(QWidget* parent, int layer_count);

    // Card creation helper
    QWidget* create_card_panel(QWidget* parent, const QString& title = QString());
    void update_mode_card_styling(int selected_action);

    // Print priority helper methods
    QBoxLayout* create_print_priority_combo(QWidget* parent);
    void populate_print_priority_dropdown(ComboBox* combobox);
    void fetch_print_priority_options();
    void update_print_priority_dropdown();

    void on_selected_simulation(QMouseEvent& e) { update_action(0); }
    void on_selected_optimaztion(QMouseEvent& e){ update_action(1); }
    void on_confirm(QMouseEvent& e);
    void on_dpi_changed(const QRect& suggested_rect) override;
    void update_action(int action);
    void show_advanced_mode();
    void on_unlimited_click(QMouseEvent& e);
    
public:
    void set_initial_action(int action) { update_action(action); }
};

class HelioPatNotEnoughDialog : public DPIDialog
{
public:
    HelioPatNotEnoughDialog(QWidget* parent = nullptr);
    ~HelioPatNotEnoughDialog();
    void on_dpi_changed(const QRect& suggested_rect) override;
};


class HelioRatingDialog : public DPIDialog
{
public:
    HelioRatingDialog(QWidget *parent = nullptr, int original = 0, int optimized = 0, std::string mean_impro = "", std::string std_impro = "");
    ~HelioRatingDialog() {};

    QString format_improvement(QString imp);
    void on_dpi_changed(const QRect &suggested_rect) override;
    std::shared_ptr<int> shared_ptr{nullptr};
    void show_rating(std::vector<QLabel *> stars, int rating);
    int original_time;
    int optimized_time;
    std::string optimized_id;
    bool finish_rating = false;
    QString quality_mean_improvement;
    QString quality_std_improvement;   
};

class HelioSimulationResultsDialog : public DPIDialog
{
public:
    HelioSimulationResultsDialog(QWidget *parent = nullptr, 
                                  HelioQuery::SimulationResult simulation = HelioQuery::SimulationResult(),
                                  int original_print_time_seconds = 0,
                                  const std::vector<std::pair<ExtrusionRole, float>>& roles_times = {});
    ~HelioSimulationResultsDialog() {};

    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_enhance_speed_quality(QMouseEvent& event);
    void on_view_details(QMouseEvent& event);

private:
    HelioQuery::SimulationResult m_simulation;
    int m_original_print_time_seconds;
    std::vector<std::pair<ExtrusionRole, float>> m_roles_times;
    Button* m_button_enhance{nullptr};
    Button* m_button_view_details{nullptr};
    Button* m_button_close{nullptr};
    
    // Fix suggestions expandable section
    QWidget* m_fix_suggestions_content{nullptr};
    Label* m_fix_suggestions_arrow{nullptr};
    Label* m_fix_suggestions_preview{nullptr};
    bool m_fix_suggestions_expanded{false};
    
    // Nested expanders within fix suggestions
    QWidget* m_advanced_content{nullptr};
    Label* m_advanced_arrow{nullptr};
    bool m_advanced_expanded{false};
    
    QWidget* m_expert_content{nullptr};
    Label* m_expert_arrow{nullptr};
    bool m_expert_expanded{false};
    
    QWidget* m_learn_more_content{nullptr};
    Label* m_learn_more_arrow{nullptr};
    bool m_learn_more_expanded{false};
    
    QScrollArea* m_fix_suggestions_scroll{nullptr};
    
    HelioInputDialogTheme get_theme() const;
    QString get_outcome_text(const HelioQuery::PrintInfo& print_info);
    QString get_analysis_text(const HelioQuery::PrintInfo& print_info);
    QString get_fix_suggestions_preview(const HelioQuery::PrintInfo& print_info);
    QString format_time_improvement(int original_seconds, double speed_factor);
    void toggle_fix_suggestions();
    void toggle_advanced();
    void toggle_expert();
    void toggle_learn_more();
    void create_fix_suggestions_section(QBoxLayout* parent_sizer, const HelioInputDialogTheme& theme);
};

}} // namespace Slic3r::GUI

#endif

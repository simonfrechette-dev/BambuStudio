#ifndef slic3r_Tab_hpp_
#define slic3r_Tab_hpp_

// Phase 4 TODO: full Qt port of Tab hierarchy
// This stub preserves the public API for callers

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

#include <QWidget>
#include <QString>
#include <QColor>

#include "Event.hpp"
#include "ConfigManipulation.hpp"
#include "libslic3r/Preset.hpp"
#include "DeviceCore/DevDefs.h"
#include "DeviceCore/DevNozzleSystem.h"
#include "I18N.hpp"

// widget_t: Qt port of wx-based function type
using widget_t = std::function<QLayout*(QWidget*)>;

// ForceOption: from UnsavedChangesDialog.hpp (forward copy)
enum ForceOption { fopTransfer, fopSave, fopDiscard, fopNone };

// Forward declarations
class TabCtrl;
class ComboBox;
class ScalableButton;  // defined in wxExtensions.hpp at global scope

namespace Slic3r {

class DynamicPrintConfig;
class ModelConfig;
class ObjectBase;
class PresetBundle;

namespace GUI {

class TabPresetComboBox;
class OG_CustomCtrl;
class ParamsPanel;
class ConfigOptionsGroup;
class Page;
class Field;
class ogStaticText;
class SwitchButton;
class MultiSwitchButton;

using ConfigOptionsGroupShp = std::shared_ptr<ConfigOptionsGroup>;
using PageShp               = std::shared_ptr<Page>;

// Minimal Page stub (full implementation in Tab.cpp)
class Page
{
public:
    Page(QWidget* parent, const QString& title, int iconID, QWidget* tab_owner) {}
    ~Page() {}

    bool  m_is_modified_values{ false };
    bool  m_is_nonsys_values{ true };
    bool  m_split_multi_line      = false;
    bool  m_option_label_at_right = false;

    std::vector<ConfigOptionsGroupShp> m_optgroups;
    DynamicPrintConfig* m_config = nullptr;

    QString   title()  const { return m_title; }
    size_t    iconID() const { return m_iconID; }
    QWidget*  parent() const { return m_parent; }
    bool      get_show() const { return m_show; }

    void set_config(DynamicPrintConfig* cfg) { m_config = cfg; }
    void reload_config() {}
    void update_visibility(ConfigOptionMode mode, bool update_controls) {}
    void activate(ConfigOptionMode mode, std::function<void()> throw_if_canceled) {}
    void clear() {}
    void msw_rescale() {}
    void sys_color_changed() {}
    void refresh() {}

    Field*   get_field(const t_config_option_key& opt_key, int opt_index = -1) const { return nullptr; }
    bool     set_value(const t_config_option_key& opt_key, const boost::any& value) { return false; }

    ConfigOptionsGroupShp new_optgroup(const QString& title, const QString& icon = QString(), int label_width = -1, bool is_extruder_og = false);
    const ConfigOptionsGroupShp get_optgroup(const QString& title) const;

    std::map<std::string, std::string> m_opt_id_map;

private:
    QWidget*  m_parent  = nullptr;
    QString   m_title;
    size_t    m_iconID  = 0;
    bool      m_show    = true;
};


class Tab : public QWidget
{
    Q_OBJECT
public:
    // BBS
    Tab(QWidget* parent, const QString& title, Preset::Type type);
    ~Tab() override = default;

    QWidget*       parent()    const { return m_qt_parent; }
    QString        title()     const { return m_title; }
    std::string    name()      const;
    Preset::Type   type()      const { return m_type; }
    bool           completed() const { return m_completed; }
    virtual bool   supports_printer_technology(const PrinterTechnology tech) const = 0;

    void       create_preset_tab();
    void       update_ui_items_related_on_parent_preset(const Preset* selected_preset_parent);
    void       load_current_preset();
    void       reactive_preset_combo_box();
    void       rebuild_page_tree();
    void       update_btns_enabling();
    void       update_preset_choice();
    bool       select_preset(std::string preset_name = "", bool delete_current = false,
                             const std::string &last_selected_ph_printer_name = "",
                             bool force_select = false, bool force_no_transfer = false);
    bool       may_discard_current_dirty_preset(PresetCollection* presets = nullptr,
                                                const std::string& new_printer_name = "",
                                                bool no_transfer = false,
                                                ForceOption force_op = ForceOption::fopNone);

    virtual void clear_pages();
    virtual void update_description_lines();
    virtual void activate_selected_page(std::function<void()> throw_if_canceled);

    void compare_preset();
    void save_preset(std::string name = std::string(), bool detach = false,
                     bool save_to_project = false, bool from_input = false,
                     std::string input_name = "");
    void delete_preset();
    void toggle_show_hide_incompatible();
    void update_show_hide_incompatible_button();
    void update_ui_from_settings();
    void update_label_colours();
    void decorate();
    void update_changed_ui();
    void get_sys_and_mod_flags(const std::string& opt_key, bool& sys_page, bool& modified_page);
    void update_changed_tree_ui();
    void update_undo_buttons();
    void update_extruder_switch_colors();
    void update_all_extruder_options_status();
    bool disable_arc_fitting();
    void on_roll_back_value(const bool to_sys = false);

    PageShp       add_options_page(const QString& title, const std::string& icon, bool is_extruder_pages = false);
    static QString translate_category(const QString& title, Preset::Type preset_type);

    virtual void OnActivate();
    virtual void on_preset_loaded() {}
    virtual void build() = 0;
    virtual void update() = 0;
    virtual void toggle_options() = 0;
    virtual void init_options_list();
    virtual void update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) {}
    void         load_initial_data();
    void         update_dirty();
    void         update_tab_ui(bool update_plater_presets = false);
    void         load_config(const DynamicPrintConfig& config);
    virtual void reload_config();
    void         update_mode();
    void         update_visibility();
    virtual void msw_rescale();
    virtual void sys_color_changed();

    Field*       get_field(const t_config_option_key& opt_key, int opt_index = -1) const;
    Field*       get_field(const t_config_option_key& opt_key, Page** selected_page, int opt_index = -1);
    void         toggle_option(const std::string& opt_key, bool toggle, int opt_index = -1);
    void         toggle_line(const std::string& opt_key, bool toggle, int opt_index = -1);
    bool         current_preset_is_dirty() const;
    bool         saved_preset_is_dirty() const;
    void         update_saved_preset_from_current_preset();
    void         update_pages_with_multi_variant();

    DynamicPrintConfig*   get_config()    { return m_config; }
    PresetCollection*     get_presets()   { return m_presets; }
    TabPresetComboBox*    get_combo_box() { return m_presets_choice; }

    virtual void on_value_change(const std::string& opt_key, const boost::any& value);

    void update_wiping_button_visibility();
    void activate_option(const std::string& opt_key, const QString& category);
    void apply_searcher();
    void cache_config_diff(const std::vector<std::string>& selected_options);
    void apply_config_from_cache();
    void show_timelapse_warning_dialog();

    bool update_current_page_in_background(int& item);
    void unselect_tree_item();
    void set_expanded(bool value);
    void restore_last_select_item();

    static bool validate_custom_gcode(const QString& title, const std::string& gcode);
    bool        validate_custom_gcodes();
    bool        validate_custom_gcodes_was_shown{ false };
    void        set_just_edit(bool just_edit);

    void update_extruder_variants(int extruder_id = -1, bool reload = true);
    void switch_excluder(int extruder_id = -1, bool reload = true);
    void sync_excluder();
    void update_nozzle_status_display();
    int  get_current_active_extruder();
    void show_wiki();

    std::vector<DevNozzle> collect_nozzles(int extruder_id, ExtruderType ext_type, NozzleFlowType flow_type, bool& connected);
    NozzleVolumeType       get_actual_nozzle_volume_type(int extruder_id);

    // Public data members accessed directly by callers
    PresetBundle*         m_preset_bundle  = nullptr;
    bool                  m_show_btn_incompatible_presets = false;
    PresetCollection*     m_presets        = nullptr;
    DynamicPrintConfig*   m_config         = nullptr;
    ogStaticText*         m_parent_preset_description_line = nullptr;
    ScalableButton*       m_detach_preset_btn = nullptr;
    std::map<std::string, QColor> m_colored_Label_colors;
    int                   m_update_cnt    = 0;
    SwitchButton*         m_mode_view     = nullptr;
    MultiSwitchButton*    m_extruder_switch = nullptr;
    MultiSwitchButton*    m_variant_combo   = nullptr;
    ScalableButton*       m_extruder_sync   = nullptr;
    QWidget*              m_extruder_sync_box = nullptr;
    std::vector<NozzleVolumeType> m_actual_nozzle_volumes;
    std::unordered_map<std::string, int> m_multi_variant_status;

    const std::map<QString, std::string>& get_category_icon_map() { return m_category_icon; }

protected:
    QWidget*           m_qt_parent      = nullptr;
    Preset::Type           m_type           = Preset::TYPE_INVALID;
    std::string            m_name;
    QString                m_title;
    TabPresetComboBox*     m_presets_choice = nullptr;
    std::vector<PageShp>   m_pages;
    Page*                  m_active_page    = nullptr;
    bool                   m_completed      = false;
    int                    m_em_unit        = 0;
    bool                   m_is_default_preset = false;
    bool                   m_just_edit      = false;
    bool                   m_postpone_update_ui = false;
    ConfigOptionMode       m_mode           = comAdvanced;
    DynamicPrintConfig     m_cache_config;
    std::vector<std::string> m_cache_options;
    ConfigManipulation     m_config_manipulation;
    std::map<QString, std::string> m_category_icon;
    std::vector<Preset::Type> m_dependent_tabs;
    std::map<std::string, int> m_options_list;
    std::map<std::string, int> m_all_extruder_options_status;
    int                    m_opt_status_value = 0;
    bool                   m_is_modified_values{ false };
    bool                   m_is_nonsys_values{ true };
    bool                   m_page_switch_running = false;
    bool                   m_page_switch_planned = false;
    bool                   m_is_timelapse_wipe_tower_already_prompted = false;
    bool                   m_show_incompatible_presets = false;
    bool                   m_disable_tree_sel_changed_event = false;

    struct PresetDependencies {
        Preset::Type  type          = Preset::TYPE_INVALID;
        QWidget*      checkbox      = nullptr;
        ScalableButton* btn         = nullptr;
        std::string   key_list;
        std::string   key_condition;
        QString       dialog_title;
        QString       dialog_label;
    };
    PresetDependencies m_compatible_printers;
    PresetDependencies m_compatible_prints;

    void create_line_with_widget(ConfigOptionsGroup* optgroup, const std::string& opt_key,
                                 const std::string& path, widget_t widget);
    void compatible_widget_reload(PresetDependencies& deps);
    void load_key_value(const std::string& opt_key, const boost::any& value, bool saved_value = false);
    bool tree_sel_change_delayed(QEvent* event);
    void on_presets_changed();
    void build_preset_description_line(ConfigOptionsGroup* optgroup);
    void update_preset_description_line();
    void update_frequently_changed_parameters();
    void set_tooltips_text();
    void filter_diff_option(std::vector<std::string>& options);
    ConfigManipulation get_config_manipulation();
};


class TabPrint : public Tab
{
public:
    TabPrint(QWidget* parent, Preset::Type type = Preset::TYPE_PRINT)
        : Tab(parent, _L("Process"), type) {}
    ~TabPrint() override = default;

    void build() override;
    void reload_config() override;
    void update_description_lines() override;
    void toggle_options() override;
    void update() override;
    void clear_pages() override;
    bool supports_printer_technology(const PrinterTechnology tech) const override { return tech == ptFFF; }

private:
    ogStaticText* m_recommended_thin_wall_thickness_description_line = nullptr;
    ogStaticText* m_top_bottom_shell_thickness_explanation = nullptr;
};


class TabPrintModel : public TabPrint
{
public:
    TabPrintModel(QWidget* parent, std::vector<std::string> const& keys);
    ~TabPrintModel() override = default;

    void build() override;
    void set_model_config(std::map<ObjectBase*, ModelConfig*> const& object_configs);
    bool has_model_config() const { return !m_object_configs.empty(); }
    const std::map<ObjectBase*, ModelConfig*>& get_object_configs() const { return m_object_configs; }
    void update_model_config();
    virtual void reset_model_config();
    bool has_key(std::string const& key);

protected:
    void activate_selected_page(std::function<void()> throw_if_canceled) override;
    void on_value_change(const std::string& opt_key, const boost::any& value) override;
    virtual void notify_changed(ObjectBase* object) = 0;
    void reload_config() override;
    void update_custom_dirty(std::vector<std::string>& dirty_options, std::vector<std::string>& nonsys_options) override;

    std::vector<std::string>         m_keys;
    PresetCollection                 m_prints;
    Tab*                             m_parent_tab = nullptr;
    std::map<ObjectBase*, ModelConfig*> m_object_configs;
    std::vector<std::string>         m_all_keys;
    std::vector<std::string>         m_null_keys;
    bool                             m_back_to_sys = false;
};


class TabPrintPlate : public TabPrintModel
{
public:
    TabPrintPlate(QWidget* parent);
    ~TabPrintPlate() override = default;
    void build() override;
    void reset_model_config() override;
    void update_bed_type_list();
    void update_mixed_filament_seq_state();
    int  show_spiral_mode_settings_dialog(bool is_object_config) { return m_config_manipulation.show_spiral_mode_settings_dialog(is_object_config); }

protected:
    void on_value_change(const std::string& opt_key, const boost::any& value) override;
    void notify_changed(ObjectBase* object) override;
    void update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) override;
};


class TabPrintObject : public TabPrintModel
{
public:
    TabPrintObject(QWidget* parent);
    ~TabPrintObject() override = default;
protected:
    void notify_changed(ObjectBase* object) override;
};


class TabPrintPart : public TabPrintModel
{
public:
    TabPrintPart(QWidget* parent);
    ~TabPrintPart() override = default;
protected:
    void notify_changed(ObjectBase* object) override;
};


class TabPrintLayer : public TabPrintModel
{
public:
    TabPrintLayer(QWidget* parent);
    ~TabPrintLayer() override = default;
protected:
    void notify_changed(ObjectBase* object) override;
    void update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) override;
};


class TabFilament : public Tab
{
public:
    TabFilament(QWidget* parent)
        : Tab(parent, _L("Filament"), Slic3r::Preset::TYPE_FILAMENT) {}
    ~TabFilament() override = default;

    void build() override;
    void reload_config() override;
    void update_description_lines() override;
    void toggle_options() override;
    void update() override;
    void init_options_list() override;
    void clear_pages() override;
    bool supports_printer_technology(const PrinterTechnology tech) const override { return tech == ptFFF; }

private:
    ogStaticText* m_volumetric_speed_description_line = nullptr;
    ogStaticText* m_cooling_description_line = nullptr;
    void add_filament_overrides_page();
    void update_filament_overrides_page();
    void update_volumetric_flow_preset_hints();
    std::map<std::string, QWidget*> m_overrides_options;
};


class TabPrinter : public Tab
{
public:
    ScalableButton* m_reset_to_filament_color = nullptr;
    size_t  m_extruders_count = 0;
    size_t  m_extruders_count_old = 0;
    size_t  m_initial_extruders_count = 0;
    size_t  m_sys_extruders_count = 0;
    size_t  m_cache_extruder_count = 0;
    std::vector<std::string> m_extruder_variant_list;
    PrinterTechnology m_printer_technology = ptFFF;

    TabPrinter(QWidget* parent)
        : Tab(parent, _L("Machine"), Slic3r::Preset::TYPE_PRINTER) {}
    ~TabPrinter() override = default;

    void build() override;
    void build_fff();
    void build_sla();
    void reload_config() override;
    void activate_selected_page(std::function<void()> throw_if_canceled) override;
    void clear_pages() override;
    void toggle_options() override;
    void update() override;
    void update_fff();
    void update_sla();
    void update_pages();
    void extruders_count_changed(size_t extruders_count);
    PageShp build_kinematics_page();
    void build_unregular_pages(bool from_initial_build = false);
    void on_preset_loaded() override;
    void init_options_list() override;
    void msw_rescale() override;
    bool supports_printer_technology(const PrinterTechnology /*tech*/) const override { return true; }
    void set_extruder_volume_type(int extruder_id, NozzleVolumeType type);
    void cache_extruder_cnt();
    bool apply_extruder_cnt_from_cache();

private:
    bool m_has_single_extruder_MM_page = false;
    bool m_use_silent_mode = false;
    bool m_rebuild_kinematics_page = false;
    ogStaticText* m_fff_print_host_upload_description_line = nullptr;
    ogStaticText* m_sla_print_host_upload_description_line = nullptr;
    std::vector<PageShp> m_pages_fff;
    std::vector<PageShp> m_pages_sla;
    std::vector<int>     m_extruder_type;
    std::string          m_base_preset_name;
    std::string          m_base_preset_model;
};


class TabSLAMaterial : public Tab
{
public:
    TabSLAMaterial(QWidget* parent)
        : Tab(parent, _(_devL("Material Settings")), Slic3r::Preset::TYPE_SLA_MATERIAL) {}
    ~TabSLAMaterial() override = default;

    void build() override;
    void reload_config() override;
    void toggle_options() override;
    void update() override;
    bool supports_printer_technology(const PrinterTechnology tech) const override { return tech == ptSLA; }
};


class TabSLAPrint : public Tab
{
public:
    TabSLAPrint(QWidget* parent)
        : Tab(parent, _(L("Process Settings")), Slic3r::Preset::TYPE_SLA_PRINT) {}
    ~TabSLAPrint() override = default;

    ogStaticText* m_support_object_elevation_description_line = nullptr;

    void build() override;
    void reload_config() override;
    void update_description_lines() override;
    void toggle_options() override;
    void update() override;
    void clear_pages() override;
    bool supports_printer_technology(const PrinterTechnology tech) const override { return tech == ptSLA; }
};

} // namespace GUI
} // namespace Slic3r

#endif /* slic3r_Tab_hpp_ */

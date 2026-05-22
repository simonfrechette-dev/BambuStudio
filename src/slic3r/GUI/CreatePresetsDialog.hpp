#ifndef slic3r_CreatePresetsDialog_hpp_
#define slic3r_CreatePresetsDialog_hpp_

#include "libslic3r/Preset.hpp"
#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "miniz.h"
#include "ParamsDialog.hpp"
#include "json_diff.hpp"

namespace Slic3r {
namespace GUI {

class CreateFilamentPresetDialog : public DPIDialog
{
public:
    CreateFilamentPresetDialog(QWidget *parent);
    ~CreateFilamentPresetDialog();

protected:
    enum FilamentOptionType {
        VENDOR = 0,
        TYPE,
        SERIAL,
        FILAMENT_PRESET,
        PRESET_FOR_PRINTER,
        FILAMENT_NAME_COUNT
    };

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;
    bool        is_check_box_selected();
    QBoxLayout *create_item(FilamentOptionType option_type);
    QBoxLayout *create_vendor_item();
    QBoxLayout *create_type_item();
    QBoxLayout *create_serial_item();
    QBoxLayout *create_filament_preset_item();
    QBoxLayout *create_filament_preset_for_printer_item();
    QBoxLayout *create_button_item();

private:
    void          clear_filament_preset_map();
    QStringList get_filament_preset_choices();
    QBoxLayout *  create_radio_item(QString title, QWidget *parent, QString tooltip, std::vector<std::pair<RadioBox *, QString>> &radiobox_list);
    void          select_curr_radiobox(std::vector<std::pair<RadioBox *, QString>> &radiobox_list, int btn_idx);
    QString      curr_create_filament_type();
    void          get_filament_presets_by_machine();
    void          get_all_filament_presets();
    void          get_all_visible_printer_name();
    void          update_dialog_size();
    template<typename T>
    void          sort_printer_by_nozzle(std::vector<std::pair<std::string, T>> &printer_name_to_filament_preset);

private:
    struct CreateType
    {
        QString base_filament;
        QString base_filament_preset;
    };

private:
    std::vector<std::pair<RadioBox *, QString>>                     m_create_type_btns;
    std::unordered_map<::CheckBox *, std::pair<std::string, Preset *>> m_filament_preset;
    std::unordered_map<::CheckBox *, std::pair<std::string, Preset *>> m_machint_filament_preset;
    std::unordered_map<std::string, std::vector<Preset *>>           m_filament_choice_map;
    std::unordered_map<QString, std::string>                        m_public_name_to_filament_id_map;
    std::unordered_map<std::string, Preset *>                        m_all_presets_map;
    std::unordered_set<std::string>                                  m_system_filament_types_set;
    std::set<std::string>                                            m_visible_printers;
    CreateType                                                       m_create_type;
    Button *                                                         m_button_create                = nullptr;
    Button *                                                         m_button_cancel                = nullptr;
    ComboBox *                                                       m_filament_vendor_combobox     = nullptr;
    ::CheckBox *                                                     m_can_not_find_vendor_checkbox = nullptr;
    ComboBox *                                                       m_filament_type_combobox       = nullptr;
    ComboBox *                                                       m_exist_vendor_combobox        = nullptr;
    ComboBox *                                                       m_filament_preset_combobox     = nullptr;
    TextInput *                                                      m_filament_custom_vendor_input = nullptr;
    QGridLayout *                                                    m_filament_presets_sizer       = nullptr;
    QWidget *                                                        m_filament_preset_panel        = nullptr;
    QScrollArea *                                               m_scrolled_preset_panel        = nullptr;
    TextInput *                                                      m_filament_serial_input        = nullptr;
    QBoxLayout *                                                     m_scrolled_sizer               = nullptr;
    QLabel *                                                   m_filament_preset_text         = nullptr;

};

class CreatePrinterPresetDialog : public DPIDialog
{
public:
    CreatePrinterPresetDialog(QWidget *parent);
    ~CreatePrinterPresetDialog();

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

/********************************************************    Control Construction    *****************************************************/
    QBoxLayout *create_step_switch_item();
    //Create Printer Page1
    void        create_printer_page1(QWidget *parent);
    QBoxLayout *create_type_item(QWidget *parent);
    QBoxLayout *create_printer_item(QWidget *parent);
    QBoxLayout *create_nozzle_diameter_item(QWidget *parent);
    QBoxLayout *create_bed_shape_item(QWidget *parent);
    QBoxLayout *create_bed_size_item(QWidget *parent);
    QBoxLayout *create_origin_item(QWidget *parent);
    QBoxLayout *create_hot_bed_stl_item(QWidget *parent);
    QBoxLayout *create_hot_bed_svg_item(QWidget *parent);
    QBoxLayout *create_max_print_height_item(QWidget *parent);
    QBoxLayout *create_page1_btns_item(QWidget *parent);
    //Improt Presets Page2
    void create_printer_page2(QWidget *parent);
    QBoxLayout *create_printer_preset_item(QWidget *parent);
    QBoxLayout *create_presets_item(QWidget *parent);
    QBoxLayout *create_presets_template_item(QWidget *parent);
    QBoxLayout *create_page2_btns_item(QWidget *parent);

    void show_page1();
    void show_page2();

/**********************************************************    Data Interaction    *******************************************************/
    bool          data_init();
    void          on_select_printer_model(QEvent &e);
    void          set_current_visible_printer();
    void          select_curr_radiobox(std::vector<std::pair<RadioBox *, QString>> &radiobox_list, int btn_idx);
    void          select_all_preset_template(std::vector<std::pair<::CheckBox *, Preset *>> &preset_templates);
    void          deselect_all_preset_template(std::vector<std::pair<::CheckBox *, Preset *>> &preset_templates);
    void          update_presets_list(bool jast_template = false);
    void          on_preset_model_value_change(QEvent &e);
    void          clear_preset_combobox();
    bool          save_printable_area_config(Preset *preset);
    bool          check_printable_area();
    bool          validate_input_valid();
    void          load_texture();
    void          load_model_stl();
    bool          load_system_and_user_presets_with_curr_model(PresetBundle &temp_preset_bundle, bool just_template = false);
    void          generate_process_presets_data(std::vector<Preset const *> presets, std::string nozzle);
    void          update_preset_list_size();
    std::string   get_printer_vendor() const;
    std::string   get_printer_model() const;
    std::string   get_nozzle_diameter() const;
    std::string   get_custom_printer_model() const;
    std::string   get_custom_printer_name() const;
    QStringList printer_preset_sort_with_nozzle_diameter(const VendorProfile &vendor_profile, float nozzle_diameter);

    QBoxLayout *create_radio_item(QString title, QWidget *parent, QString tooltip, std::vector<std::pair<RadioBox *, QString>> &radiobox_list);

    QString    curr_create_preset_type() const;
    QString    curr_create_printer_type() const;

private:
    struct CreatePrinterType
    {
        QString create_printer;
        QString create_nozzle;
        QString base_template;
        QString base_curr_printer;
    };

    CreatePrinterType                                  m_create_type;
    std::vector<std::pair<RadioBox *, QString>>       m_create_type_btns;
    std::vector<std::pair<RadioBox *, QString>>       m_create_presets_btns;
    std::vector<std::pair<::CheckBox *, Preset *>>           m_filament_preset;
    std::vector<std::pair<::CheckBox *, Preset *>>           m_process_preset;
    std::unordered_map<std::string, std::shared_ptr<Preset>> m_printer_name_to_preset;
    VendorProfile                                      m_printer_preset_vendor_selected;
    Slic3r::VendorProfile::PrinterModel                m_printer_preset_model_selected;
    bool                                               rewritten                        = false;
    Preset *                                           m_printer_preset                 = nullptr;
    QLabel *                                   m_step_1                         = nullptr;
    QLabel *                                   m_step_2                         = nullptr;
    Button *                                           m_button_OK                      = nullptr;
    Button *                                           m_button_create                  = nullptr;
    Button *                                           m_button_page1_cancel            = nullptr;
    Button *                                           m_button_page2_cancel            = nullptr;
    Button *                                           m_button_page2_back              = nullptr;
    Button *                                           m_button_bed_stl                 = nullptr;
    Button *                                           m_button_bed_svg                 = nullptr;
    QScrollArea *                                 m_page1                          = nullptr;
    QWidget *                                          m_page2                          = nullptr;
    QScrollArea *                                 m_scrolled_preset_window         = nullptr;
    QBoxLayout *                                       m_scrooled_preset_sizer          = nullptr;
    ComboBox *                                         m_select_vendor                  = nullptr;
    ComboBox *                                         m_select_model                   = nullptr;
    ComboBox *                                         m_select_printer                 = nullptr;
    ::CheckBox *                                       m_can_not_find_vendor_combox     = nullptr;
    ::CheckBox *                                       m_can_not_find_nozzle_checkbox   = nullptr;
    QLabel *                                     m_can_not_find_vendor_text       = nullptr;
    QLineEdit *                                       m_custom_vendor_text_ctrl        = nullptr;
    QLineEdit *                                       m_custom_model_text_ctrl         = nullptr;
    QLineEdit *                                       m_custom_nozzle_diameter_ctrl    = nullptr;
    ComboBox *                                         m_nozzle_diameter                = nullptr;
    ComboBox *                                         m_printer_vendor                 = nullptr;
    ComboBox *                                         m_printer_model                  = nullptr;
    TextInput *                                        m_bed_size_x_input               = nullptr;
    TextInput *                                        m_bed_size_y_input               = nullptr;
    TextInput *                                        m_bed_origin_x_input             = nullptr;
    TextInput *                                        m_bed_origin_y_input             = nullptr;
    TextInput *                                        m_print_height_input             = nullptr;
    QGridLayout *                                      m_filament_preset_template_sizer = nullptr;
    QGridLayout *                                      m_process_preset_template_sizer  = nullptr;
    QWidget *                                          m_filament_preset_panel          = nullptr;
    QWidget *                                          m_process_preset_panel           = nullptr;
    QWidget *                                          m_preset_template_panel          = nullptr;
    QBoxLayout *                                       m_filament_sizer                 = nullptr;
    QWidget *                                          m_printer_info_panel             = nullptr;
    QBoxLayout *                                       m_page1_sizer                    = nullptr;
    QBoxLayout *                                       m_printer_info_sizer             = nullptr;
    QBoxLayout *                                       m_page2_sizer                    = nullptr;
    QLabel *                                     m_upload_stl_tip_text            = nullptr;
    QLabel *                                     m_upload_svg_tip_text            = nullptr;
    std::string                                        m_custom_texture;
    std::string                                        m_custom_model;
};

enum SuccessType {
    PRINTER = 0,
    FILAMENT
};

class CreatePresetSuccessfulDialog : public DPIDialog
{
public:
    CreatePresetSuccessfulDialog(QWidget *parent, const SuccessType &create_success_type);
    ~CreatePresetSuccessfulDialog();

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

private:
    Button *m_button_ok     = nullptr;
    Button *m_button_cancel = nullptr;
};

class ExportConfigsDialog : public DPIDialog
{
public:
    ExportConfigsDialog(QWidget *parent);
    ~ExportConfigsDialog();//to do: delete preset

protected:

    struct ExportType
    {
        QString preset_bundle;
        QString filament_bundle;
        QString printer_preset;
        QString filament_preset;
        QString process_preset;
    };

    enum ExportCase {
        INITIALIZE_FAIL = 0,
        ADD_FILE_FAIL,
        ADD_BUNDLE_STRUCTURE_FAIL,
        FINALIZE_FAIL,
        OPEN_ZIP_WRITTEN_FILE,
        EXPORT_CANCEL,
        EXPORT_SUCCESS,
        CASE_COUNT,
    };

private:
    void        data_init();
    void        select_curr_radiobox(std::vector<std::pair<RadioBox *, QString>> &radiobox_list, int btn_idx);
    void        on_dpi_changed(const QRect &suggested_rect) override;
    void        show_export_result(const ExportCase &export_case);
    bool        has_check_box_selected();
    bool        preset_is_not_compatible_bbl_printer(Preset *preset);
    bool        earse_preset_fields_for_safe(Preset *preset);
    std::string initial_file_path(const QString &path, const std::string &sub_file_path);
    std::string initial_file_name(const QString &path, const std::string file_name);
    QBoxLayout *create_export_config_item(QWidget *parent);
    QBoxLayout *create_button_item(QWidget *parent);
    QBoxLayout *create_select_printer(QWidget *parent);
    QBoxLayout *create_radio_item(QString title, QWidget *parent, QString tooltip, std::vector<std::pair<RadioBox *, QString>> &radiobox_list);
    std::string create_structure_file(json &structure);
    int         initial_zip_archive(mz_zip_archive &zip_archive, const std::string &file_path);
    ExportCase  save_zip_archive_to_file(mz_zip_archive &zip_archive);
    ExportCase  save_presets_to_zip(const std::string &export_file, const std::vector<std::pair<std::string, std::string>> &config_paths);
    ExportCase  archive_preset_bundle_to_file(const QString &path);
    ExportCase  archive_filament_bundle_to_file(const QString &path);
    ExportCase  archive_printer_preset_to_file(const QString &path);
    ExportCase  archive_filament_preset_to_file(const QString &path);
    ExportCase  archive_process_preset_to_file(const QString &path);

private:
    std::vector<std::pair<RadioBox *, QString>>           m_export_type_btns;
    std::vector<std::pair<::CheckBox *, Preset *>>         m_preset;         // for printer preset bundle,printer preset, process preset export
    std::vector<std::pair<::CheckBox *, std::string>>      m_printer_name;    // for filament and peocess preset export, collaborate with m_filament_name_to_presets
    std::unordered_map<std::string, Preset *>              m_printer_presets;//first: printer name, second: printer presets have same printer name
    std::unordered_map<std::string, std::vector<Preset *>> m_filament_presets;//first: printer name, second: filament presets have same printer name
    std::unordered_map<std::string, std::vector<Preset *>> m_process_presets;//first: printer name, second: filament presets have same printer name
    std::unordered_map<std::string, std::vector<std::pair<std::string, Preset *>>> m_filament_name_to_presets;//first: filament name, second presets have same filament name and printer name in vector
    ExportType                                             m_exprot_type;
    QBoxLayout *                                           m_main_sizer             = nullptr;
    QScrollArea *                                     m_scrolled_preset_window = nullptr;
    QGridLayout *                                          m_preset_sizer   = nullptr;
    QWidget *                                              m_presets_window = nullptr;
    Button *                                               m_button_ok      = nullptr;
    Button *                                               m_button_cancel  = nullptr;
    QLabel *                                         m_serial_text    = nullptr;
};

class CreatePresetForPrinterDialog : public DPIDialog
{
public:
    CreatePresetForPrinterDialog(QWidget *parent, std::string filament_type, std::string filament_id, std::string filament_vendor, std::string filament_name);
    ~CreatePresetForPrinterDialog();

private:
    void        on_dpi_changed(const QRect &suggested_rect) override;
    void        get_visible_printer_and_compatible_filament_presets();
    QBoxLayout *create_selected_printer_preset_sizer();
    QBoxLayout *create_selected_filament_preset_sizer();
    QBoxLayout *create_button_sizer();

private:
    std::string                                                                       m_filament_id;
    std::string                                                                       m_filament_name;
    std::string                                                                       m_filament_vendor;
    std::string                                                                       m_filament_type;
    std::shared_ptr<PresetBundle>                                                     m_preset_bundle;
    ComboBox *                                                                        m_selected_printer  = nullptr;
    ComboBox *                                                                        m_selected_filament = nullptr;
    Button *                                                                          m_ok_btn            = nullptr;
    Button *                                                                          m_cancel_btn        = nullptr;
    std::unordered_map<QString, std::shared_ptr<Preset>>                             filament_choice_to_filament_preset;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Preset>>>             m_printer_compatible_filament_presets; // need be used when add presets

};

class EditFilamentPresetDialog;

class PresetTree
{
public:
    PresetTree(EditFilamentPresetDialog *dialog);

    QWidget *get_preset_tree(std::pair<std::string, std::vector<std::shared_ptr<Preset>>> printer_and_presets);

private:
    QWidget *get_root_item(QWidget *parent, const std::string &printer_name);

    QWidget *get_child_item(QWidget *parent, std::shared_ptr<Preset> preset, std::string printer_name, int preset_index, bool is_last = false);

    void delete_preset(std::string printer_name, int need_delete_preset_index);

    void edit_preset(std::string printer_name, int need_edit_preset_index);

private:
    EditFilamentPresetDialog *                                   m_parent_dialog = nullptr;
    std::pair<std::string, std::vector<std::shared_ptr<Preset>>> m_printer_and_presets;

};

class EditFilamentPresetDialog : public DPIDialog
{
public:
    EditFilamentPresetDialog(QWidget *parent, FilamentInfomation *filament_info);
    ~EditFilamentPresetDialog();

    QWidget *get_preset_tree_panel() { return m_preset_tree_panel; }
    std::shared_ptr<Preset> get_need_edit_preset() { return m_need_edit_preset; }
    void     set_printer_name(const std::string &printer_name) { m_selected_printer = printer_name; }
    void     set_need_delete_preset_index(int need_delete_preset_index) { m_need_delete_preset_index = need_delete_preset_index; }
    void     set_need_edit_preset_index(int need_edit_preset_index) { m_need_edit_preset_index = need_edit_preset_index; }
    void     delete_preset();
    void     edit_preset();

private:
    void        on_dpi_changed(const QRect &suggested_rect) override;
    bool        get_same_filament_id_presets(std::string filament_id);
    void        update_preset_tree();
    QBoxLayout *create_filament_basic_info();
    QBoxLayout *create_add_filament_btn();
    QBoxLayout *create_preset_tree_sizer();
    QBoxLayout *create_button_sizer();

private:
    PresetTree *                                                          m_preset_tree_creater = nullptr;
    std::string                                                           m_filament_id;
    std::string                                                           m_filament_name;
    std::string                                                           m_vendor_name;
    std::string                                                           m_filament_type;
    std::string                                                           m_filament_serial;
    Button *                                                              m_add_filament_btn         = nullptr;
    Button *                                                              m_del_filament_btn         = nullptr;
    Button *                                                              m_ok_btn                   = nullptr;
    QBoxLayout *                                                          m_preset_tree_sizer        = nullptr;
    QWidget *                                                             m_preset_tree_panel        = nullptr;
    QScrollArea *                                                    m_preset_tree_window       = nullptr;
    QBoxLayout *                                                          m_main_sizer               = nullptr;
    QLabel *                                                        m_note_text                = nullptr;
    int                                                                   m_need_delete_preset_index = -1;
    int                                                                   m_need_edit_preset_index   = -1;
    std::shared_ptr<Preset>                                               m_need_edit_preset;
    std::string                                                           m_selected_printer         = "";
    std::unordered_map<std::string, std::vector<std::shared_ptr<Preset>>> m_printer_compatible_presets;

};

}
}
#endif
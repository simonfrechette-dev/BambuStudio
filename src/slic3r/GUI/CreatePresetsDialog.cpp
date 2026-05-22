// CreatePresetsDialog.cpp — Qt6 port (stub implementations)
#include "CreatePresetsDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "ParamsDialog.hpp"

namespace Slic3r { namespace GUI {

// ─── CreateFilamentPresetDialog ───────────────────────────────────────────────

CreateFilamentPresetDialog::CreateFilamentPresetDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Create Filament Preset"));
    setMinimumWidth(560);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 16);
    main_layout->setSpacing(8);

    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Create Filament Preset"));
    main_layout->addWidget(title);

    // Vendor row
    main_layout->addLayout(create_vendor_item());
    // Type row
    main_layout->addLayout(create_type_item());
    // Serial row
    main_layout->addLayout(create_serial_item());
    // Filament preset
    main_layout->addLayout(create_filament_preset_item());
    // Preset for printer
    main_layout->addLayout(create_filament_preset_for_printer_item());
    // Buttons
    main_layout->addLayout(create_button_item());
}

CreateFilamentPresetDialog::~CreateFilamentPresetDialog() {}

void CreateFilamentPresetDialog::on_dpi_changed(const QRect & /*r*/) {}

bool CreateFilamentPresetDialog::is_check_box_selected()
{
    for (auto &[cb, p] : m_filament_preset)
        if (cb->isChecked()) return true;
    return false;
}

QBoxLayout *CreateFilamentPresetDialog::create_item(FilamentOptionType /*type*/)
{
    return new QHBoxLayout();
}

QBoxLayout *CreateFilamentPresetDialog::create_vendor_item()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Vendor:"));
    lbl->setFixedWidth(130);
    row->addWidget(lbl);
    m_filament_vendor_combobox = new ComboBox(this);
    row->addWidget(m_filament_vendor_combobox, 1);
    m_can_not_find_vendor_checkbox = new CheckBox(this);
    row->addWidget(m_can_not_find_vendor_checkbox);
    auto *not_found_lbl = new Label(this, Label::Body_12);
    not_found_lbl->setText(_L("Can't find vendor"));
    row->addWidget(not_found_lbl);
    return row;
}

QBoxLayout *CreateFilamentPresetDialog::create_type_item()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Filament type:"));
    lbl->setFixedWidth(130);
    row->addWidget(lbl);
    m_filament_type_combobox = new ComboBox(this);
    row->addWidget(m_filament_type_combobox, 1);
    return row;
}

QBoxLayout *CreateFilamentPresetDialog::create_serial_item()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Serial / Name:"));
    lbl->setFixedWidth(130);
    row->addWidget(lbl);
    m_filament_serial_input = new TextInput(this);
    row->addWidget(m_filament_serial_input, 1);
    return row;
}

QBoxLayout *CreateFilamentPresetDialog::create_filament_preset_item()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Base preset:"));
    lbl->setFixedWidth(130);
    row->addWidget(lbl);
    m_filament_preset_combobox = new ComboBox(this);
    row->addWidget(m_filament_preset_combobox, 1);
    return row;
}

QBoxLayout *CreateFilamentPresetDialog::create_filament_preset_for_printer_item()
{
    auto *col = new QVBoxLayout();
    m_filament_preset_text = new QLabel(_L("Preset for printer:"), this);
    col->addWidget(m_filament_preset_text);

    m_filament_preset_panel = new QWidget(this);
    m_filament_presets_sizer = new QGridLayout(m_filament_preset_panel);
    m_scrolled_preset_panel = new QScrollArea(this);
    m_scrolled_preset_panel->setWidget(m_filament_preset_panel);
    m_scrolled_preset_panel->setWidgetResizable(true);
    m_scrolled_preset_panel->setFixedHeight(180);
    col->addWidget(m_scrolled_preset_panel);
    return col;
}

QBoxLayout *CreateFilamentPresetDialog::create_button_item()
{
    auto *row = new QHBoxLayout();
    row->addStretch(1);
    m_button_cancel = new Button(this, _L("Cancel"));
    row->addWidget(m_button_cancel);
    m_button_create = new Button(this, _L("Create"));
    m_button_create->SetValue(true);
    row->addWidget(m_button_create);
    connect(m_button_create, &Button::clicked, this, &QDialog::accept);
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    return row;
}

void CreateFilamentPresetDialog::clear_filament_preset_map() {}
QStringList CreateFilamentPresetDialog::get_filament_preset_choices() { return {}; }
QBoxLayout *CreateFilamentPresetDialog::create_radio_item(QString /*title*/, QWidget * /*parent*/,
    QString /*tooltip*/, std::vector<std::pair<RadioBox *, QString>> & /*list*/)
{ return new QHBoxLayout(); }
void CreateFilamentPresetDialog::select_curr_radiobox(
    std::vector<std::pair<RadioBox *, QString>> & /*list*/, int /*idx*/) {}
QString CreateFilamentPresetDialog::curr_create_filament_type() { return {}; }
void CreateFilamentPresetDialog::get_filament_presets_by_machine() {}
void CreateFilamentPresetDialog::get_all_filament_presets() {}
void CreateFilamentPresetDialog::get_all_visible_printer_name() {}
void CreateFilamentPresetDialog::update_dialog_size() {}

// ─── CreatePrinterPresetDialog ───────────────────────────────────────────────

CreatePrinterPresetDialog::CreatePrinterPresetDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Create Printer Preset"));
    setMinimumSize(600, 500);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 16);
    main_layout->setSpacing(8);

    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Create Printer Preset"));
    main_layout->addWidget(title);

    // Steps indicator
    auto *steps_row = new QHBoxLayout();
    m_step_1 = new QLabel(_L("Step 1: Basic Info"), this);
    m_step_1->setStyleSheet("font-weight: bold;");
    steps_row->addWidget(m_step_1);
    m_step_2 = new QLabel(_L("Step 2: Import Presets"), this);
    steps_row->addWidget(m_step_2);
    steps_row->addStretch(1);
    main_layout->addLayout(steps_row);

    // Page 1 (scrollable)
    m_page1 = new QScrollArea(this);
    m_page1->setWidgetResizable(true);
    auto *p1_content = new QWidget(m_page1);
    m_page1->setWidget(p1_content);
    create_printer_page1(p1_content);
    main_layout->addWidget(m_page1, 1);

    // Page 2 (hidden initially)
    m_page2 = new QWidget(this);
    create_printer_page2(m_page2);
    m_page2->hide();
    main_layout->addWidget(m_page2, 1);
}

CreatePrinterPresetDialog::~CreatePrinterPresetDialog() {}
void CreatePrinterPresetDialog::on_dpi_changed(const QRect & /*r*/) {}

QBoxLayout *CreatePrinterPresetDialog::create_step_switch_item() { return new QHBoxLayout(); }
void CreatePrinterPresetDialog::create_printer_page1(QWidget * /*p*/) {}
QBoxLayout *CreatePrinterPresetDialog::create_type_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_printer_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_nozzle_diameter_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_bed_shape_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_bed_size_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_origin_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_hot_bed_stl_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_hot_bed_svg_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_max_print_height_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_page1_btns_item(QWidget * /*p*/) { return new QHBoxLayout(); }
void CreatePrinterPresetDialog::create_printer_page2(QWidget * /*p*/) {}
QBoxLayout *CreatePrinterPresetDialog::create_printer_preset_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_presets_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_presets_template_item(QWidget * /*p*/) { return new QHBoxLayout(); }
QBoxLayout *CreatePrinterPresetDialog::create_page2_btns_item(QWidget * /*p*/) { return new QHBoxLayout(); }
void CreatePrinterPresetDialog::show_page1() { if (m_page1) m_page1->show(); if (m_page2) m_page2->hide(); }
void CreatePrinterPresetDialog::show_page2() { if (m_page1) m_page1->hide(); if (m_page2) m_page2->show(); }
bool CreatePrinterPresetDialog::data_init() { return true; }
void CreatePrinterPresetDialog::on_select_printer_model(QEvent & /*e*/) {}
void CreatePrinterPresetDialog::set_current_visible_printer() {}
void CreatePrinterPresetDialog::select_curr_radiobox(std::vector<std::pair<RadioBox *, QString>> & /*list*/, int /*idx*/) {}
void CreatePrinterPresetDialog::select_all_preset_template(std::vector<std::pair<CheckBox *, Preset *>> & /*list*/) {}
void CreatePrinterPresetDialog::deselect_all_preset_template(std::vector<std::pair<CheckBox *, Preset *>> & /*list*/) {}
void CreatePrinterPresetDialog::update_presets_list(bool /*just_template*/) {}
void CreatePrinterPresetDialog::on_preset_model_value_change(QEvent & /*e*/) {}
void CreatePrinterPresetDialog::clear_preset_combobox() {}
bool CreatePrinterPresetDialog::save_printable_area_config(Preset * /*p*/) { return true; }
bool CreatePrinterPresetDialog::check_printable_area() { return true; }
bool CreatePrinterPresetDialog::validate_input_valid() { return true; }
void CreatePrinterPresetDialog::load_texture() {}
void CreatePrinterPresetDialog::load_model_stl() {}
bool CreatePrinterPresetDialog::load_system_and_user_presets_with_curr_model(
    PresetBundle & /*bundle*/, bool /*just_template*/) { return true; }
void CreatePrinterPresetDialog::generate_process_presets_data(
    std::vector<Preset const *> /*presets*/, std::string /*nozzle*/) {}
void CreatePrinterPresetDialog::update_preset_list_size() {}
std::string CreatePrinterPresetDialog::get_printer_vendor() const { return {}; }
std::string CreatePrinterPresetDialog::get_printer_model() const { return {}; }
std::string CreatePrinterPresetDialog::get_nozzle_diameter() const { return {}; }
std::string CreatePrinterPresetDialog::get_custom_printer_model() const { return {}; }
std::string CreatePrinterPresetDialog::get_custom_printer_name() const { return {}; }
QStringList CreatePrinterPresetDialog::printer_preset_sort_with_nozzle_diameter(
    const VendorProfile & /*vendor*/, float /*nozzle*/) { return {}; }
QBoxLayout *CreatePrinterPresetDialog::create_radio_item(
    QString /*title*/, QWidget * /*parent*/, QString /*tooltip*/,
    std::vector<std::pair<RadioBox *, QString>> & /*list*/)
{ return new QHBoxLayout(); }
QString CreatePrinterPresetDialog::curr_create_preset_type() const { return {}; }
QString CreatePrinterPresetDialog::curr_create_printer_type() const { return {}; }

// ─── CreatePresetSuccessfulDialog ────────────────────────────────────────────

CreatePresetSuccessfulDialog::CreatePresetSuccessfulDialog(QWidget *parent,
                                                           const SuccessType &type)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Preset Created"));
    setMinimumWidth(360);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(12);

    auto *icon = new Label(this, Label::Head_16);
    icon->setText(type == SuccessType::PRINTER ? _L("Printer preset created successfully!")
                                               : _L("Filament preset created successfully!"));
    layout->addWidget(icon);

    auto *btn_row = new QHBoxLayout();
    btn_row->addStretch(1);

    m_button_cancel = new Button(this, _L("Close"));
    btn_row->addWidget(m_button_cancel);

    m_button_ok = new Button(this, _L("Open Preset"));
    m_button_ok->SetValue(true);
    btn_row->addWidget(m_button_ok);
    layout->addLayout(btn_row);

    connect(m_button_ok,     &Button::clicked, this, &QDialog::accept);
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
}

CreatePresetSuccessfulDialog::~CreatePresetSuccessfulDialog() {}
void CreatePresetSuccessfulDialog::on_dpi_changed(const QRect & /*r*/) {}

// ─── ExportConfigsDialog ─────────────────────────────────────────────────────

ExportConfigsDialog::ExportConfigsDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Export Configs"));
    setMinimumSize(500, 400);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 16);
    main_layout->setSpacing(8);

    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Export Printer / Filament / Process Configs"));
    main_layout->addWidget(title);

    m_main_sizer = main_layout;

    main_layout->addLayout(create_export_config_item(this));
    main_layout->addLayout(create_button_item(this));
}

ExportConfigsDialog::~ExportConfigsDialog() {}
void ExportConfigsDialog::on_dpi_changed(const QRect & /*r*/) {}
void ExportConfigsDialog::data_init() {}
void ExportConfigsDialog::select_curr_radiobox(
    std::vector<std::pair<RadioBox *, QString>> & /*list*/, int /*idx*/) {}
void ExportConfigsDialog::show_export_result(const ExportCase & /*c*/) {}
bool ExportConfigsDialog::has_check_box_selected() { return false; }
bool ExportConfigsDialog::preset_is_not_compatible_bbl_printer(Preset * /*p*/) { return false; }
bool ExportConfigsDialog::earse_preset_fields_for_safe(Preset * /*p*/) { return true; }
std::string ExportConfigsDialog::initial_file_path(const QString & /*path*/, const std::string & /*sub*/) { return {}; }
std::string ExportConfigsDialog::initial_file_name(const QString & /*path*/, const std::string /*name*/) { return {}; }
QBoxLayout *ExportConfigsDialog::create_export_config_item(QWidget * /*parent*/) { return new QVBoxLayout(); }
QBoxLayout *ExportConfigsDialog::create_button_item(QWidget *parent)
{
    auto *row = new QHBoxLayout();
    row->addStretch(1);
    m_button_cancel = new Button(parent, _L("Cancel"));
    row->addWidget(m_button_cancel);
    m_button_ok = new Button(parent, _L("Export"));
    m_button_ok->SetValue(true);
    row->addWidget(m_button_ok);
    connect(m_button_ok,     &Button::clicked, this, &QDialog::accept);
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    return row;
}
QBoxLayout *ExportConfigsDialog::create_select_printer(QWidget * /*parent*/) { return new QHBoxLayout(); }
QBoxLayout *ExportConfigsDialog::create_radio_item(
    QString /*title*/, QWidget * /*parent*/, QString /*tooltip*/,
    std::vector<std::pair<RadioBox *, QString>> & /*list*/)
{ return new QHBoxLayout(); }
std::string ExportConfigsDialog::create_structure_file(json & /*j*/) { return {}; }
int ExportConfigsDialog::initial_zip_archive(mz_zip_archive & /*zip*/, const std::string & /*path*/) { return 0; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::save_zip_archive_to_file(mz_zip_archive & /*zip*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::save_presets_to_zip(
    const std::string & /*file*/, const std::vector<std::pair<std::string, std::string>> & /*paths*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::archive_preset_bundle_to_file(const QString & /*path*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::archive_filament_bundle_to_file(const QString & /*path*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::archive_printer_preset_to_file(const QString & /*path*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::archive_filament_preset_to_file(const QString & /*path*/)
{ return ExportCase::EXPORT_CANCEL; }
ExportConfigsDialog::ExportCase ExportConfigsDialog::archive_process_preset_to_file(const QString & /*path*/)
{ return ExportCase::EXPORT_CANCEL; }

// ─── CreatePresetForPrinterDialog ────────────────────────────────────────────

CreatePresetForPrinterDialog::CreatePresetForPrinterDialog(QWidget *parent,
    std::string filament_type, std::string filament_id,
    std::string filament_vendor, std::string filament_name)
    : DPIDialog(parent)
    , m_filament_id(std::move(filament_id))
    , m_filament_name(std::move(filament_name))
    , m_filament_vendor(std::move(filament_vendor))
    , m_filament_type(std::move(filament_type))
{
    setWindowTitle(_L("Create Preset for Printer"));
    setMinimumWidth(480);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 16);
    layout->setSpacing(8);

    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Create Filament Preset for Printer"));
    layout->addWidget(title);

    layout->addLayout(create_selected_printer_preset_sizer());
    layout->addLayout(create_selected_filament_preset_sizer());
    layout->addLayout(create_button_sizer());
}

CreatePresetForPrinterDialog::~CreatePresetForPrinterDialog() {}
void CreatePresetForPrinterDialog::on_dpi_changed(const QRect & /*r*/) {}
void CreatePresetForPrinterDialog::get_visible_printer_and_compatible_filament_presets() {}
QBoxLayout *CreatePresetForPrinterDialog::create_selected_printer_preset_sizer()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Printer:"));
    lbl->setFixedWidth(120);
    row->addWidget(lbl);
    m_selected_printer = new ComboBox(this);
    row->addWidget(m_selected_printer, 1);
    return row;
}
QBoxLayout *CreatePresetForPrinterDialog::create_selected_filament_preset_sizer()
{
    auto *row = new QHBoxLayout();
    auto *lbl = new Label(this, Label::Body_14);
    lbl->setText(_L("Base filament:"));
    lbl->setFixedWidth(120);
    row->addWidget(lbl);
    m_selected_filament = new ComboBox(this);
    row->addWidget(m_selected_filament, 1);
    return row;
}
QBoxLayout *CreatePresetForPrinterDialog::create_button_sizer()
{
    auto *row = new QHBoxLayout();
    row->addStretch(1);
    m_cancel_btn = new Button(this, _L("Cancel"));
    row->addWidget(m_cancel_btn);
    m_ok_btn = new Button(this, _L("Create"));
    m_ok_btn->SetValue(true);
    row->addWidget(m_ok_btn);
    connect(m_ok_btn,     &Button::clicked, this, &QDialog::accept);
    connect(m_cancel_btn, &Button::clicked, this, &QDialog::reject);
    return row;
}

// ─── PresetTree ──────────────────────────────────────────────────────────────

PresetTree::PresetTree(EditFilamentPresetDialog *dialog)
    : m_parent_dialog(dialog)
{}

QWidget *PresetTree::get_preset_tree(
    std::pair<std::string, std::vector<std::shared_ptr<Preset>>> printer_and_presets)
{
    m_printer_and_presets = std::move(printer_and_presets);
    auto *w = new QWidget();
    auto *layout = new QVBoxLayout(w);
    layout->addWidget(get_root_item(w, m_printer_and_presets.first));
    int idx = 0;
    for (auto &preset : m_printer_and_presets.second) {
        bool last = (idx == (int)m_printer_and_presets.second.size() - 1);
        layout->addWidget(get_child_item(w, preset, m_printer_and_presets.first, idx, last));
        ++idx;
    }
    return w;
}

QWidget *PresetTree::get_root_item(QWidget *parent, const std::string &printer_name)
{
    auto *w = new QWidget(parent);
    auto *row = new QHBoxLayout(w);
    auto *lbl = new Label(w, Label::Head_14);
    lbl->setText(QString::fromStdString(printer_name));
    row->addWidget(lbl);
    return w;
}

QWidget *PresetTree::get_child_item(QWidget *parent, std::shared_ptr<Preset> preset,
    std::string printer_name, int preset_index, bool /*is_last*/)
{
    auto *w = new QWidget(parent);
    auto *row = new QHBoxLayout(w);
    auto *lbl = new Label(w, Label::Body_14);
    lbl->setText(preset ? QString::fromStdString(preset->name) : QString());
    row->addWidget(lbl, 1);
    auto *del_btn = new Button(w, _L("Delete"));
    // lambda captured by value; PresetTree is not QObject so connect to del_btn itself
    auto *self = this;
    QObject::connect(del_btn, &Button::clicked, del_btn, [self, printer_name, preset_index]() {
        self->delete_preset(printer_name, preset_index);
    });
    row->addWidget(del_btn);
    auto *edit_btn = new Button(w, _L("Edit"));
    QObject::connect(edit_btn, &Button::clicked, edit_btn, [self, printer_name, preset_index]() {
        self->edit_preset(printer_name, preset_index);
    });
    row->addWidget(edit_btn);
    return w;
}

void PresetTree::delete_preset(std::string /*printer_name*/, int /*idx*/) {}
void PresetTree::edit_preset(std::string /*printer_name*/, int /*idx*/) {}

// ─── EditFilamentPresetDialog ─────────────────────────────────────────────────

EditFilamentPresetDialog::EditFilamentPresetDialog(QWidget *parent,
                                                   FilamentInfomation * /*info*/)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Edit Filament Preset"));
    setMinimumSize(540, 420);

    auto *main_layout = new QVBoxLayout(this);
    m_main_sizer = main_layout;
    main_layout->setContentsMargins(20, 20, 20, 16);
    main_layout->setSpacing(8);

    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Edit Filament Preset"));
    main_layout->addWidget(title);

    main_layout->addLayout(create_filament_basic_info());
    main_layout->addLayout(create_add_filament_btn());

    // Preset tree scroll area
    m_preset_tree_window = new QScrollArea(this);
    m_preset_tree_window->setWidgetResizable(true);
    m_preset_tree_panel = new QWidget(m_preset_tree_window);
    m_preset_tree_sizer = new QVBoxLayout(m_preset_tree_panel);
    m_preset_tree_window->setWidget(m_preset_tree_panel);
    main_layout->addWidget(m_preset_tree_window, 1);

    main_layout->addLayout(create_button_sizer());

    m_preset_tree_creater = new PresetTree(this);
}

EditFilamentPresetDialog::~EditFilamentPresetDialog() { delete m_preset_tree_creater; }
void EditFilamentPresetDialog::on_dpi_changed(const QRect & /*r*/) {}
bool EditFilamentPresetDialog::get_same_filament_id_presets(std::string /*id*/) { return false; }
void EditFilamentPresetDialog::update_preset_tree() {}
QBoxLayout *EditFilamentPresetDialog::create_filament_basic_info()
{
    auto *col = new QVBoxLayout();
    auto *note = new QLabel(_L("Filament preset details"), this);
    m_note_text = note;
    col->addWidget(note);
    return col;
}
QBoxLayout *EditFilamentPresetDialog::create_add_filament_btn()
{
    auto *row = new QHBoxLayout();
    m_add_filament_btn = new Button(this, _L("+ Add Printer"));
    row->addWidget(m_add_filament_btn);
    row->addStretch(1);
    m_del_filament_btn = new Button(this, _L("Delete Selected"));
    row->addWidget(m_del_filament_btn);
    return row;
}
QBoxLayout *EditFilamentPresetDialog::create_preset_tree_sizer()
{
    return m_preset_tree_sizer;
}
QBoxLayout *EditFilamentPresetDialog::create_button_sizer()
{
    auto *row = new QHBoxLayout();
    row->addStretch(1);
    m_ok_btn = new Button(this, _L("Done"));
    m_ok_btn->SetValue(true);
    row->addWidget(m_ok_btn);
    connect(m_ok_btn, &Button::clicked, this, &QDialog::accept);
    return row;
}
void EditFilamentPresetDialog::delete_preset() {}
void EditFilamentPresetDialog::edit_preset() {}

}} // namespace Slic3r::GUI

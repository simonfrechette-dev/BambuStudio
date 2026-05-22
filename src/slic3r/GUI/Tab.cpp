// Qt6 port of Tab.cpp — settings tabs (Print, Filament, Printer)
#include "Tab.hpp"
#include "GUI_App.hpp"
#include "PresetComboBoxes.hpp"
#include "OptionsGroup.hpp"
#include "libslic3r/PrintConfig.hpp"
#include "I18N.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QSplitter>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

namespace Slic3r { namespace GUI {

// ─── Page ────────────────────────────────────────────────────────────────────

ConfigOptionsGroupShp Page::new_optgroup(const QString& title, const QString& icon,
                                          int label_width, bool is_extruder_og)
{
    auto optgroup = std::make_shared<ConfigOptionsGroup>(m_parent, title, icon, m_config, true);
    if (label_width >= 0) optgroup->label_width = label_width;
    m_optgroups.push_back(optgroup);
    return optgroup;
}

const ConfigOptionsGroupShp Page::get_optgroup(const QString& title) const
{
    for (const auto& og : m_optgroups) {
        if (og->title == title) return og;
    }
    return nullptr;
}

// ─── Tab base ────────────────────────────────────────────────────────────────

Tab::Tab(QWidget* parent, const QString& title, Preset::Type type)
    : QWidget(parent)
    , m_qt_parent(parent)
    , m_title(title)
    , m_type(type)
{
    m_preset_bundle = wxGetApp().preset_bundle;
    if (m_preset_bundle) {
        switch (type) {
        case Preset::TYPE_PRINT:     m_presets = &m_preset_bundle->prints;    break;
        case Preset::TYPE_FILAMENT:  m_presets = &m_preset_bundle->filaments; break;
        case Preset::TYPE_PRINTER:   m_presets = &m_preset_bundle->printers;  break;
        default: break;
        }
        if (m_presets) {
            // Copy the selected preset config into m_cache_config; point m_config at it
            m_cache_config = m_presets->get_selected_preset().config;
            m_config = &m_cache_config;
        }
    }
}

void Tab::create_preset_tab()
{
    // Register with GUI_App
    wxGetApp().tabs_list.push_back(this);

    // Create main layout: preset combo at top, content below
    auto main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(2);

    // Preset combo + action buttons row
    auto* combo_row = new QHBoxLayout();
    combo_row->setContentsMargins(0, 0, 0, 0);
    combo_row->setSpacing(4);
    m_presets_choice = new TabPresetComboBox(this, m_type);
    combo_row->addWidget(m_presets_choice, 1);

    // Save preset button
    auto* btn_save = new QPushButton(tr("Save"), this);
    btn_save->setFixedHeight(24);
    btn_save->setToolTip(tr("Save the current settings to the preset"));
    connect(btn_save, &QPushButton::clicked, this, [this]() {
        if (!m_presets || !m_config) return;
        Preset& edited = m_presets->get_edited_preset();
        // If preset is a default/built-in, prompt for a new name
        std::string save_name = edited.name;
        if (edited.is_default) {
            bool ok = false;
            const QString name = QInputDialog::getText(
                this, tr("Save Preset As"),
                tr("Enter a name for the new preset:"),
                QLineEdit::Normal,
                QString::fromStdString(edited.name), &ok);
            if (!ok || name.trimmed().isEmpty()) return;
            save_name = name.trimmed().toStdString();
        }
        edited.config = *m_config;
        edited.is_dirty = false;
        m_presets->save_current_preset(save_name, false, false);
        // Strip any dirty "*" marker from the combo
        if (m_presets_choice) {
            const int idx = m_presets_choice->GetSelection();
            if (idx >= 0 && idx < (int)m_presets_choice->GetCount()) {
                const QString cur = m_presets_choice->GetString(idx);
                if (cur.startsWith(QStringLiteral("*"))) {
                    m_presets_choice->blockSignals(true);
                    m_presets_choice->SetString(idx, cur.mid(1));
                    m_presets_choice->blockSignals(false);
                }
            }
        }
    });
    combo_row->addWidget(btn_save);

    // Revert preset button — discards unsaved changes
    auto* btn_revert = new QPushButton(tr("Revert"), this);
    btn_revert->setFixedHeight(24);
    btn_revert->setToolTip(tr("Revert to the last saved state of this preset"));
    connect(btn_revert, &QPushButton::clicked, this, [this]() {
        if (!m_presets) return;
        // Reload the on-disk preset, discarding any edits
        const std::string name = m_presets->get_selected_preset().name;
        m_presets->select_preset_by_name(name, true /*force*/);
        m_cache_config = m_presets->get_selected_preset().config;
        // Strip dirty marker
        if (m_presets_choice) {
            const int idx = m_presets_choice->GetSelection();
            if (idx >= 0 && idx < (int)m_presets_choice->GetCount()) {
                const QString cur = m_presets_choice->GetString(idx);
                if (cur.startsWith(QStringLiteral("*"))) {
                    m_presets_choice->blockSignals(true);
                    m_presets_choice->SetString(idx, cur.mid(1));
                    m_presets_choice->blockSignals(false);
                }
            }
        }
        load_current_preset();
    });
    combo_row->addWidget(btn_revert);

    // Export preset to a .json file
    auto* btn_export = new QPushButton(tr("Export…"), this);
    btn_export->setFixedHeight(24);
    btn_export->setToolTip(tr("Export current preset settings to a JSON file"));
    connect(btn_export, &QPushButton::clicked, this, [this]() {
        if (!m_config) return;
        const QString default_name = m_presets
            ? QString::fromStdString(m_presets->get_selected_preset().name) + ".json"
            : QStringLiteral("preset.json");
        const QString path = QFileDialog::getSaveFileName(
            this, tr("Export Preset"), default_name,
            tr("JSON files (*.json);;All files (*)"));
        if (path.isEmpty()) return;
        QJsonObject obj;
        for (const auto& key : m_config->keys()) {
            const std::string serialized = m_config->opt_serialize(key);
            obj.insert(QString::fromStdString(key),
                       QJsonValue(QString::fromStdString(serialized)));
        }
        QFile f(path);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text))
            f.write(QJsonDocument(obj).toJson());
    });
    combo_row->addWidget(btn_export);

    // Import preset from a .json file
    auto* btn_import = new QPushButton(tr("Import…"), this);
    btn_import->setFixedHeight(24);
    btn_import->setToolTip(tr("Import preset settings from a JSON file"));
    connect(btn_import, &QPushButton::clicked, this, [this]() {
        if (!m_config) return;
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Import Preset"), QString(),
            tr("JSON files (*.json);;All files (*)"));
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Import Failed"),
                tr("Could not open file:\n%1").arg(path));
            return;
        }
        const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        if (doc.isNull() || !doc.isObject()) {
            QMessageBox::warning(this, tr("Import Failed"),
                tr("File does not contain a valid JSON preset."));
            return;
        }
        const QJsonObject obj = doc.object();
        int applied = 0;
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            const std::string key = it.key().toStdString();
            const std::string val = it.value().toString().toStdString();
            if (!m_config->has(key)) continue;
            try {
                m_config->option(key)->deserialize(val);
                ++applied;
            } catch (...) {}
        }
        if (applied > 0) {
            // Mark preset dirty (same as on_value_change)
            if (m_presets) m_presets->get_edited_preset().is_dirty = true;
            if (m_presets_choice) {
                const int idx = m_presets_choice->GetSelection();
                if (idx >= 0 && idx < (int)m_presets_choice->GetCount()) {
                    const QString cur = m_presets_choice->GetString(idx);
                    if (!cur.startsWith(QStringLiteral("*"))) {
                        m_presets_choice->blockSignals(true);
                        m_presets_choice->SetString(idx, QStringLiteral("*") + cur);
                        m_presets_choice->blockSignals(false);
                    }
                }
            }
            reload_config();
            toggle_options();
        }
        QMessageBox::information(this, tr("Import Complete"),
            tr("Applied %1 settings from preset file.").arg(applied));
    });
    combo_row->addWidget(btn_import);

    main_layout->addLayout(combo_row);

    // Wire preset selection → reload config on combo change
    connect(m_presets_choice, &ComboBox::selectionChanged, this, [this](int combo_idx) {
        if (!m_presets || !m_presets_choice) return;
        QString qname = m_presets_choice->GetString(combo_idx);
        if (qname.startsWith(QStringLiteral("*")))
            qname = qname.mid(1); // strip dirty marker
        if (qname.isEmpty()) return;
        // Prompt user if there are unsaved changes
        if (!may_discard_current_dirty_preset(nullptr, qname.toStdString())) {
            // Revert combo selection to current preset
            m_presets_choice->blockSignals(true);
            m_presets_choice->update();
            m_presets_choice->blockSignals(false);
            return;
        }
        if (m_presets->select_preset_by_name(qname.toStdString(), false)) {
            m_cache_config = m_presets->get_selected_preset().config;
            load_current_preset();
        }
    });

    // Content area: left tree + right pages
    auto splitter = new QSplitter(Qt::Horizontal, this);

    auto page_list = new QListWidget(splitter);
    page_list->setMaximumWidth(160);
    page_list->setMinimumWidth(80);

    auto content_stack = new QStackedWidget(splitter);

    splitter->addWidget(page_list);
    splitter->addWidget(content_stack);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    main_layout->addWidget(splitter, 1);

    // Build pages (pure virtual — subclass fills them)
    build();

    // Add each page to the list and content stack
    for (auto& page : m_pages) {
        page_list->addItem(page->title());
        auto scroll = new QScrollArea(content_stack);
        scroll->setWidgetResizable(true);

        auto container = new QWidget();
        auto vbox = new QVBoxLayout(container);
        vbox->setAlignment(Qt::AlignTop);

        // Activate each optgroup (creates widgets) and wire change callbacks
        for (auto& og : page->m_optgroups) {
            // Activate first (builds widgets, sets initial values) THEN wire
            // the change callback so initial population doesn't mark as dirty.
            og->activate();
            og->m_on_change = [this](const t_config_option_key& key, const boost::any& val) {
                on_value_change(key, val);
            };
            if (og->stb) vbox->addWidget(og->stb);
        }
        vbox->addStretch(1);
        scroll->setWidget(container);
        content_stack->addWidget(scroll);
    }

    // Selection changes page
    QObject::connect(page_list, &QListWidget::currentRowChanged,
                     content_stack, &QStackedWidget::setCurrentIndex);
    if (page_list->count() > 0) page_list->setCurrentRow(0);

    m_completed = true;

    // Populate the preset combo box now that everything is set up.
    if (m_presets_choice) m_presets_choice->update();
}

PageShp Tab::add_options_page(const QString& title, const std::string& icon, bool)
{
    auto page = std::make_shared<Page>(this, title, 0, this);
    page->set_config(m_config);
    m_pages.push_back(page);
    return page;
}

// ─── Stubs for complex operations ─────────────────────────────────────────────

void Tab::clear_pages()
{
    m_pages.clear();
    m_active_page = nullptr;
}

void Tab::update_description_lines() {}
void Tab::activate_selected_page(std::function<void()>) {}
void Tab::OnActivate() {}
void Tab::init_options_list() {}
void Tab::msw_rescale() {}
void Tab::sys_color_changed() {}

void Tab::on_value_change(const std::string& opt_key, const boost::any& /*value*/)
{
    // config update already handled by ConfigOptionsGroup::change_opt_value
    // Mark preset as unsaved and reflect in the combo title
    if (!m_presets || !m_presets_choice) return;
    m_presets->get_edited_preset().is_dirty = true;
    // Prefix the current item text with "*" if not already there
    const int idx = m_presets_choice->GetSelection();
    if (idx >= 0 && idx < (int)m_presets_choice->GetCount()) {
        const QString cur = m_presets_choice->GetString(idx);
        if (!cur.startsWith(QStringLiteral("*"))) {
            m_presets_choice->blockSignals(true);
            m_presets_choice->SetString(idx, QStringLiteral("*") + cur);
            m_presets_choice->blockSignals(false);
        }
    }
    // Let the subclass toggle dependent options
    toggle_options();
}

void Tab::reload_config()
{
    for (auto& page : m_pages)
        for (auto& og : page->m_optgroups)
            og->reload_config();
}

void Tab::load_current_preset()
{
    if (!m_presets || !m_config) return;
    reload_config();
    toggle_options();
}

void Tab::reactive_preset_combo_box()
{
    if (m_presets_choice) m_presets_choice->update();
}

bool Tab::select_preset(std::string preset_name, bool /*delete_current*/,
                        const std::string& /*last_selected*/, bool /*force_select*/, bool /*force_no_transfer*/)
{
    if (!m_presets) return false;
    if (preset_name.empty())
        preset_name = m_presets->get_selected_preset().name;
    if (!m_presets->select_preset_by_name(preset_name, false))
        return false;
    m_cache_config = m_presets->get_selected_preset().config;
    if (m_presets_choice) {
        m_presets_choice->blockSignals(true);
        // Strip any dirty "*" markers from all combo items before reloading
        for (unsigned int i = 0; i < m_presets_choice->GetCount(); ++i) {
            QString s = m_presets_choice->GetString(i);
            if (s.startsWith(QStringLiteral("*")))
                m_presets_choice->SetString(i, s.mid(1));
        }
        m_presets_choice->update();
        m_presets_choice->blockSignals(false);
    }
    load_current_preset();
    return true;
}

bool Tab::may_discard_current_dirty_preset(PresetCollection*, const std::string&, bool, ::ForceOption)
{
    if (!m_presets) return true;
    const Preset& edited = m_presets->get_edited_preset();
    if (!edited.is_dirty) return true;

    const int result = QMessageBox::question(
        this,
        tr("Unsaved Changes"),
        tr("The preset \"%1\" has unsaved changes.\nDiscard changes and continue?")
            .arg(QString::fromStdString(edited.name)),
        QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Cancel);
    return result == QMessageBox::Discard;
}

void Tab::rebuild_page_tree() {}
void Tab::update_btns_enabling() {}
void Tab::update_preset_choice() { if (m_presets_choice) m_presets_choice->update(); }

Field* Tab::get_field(const t_config_option_key& opt_key, int opt_index) const
{
    for (const auto& page : m_pages) {
        for (const auto& og : page->m_optgroups) {
            Field* f = og->get_fieldc(opt_key, opt_index);
            if (f) return f;
        }
    }
    return nullptr;
}

Field* Tab::get_field(const t_config_option_key& opt_key, Page** selected_page, int opt_index)
{
    for (auto& page : m_pages) {
        for (auto& og : page->m_optgroups) {
            Field* f = og->get_fieldc(opt_key, opt_index);
            if (f) {
                if (selected_page) *selected_page = page.get();
                return f;
            }
        }
    }
    return nullptr;
}

void Tab::toggle_option(const std::string& opt_key, bool toggle, int opt_index)
{
    Field* f = get_field(opt_key, opt_index);
    if (f) f->toggle(toggle);
}

void Tab::toggle_line(const std::string& opt_key, bool toggle, int opt_index)
{
    toggle_option(opt_key, toggle, opt_index);
}

// ─── TabPrint ────────────────────────────────────────────────────────────────

void TabPrint::build()
{
    if (!m_config) return;

    // Quality page
    {
        auto page = add_options_page(_L("Quality"), "quality");
        auto og_lh = page->new_optgroup(_L("Layer height"));
        og_lh->append_single_option_line("layer_height");
        og_lh->append_single_option_line("initial_layer_print_height");
        og_lh->append_single_option_line("adaptive_layer_height");

        auto og_wall = page->new_optgroup(_L("Wall"));
        og_wall->append_single_option_line("wall_loops");
        og_wall->append_single_option_line("top_shell_layers");
        og_wall->append_single_option_line("bottom_shell_layers");
        og_wall->append_single_option_line("top_shell_thickness");
        og_wall->append_single_option_line("bottom_shell_thickness");
        og_wall->append_single_option_line("seam_position");
        og_wall->append_single_option_line("detect_overhang_wall");

        auto og_surf = page->new_optgroup(_L("Surface"));
        og_surf->append_single_option_line("top_surface_pattern");
        og_surf->append_single_option_line("bottom_surface_pattern");
        og_surf->append_single_option_line("ironing_type");

        auto og_flow = page->new_optgroup(_L("Flow"));
        og_flow->append_single_option_line("initial_layer_line_width");
        og_flow->append_single_option_line("outer_wall_line_width");
        og_flow->append_single_option_line("inner_wall_line_width");
        og_flow->append_single_option_line("sparse_infill_line_width");

        auto og_fuzz = page->new_optgroup(_L("Fuzzy skin"));
        og_fuzz->append_single_option_line("fuzzy_skin");
        og_fuzz->append_single_option_line("fuzzy_skin_thickness");
        og_fuzz->append_single_option_line("fuzzy_skin_point_distance");

        auto og_misc = page->new_optgroup(_L("Features"));
        og_misc->append_single_option_line("detect_thin_wall");
        og_misc->append_single_option_line("detect_overhang_wall");
        og_misc->append_single_option_line("make_overhang_printable");
    }

    // Speed page
    {
        auto page = add_options_page(_L("Speed"), "speed");
        auto og = page->new_optgroup(_L("Print speed"));
        og->append_single_option_line("outer_wall_speed");
        og->append_single_option_line("inner_wall_speed");
        og->append_single_option_line("sparse_infill_speed");
        og->append_single_option_line("top_surface_speed");
        og->append_single_option_line("initial_layer_speed");
        og->append_single_option_line("travel_speed");
        og->append_single_option_line("support_speed");
        og->append_single_option_line("bridge_speed");

        auto og_cooling = page->new_optgroup(_L("Cooling"));
        og_cooling->append_single_option_line("slow_down_for_layer_cooling");
        og_cooling->append_single_option_line("slow_down_min_speed");
        og_cooling->append_single_option_line("slow_down_layer_time");

        auto og_jerk = page->new_optgroup(_L("Jerk"));
        og_jerk->append_single_option_line("outer_wall_jerk");
        og_jerk->append_single_option_line("top_surface_jerk");
        og_jerk->append_single_option_line("travel_jerk");

        auto og_accel = page->new_optgroup(_L("Acceleration"));
        og_accel->append_single_option_line("outer_wall_acceleration");
        og_accel->append_single_option_line("inner_wall_acceleration");
        og_accel->append_single_option_line("top_surface_acceleration");
        og_accel->append_single_option_line("bridge_acceleration");
        og_accel->append_single_option_line("initial_layer_acceleration");
    }

    // Support page
    {
        auto page = add_options_page(_L("Support"), "support");
        auto og = page->new_optgroup(_L("Support material"));
        og->append_single_option_line("enable_support");
        og->append_single_option_line("support_type");
        og->append_single_option_line("support_threshold_angle");
        og->append_single_option_line("support_on_build_plate_only");
        og->append_single_option_line("support_base_pattern");
        og->append_single_option_line("bridge_no_support");

        auto og_raft = page->new_optgroup(_L("Raft"));
        og_raft->append_single_option_line("raft_layers");
        og_raft->append_single_option_line("raft_first_layer_density");

        auto og_brim = page->new_optgroup(_L("Brim / Skirt"));
        og_brim->append_single_option_line("brim_type");
        og_brim->append_single_option_line("brim_width");
        og_brim->append_single_option_line("skirt_loops");
        og_brim->append_single_option_line("skirt_distance");
    }

    // Infill page
    {
        auto page = add_options_page(_L("Infill"), "infill");
        auto og = page->new_optgroup(_L("Infill"));
        og->append_single_option_line("sparse_infill_density");
        og->append_single_option_line("sparse_infill_pattern");
        og->append_single_option_line("infill_combination");
        og->append_single_option_line("infill_shift");

        auto og_anchor = page->new_optgroup(_L("Anchoring"));
        og_anchor->append_single_option_line("infill_anchor");
        og_anchor->append_single_option_line("infill_anchor_max");

        auto og_bridge = page->new_optgroup(_L("Bridging"));
        og_bridge->append_single_option_line("thick_bridges");
        og_bridge->append_single_option_line("bridge_flow");
        og_bridge->append_single_option_line("bridge_angle");
    }

    // Others page
    {
        auto page = add_options_page(_L("Others"), "advanced");
        auto og = page->new_optgroup(_L("Draft shield"));
        og->append_single_option_line("draft_shield");

        auto og_pt = page->new_optgroup(_L("Prime tower"));
        og_pt->append_single_option_line("enable_prime_tower");
        og_pt->append_single_option_line("prime_tower_width");
        og_pt->append_single_option_line("flush_into_infill");
        og_pt->append_single_option_line("flush_into_support");

        auto og_misc = page->new_optgroup(_L("Miscellaneous"));
        og_misc->append_single_option_line("notes");
    }

    // Advanced page
    {
        auto page = add_options_page(_L("Advanced"), "advanced");
        auto og = page->new_optgroup(_L("Slicing"));
        og->append_single_option_line("resolution");
        og->append_single_option_line("slice_closing_radius");
        og->append_single_option_line("enable_arc_fitting");

        auto og_comp = page->new_optgroup(_L("Compensation"));
        og_comp->append_single_option_line("xy_hole_compensation");
        og_comp->append_single_option_line("xy_contour_compensation");
        og_comp->append_single_option_line("elefant_foot_compensation");

        auto og_multi = page->new_optgroup(_L("Multi-colour"));
        og_multi->append_single_option_line("wipe_on_loops");
        og_multi->append_single_option_line("prime_volume");

        auto og_seam = page->new_optgroup(_L("Seam"));
        og_seam->append_single_option_line("seam_gap");
        og_seam->append_single_option_line("wipe_speed");
    }
}

void TabPrint::update() { toggle_options(); }
void TabPrint::toggle_options()
{
    if (!m_config) return;
    // Show/hide support-related options based on enable_support
    const bool support_en = m_config->opt_bool("enable_support");
    for (const auto& key : std::vector<std::string>{
            "support_type", "support_threshold_angle",
            "support_on_build_plate_only", "support_base_pattern"}) {
        toggle_option(key, support_en);
    }
    // Show fuzzy skin thickness/distance only when fuzzy_skin != None/Disabled
    const auto fuzzy = m_config->opt_enum<FuzzySkinType>("fuzzy_skin");
    const bool fuzzy_en = (fuzzy != FuzzySkinType::None && fuzzy != FuzzySkinType::Disabled_fuzzy);
    toggle_option("fuzzy_skin_thickness",     fuzzy_en);
    toggle_option("fuzzy_skin_point_distance", fuzzy_en);
    // Show prime tower width only when enable_prime_tower
    const bool prime_tower = m_config->opt_bool("enable_prime_tower");
    toggle_option("prime_tower_width", prime_tower);
    toggle_option("flush_into_infill", prime_tower);
    toggle_option("flush_into_support", prime_tower);
}
void TabPrint::update_description_lines() {}
void TabPrint::reload_config() { Tab::reload_config(); }
void TabPrint::clear_pages() { Tab::clear_pages(); }

// ─── TabFilament ─────────────────────────────────────────────────────────────

void TabFilament::build()
{
    if (!m_config) return;

    // Filament page
    {
        auto page = add_options_page(_L("Filament"), "filament");
        auto og = page->new_optgroup(_L("Filament"));
        og->append_single_option_line("filament_type");
        og->append_single_option_line("filament_colour");
        og->append_single_option_line("filament_diameter");
        og->append_single_option_line("filament_density");
        og->append_single_option_line("filament_flow_ratio");

        auto og_spool = page->new_optgroup(_L("Spool"));
        og_spool->append_single_option_line("filament_cost");
        og_spool->append_single_option_line("filament_spool_weight");

        auto og2 = page->new_optgroup(_L("Advanced"));
        og2->append_single_option_line("filament_max_volumetric_speed");
        og2->append_single_option_line("overhang_fan_threshold");
        og2->append_single_option_line("filament_notes");
    }

    // Temperature page
    {
        auto page = add_options_page(_L("Temperature"), "temperature");
        auto og = page->new_optgroup(_L("Nozzle temperature"));
        og->append_single_option_line("nozzle_temperature");
        og->append_single_option_line("nozzle_temperature_initial_layer");
        auto og2 = page->new_optgroup(_L("Bed temperature"));
        og2->append_single_option_line("bed_temperature");
        og2->append_single_option_line("bed_temperature_initial_layer");
    }

    // Cooling page
    {
        auto page = add_options_page(_L("Cooling"), "cooling");
        auto og = page->new_optgroup(_L("Fan speed"));
        og->append_single_option_line("fan_min_speed");
        og->append_single_option_line("fan_max_speed");
        og->append_single_option_line("overhang_fan_speed");
        og->append_single_option_line("additional_cooling_fan_speed");

        auto og2 = page->new_optgroup(_L("Slow down"));
        og2->append_single_option_line("slow_down_for_layer_cooling");
        og2->append_single_option_line("slow_down_layer_time");
        og2->append_single_option_line("slow_down_min_speed");
        og2->append_single_option_line("fan_cooling_layer_time");
        og2->append_single_option_line("close_fan_the_first_x_layers");
    }

    // Retraction page
    {
        auto page = add_options_page(_L("Retraction"), "retraction");
        auto og = page->new_optgroup(_L("Retraction"));
        og->append_single_option_line("retraction_length");
        og->append_single_option_line("retraction_speed");
        og->append_single_option_line("deretraction_speed");
        og->append_single_option_line("retraction_minimum_travel");
        og->append_single_option_line("wipe");
        og->append_single_option_line("retract_before_wipe");

        auto og_zhop = page->new_optgroup(_L("Z-hop"));
        og_zhop->append_single_option_line("z_hop");
        og_zhop->append_single_option_line("retract_lift_above");
        og_zhop->append_single_option_line("retract_lift_below");
    }
}

void TabFilament::update() { toggle_options(); }
void TabFilament::toggle_options()
{
    if (!m_config) return;
    // Show retract_before_wipe % only when wipe is enabled
    // wipe is coBools (per-extruder) — use index 0
    bool wipe_en = false;
    if (const ConfigOption* opt = m_config->option("wipe")) {
        if (opt->type() == coBools) {
            const auto* bools = static_cast<const ConfigOptionBools*>(opt);
            if (!bools->values.empty()) wipe_en = bools->values[0];
        } else {
            wipe_en = opt->getBool();
        }
    }
    toggle_option("retract_before_wipe", wipe_en);
    // Show slow_down_layer_time and slow_down_min_speed only when slow_down_for_layer_cooling is enabled
    // slow_down_for_layer_cooling is coBools — use index 0
    bool slow_down_en = false;
    if (const ConfigOption* opt2 = m_config->option("slow_down_for_layer_cooling")) {
        if (opt2->type() == coBools) {
            const auto* bools = static_cast<const ConfigOptionBools*>(opt2);
            if (!bools->values.empty()) slow_down_en = bools->values[0];
        } else {
            slow_down_en = opt2->getBool();
        }
    }
    toggle_option("slow_down_layer_time", slow_down_en);
    toggle_option("slow_down_min_speed", slow_down_en);
}
void TabFilament::reload_config() { Tab::reload_config(); }
void TabFilament::update_description_lines() {}
void TabFilament::clear_pages() { Tab::clear_pages(); }
void TabFilament::init_options_list() {}

// ─── TabPrinter ──────────────────────────────────────────────────────────────

void TabPrinter::build()
{
    if (!m_config) return;

    // ── Machine page ──────────────────────────────────────────────────────
    {
        auto page = add_options_page(_L("Machine"), "machine");

        auto og_nozzle = page->new_optgroup(_L("Nozzle"));
        og_nozzle->append_single_option_line("nozzle_diameter");
        og_nozzle->append_single_option_line("nozzle_type");
        og_nozzle->append_single_option_line("nozzle_volume");

        auto og_size = page->new_optgroup(_L("Print volume"));
        og_size->append_single_option_line("printable_height");
        og_size->append_single_option_line("bed_exclude_area");

        auto og_extruder = page->new_optgroup(_L("Extruder"));
        og_extruder->append_single_option_line("extruder_type");
        og_extruder->append_single_option_line("single_extruder_multi_material");
        og_extruder->append_single_option_line("extruder_clearance_radius");
        og_extruder->append_single_option_line("extruder_clearance_height_to_rod");

        auto og_misc = page->new_optgroup(_L("Misc"));
        og_misc->append_single_option_line("use_relative_e_distances");
        og_misc->append_single_option_line("use_firmware_retraction");
        og_misc->append_single_option_line("lift_z_offset");
    }

    // ── Speed / Limits page ───────────────────────────────────────────────
    {
        auto page = add_options_page(_L("Speed"), "speed");

        auto og_max_speed = page->new_optgroup(_L("Maximum speeds (mm/s)"));
        og_max_speed->append_single_option_line("machine_max_speed_x");
        og_max_speed->append_single_option_line("machine_max_speed_y");
        og_max_speed->append_single_option_line("machine_max_speed_z");

        auto og_acc = page->new_optgroup(_L("Maximum accelerations (mm/s²)"));
        og_acc->append_single_option_line("machine_max_acceleration_x");
        og_acc->append_single_option_line("machine_max_acceleration_y");
        og_acc->append_single_option_line("machine_max_acceleration_extruding");
        og_acc->append_single_option_line("machine_max_acceleration_retracting");
        og_acc->append_single_option_line("machine_max_acceleration_travel");

        auto og_jerk = page->new_optgroup(_L("Maximum jerk (mm/s)"));
        og_jerk->append_single_option_line("machine_max_jerk_x");
        og_jerk->append_single_option_line("machine_max_jerk_y");
        og_jerk->append_single_option_line("machine_max_jerk_z");
        og_jerk->append_single_option_line("machine_max_jerk_e");

        auto og_min = page->new_optgroup(_L("Minimum rates (mm/s)"));
        og_min->append_single_option_line("machine_min_extruding_rate");
        og_min->append_single_option_line("machine_min_travel_rate");
    }

    // ── GCode page ────────────────────────────────────────────────────────
    {
        auto page = add_options_page(_L("GCode"), "advanced");

        auto og_flavor = page->new_optgroup(_L("GCode"));
        og_flavor->append_single_option_line("gcode_flavor");
        og_flavor->append_single_option_line("silent_mode");
        og_flavor->append_single_option_line("remaining_times");

        auto og_scripts = page->new_optgroup(_L("Printer scripts"));
        og_scripts->append_single_option_line("machine_start_gcode");
        og_scripts->append_single_option_line("machine_end_gcode");
        og_scripts->append_single_option_line("machine_pause_gcode");
        og_scripts->append_single_option_line("template_custom_gcode");

        auto og_layer = page->new_optgroup(_L("Layer change scripts"));
        og_layer->append_single_option_line("before_layer_change_gcode");
        og_layer->append_single_option_line("layer_change_gcode");
        og_layer->append_single_option_line("time_lapse_gcode");

        auto og_mm = page->new_optgroup(_L("Multimaterial scripts"));
        og_mm->append_single_option_line("change_filament_gcode");
    }

    // ── Multimaterial page ────────────────────────────────────────────────
    {
        auto page = add_options_page(_L("Multimaterial"), "filament");

        auto og_ams = page->new_optgroup(_L("AMS / multi-extruder"));
        og_ams->append_single_option_line("extruders_count");
        og_ams->append_single_option_line("enable_long_retraction_when_cut");

        auto og_wipe = page->new_optgroup(_L("Wipe tower"));
        og_wipe->append_single_option_line("enable_prime_tower");
        og_wipe->append_single_option_line("prime_tower_width");
        og_wipe->append_single_option_line("wipe_tower_no_sparse_layers");
    }
}

void TabPrinter::update() { toggle_options(); }
void TabPrinter::toggle_options()
{
    if (!m_config) return;

    // In silent mode (stealth), min extruding/travel rates are relevant
    const bool silent = m_config->opt_bool("silent_mode");
    toggle_option("machine_min_extruding_rate", silent);
    toggle_option("machine_min_travel_rate",    silent);
}
void TabPrinter::update_pages() {}
void TabPrinter::build_fff() {}
void TabPrinter::build_sla() {}
void TabPrinter::reload_config() { Tab::reload_config(); }
void TabPrinter::activate_selected_page(std::function<void()>) {}
void TabPrinter::clear_pages() { Tab::clear_pages(); }
void TabPrinter::update_fff() {}
void TabPrinter::update_sla() {}
void TabPrinter::extruders_count_changed(size_t) {}
PageShp TabPrinter::build_kinematics_page() { return add_options_page(_L("Kinematics"), ""); }
void TabPrinter::build_unregular_pages(bool) {}
void TabPrinter::on_preset_loaded() {}
void TabPrinter::init_options_list() {}
void TabPrinter::msw_rescale() { Tab::msw_rescale(); }
void TabPrinter::set_extruder_volume_type(int, NozzleVolumeType) {}
void TabPrinter::cache_extruder_cnt() {}
bool TabPrinter::apply_extruder_cnt_from_cache() { return false; }

// ─── TabPrintModel ────────────────────────────────────────────────────────────

TabPrintModel::TabPrintModel(QWidget* parent, std::vector<std::string> const& keys)
    : TabPrint(parent, Preset::TYPE_PRINT)
    , m_keys(keys)
{}

void TabPrintModel::build() {}
void TabPrintModel::set_model_config(std::map<ObjectBase*, ModelConfig*> const&) {}
void TabPrintModel::update_model_config() {}
void TabPrintModel::reset_model_config() {}
bool TabPrintModel::has_key(std::string const& k)
{
    return std::find(m_keys.begin(), m_keys.end(), k) != m_keys.end();
}
void TabPrintModel::activate_selected_page(std::function<void()>) {}
void TabPrintModel::on_value_change(const std::string& opt_key, const boost::any& value)
{
    Tab::on_value_change(opt_key, value);
}
void TabPrintModel::reload_config() { Tab::reload_config(); }
void TabPrintModel::update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) {}

// ─── TabPrintPlate ────────────────────────────────────────────────────────────

TabPrintPlate::TabPrintPlate(QWidget* parent)
    : TabPrintModel(parent, {})
{}

void TabPrintPlate::build() {}
void TabPrintPlate::reset_model_config() {}
void TabPrintPlate::update_bed_type_list() {}
void TabPrintPlate::update_mixed_filament_seq_state() {}
void TabPrintPlate::on_value_change(const std::string& opt_key, const boost::any& value)
{
    TabPrintModel::on_value_change(opt_key, value);
}
void TabPrintPlate::notify_changed(ObjectBase*) {}
void TabPrintPlate::update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) {}

// ─── TabPrintObject ───────────────────────────────────────────────────────────

TabPrintObject::TabPrintObject(QWidget* parent)
    : TabPrintModel(parent, {})
{}

void TabPrintObject::notify_changed(ObjectBase*) {}

// ─── TabPrintPart ─────────────────────────────────────────────────────────────

TabPrintPart::TabPrintPart(QWidget* parent)
    : TabPrintModel(parent, {})
{}

void TabPrintPart::notify_changed(ObjectBase*) {}

// ─── TabPrintLayer ────────────────────────────────────────────────────────────

TabPrintLayer::TabPrintLayer(QWidget* parent)
    : TabPrintModel(parent, {})
{}

void TabPrintLayer::notify_changed(ObjectBase*) {}
void TabPrintLayer::update_custom_dirty(std::vector<std::string>&, std::vector<std::string>&) {}

// ─── TabSLAMaterial ───────────────────────────────────────────────────────────

void TabSLAMaterial::build() {}
void TabSLAMaterial::reload_config() { Tab::reload_config(); }
void TabSLAMaterial::toggle_options() {}
void TabSLAMaterial::update() {}

// ─── TabSLAPrint ──────────────────────────────────────────────────────────────

void TabSLAPrint::build() {}
void TabSLAPrint::reload_config() { Tab::reload_config(); }
void TabSLAPrint::update_description_lines() {}
void TabSLAPrint::toggle_options() {}
void TabSLAPrint::update() {}
void TabSLAPrint::clear_pages() { Tab::clear_pages(); }

}} // namespace Slic3r::GUI

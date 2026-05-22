// Qt6 port of OptionsGroup.cpp — container for settings rows in the Tab system
#include "OptionsGroup.hpp"
#include "OG_CustomCtrl.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"

#include <boost/log/trivial.hpp>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QSizePolicy>

namespace Slic3r { namespace GUI {

// ─── OptionsGroup ─────────────────────────────────────────────────────────────

OptionsGroup::OptionsGroup(QWidget* _parent, const QString& title, const QString& _icon,
                           bool is_tab_opt, column_t extra_clmn)
    : m_parent(_parent)
    , title(title)
    , icon(_icon)
    , extra_column(extra_clmn)
    , m_use_custom_ctrl(is_tab_opt)
{}

QWidget* OptionsGroup::ctrl_parent() const
{
    if (custom_ctrl && m_use_custom_ctrl_as_parent)
        return custom_ctrl;
    return m_parent;
}

void OptionsGroup::append_line(const Line& line)
{
    m_lines.push_back(line);
}

void OptionsGroup::append_separator()
{
    m_lines.push_back(Line{});   // separator line
}

Line* OptionsGroup::get_line(const std::string& opt_key)
{
    for (Line& l : m_lines) {
        for (const Option& o : l.get_options()) {
            if (o.opt_id == opt_key) return &l;
        }
    }
    return nullptr;
}

bool OptionsGroup::activate(std::function<void()> throw_if_canceled, int horiz_alignment)
{
    if (sizer) return false; // already activated

    // Create outer widget
    auto outer = new QWidget(m_parent);
    auto outer_vbox = new QVBoxLayout(outer);
    outer_vbox->setContentsMargins(0, 0, 0, 0);
    outer_vbox->setSpacing(0);

    // Title / static box
    stb = outer;

    if (!title.isEmpty()) {
        auto title_label = new QLabel("<b>" + title + "</b>", outer);
        title_label->setTextFormat(Qt::RichText);
        outer_vbox->addWidget(title_label);
        auto separator = new QFrame(outer);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        outer_vbox->addWidget(separator);
    }

    m_grid_sizer = new QGridLayout();
    m_grid_sizer->setColumnStretch(1, 1);
    m_grid_sizer->setHorizontalSpacing(8);
    m_grid_sizer->setVerticalSpacing(4);
    outer_vbox->addLayout(m_grid_sizer);

    sizer = outer_vbox;

    for (auto& line : m_lines) {
        try { throw_if_canceled(); } catch (...) { return false; }
        activate_line(line);
    }

    ctrl_horiz_alignment = horiz_alignment;
    return true;
}

void OptionsGroup::activate_line(Line& line)
{
    if (!m_grid_sizer) return;

    if (line.is_separator()) {
        auto sep = new QFrame(ctrl_parent());
        sep->setFrameShape(QFrame::HLine);
        int row = m_grid_sizer->rowCount();
        m_grid_sizer->addWidget(sep, row, 0, 1, 3);
        return;
    }

    if (!line.toggle_visible) return;

    int row = m_grid_sizer->rowCount();

    // Near-label widget
    if (line.near_label_widget) {
        QWidget* w_win = line.near_label_widget(ctrl_parent());
        if (w_win) m_grid_sizer->addWidget(w_win, row, 0);
    }

    // Label
    if (!line.label.isEmpty() && label_width > 0) {
        auto lbl = new QLabel(line.label, ctrl_parent());
        lbl->setWordWrap(true);
        if (label_width > 0) lbl->setFixedWidth(label_width);
        if (!line.label_tooltip.isEmpty()) lbl->setToolTip(line.label_tooltip);
        m_grid_sizer->addWidget(lbl, row, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    // Custom widget for the line
    if (line.widget) {
        QLayout* w_sizer = line.widget(ctrl_parent());
        if (w_sizer) m_grid_sizer->addLayout(w_sizer, row, 1);
        if (line.extra_widget_sizer)
            m_grid_sizer->addLayout(line.extra_widget_sizer, row, 2);
        return;
    }

    // Build fields
    const auto& options = line.get_options();
    if (options.empty()) return;

    if (options.size() == 1) {
        const t_field& field = build_field(options[0]);
        if (is_window_field(field))
            m_grid_sizer->addWidget(field->getWindow(), row, 1);
        else if (is_sizer_field(field))
            m_grid_sizer->addLayout(field->getSizer(), row, 1);
    } else {
        // Multiple options on one line → wrap in HBoxLayout
        auto hbox = new QHBoxLayout();
        hbox->setContentsMargins(0, 0, 0, 0);
        for (const Option& opt : options) {
            const t_field& field = build_field(opt);
            if (is_window_field(field)) hbox->addWidget(field->getWindow());
            else if (is_sizer_field(field)) hbox->addLayout(field->getSizer());
        }
        m_grid_sizer->addLayout(hbox, row, 1);
    }

    // Extra widgets
    for (const widget_t& wgt : line.get_extra_widgets()) {
        QLayout* w = wgt(ctrl_parent());
        if (w) {
            int col = m_grid_sizer->columnCount();
            m_grid_sizer->addLayout(w, row, col);
        }
    }
}

Line OptionsGroup::create_single_option_line(const Option& option,
                                              const std::string& /*path*/,
                                              bool /*subline*/) const
{
    QString label = option.opt.full_label.empty()
        ? QString::fromStdString(option.opt.label)
        : QString::fromStdString(option.opt.full_label);
    QString tooltip = QString::fromStdString(option.opt.tooltip);
    Line line(label, tooltip);
    line.append_option(option);
    return line;
}

void OptionsGroup::clear(bool destroy_custom_ctrl)
{
    m_fields.clear();
    m_lines.clear();
    if (destroy_custom_ctrl && custom_ctrl) {
        custom_ctrl->deleteLater();
        custom_ctrl = nullptr;
    }
    sizer = nullptr;
    m_grid_sizer = nullptr;
    stb = nullptr;
}

void OptionsGroup::show_field(const t_config_option_key& opt_key, bool show)
{
    auto it = m_fields.find(opt_key);
    if (it == m_fields.end()) return;
    Field* f = it->second.get();
    QWidget* w = f->getWindow();
    if (w) w->setVisible(show);
    else if (QLayout* l = f->getSizer()) {
        for (int i = 0; i < l->count(); ++i) {
            QLayoutItem* item = l->itemAt(i);
            if (item->widget()) item->widget()->setVisible(show);
        }
    }
}

void OptionsGroup::clear_fields_except_of(const std::vector<std::string> left_fields)
{
    auto it = m_fields.begin();
    while (it != m_fields.end()) {
        if (std::find(left_fields.begin(), left_fields.end(), it->first) == left_fields.end())
            it = m_fields.erase(it);
        else
            ++it;
    }
}

void OptionsGroup::set_name(const QString& new_name)
{
    // update title label if needed
}

void OptionsGroup::set_max_win_width(int /*max_win_width*/)
{
    // no-op: max width tracking not yet implemented
}

bool OptionsGroup::is_legend_line()
{
    return !m_lines.empty() && m_lines.front().full_width > 0;
}

void OptionsGroup::remove_option_if(std::function<bool(std::string const&)> const& comp)
{
    for (auto it = m_options.begin(); it != m_options.end(); ) {
        if (comp(it->first)) it = m_options.erase(it);
        else ++it;
    }
    m_lines.erase(
        std::remove_if(m_lines.begin(), m_lines.end(), [&](Line& l) {
            for (const Option& o : l.get_options())
                if (comp(o.opt_id)) return true;
            return false;
        }), m_lines.end());
}

void OptionsGroup::on_change_OG(const t_config_option_key& opt_id, const boost::any& value)
{
    if (m_on_change) m_on_change(opt_id, value);
}

const t_field& OptionsGroup::build_field(const t_config_option_key& id, const ConfigOptionDef& opt)
{
    // Build the right kind of field depending on option type
    switch (opt.type) {
    case coBool:
    case coBools:
        m_fields.emplace(id, Field::Create<CheckBox>(ctrl_parent(), opt, id));
        break;
    case coInt:
    case coInts:
        m_fields.emplace(id, Field::Create<SpinCtrl>(ctrl_parent(), opt, id));
        break;
    case coFloat:
    case coFloats:
    case coPercent:
    case coPercents:
    case coFloatOrPercent:
        if (opt.gui_type == ConfigOptionDef::GUIType::slider)
            m_fields.emplace(id, Field::Create<SliderCtrl>(ctrl_parent(), opt, id));
        else
            m_fields.emplace(id, Field::Create<TextCtrl>(ctrl_parent(), opt, id));
        break;
    case coString:
        m_fields.emplace(id, Field::Create<TextCtrl>(ctrl_parent(), opt, id));
        break;
    case coStrings:
        if (opt.gui_type == ConfigOptionDef::GUIType::color)
            m_fields.emplace(id, Field::Create<ColourPicker>(ctrl_parent(), opt, id));
        else
            m_fields.emplace(id, Field::Create<TextCtrl>(ctrl_parent(), opt, id));
        break;
    case coEnum:
        m_fields.emplace(id, Field::Create<Choice>(ctrl_parent(), opt, id));
        break;
    case coPoint:
    case coPoints:
        m_fields.emplace(id, Field::Create<PointCtrl>(ctrl_parent(), opt, id));
        break;
    default:
        m_fields.emplace(id, Field::Create<TextCtrl>(ctrl_parent(), opt, id));
        break;
    }
    // wire up change callback
    Field* f = m_fields.at(id).get();
    f->m_on_change = [this](const t_config_option_key& id2, const boost::any& val) {
        on_change_OG(id2, val);
    };
    f->m_on_kill_focus = [this](const std::string& id2) {
        on_kill_focus(id2);
    };
    f->m_back_to_initial_value = [this](const std::string& id2) {
        back_to_initial_value(id2);
    };
    f->m_back_to_sys_value = [this](const std::string& id2) {
        back_to_sys_value(id2);
    };
    return m_fields.at(id);
}

const t_field& OptionsGroup::build_field(const t_config_option_key& id)
{
    const auto& it = m_options.find(id);
    if (it != m_options.end())
        return build_field(id, it->second.opt);
    // If not in m_options, search m_lines
    for (const Line& l : m_lines) {
        for (const Option& o : l.get_options()) {
            if (o.opt_id == id)
                return build_field(id, o.opt);
        }
    }
    // fallback: empty text ctrl
    ConfigOptionDef empty;
    return build_field(id, empty);
}

const t_field& OptionsGroup::build_field(const Option& opt)
{
    return build_field(opt.opt_id, opt.opt);
}

QString OptionsGroup::get_url(const std::string&)  { return {}; }
bool    OptionsGroup::launch_browser(const std::string&) { return false; }

// ─── ConfigOptionsGroup ───────────────────────────────────────────────────────

Option ConfigOptionsGroup::get_option(const std::string& opt_key, int opt_index)
{
    if (!m_config) {
        BOOST_LOG_TRIVIAL(error) << "ConfigOptionsGroup: no config set for " << opt_key;
        ConfigOptionDef def;
        def.label = opt_key;
        def.type = coString;
        return Option(def, opt_key);
    }
    const ConfigDef* config_def = m_config->def();
    if (!config_def) {
        ConfigOptionDef def;
        def.label = opt_key;
        def.type = coString;
        return Option(def, opt_key);
    }
    const ConfigOptionDef* def = config_def->get(opt_key);
    if (!def) {
        ConfigOptionDef fallback;
        fallback.label = opt_key;
        fallback.type = coString;
        return Option(fallback, opt_key);
    }
    std::string id = opt_key;
    if (opt_index >= 0) {
        id += "#" + std::to_string(opt_index);
        m_opt_map[id] = {opt_key, opt_index};
    } else {
        m_opt_map[id] = {opt_key, 0};
    }
    return Option(*def, id);
}

void ConfigOptionsGroup::on_change_OG(const t_config_option_key& opt_id, const boost::any& value)
{
    if (!m_config) return;
    auto it = m_opt_map.find(opt_id);
    if (it != m_opt_map.end())
        change_opt_value(it->second.first, value, it->second.second);
    else
        change_opt_value(opt_id, value);
    OptionsGroup::on_change_OG(opt_id, value);
}

void ConfigOptionsGroup::change_opt_value(const t_config_option_key& opt_key,
                                           const boost::any& value, int opt_index)
{
    if (!m_config) return;
    DynamicPrintConfig* cfg = const_cast<DynamicPrintConfig*>(m_config);
    const ConfigOption* existing = cfg->option(opt_key);
    if (!existing) return;

    try {
        switch (existing->type()) {
        case coBool:
            if (auto* o = cfg->opt<ConfigOptionBool>(opt_key))
                o->value = boost::any_cast<bool>(value);
            break;
        case coBools:
            if (auto* o = cfg->opt<ConfigOptionBools>(opt_key)) {
                bool v = boost::any_cast<bool>(value);
                int idx = opt_index >= 0 ? opt_index : 0;
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            }
            break;
        case coInt:
            if (auto* o = cfg->opt<ConfigOptionInt>(opt_key))
                o->value = boost::any_cast<int>(value);
            break;
        case coInts:
            if (auto* o = cfg->opt<ConfigOptionInts>(opt_key)) {
                int v = boost::any_cast<int>(value);
                int idx = opt_index >= 0 ? opt_index : 0;
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            }
            break;
        case coFloat:
            if (auto* o = cfg->opt<ConfigOptionFloat>(opt_key))
                o->value = boost::any_cast<double>(value);
            break;
        case coPercent:
            if (auto* o = cfg->opt<ConfigOptionPercent>(opt_key))
                o->value = boost::any_cast<double>(value);
            break;
        case coPercents:
            if (auto* o = cfg->opt<ConfigOptionPercents>(opt_key)) {
                double v = boost::any_cast<double>(value);
                int idx = opt_index >= 0 ? opt_index : 0;
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            }
            break;
        case coFloatOrPercent:
            if (auto* o = cfg->opt<ConfigOptionFloatOrPercent>(opt_key))
                o->value = boost::any_cast<double>(value);
            break;
        case coFloats: {
            double v = boost::any_cast<double>(value);
            int idx = opt_index >= 0 ? opt_index : 0;
            // ConfigOptionFloatsNullable and ConfigOptionFloats both derive from
            // ConfigOptionFloatsTempl but are different template instantiations.
            // Try non-nullable first, then nullable.
            if (auto* o = cfg->opt<ConfigOptionFloats>(opt_key)) {
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            } else if (auto* o = cfg->opt<ConfigOptionFloatsNullable>(opt_key)) {
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            }
            break;
        }
        case coString:
            if (auto* o = cfg->opt<ConfigOptionString>(opt_key))
                o->value = boost::any_cast<std::string>(value);
            break;
        case coStrings:
            if (auto* o = cfg->opt<ConfigOptionStrings>(opt_key)) {
                std::string v = boost::any_cast<std::string>(value);
                int idx = opt_index >= 0 ? opt_index : 0;
                if (idx < (int)o->values.size())
                    o->values[idx] = v;
                else if (idx == 0 && o->values.empty())
                    o->values.push_back(v);
            }
            break;
        case coEnum:
            // Choice::get_value() returns int (enum index) for non-editable combos
            if (value.type() == typeid(int)) {
                cfg->option(opt_key)->setInt(boost::any_cast<int>(value));
            } else {
                // Editable combos return string — fall through to deserialize
                ConfigSubstitutionContext ctx(ForwardCompatibilitySubstitutionRule::Disable);
                cfg->set_deserialize(opt_key, boost::any_cast<std::string>(value), ctx);
            }
            break;
        default: {
            // Fallback: try string deserialization for enum types
            ConfigSubstitutionContext ctx(ForwardCompatibilitySubstitutionRule::Disable);
            cfg->set_deserialize(opt_key, boost::any_cast<std::string>(value), ctx);
            break;
        }
        }
        if (m_modelconfig) m_modelconfig->touch();
    } catch (...) {
        // best-effort — ignore type cast failures
    }
}

void ConfigOptionsGroup::back_to_initial_value(const std::string& opt_key)
{
    if (!m_get_initial_config) return;
    DynamicPrintConfig initial = m_get_initial_config();
    back_to_config_value(initial, opt_key);
}

void ConfigOptionsGroup::back_to_sys_value(const std::string& opt_key)
{
    if (!m_get_sys_config) return;
    DynamicPrintConfig sys = m_get_sys_config();
    back_to_config_value(sys, opt_key);
}

void ConfigOptionsGroup::back_to_config_value(const DynamicPrintConfig& config,
                                               const std::string& opt_key)
{
    boost::any val = get_config_value(config, opt_key);
    set_value(opt_key, val, false);
}

void ConfigOptionsGroup::on_kill_focus(const std::string& opt_key)
{
    if (m_fill_empty_value) m_fill_empty_value(opt_key);
    OptionsGroup::on_kill_focus(opt_key);
}

void ConfigOptionsGroup::reload_config()
{
    if (!m_config) return;
    for (auto& kv : m_fields) {
        // Resolve opt_key and opt_index from the field id
        std::string opt_key = kv.first;
        int opt_index = -1;
        auto it = m_opt_map.find(kv.first);
        if (it != m_opt_map.end()) {
            opt_key   = it->second.first;
            opt_index = it->second.second;
        } else {
            // Try parsing "key#N" format
            auto hash_pos = kv.first.rfind('#');
            if (hash_pos != std::string::npos) {
                opt_key   = kv.first.substr(0, hash_pos);
                opt_index = std::stoi(kv.first.substr(hash_pos + 1));
            }
        }
        boost::any val = get_config_value(*m_config, opt_key, opt_index);
        kv.second->set_value(val, false);
    }
}

void ConfigOptionsGroup::Hide()
{
    if (stb) stb->setVisible(false);
}

void ConfigOptionsGroup::Show(const bool show)
{
    if (stb) stb->setVisible(show);
}

bool ConfigOptionsGroup::is_visible(ConfigOptionMode mode)
{
    if (m_options_mode.empty()) return true;
    return std::any_of(m_options_mode.begin(), m_options_mode.end(),
                       [mode](ConfigOptionMode m) { return m <= mode; });
}

bool ConfigOptionsGroup::update_visibility(ConfigOptionMode mode)
{
    bool any_visible = false;
    // iterate lines and hide/show based on mode
    for (Line& line : m_lines) {
        bool visible = true;
        for (const Option& opt : line.get_options()) {
            if (opt.opt.mode > mode) { visible = false; break; }
        }
        line.toggle_visible = visible;
        if (visible) any_visible = true;
        // hide widgets
        for (const Option& opt : line.get_options()) {
            auto it = m_fields.find(opt.opt_id);
            if (it != m_fields.end()) {
                QWidget* w = it->second->getWindow();
                if (w) w->setVisible(visible);
            }
        }
    }
    return any_visible;
}

void ConfigOptionsGroup::msw_rescale()
{
    for (auto& kv : m_fields) kv.second->msw_rescale();
}

void ConfigOptionsGroup::sys_color_changed()
{
    for (auto& kv : m_fields) kv.second->sys_color_changed();
}

void ConfigOptionsGroup::refresh()
{
    reload_config();
}

boost::any ConfigOptionsGroup::config_value(const std::string& opt_key, int opt_index, bool deserialize)
{
    if (!m_config) return boost::any{};
    return get_config_value(*m_config, opt_key, opt_index);
}

boost::any ConfigOptionsGroup::get_config_value(const DynamicPrintConfig& config,
                                                  const std::string& opt_key,
                                                  int opt_index)
{
    const ConfigOption* opt = config.option(opt_key);
    if (!opt) return boost::any{};
    const ConfigDef* def = config.def();
    const ConfigOptionDef* def_opt = def ? def->get(opt_key) : nullptr;

    switch (opt->type()) {
    case coBool:   return boost::any(static_cast<const ConfigOptionBool*>(opt)->value);
    case coBools:  {
        int idx = std::max(0, opt_index);
        const auto* v = static_cast<const ConfigOptionBools*>(opt);
        return boost::any(idx < (int)v->values.size() ? v->values[idx] : false);
    }
    case coInt:    return boost::any(static_cast<const ConfigOptionInt*>(opt)->value);
    case coInts:   {
        int idx = std::max(0, opt_index);
        const auto* v = static_cast<const ConfigOptionInts*>(opt);
        return boost::any(idx < (int)v->values.size() ? v->values[idx] : 0);
    }
    case coFloat:  return boost::any(static_cast<const ConfigOptionFloat*>(opt)->value);
    case coFloats: {
        int idx = std::max(0, opt_index);
        // ConfigOptionFloats (non-nullable) and ConfigOptionFloatsNullable are different types.
        if (const auto* v = dynamic_cast<const ConfigOptionFloats*>(opt))
            return boost::any(idx < (int)v->values.size() ? v->values[idx] : 0.0);
        if (const auto* v = dynamic_cast<const ConfigOptionFloatsNullable*>(opt))
            return boost::any(idx < (int)v->values.size() ? (double)v->values[idx] : 0.0);
        return boost::any(0.0);
    }
    case coPercent: return boost::any(static_cast<const ConfigOptionPercent*>(opt)->value);
    case coPercents: {
        int idx = std::max(0, opt_index);
        const auto* v = static_cast<const ConfigOptionPercents*>(opt);
        return boost::any(idx < (int)v->values.size() ? v->values[idx] : 0.0);
    }
    case coFloatOrPercent: return boost::any(static_cast<const ConfigOptionFloatOrPercent*>(opt)->value);
    case coString:  return boost::any(static_cast<const ConfigOptionString*>(opt)->value);
    case coStrings: {
        int idx = std::max(0, opt_index);
        const auto* v = static_cast<const ConfigOptionStrings*>(opt);
        return boost::any(idx < (int)v->values.size() ? v->values[idx] : std::string{});
    }
    case coEnum:   return boost::any(opt->getInt());
    case coPoint:  return boost::any(static_cast<const ConfigOptionPoint*>(opt)->value);
    default:       return boost::any(opt->serialize());
    }
}

boost::any ConfigOptionsGroup::get_config_value2(const DynamicPrintConfig& config,
                                                   const std::string& opt_key,
                                                   int opt_index)
{
    return get_config_value(config, opt_key, opt_index);
}

Field* ConfigOptionsGroup::get_fieldc(const t_config_option_key& opt_key, int opt_index)
{
    if (opt_index < 0) return get_field(opt_key);
    std::string id = opt_key + "#" + std::to_string(opt_index);
    return get_field(id);
}

std::pair<OG_CustomCtrl*, bool*> ConfigOptionsGroup::get_custom_ctrl_with_blinking_ptr(
    const t_config_option_key& opt_key, int opt_index)
{
    return {custom_ctrl, nullptr};
}

// ─── ExtruderOptionsGroup ─────────────────────────────────────────────────────

void ExtruderOptionsGroup::on_change_OG(const t_config_option_key& opt_id,
                                         const boost::any& value)
{
    ConfigOptionsGroup::on_change_OG(opt_id, value);
}

// ─── ogStaticText ─────────────────────────────────────────────────────────────

ogStaticText::ogStaticText(QWidget* parent, const QString& text)
    : QLabel(text, parent)
{}

void ogStaticText::SetText(const QString& value, bool wrap)
{
    setText(value);
    setWordWrap(wrap);
}

void ogStaticText::SetPathEnd(const std::string&) {}
void ogStaticText::FocusText(bool focus)
{
    QFont f = font();
    f.setBold(focus);
    setFont(f);
}

}} // namespace Slic3r::GUI

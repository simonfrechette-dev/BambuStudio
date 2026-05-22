// Qt6 port of Field.cpp — form-field widgets for the settings tabs
#include "Field.hpp"
#include "GUI_App.hpp"

#include "libslic3r/PrintConfig.hpp"

#include <cmath>
#include <regex>
#include <boost/algorithm/string/predicate.hpp>

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>

#include "libslic3r/Config.hpp"
#include "I18N.hpp"
#include "Widgets/SpinInput.hpp"

namespace Slic3r { namespace GUI {

// ─── helpers ──────────────────────────────────────────────────────────────────

QString double_to_string(double const value, const int max_precision)
{
    if (std::isnan(value)) return "nan";
    if (std::isinf(value)) return value > 0 ? "inf" : "-inf";
    return QString::number(value, 'f', max_precision).remove(QRegularExpression("[0]+$")).remove(QRegularExpression("[.]+$"));
}

QString get_thumbnail_string(const Vec2d& value)
{
    return QString("%1x%2").arg((int)value.x()).arg((int)value.y());
}

QString get_thumbnails_string(const std::vector<Vec2d>& values)
{
    QStringList parts;
    for (const Vec2d& v : values)
        parts << get_thumbnail_string(v);
    return parts.join(", ");
}

// ─── Field base ───────────────────────────────────────────────────────────────

Field::~Field() = default;

void Field::PostInitialize()
{
    BUILD();
}

void Field::on_kill_focus()
{
    if (m_on_kill_focus) m_on_kill_focus(m_opt_id);
}

void Field::on_change_field()
{
    if (!m_disable_change_event && m_on_change)
        m_on_change(m_opt_id, get_value());
}

void Field::toggle(bool en)
{
    QWidget* w = getWindow();
    if (w) w->setVisible(en);
}

QString Field::get_tooltip_text(const QString& default_string)
{
    return default_string;
}

bool Field::is_matched(const std::string& string, const std::string& pattern)
{
    std::regex re(pattern);
    return std::regex_match(string, re);
}

void Field::get_value_by_opt_type(QString& str, const bool check_value)
{
    switch (m_opt.type) {
    case coInt:
        str = QString::number(boost::any_cast<int>(m_value));
        break;
    case coPercent:
    case coFloat: {
        double val = boost::any_cast<double>(m_value);
        str = double_to_string(val);
        if (m_opt.type == coPercent) str += "%";
        break;
    }
    case coString:
    case coStrings:
        str = QString::fromStdString(boost::any_cast<std::string>(m_value));
        break;
    default:
        str = "";
        break;
    }
}

void Field::msw_rescale() {}
void Field::sys_color_changed() {}

int Field::def_width()         { return 80; }
int Field::def_width_wider()   { return 120; }
int Field::def_width_thinner() { return 50; }

// ─── TextCtrl ─────────────────────────────────────────────────────────────────

void TextCtrl::BUILD()
{
    if (m_opt.multiline) {
        // For multiline options (e.g. GCode scripts) use QPlainTextEdit
        auto* te = new QPlainTextEdit(m_parent);
        int line_h = QFontMetrics(te->font()).height();
        int h = m_opt.height > 0 ? m_opt.height * (line_h + 2) : 6 * (line_h + 2);
        te->setMinimumHeight(h);
        te->setMaximumHeight(h);
        if (!m_opt.tooltip.empty())
            te->setToolTip(QString::fromStdString(m_opt.tooltip));
        if (m_opt.readonly) te->setReadOnly(true);
        if (m_opt.default_value) {
            std::string init_str = m_opt.default_value->serialize();
            te->setPlainText(QString::fromStdString(init_str));
            m_value = init_str;
        }
        QObject::connect(te, &QPlainTextEdit::textChanged, [this, te]() {
            m_value = te->toPlainText().toStdString();
            on_change_field();
        });
        window = te;
    } else {
        auto w = new QLineEdit(m_parent);
        int width = def_width();
        if (m_opt.width >= 0) width = m_opt.width;
        w->setFixedWidth(width);
        if (!m_opt.tooltip.empty())
            w->setToolTip(QString::fromStdString(m_opt.tooltip));
        if (m_opt.readonly) w->setReadOnly(true);
        // set initial value from default
        if (m_opt.default_value) {
            std::string init_str = m_opt.default_value->serialize();
            // For multi-value types, take only the first token
            auto comma = init_str.find(',');
            if (comma != std::string::npos) init_str = init_str.substr(0, comma);
            w->setText(QString::fromStdString(init_str));
            m_value = init_str;
        }
        // emit change when editing finished
        QObject::connect(w, &QLineEdit::editingFinished, [this]() {
            m_value = std::string(text_ctrl()->text().toStdString());
            on_change_field();
        });
        QObject::connect(w, &QLineEdit::textChanged, [this](const QString&) {
            // mark as modified
        });
        window = w;
    }
}

QLineEdit* TextCtrl::text_ctrl()
{
    return qobject_cast<QLineEdit*>(window);
}

static QString textctrl_get_text(QWidget* w)
{
    if (auto* le = qobject_cast<QLineEdit*>(w)) return le->text();
    if (auto* te = qobject_cast<QPlainTextEdit*>(w)) return te->toPlainText();
    return {};
}

static void textctrl_set_text(QWidget* w, const QString& s)
{
    if (auto* le = qobject_cast<QLineEdit*>(w)) le->setText(s);
    else if (auto* te = qobject_cast<QPlainTextEdit*>(w)) te->setPlainText(s);
}

void TextCtrl::set_value(const std::string& value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (window) textctrl_set_text(window, QString::fromStdString(value));
    m_value = value;
    m_disable_change_event = false;
}

void TextCtrl::set_value(const boost::any& value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (value.type() == typeid(std::string))
        set_value(boost::any_cast<std::string>(value), false);
    else if (value.type() == typeid(double))
        set_value(double_to_string(boost::any_cast<double>(value)).toStdString(), false);
    else if (value.type() == typeid(int))
        set_value(std::to_string(boost::any_cast<int>(value)), false);
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

void TextCtrl::set_last_meaningful_value()
{
    if (!m_last_meaningful_value.empty())
        set_value(m_last_meaningful_value, false);
}

void TextCtrl::set_na_value()
{
    if (window) textctrl_set_text(window, "N/A");
}

boost::any& TextCtrl::get_value()
{
    if (window)
        m_value = textctrl_get_text(window).toStdString();
    return m_value;
}

bool TextCtrl::value_was_changed()
{
    if (!window) return false;
    return textctrl_get_text(window).toStdString() != boost::any_cast<std::string>(m_value);
}

void TextCtrl::propagate_value()
{
    if (value_was_changed()) on_change_field();
}

void TextCtrl::msw_rescale() {}

void TextCtrl::enable()
{
    if (window) window->setEnabled(true);
}
void TextCtrl::disable()
{
    if (window) window->setEnabled(false);
}

// ─── CheckBox ─────────────────────────────────────────────────────────────────

void CheckBox::BUILD()
{
    auto w = new QCheckBox(m_parent);
    if (!m_opt.tooltip.empty()) w->setToolTip(QString::fromStdString(m_opt.tooltip));
    if (m_opt.readonly) w->setEnabled(false);
    bool initial = false;
    if (const ConfigOptionBool* opt = dynamic_cast<const ConfigOptionBool*>(m_opt.default_value.get()))
        initial = opt->value;
    else if (const ConfigOptionBools* opt2 = dynamic_cast<const ConfigOptionBools*>(m_opt.default_value.get()))
        if (!opt2->values.empty()) initial = opt2->values[0];
    w->setChecked(initial);
    QObject::connect(w, &QCheckBox::stateChanged, [this](int) {
        m_value = static_cast<QCheckBox*>(window)->isChecked();
        on_change_field();
    });
    window = w;
}

void CheckBox::set_value(const bool value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (window) static_cast<QCheckBox*>(window)->setChecked(value);
    m_value = value;
    m_disable_change_event = false;
}

void CheckBox::set_value(const boost::any& value, bool change_event)
{
    bool v = false;
    try {
        if (value.type() == typeid(bool))        v = boost::any_cast<bool>(value);
        else if (value.type() == typeid(int))    v = (bool)boost::any_cast<int>(value);
    } catch (...) {}
    set_value(v, change_event);
}

void CheckBox::set_last_meaningful_value()
{
    if (!m_last_meaningful_value.empty())
        set_value(m_last_meaningful_value, false);
}

void CheckBox::set_na_value()
{
    if (window) static_cast<QCheckBox*>(window)->setCheckState(Qt::PartiallyChecked);
    m_is_na_val = true;
}

boost::any& CheckBox::get_value()
{
    if (window) m_value = static_cast<QCheckBox*>(window)->isChecked();
    return m_value;
}

void CheckBox::msw_rescale() {}

// ─── SpinCtrl ─────────────────────────────────────────────────────────────────

void SpinCtrl::BUILD()
{
    auto w = new SpinInput(m_parent, "", "",
                           {}, {},
                           0,
                           m_opt.min == INT_MIN ? 0   : m_opt.min,
                           m_opt.max == INT_MAX ? 100 : m_opt.max,
                           0);
    if (!m_opt.tooltip.empty()) w->setToolTip(QString::fromStdString(m_opt.tooltip));
    int initial = 0;
    if (const ConfigOptionInt* opt = dynamic_cast<const ConfigOptionInt*>(m_opt.default_value.get()))
        initial = opt->value;
    else if (const ConfigOptionInts* opt2 = dynamic_cast<const ConfigOptionInts*>(m_opt.default_value.get()))
        if (!opt2->values.empty()) initial = opt2->values[0];
    w->SetValue(initial);
    tmp_value = initial;
    QObject::connect(w, &SpinInput::valueChanged, [this](int v) {
        tmp_value = v;
        propagate_value();
    });
    window = w;
}

void SpinCtrl::propagate_value()
{
    m_value = tmp_value;
    on_change_field();
}

void SpinCtrl::set_value(const std::string& value, bool change_event)
{
    int v = value.empty() ? 0 : std::stoi(value);
    set_value(boost::any(v), change_event);
}

void SpinCtrl::set_value(const boost::any& value, bool change_event)
{
    int v = 0;
    try {
        if (value.type() == typeid(int))    v = boost::any_cast<int>(value);
        else if (value.type() == typeid(double)) v = (int)boost::any_cast<double>(value);
    } catch (...) {}
    m_disable_change_event = !change_event;
    if (window) static_cast<SpinInput*>(window)->SetValue(v);
    tmp_value = v;
    m_value = v;
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

boost::any& SpinCtrl::get_value()
{
    if (window) m_value = static_cast<SpinInput*>(window)->GetValue();
    return m_value;
}

void SpinCtrl::msw_rescale() {}

// ─── Choice ───────────────────────────────────────────────────────────────────

namespace { std::map<std::string, DynamicList*> s_dynamic_lists; }

void Choice::register_dynamic_list(std::string const& optname, DynamicList* list)
{
    s_dynamic_lists[optname] = list;
}

Choice::~Choice()
{
    if (m_list) m_list->remove_choice(this);
}

void Choice::BUILD()
{
    auto w = new QComboBox(m_parent);
    if (!m_opt.tooltip.empty()) w->setToolTip(QString::fromStdString(m_opt.tooltip));
    if (m_opt.gui_type == ConfigOptionDef::GUIType::select_open)
        w->setEditable(true), m_is_editable = true;
    // populate enum values
    for (const std::string& s : m_opt.enum_labels)
        w->addItem(QString::fromStdString(s));
    if (w->count() == 0) {
        for (const std::string& s : m_opt.enum_values)
            w->addItem(QString::fromStdString(s));
    }
    // dynamic list
    auto it = s_dynamic_lists.find(m_opt_id);
    if (it != s_dynamic_lists.end()) {
        m_list = it->second;
        m_list->add_choice(this);
        m_list->apply_on(this);
    }
    // Set initial selection from default value
    if (m_opt.default_value) {
        int def_idx = m_opt.default_value->getInt();
        if (def_idx >= 0 && def_idx < w->count()) {
            w->setCurrentIndex(def_idx);
            m_value = def_idx;
        }
    }
    QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx) {
        if (m_suppress_scroll) return;
        m_last_selected = idx;
        on_change_field();
    });
    window = w;
}

void Choice::set_selection()
{
    if (!window) return;
    auto w = static_cast<QComboBox*>(window);
    if (m_value.type() == typeid(int))
        w->setCurrentIndex(boost::any_cast<int>(m_value));
    else if (m_value.type() == typeid(std::string))
        w->setCurrentText(QString::fromStdString(boost::any_cast<std::string>(m_value)));
}

void Choice::set_value(const std::string& value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (window) {
        auto w = static_cast<QComboBox*>(window);
        int idx = w->findText(QString::fromStdString(value));
        if (idx < 0 && m_is_editable) { w->setCurrentText(QString::fromStdString(value)); }
        else if (idx >= 0) w->setCurrentIndex(idx);
    }
    m_value = value;
    m_disable_change_event = false;
}

void Choice::set_value(const boost::any& value, bool change_event)
{
    m_disable_change_event = !change_event;
    m_value = value;
    if (window) {
        auto w = static_cast<QComboBox*>(window);
        if (value.type() == typeid(int))
            w->setCurrentIndex(boost::any_cast<int>(value));
        else if (value.type() == typeid(std::string))
            w->setCurrentText(QString::fromStdString(boost::any_cast<std::string>(value)));
    }
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

void Choice::set_values(const std::vector<std::string>& values)
{
    if (!window) return;
    auto w = static_cast<QComboBox*>(window);
    w->clear();
    for (const auto& s : values) w->addItem(QString::fromStdString(s));
}

void Choice::set_values(const QStringList& values)
{
    if (!window) return;
    auto w = static_cast<QComboBox*>(window);
    w->clear();
    w->addItems(values);
}

boost::any& Choice::get_value()
{
    if (!window) return m_value;
    auto w = static_cast<QComboBox*>(window);
    if (m_is_editable)
        m_value = w->currentText().toStdString();
    else
        m_value = w->currentIndex();
    return m_value;
}

void Choice::set_last_meaningful_value()
{
    if (!m_last_meaningful_value.empty()) set_value(m_last_meaningful_value, false);
}

void Choice::set_na_value()
{
    if (window) static_cast<QComboBox*>(window)->setCurrentIndex(-1);
}

void Choice::enable()
{
    if (window) window->setEnabled(true);
}
void Choice::disable()
{
    if (window) window->setEnabled(false);
}

void Choice::propagate_value()
{
    on_change_field();
}

void Choice::suppress_scroll()
{
    m_suppress_scroll = true;
}

void Choice::msw_rescale() {}

// ─── ColourPicker ─────────────────────────────────────────────────────────────

void ColourPicker::BUILD()
{
    auto container = new QWidget(m_parent);
    auto hbox = new QHBoxLayout(container);
    hbox->setContentsMargins(0, 0, 0, 0);

    auto btn = new QPushButton(container);
    btn->setFixedSize(40, 24);
    btn->setFlat(true);
    hbox->addWidget(btn);

    auto clear = new QPushButton("×", container);
    clear->setFixedSize(20, 24);
    clear->setFlat(true);
    hbox->addWidget(clear);

    m_color_picker = btn;
    m_clear_button = clear;

    // apply default colour
    std::string defval;
    if (const ConfigOptionStrings* opt = dynamic_cast<const ConfigOptionStrings*>(m_opt.default_value.get())) {
        if (!opt->values.empty()) defval = opt->values[0];
    } else if (const ConfigOptionString* opt2 = dynamic_cast<const ConfigOptionString*>(m_opt.default_value.get())) {
        defval = opt2->value;
    }
    if (!defval.empty()) {
        QColor c(QString::fromStdString(defval));
        if (c.isValid()) {
            btn->setStyleSheet(QString("background-color: %1;").arg(c.name()));
            m_value = defval;
        }
    }

    QObject::connect(btn, &QPushButton::clicked, [this]() {
        std::string cur = m_value.empty() ? std::string("#FFFFFF")
                                          : boost::any_cast<std::string>(m_value);
        QColor initial(QString::fromStdString(cur));
        QColor color = QColorDialog::getColor(initial.isValid() ? initial : Qt::white,
                                               m_parent, "Choose colour");
        if (color.isValid()) {
            m_color_picker->setStyleSheet(
                QString("background-color: %1;").arg(color.name()));
            m_value = color.name().toStdString();
            on_change_field();
        }
    });
    QObject::connect(clear, &QPushButton::clicked, [this]() {
        clear_color();
    });

    window = container;
}

void ColourPicker::clear_color()
{
    if (m_color_picker)
        m_color_picker->setStyleSheet("background-color: transparent;");
    m_value = std::string();
    on_change_field();
}

void ColourPicker::set_undef_value(QWidget*)
{
    if (m_color_picker)
        m_color_picker->setStyleSheet("background-color: transparent;");
}

void ColourPicker::set_value(const std::string& value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (m_color_picker) {
        QColor c(QString::fromStdString(value));
        m_color_picker->setStyleSheet(c.isValid()
            ? QString("background-color: %1;").arg(c.name())
            : "background-color: transparent;");
    }
    m_value = value;
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

void ColourPicker::set_value(const boost::any& value, bool change_event)
{
    std::string s;
    try {
        if (value.type() == typeid(std::string)) s = boost::any_cast<std::string>(value);
    } catch (...) {}
    set_value(s, change_event);
}

boost::any& ColourPicker::get_value()
{
    return m_value;
}

void ColourPicker::msw_rescale() {}
void ColourPicker::sys_color_changed() {}
void ColourPicker::update_clear_button_visibility() {}
void ColourPicker::update_clear_button_icon() {}
void ColourPicker::save_colors_to_config() {}

// ─── PointCtrl ────────────────────────────────────────────────────────────────

void PointCtrl::BUILD()
{
    auto container = new QWidget(m_parent);
    auto hbox = new QHBoxLayout(container);
    hbox->setContentsMargins(0, 0, 0, 0);

    auto x = new QLineEdit(container);
    auto y = new QLineEdit(container);
    x->setFixedWidth(60); y->setFixedWidth(60);
    x->setPlaceholderText("X"); y->setPlaceholderText("Y");

    hbox->addWidget(new QLabel("x:", container));
    hbox->addWidget(x);
    hbox->addWidget(new QLabel("y:", container));
    hbox->addWidget(y);

    x_textctrl = x; y_textctrl = y;
    sizer = hbox;

    Vec2d defval(0, 0);
    if (const ConfigOptionPoint* opt = dynamic_cast<const ConfigOptionPoint*>(m_opt.default_value.get()))
        defval = opt->value;
    x->setText(double_to_string(defval.x()));
    y->setText(double_to_string(defval.y()));
    m_value = defval;

    auto propagate = [this](QWidget*) { propagate_value(x_textctrl); };
    QObject::connect(x, &QLineEdit::editingFinished, [this]() { propagate_value(x_textctrl); });
    QObject::connect(y, &QLineEdit::editingFinished, [this]() { propagate_value(y_textctrl); });
}

bool PointCtrl::value_was_changed(QWidget*)
{
    return true;
}

void PointCtrl::propagate_value(QWidget*)
{
    on_change_field();
}

void PointCtrl::set_value(const Vec2d& value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (x_textctrl) static_cast<QLineEdit*>(x_textctrl)->setText(double_to_string(value.x()));
    if (y_textctrl) static_cast<QLineEdit*>(y_textctrl)->setText(double_to_string(value.y()));
    m_value = value;
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

void PointCtrl::set_value(const boost::any& value, bool change_event)
{
    Vec2d v(0, 0);
    try {
        if (value.type() == typeid(Vec2d)) v = boost::any_cast<Vec2d>(value);
    } catch (...) {}
    set_value(v, change_event);
}

boost::any& PointCtrl::get_value()
{
    double x = 0, y = 0;
    if (x_textctrl) x = static_cast<QLineEdit*>(x_textctrl)->text().toDouble();
    if (y_textctrl) y = static_cast<QLineEdit*>(y_textctrl)->text().toDouble();
    m_value = Vec2d(x, y);
    return m_value;
}

void PointCtrl::msw_rescale() {}
void PointCtrl::sys_color_changed() {}

// ─── StaticText ───────────────────────────────────────────────────────────────

void StaticText::BUILD()
{
    auto w = new QLabel(m_parent);
    if (!m_opt.tooltip.empty()) w->setToolTip(QString::fromStdString(m_opt.tooltip));
    std::string defval;
    if (const ConfigOptionString* opt = dynamic_cast<const ConfigOptionString*>(m_opt.default_value.get()))
        defval = opt->value;
    w->setText(QString::fromStdString(defval));
    m_value = defval;
    window = w;
}

void StaticText::set_value(const std::string& value, bool change_event)
{
    m_value = value;
    if (window) static_cast<QLabel*>(window)->setText(QString::fromStdString(value));
}

void StaticText::set_value(const boost::any& value, bool change_event)
{
    std::string s;
    try {
        if (value.type() == typeid(std::string)) s = boost::any_cast<std::string>(value);
    } catch (...) {}
    set_value(s, change_event);
}

void StaticText::msw_rescale() {}

// ─── SliderCtrl ───────────────────────────────────────────────────────────────

void SliderCtrl::BUILD()
{
    auto container = new QWidget(m_parent);
    auto hbox = new QHBoxLayout(container);
    hbox->setContentsMargins(0, 0, 0, 0);

    int min = (m_opt.min == INT_MIN) ? 0   : m_opt.min;
    int max = (m_opt.max == INT_MAX) ? 100 : m_opt.max;

    auto slider = new QSlider(Qt::Horizontal, container);
    slider->setRange(min * m_scale, max * m_scale);
    auto edit = new QLineEdit(container);
    edit->setFixedWidth(50);

    hbox->addWidget(slider, 1);
    hbox->addWidget(edit);

    m_slider = slider; m_textctrl = edit; m_sizer = hbox;

    int initial = 0;
    if (const ConfigOptionInt* opt = dynamic_cast<const ConfigOptionInt*>(m_opt.default_value.get()))
        initial = opt->value;
    slider->setValue(initial * m_scale);
    edit->setText(QString::number(initial));
    m_value = initial;

    QObject::connect(slider, &QSlider::valueChanged, [this](int v) {
        int real = v / m_scale;
        static_cast<QLineEdit*>(m_textctrl)->setText(QString::number(real));
        m_value = real;
        on_change_field();
    });
    QObject::connect(edit, &QLineEdit::editingFinished, [this]() {
        int v = static_cast<QLineEdit*>(m_textctrl)->text().toInt();
        static_cast<QSlider*>(m_slider)->setValue(v * m_scale);
        m_value = v;
        on_change_field();
    });
}

void SliderCtrl::set_value(const int value, bool change_event)
{
    m_disable_change_event = !change_event;
    if (m_slider) static_cast<QSlider*>(m_slider)->setValue(value * m_scale);
    if (m_textctrl) static_cast<QLineEdit*>(m_textctrl)->setText(QString::number(value));
    m_value = value;
    m_disable_change_event = false;
    if (change_event) on_change_field();
}

void SliderCtrl::set_value(const boost::any& value, bool change_event)
{
    int v = 0;
    try {
        if (value.type() == typeid(int))    v = boost::any_cast<int>(value);
        else if (value.type() == typeid(double)) v = (int)boost::any_cast<double>(value);
    } catch (...) {}
    set_value(v, change_event);
}

boost::any& SliderCtrl::get_value()
{
    if (m_textctrl) m_value = static_cast<QLineEdit*>(m_textctrl)->text().toInt();
    return m_value;
}

// ─── MultiVariantTextCtrl ─────────────────────────────────────────────────────

void MultiVariantTextCtrl::BUILD()
{
    auto container = new QWidget(m_parent);
    auto hbox = new QHBoxLayout(container);
    hbox->setContentsMargins(0, 0, 0, 0);

    // For the default case, create a single text ctrl
    m_text_ctrls.push_back(VariantTextCtrl(Field::Create<TextCtrl>(m_parent, m_opt, m_opt_id), 0, ""));
    QWidget* w = m_text_ctrls[0].text_ctrl->getWindow();
    if (w) { w->setParent(container); hbox->addWidget(w); }

    m_variant_sizer = hbox;
}

void MultiVariantTextCtrl::set_value(const boost::any& value, bool change_event)
{
    std::string s;
    try { if (value.type() == typeid(std::string)) s = boost::any_cast<std::string>(value); } catch (...) {}
    set_value(s, change_event);
}

boost::any& MultiVariantTextCtrl::get_value()
{
    if (!m_text_ctrls.empty())
        m_value = m_text_ctrls[0].text_ctrl->get_value();
    return m_value;
}

void MultiVariantTextCtrl::enable()  { for (auto& vc : m_text_ctrls) vc.text_ctrl->enable(); }
void MultiVariantTextCtrl::disable() { for (auto& vc : m_text_ctrls) vc.text_ctrl->disable(); }
void MultiVariantTextCtrl::msw_rescale() {}

void MultiVariantTextCtrl::refresh_text_ctrls_layout(QWidget*) {}

int MultiVariantTextCtrl::get_index_for_extruder(int, NozzleVolumeType) { return 0; }

std::vector<std::pair<int, QString>> MultiVariantTextCtrl::get_current_layout()
{
    return {};
}

t_field MultiVariantTextCtrl::create_text_ctrl(int opt_index, QWidget* parent)
{
    QWidget* p = parent ? parent : m_parent;
    return Field::Create<TextCtrl>(p, m_opt, m_opt_id);
}

// ─── DynamicList ──────────────────────────────────────────────────────────────

void DynamicList::update()
{
    for (Choice* c : m_choices) apply_on(c);
}

void DynamicList::add_choice(Choice* c)
{
    m_choices.push_back(c);
}

void DynamicList::remove_choice(Choice* c)
{
    m_choices.erase(std::remove(m_choices.begin(), m_choices.end(), c), m_choices.end());
}

}} // namespace Slic3r::GUI

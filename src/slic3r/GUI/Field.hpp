#ifndef SLIC3R_GUI_FIELD_HPP
#define SLIC3R_GUI_FIELD_HPP

// Phase 3 TODO: full Qt port of Field (form widgets)
// All wx widget types replaced with Qt equivalents

#include <memory>
#include <cstdint>
#include <functional>
#include <vector>
#include <boost/any.hpp>

#include <QWidget>
#include <QLayout>
#include <QColor>

#include "libslic3r/libslic3r.h"
#include "libslic3r/Config.hpp"
#include "libslic3r/Utils.hpp"
#include "GUI.hpp"
#include "Widgets/SpinInput.hpp"

namespace Slic3r { namespace GUI {

class Field;
using t_field = std::unique_ptr<Field>;
using t_kill_focus  = std::function<void(const std::string&)>;
using t_change      = std::function<void(const t_config_option_key&, const boost::any&)>;
using t_back_to_init = std::function<void(const std::string&)>;

QString double_to_string(double const value, const int max_precision = 4);
QString get_thumbnail_string(const Vec2d& value);
QString get_thumbnails_string(const std::vector<Vec2d>& values);

class Field {
protected:
    virtual void PostInitialize();
    virtual void BUILD() = 0;

    void on_kill_focus();
    void on_change_field();

    class EnterPressed {
    public:
        EnterPressed(Field* field) : m_parent(field) { m_parent->set_enter_pressed(true);  }
        ~EnterPressed()    { m_parent->set_enter_pressed(false); }
    private:
        Field* m_parent;
    };

public:
    void on_back_to_initial_value();
    void on_back_to_sys_value();

    QWidget*       m_parent {nullptr};
    t_kill_focus   m_on_kill_focus {nullptr};
    t_change       m_on_change {nullptr};
    t_back_to_init m_back_to_initial_value{ nullptr };
    t_back_to_init m_back_to_sys_value{ nullptr };

    bool m_disable_change_event {false};
    bool m_is_modified_value {false};
    bool m_is_nonsys_value {true};
    bool parent_is_custom_ctrl{ false };

    const ConfigOptionDef       m_opt {ConfigOptionDef()};
    const t_config_option_key   m_opt_id;
    int                         m_opt_idx = 0;
    double                      opt_height{ 0.0 };

    virtual void       set_value(const boost::any& value, bool change_event) = 0;
    virtual void       set_last_meaningful_value() {}
    virtual void       set_na_value() {}
    virtual boost::any& get_value() = 0;

    virtual void       enable() = 0;
    virtual void       disable() = 0;
    void               toggle(bool en);

    virtual QString    get_tooltip_text(const QString& default_string);

    void               field_changed() { on_change_field(); }

    Field(const ConfigOptionDef& opt, const t_config_option_key& id)
        : m_opt(opt), m_opt_id(id) {}
    Field(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id)
        : m_parent(parent), m_opt(opt), m_opt_id(id) {}
    virtual ~Field();

    virtual QLayout* getSizer()  { return nullptr; }
    virtual QWidget* getWindow() { return nullptr; }

    bool is_matched(const std::string& string, const std::string& pattern);
    void get_value_by_opt_type(QString& str, const bool check_value = true);

    template<class T>
    static t_field Create(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id)
    {
        auto p = Slic3r::make_unique<T>(parent, opt, id);
        p->PostInitialize();
        return std::move(p);
    }

    bool set_undo_bitmap(const ScalableBitmap *bmp) {
        if (m_undo_bitmap != bmp) { m_undo_bitmap = bmp; return true; }
        return false;
    }
    bool set_undo_to_sys_bitmap(const ScalableBitmap *bmp) {
        if (m_undo_to_sys_bitmap != bmp) { m_undo_to_sys_bitmap = bmp; return true; }
        return false;
    }
    bool set_label_colour(const QColor *clr) {
        if (m_label_color != clr) { m_label_color = clr; }
        return false;
    }
    bool set_undo_tooltip(const QString *tip) {
        if (m_undo_tooltip != tip) { m_undo_tooltip = tip; return true; }
        return false;
    }
    bool set_undo_to_sys_tooltip(const QString *tip) {
        if (m_undo_to_sys_tooltip != tip) { m_undo_to_sys_tooltip = tip; return true; }
        return false;
    }

    bool*  get_blink_ptr() { return &m_blink; }

    virtual void msw_rescale();
    virtual void sys_color_changed();

    bool get_enter_pressed() const { return bEnterPressed; }
    void set_enter_pressed(bool pressed) { bEnterPressed = pressed; }

    static int def_width();
    static int def_width_wider();
    static int def_width_thinner();

    const ScalableBitmap* undo_bitmap()        { return m_undo_bitmap; }
    const QString*        undo_tooltip()        { return m_undo_tooltip; }
    const ScalableBitmap* undo_to_sys_bitmap()  { return m_undo_to_sys_bitmap; }
    const QString*        undo_to_sys_tooltip() { return m_undo_to_sys_tooltip; }
    const QColor*         label_color()         { return m_label_color; }
    bool                  blink()               { return m_blink; }
    bool                  combine_side_text()   { return m_combine_side_text; }

protected:
    const ScalableBitmap* m_undo_bitmap         = nullptr;
    const QString*        m_undo_tooltip         = nullptr;
    const ScalableBitmap* m_undo_to_sys_bitmap  = nullptr;
    const QString*        m_undo_to_sys_tooltip  = nullptr;
    bool                  m_blink{ false };
    const QColor*         m_label_color          = nullptr;
    boost::any            m_value;
    boost::any            m_last_meaningful_value;
    int                   m_em_unit = 0;
    bool                  m_combine_side_text = false;
    bool                  bEnterPressed = false;

    friend class OptionsGroup;
};

inline bool is_bad_field(const t_field& obj)  { return obj->getSizer() == nullptr && obj->getWindow() == nullptr; }
inline bool is_window_field(const t_field& obj) { return !is_bad_field(obj) && obj->getWindow() != nullptr && obj->getSizer() == nullptr; }
inline bool is_sizer_field(const t_field& obj)  { return !is_bad_field(obj) && obj->getSizer() != nullptr; }
inline bool is_pure_sizer_field(const t_field& obj) { return is_sizer_field(obj) && obj->getWindow() == nullptr; }


class TextCtrl : public Field {
    using Field::Field;
public:
    TextCtrl(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    TextCtrl(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~TextCtrl() {}

    void BUILD() override;
    bool value_was_changed();
    void propagate_value();
    QWidget* window {nullptr};

    void set_value(const std::string& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    void set_last_meaningful_value() override;
    void set_na_value() override;
    boost::any& get_value() override;

    void msw_rescale() override;
    void enable() override;
    void disable() override;
    QWidget* getWindow() override { return window; }
    QLineEdit* text_ctrl();
};


class CheckBox : public Field {
    using Field::Field;
    bool m_is_na_val {false};
public:
    CheckBox(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    CheckBox(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~CheckBox() {}

    QWidget* window{ nullptr };
    void BUILD() override;

    void set_value(const bool value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    void set_last_meaningful_value() override;
    void set_na_value() override;
    boost::any& get_value() override;

    void msw_rescale() override;
    void enable() override { if (window) window->setEnabled(true); }
    void disable() override { if (window) window->setEnabled(false); }
    QWidget* getWindow() override { return window; }
};


class SpinCtrl : public Field {
    using Field::Field;
    static const int UNDEF_VALUE = INT_MIN;
    bool suppress_propagation {false};
public:
    SpinCtrl(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id), tmp_value(UNDEF_VALUE) {}
    SpinCtrl(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id), tmp_value(UNDEF_VALUE) {}
    ~SpinCtrl() {}

    int tmp_value;
    QWidget* window{ nullptr };
    void BUILD() override;
    void propagate_value();

    void set_value(const std::string& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override;

    void msw_rescale() override;
    void enable() override { if (window) window->setEnabled(true); }
    void disable() override { if (window) window->setEnabled(false); }
    QWidget* getWindow() override { return window; }
};


class Choice;

class DynamicList
{
public:
    virtual ~DynamicList() {}
    virtual void    apply_on(Choice* choice) = 0;
    virtual QString get_value(int index) = 0;
    virtual int     index_of(QString value) = 0;

protected:
    void update();
    std::vector<Choice*> m_choices;

private:
    friend class Choice;
    void add_choice(Choice* choice);
    void remove_choice(Choice* choice);
};


class Choice : public Field {
    using Field::Field;
    DynamicList* m_list = nullptr;
public:
    Choice(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    Choice(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~Choice();

    static void register_dynamic_list(std::string const& optname, DynamicList* list);

    QWidget* window{ nullptr };
    void BUILD() override;
    void propagate_value();

    bool m_is_editable     { false };
    bool m_is_dropped      { false };
    bool m_suppress_scroll { false };
    int  m_last_selected   { -1 };  // was wxNOT_FOUND

    void set_selection();
    void set_value(const std::string& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    void set_values(const std::vector<std::string>& values);
    void set_values(const QStringList& values);
    boost::any& get_value() override;

    void set_last_meaningful_value() override;
    void set_na_value() override;
    void msw_rescale() override;
    void enable() override;
    void disable() override;
    QWidget* getWindow() override { return window; }
    void suppress_scroll();
};


class ColourPicker : public Field {
    using Field::Field;
    void set_undef_value(QWidget* field);
    void clear_color();
public:
    ColourPicker(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    ColourPicker(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~ColourPicker() {}

    QWidget* window{ nullptr };
    void BUILD() override;

    void set_value(const std::string& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override;
    void msw_rescale() override;
    void sys_color_changed() override;

    void enable() override { if (window) window->setEnabled(true); }
    void disable() override { if (window) window->setEnabled(false); }
    QWidget* getWindow() override { return window; }

private:
    QWidget* m_color_picker = nullptr;
    QWidget* m_clear_button = nullptr;
    void update_clear_button_visibility();
    void update_clear_button_icon();
    void save_colors_to_config();
};


class PointCtrl : public Field {
    using Field::Field;
public:
    PointCtrl(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    PointCtrl(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~PointCtrl() {}

    QLayout*  sizer{ nullptr };
    QWidget*  x_textctrl{ nullptr };
    QWidget*  y_textctrl{ nullptr };

    void BUILD() override;
    bool value_was_changed(QWidget* win);
    void propagate_value(QWidget* win);
    void set_value(const Vec2d& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override;

    void msw_rescale() override;
    void sys_color_changed() override;

    void enable() override {
        if (x_textctrl) x_textctrl->setEnabled(true);
        if (y_textctrl) y_textctrl->setEnabled(true);
    }
    void disable() override {
        if (x_textctrl) x_textctrl->setEnabled(false);
        if (y_textctrl) y_textctrl->setEnabled(false);
    }
    QLayout* getSizer() override { return sizer; }
    QWidget* getWindow() override { return x_textctrl; }
};


class StaticText : public Field {
    using Field::Field;
public:
    StaticText(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    StaticText(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~StaticText() {}

    QWidget* window{ nullptr };
    void BUILD() override;

    void set_value(const std::string& value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override { return m_value; }
    void msw_rescale() override;
    void enable() override  { if (window) window->setEnabled(true); }
    void disable() override { if (window) window->setEnabled(false); }
    QWidget* getWindow() override { return window; }
};


class SliderCtrl : public Field {
    using Field::Field;
public:
    SliderCtrl(const ConfigOptionDef& opt, const t_config_option_key& id) : Field(opt, id) {}
    SliderCtrl(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id) : Field(parent, opt, id) {}
    ~SliderCtrl() {}

    QLayout* m_sizer{ nullptr };
    QWidget* m_textctrl{ nullptr };
    QWidget* m_slider{ nullptr };
    int      m_scale = 10;

    void BUILD() override;
    void set_value(const int value, bool change_event = false);
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override;

    void enable() override {
        if (m_slider)   m_slider->setEnabled(true);
        if (m_textctrl) m_textctrl->setEnabled(true);
    }
    void disable() override {
        if (m_slider)   m_slider->setEnabled(false);
        if (m_textctrl) m_textctrl->setEnabled(false);
    }
    QLayout* getSizer() override { return m_sizer; }
    QWidget* getWindow() override { return m_slider; }
};


class MultiVariantTextCtrl : public Field {
    using Field::Field;
public:
    struct VariantTextCtrl {
        t_field  text_ctrl;
        int      opt_index;
        QString  label_text;

        VariantTextCtrl(t_field&& tc, int idx, const QString& txt)
            : text_ctrl(std::move(tc)), opt_index(idx), label_text(txt) {}
    };

    MultiVariantTextCtrl(const ConfigOptionDef& opt, const t_config_option_key& id)
        : Field(opt, id) {}
    MultiVariantTextCtrl(QWidget* parent, const ConfigOptionDef& opt, const t_config_option_key& id)
        : Field(parent, opt, id) {}
    ~MultiVariantTextCtrl() {}

    void BUILD() override;
    void set_value(const boost::any& value, bool change_event = false) override;
    boost::any& get_value() override;
    void enable() override;
    void disable() override;
    void msw_rescale() override;
    QLayout* getSizer() override { return m_variant_sizer; }
    void refresh_text_ctrls_layout(QWidget* parent = nullptr);

    int get_index_for_extruder(int extruder_id, NozzleVolumeType nozzle_type);
    std::vector<std::pair<int, QString>> get_current_layout();
    t_field create_text_ctrl(int opt_index, QWidget* parent = nullptr);

    std::vector<VariantTextCtrl> m_text_ctrls;
    QLayout*      m_variant_sizer  = nullptr;
    PresetBundle* m_preset_bundle  = nullptr;
};

} // namespace GUI
} // namespace Slic3r

#endif /* SLIC3R_GUI_FIELD_HPP */

#ifndef slic3r_Preferences_hpp_
#define slic3r_Preferences_hpp_

#include "GUI.hpp"
#include "GUI_Utils.hpp"
#include <QColor>
#include <QLayout>
#include <QScrollArea>
#include <QTimer>
#include <list>
#include <unordered_map>

#include <vector>
#include <list>
#include <map>
#include "Widgets/ComboBox.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/LinkLabel.hpp"
namespace Slic3r { namespace GUI {


#define DESIGN_SELECTOR_NOMORE_COLOR QColor(248, 248, 248)
#define DESIGN_GRAY900_COLOR QColor(38, 46, 48)
#define DESIGN_GRAY800_COLOR QColor(50, 58, 61)
#define DESIGN_GRAY600_COLOR QColor(144, 144, 144)
#define DESIGN_GRAY400_COLOR QColor(166, 169, 170)

class Selector
{
public:
    int       m_index;
    QWidget *m_tab_button;
    QWidget *m_tab_text;
};
// SelectorHash replaced with std::unordered_map
using SelectorHash = std::unordered_map<int, Selector *>;

class RadioBox;
class RadioSelector
{
public:
    QString  m_param_name;
    int       m_groupid;
    RadioBox *m_radiobox;
    bool      m_selected = false;
};

// RadioSelectorList replaced with std::list
using RadioSelectorList = std::list<RadioSelector>;
class CheckBox;
class TextInput;



#define DESIGN_RESOUTION_PREFERENCES QSize(540, -1)
#define DESIGN_TITLE_SIZE QSize(100, -1)
#define DESIGN_COMBOBOX_SIZE QSize(140, -1)
#define DESIGN_LARGE_COMBOBOX_SIZE QSize(160, -1)
#define DESIGN_INPUT_SIZE QSize(100, -1)


class PreferencesDialog : public DPIDialog
{
private:
    AppConfig *app_config;

protected:
    QLayout *  m_sizer_body;
    QScrollArea* m_scrolledWindow{nullptr};

    // bool								m_settings_layout_changed {false};
    bool m_seq_top_layer_only_changed{false};
    bool m_recreate_GUI{false};
    bool m_use_12h_time_format_changed{false};
    std::string m_original_use_12h_time_format;

public:
    bool seq_top_layer_only_changed() const { return m_seq_top_layer_only_changed; }
    bool recreate_GUI() const { return m_recreate_GUI; }
    bool use_12h_time_format_changed() const { return m_use_12h_time_format_changed; }
    void on_dpi_changed(const QRect &suggested_rect) override;

public:
    PreferencesDialog(QWidget *parent = nullptr);

    ~PreferencesDialog();

    QString m_backup_interval_time;

    void      create();

    // debug mode
    ::CheckBox * m_developer_mode_ckeckbox   = {nullptr};
    ::CheckBox * m_internal_developer_mode_ckeckbox = {nullptr};
    ::CheckBox * m_dark_mode_ckeckbox        = {nullptr};
    ::TextInput *m_backup_interval_textinput = {nullptr};

    QString m_developer_mode_def;
    QString m_internal_developer_mode_def;
    QString m_backup_interval_def;
    QString m_iot_environment_def;

    SelectorHash      m_hash_selector;
    RadioSelectorList m_radio_group;
    // ComboBoxSelectorList    m_comxbo_group;

    QLayout *create_item_title(QString title, QWidget *parent, QString tooltip);
    QLayout *create_item_combobox(QString title, QWidget *parent, QString tooltip, std::string param,const std::vector<QString>& label_list, const std::vector<std::string>& value_list, std::function<void(int)> callback = nullptr, int title_width = 0, int combox_width = 0);
    QLayout *create_item_region_combobox(QString title, QWidget *parent, QString tooltip, std::vector<QString> vlist);
    QLayout *create_item_language_combobox(QString title, QWidget *parent, QString tooltip, int padding_left, std::string param, std::vector<std::string> vlist);
    QLayout *create_item_loglevel_combobox(QString title, QWidget *parent, QString tooltip, std::vector<QString> vlist);
    QLayout *create_item_checkbox(QString title, QWidget *parent, QString tooltip, int padding_left, std::string param);
    QLayout *create_item_darkmode_checkbox(QString title, QWidget *parent, QString tooltip, int padding_left, std::string param);
    void set_dark_mode();
    QLayout *create_item_button(QString title, QString title2, QWidget *parent, QString tooltip, std::function<void()> onclick);
    QWidget * create_item_downloads(QWidget * parent, int padding_left, std::string param);
    QLayout *create_item_input(QString title, QString title2, QWidget *parent, QString tooltip, std::string param, std::function<void(QString)> onchange = {});
    QLayout *create_item_range_input(
        QString title, QWidget *parent, QString tooltip, std::string param, float range_min, float range_max, int keep_digital,std::function<void(QString)> onchange = {});
    QLayout *create_item_range_two_input(QString                      title,
                                            QWidget *                    parent,
                                            QString                      tooltip,
                                            std::string                   param,
                                            std::string                   param1,
                                            float                         range_min,
                                            float                         range_max,
                                            int                           keep_digital,
                                            std::function<void(QString)> onchange = {},
                                            std::function<void(QString)> onchange1 = {});
    QLayout *create_item_backup_input(QString title, QWidget *parent, QString tooltip, std::string param);
    QLayout *create_item_multiple_combobox(
        QString title, QWidget *parent, QString tooltip, int padding_left, std::string parama, std::vector<QString> vlista, std::vector<QString> vlistb);
    QLayout *create_item_switch(QString title, QWidget *parent, QString tooltip, std::string param);
    QWidget *  create_item_radiobox(QString title, QWidget *parent, QString tooltip, int padding_left, int groupid, std::string param);

    QWidget * create_general_page();
    void create_gui_page();
    void create_sync_page();
    void create_shortcuts_page();
    QWidget * create_debug_page();

    void     on_select_radio(std::string param);
    QString get_select_radio(int groupid);
    // BBS
    void create_select_domain_widget();

    void Split(const std::string &src, const std::string &separator, std::vector<QString> &dest);
    int m_current_language_selected = {0};

    std::unordered_map<int, Button *> m_button_list;
    std::unordered_map<int, ::CheckBox *> m_checkbox_list;
    std::unordered_map<int, RadioBox *>   m_radiobox_list;
    std::unordered_map<int, ::ComboBox *> m_combobox_list;
    int                                   m_screen_height;

protected:
    void OnSelectTabel(int idx);
    void OnSelectRadio();
};

QEvent::Type getPreferencesSelectTabEventType();

}} // namespace Slic3r::GUI

#endif /* slic3r_Preferences_hpp_ */

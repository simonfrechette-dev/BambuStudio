#ifndef slic3r_SearchComboBox_hpp_
#define slic3r_SearchComboBox_hpp_
// Qt6 port of Search.hpp
// Original wx version backed up to Search.hpp.wx-backup

#include <vector>
#include <map>

#include <boost/nowide/convert.hpp>

#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QBoxLayout>
#include <QTreeView>
#include <QColor>
#include <QAbstractListModel>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include "libslic3r/Preset.hpp"
#include "Widgets/ScrolledWindow.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/PopupWindow.hpp"
#include "GUI_ObjectList.hpp"

namespace Slic3r {

// Qt equivalents for the three custom wx events
inline const QEvent::Type wxCUSTOMEVT_JUMP_TO_OPTION  = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type wxCUSTOMEVT_EXIT_SEARCH     = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type wxCUSTOMEVT_JUMP_TO_OBJECT  = static_cast<QEvent::Type>(QEvent::registerEventType());

namespace Search {

class SearchDialog;

struct InputInfo
{
    DynamicPrintConfig *config{nullptr};
    Preset::Type        type{Preset::TYPE_INVALID};
    ConfigOptionMode    mode{comSimple};
};

struct GroupAndCategory
{
    QString group;
    QString category;
};

struct Option
{
    bool operator<(const Option &other) const { return other.key > this->key; }

    std::wstring key;
    Preset::Type type{Preset::TYPE_INVALID};
    std::wstring label;
    std::wstring label_local;
    std::wstring group;
    std::wstring group_local;
    std::wstring category;
    std::wstring category_local;
    bool multi_category{false};

    std::string opt_key() const;
};

struct FoundOption
{
    std::string label;
    std::string marked_label;
    std::string tooltip;
    size_t      option_idx{0};
    int         outScore{0};

    void get_marked_label_and_tooltip(const char **label, const char **tooltip) const;
};

struct OptionViewParameters
{
    bool category{false};
    bool english{false};
    int  hovered_id{0};
};

class OptionsSearcher
{
    std::string                             search_line;
    Preset::Type                            search_type = Preset::TYPE_INVALID;

    std::map<std::string, GroupAndCategory> groups_and_categories;
    PrinterTechnology                       printer_technology;

    std::vector<Option>      options{};
    std::vector<FoundOption> found{};

    void append_options(DynamicPrintConfig *config, Preset::Type type, ConfigOptionMode mode);

    void sort_options();
    void sort_found()
    {
        std::sort(found.begin(), found.end(),
                  [](const FoundOption &f1, const FoundOption &f2) {
                      return f1.outScore > f2.outScore ||
                             (f1.outScore == f2.outScore && f1.label < f2.label);
                  });
    }

    size_t options_size() const { return options.size(); }
    size_t found_size() const   { return found.size(); }

public:
    OptionViewParameters view_params;

    SearchDialog *search_dialog{nullptr};

    OptionsSearcher();
    ~OptionsSearcher();

    void init(std::vector<InputInfo> input_values);
    void apply(DynamicPrintConfig *config, Preset::Type type, ConfigOptionMode mode);
    bool search();
    bool search(const std::string &search, bool force = false, Preset::Type type = Preset::TYPE_INVALID);

    void add_key(const std::string &opt_key, Preset::Type type, const QString &group, const QString &category);

    size_t size() const { return found_size(); }

    const FoundOption &operator[](const size_t pos) const noexcept { return found[pos]; }
    const Option &     get_option(size_t pos_in_filter) const;
    const Option &     get_option(const std::string &opt_key, Preset::Type type, int &variant_index) const;
    Option             get_option(const std::string &opt_key, const QString &label, Preset::Type type) const;

    const std::vector<FoundOption> &found_options() { return found; }
    const GroupAndCategory &        get_group_and_category(const std::string &opt_key) { return groups_and_categories[opt_key]; }
    std::string &                   search_string() { return search_line; }

    void set_printer_technology(PrinterTechnology pt) { printer_technology = pt; }

    void sort_options_by_key()
    {
        std::sort(options.begin(), options.end(), [](const Option &o1, const Option &o2) { return o1.key < o2.key; });
    }
    void sort_options_by_label() { sort_options(); }

    void show_dialog(Preset::Type type, QWidget *parent, TextInput *input, QWidget *search_btn);
    void dlg_sys_color_changed();
    void dlg_msw_rescale();
};

//------------------------------------------
//          SearchItem
//------------------------------------------
class SearchObjectDialog;
class SearchItem : public QWidget
{
    Q_OBJECT
public:
    QString      m_text;
    int           m_index{0};
    SearchDialog* m_sdialog{nullptr};
    SearchObjectDialog* m_search_object_dialog{nullptr};
    GUI::ObjectDataViewModelNode* m_item{nullptr};

    SearchItem(QWidget *parent, const QString &text, int index,
               SearchDialog *sdialog = nullptr,
               SearchObjectDialog *search_dialog = nullptr,
               const QString &tooltip = {});
    ~SearchItem() = default;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *evt) override;
    void leaveEvent(QEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void mouseReleaseEvent(QMouseEvent *evt) override;
};

//------------------------------------------
//          SearchDialog
//------------------------------------------
class SearchListModel;
class SearchDialog : public PopupWindow
{
    Q_OBJECT
public:
    QString search_str;
    QString default_string;

    bool prevent_list_events{false};

    QColor m_text_color;
    QColor m_bg_colour;
    QColor m_hover_colour;
    QColor m_bold_colour;
    QColor m_thumb_color;

    QBoxLayout *m_sizer_body{nullptr};
    QBoxLayout *m_sizer_main{nullptr};
    QBoxLayout *m_sizer_border{nullptr};
    QBoxLayout *m_listsizer{nullptr};

    QWidget *m_border_panel{nullptr};
    QWidget *m_client_panel{nullptr};
    QWidget *m_listPanel{nullptr};
    QWidget *m_event_tag{nullptr};
    QWidget *m_search_item_tag{nullptr};

    int       em{0};
    const int POPUP_WIDTH  = 38;
    const int POPUP_HEIGHT = 40;
    int       m_pop_width  = 400;

    TextInput    *search_line{nullptr};
    QWidget      *search_line2{nullptr};
    Preset::Type  search_type = Preset::TYPE_INVALID;

    QTreeView        *search_list{nullptr};
    ScrolledWindow   *m_scrolledWindow{nullptr};
    SearchListModel  *search_list_model{nullptr};
    QCheckBox        *check_category{nullptr};

    OptionsSearcher *searcher{nullptr};

    void OnInputText(const QString &text);
    void OnKeyDown(QKeyEvent *event);
    void OnActivate(const QModelIndex &index);
    void OnSelect(const QModelIndex &index);
    void OnCheck(int state);
    void update_list();

public:
    SearchDialog(OptionsSearcher *searcher, Preset::Type type, QWidget *parent,
                 TextInput *input, QWidget *search_btn);
    ~SearchDialog();

    void MSWDismissUnfocusedPopup();
    void Popup(QWidget *anchor = nullptr) override;
    void OnDismiss();
    void Dismiss();
    void Die();
    void ProcessSelection(const QModelIndex &selection);
    void msw_rescale();
};

// ----------------------------------------------------------------------------
// SearchListModel
// ----------------------------------------------------------------------------
class SearchListModel : public QAbstractListModel
{
    Q_OBJECT
    std::vector<std::pair<QString, int>> m_values;
    ScalableBitmap                       m_icon[5];

public:
    enum { colIcon, colMarkedText, colMax };

    SearchListModel(QWidget *parent);

    void Clear();
    void Prepend(const std::string &text);
    void msw_rescale();

    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

class SearchObjectDialog : public PopupWindow
{
    Q_OBJECT
public:
    SearchObjectDialog(GUI::ObjectList *object_list, QWidget *parent);
    ~SearchObjectDialog();

    void Popup(QWidget *anchor = nullptr) override;
    void Dismiss();
    void update_list();

public:
    GUI::ObjectList *m_object_list{nullptr};

    int       em{0};
    const int POPUP_WIDTH  = 41;
    const int POPUP_HEIGHT = 45;

    ScrolledWindow *m_scrolledWindow{nullptr};

    QColor m_text_color;
    QColor m_bg_color;
    QColor m_thumb_color;
    QColor m_bold_color;

    QBoxLayout *m_sizer_body{nullptr};
    QBoxLayout *m_sizer_main{nullptr};
    QBoxLayout *m_sizer_border{nullptr};

    QWidget *m_border_panel{nullptr};
    QWidget *m_client_panel{nullptr};
    QWidget *m_listPanel{nullptr};
};

} // namespace Search
} // namespace Slic3r

#endif // slic3r_SearchComboBox_hpp_

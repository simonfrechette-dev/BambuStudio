#ifndef slic3r_UnsavedChangesDialog_hpp_
#define slic3r_UnsavedChangesDialog_hpp_

// Qt6 port of UnsavedChangesDialog.hpp
// Original wx version backed up to UnsavedChangesDialog.hpp.wx-backup

#include <QDialog>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QIcon>
#include <QString>
#include <vector>
#include <map>
#include <memory>

#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/ScrolledWindow.hpp"

class QLabel;
class QScrollArea;
class QCheckBox;
class QBoxLayout;

namespace Slic3r {
namespace GUI {

class ModelNode;
class PresetComboBox;
class MainFrame;
using ModelNodePtrArray = std::vector<std::unique_ptr<ModelNode>>;

class ModelNode
{
    QWidget*            m_parent_win{nullptr};
    ModelNode*          m_parent;
    ModelNodePtrArray   m_children;
    Preset::Type        m_preset_type{Preset::TYPE_INVALID};
    std::string         m_icon_name;
    QString             m_old_color;
    QString             m_new_color;

public:
    bool     m_toggle{true};
    QIcon    m_icon;
    QIcon    m_old_color_bmp;
    QIcon    m_new_color_bmp;
    QString  m_text;
    QString  m_old_value;
    QString  m_new_value;
    bool     m_container{true};

    // preset(root) node
    ModelNode(Preset::Type preset_type, QWidget* parent_win, const QString& text, const std::string& icon_name);
    // category node
    ModelNode(ModelNode* parent, const QString& text, const std::string& icon_name);
    // group node
    ModelNode(ModelNode* parent, const QString& text);
    // option node
    ModelNode(ModelNode* parent, const QString& text, const QString& old_value, const QString& new_value);

    bool                IsContainer() const     { return m_container; }
    bool                IsToggled() const       { return m_toggle; }
    void                Toggle(bool toggle = true) { m_toggle = toggle; }
    bool                IsRoot() const          { return m_parent == nullptr; }
    Preset::Type        type() const            { return m_preset_type; }
    const QString&      text() const            { return m_text; }

    ModelNode*          GetParent()             { return m_parent; }
    ModelNodePtrArray&  GetChildren()           { return m_children; }
    ModelNode*          GetNthChild(unsigned int n) { return m_children[n].get(); }
    unsigned int        GetChildCount() const   { return (unsigned int)m_children.size(); }

    void Append(std::unique_ptr<ModelNode> child) { m_children.emplace_back(std::move(child)); }

    void UpdateEnabling();
    void UpdateIcons();
};


class DiffModel : public QAbstractItemModel
{
    Q_OBJECT
    QWidget*            m_parent_win{nullptr};
    ModelNodePtrArray   m_preset_nodes;
    QTreeView*          m_ctrl{nullptr};

public:
    enum { colToggle, colIconText, colOldValue, colNewValue, colMax };

    DiffModel(QWidget* parent);
    ~DiffModel() = default;

    void SetAssociatedControl(QTreeView* ctrl) { m_ctrl = ctrl; }

    QModelIndex AddPreset(Preset::Type type, const QString& preset_name, PrinterTechnology pt);
    QModelIndex AddOption(Preset::Type type, const QString& category_name, const QString& group_name,
                          const QString& option_name, const QString& old_value, const QString& new_value,
                          const std::string& category_icon_name);

    void UpdateItemEnabling(const QModelIndex& item);
    bool IsEnabledItem(const QModelIndex& item);

    void Rescale();
    void Clear();

    // QAbstractItemModel overrides
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override { return colMax; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};


class DiffViewCtrl : public QTreeView
{
    Q_OBJECT
    bool m_has_long_strings{false};
    bool m_empty_selection{false};
    int  m_em_unit;

    struct ItemData {
        std::string  opt_key;
        QString      opt_name;
        QString      old_val;
        QString      new_val;
        Preset::Type type;
        bool         is_long{false};
    };
    std::map<int, ItemData>     m_items_map; // row -> ItemData
    std::map<unsigned int, int> m_columns_width;

public:
    DiffViewCtrl(QWidget* parent, QSize size);
    ~DiffViewCtrl();

    DiffModel* model{nullptr};

    void AppendBmpTextColumn(const QString& label, unsigned model_column, int width, bool set_expander = false);
    void AppendToggleColumn_(const QString& label, unsigned model_column, int width);
    void Rescale(int em = 0);
    void Append(const std::string& opt_key, Preset::Type type, const QString& category_name,
                const QString& group_name, const QString& option_name,
                const QString& old_value, const QString& new_value,
                const std::string& category_icon_name);
    void Clear();

    QString get_short_string(const QString& full_string);
    bool    has_selection() { return !m_empty_selection; }
    void    set_em_unit(int em) { m_em_unit = em; }
    bool    has_unselected_options();

    std::vector<std::string> options(Preset::Type type, bool selected);
    std::vector<std::string> selected_options();
};


#define BOTH_SIDES_BORDER 25

struct PresetItem
{
    Preset::Type type;
    std::string  opt_key;
    QString      category_name;
    QString      group_name;
    QString      option_name;
    QString      old_value;
    QString      new_value;
};

enum ForceOption {
    fopTransfer,
    fopSave,
    fopDiscard,
    fopNone
};

class UnsavedChangesDialog : public DPIDialog
{
    Q_OBJECT
protected:
    QWidget*     m_top_line{nullptr};
    QWidget*     m_panel_tab{nullptr};
    QWidget*     m_table_top{nullptr};
    QWidget*     title_block_middle{nullptr};
    QWidget*     title_block_right{nullptr};
    QLabel*      static_temp_title{nullptr};
    QLabel*      static_oldv_title{nullptr};
    QLabel*      static_newv_title{nullptr};
    QBoxLayout*  m_sizer_bottom{nullptr};

    Button*      m_save_btn{nullptr};
    Button*      m_transfer_btn{nullptr};
    Button*      m_discard_btn{nullptr};
    Button*      m_cancel_btn{nullptr};
    QLabel*      m_action_line{nullptr};
    QLabel*      m_info_line{nullptr};
    QScrollArea* m_scrolledWindow{nullptr};

    bool m_has_long_strings{false};
    int  m_save_btn_id{-1};
    int  m_move_btn_id{-1};
    int  m_continue_btn_id{-1};

    std::string m_app_config_key;

    enum class Action { Undef, Transfer, Save, Discard };

    static constexpr char ActTransfer[] = "transfer";
    static constexpr char ActDiscard[]  = "discard";
    static constexpr char ActSave[]     = "save";

    Action m_exit_action{Action::Undef};

public:
    struct PresetData {
        std::string  name;
        Preset::Type type;
        bool         save_to_project;
        PresetData(std::string preset_name, Preset::Type preset_type, bool save_project)
            : name(preset_name), type(preset_type), save_to_project(save_project) {}
    };

private:
    std::vector<PresetItem>  m_presetitems;
    std::vector<PresetData>  names_and_types;
    int m_buttons{ActionButtons::TRANSFER | ActionButtons::SAVE};
    std::string m_new_selected_preset_name;

public:
    enum ActionButtons {
        TRANSFER        = 1,
        KEEP            = 2,
        SAVE            = 4,
        DONT_SAVE       = 8,
        REMEMBER_CHOISE = 0x10000
    };

    struct SyncExtruderParams {
        DynamicConfig*   config;
        int              from;
        int              to;
        bool             left_to_right;
        NozzleVolumeType nozzle;
    };

    UnsavedChangesDialog(Preset::Type type, PresetCollection* dependent_presets,
                         const std::string& new_selected_preset, bool no_transfer = false);
    UnsavedChangesDialog(const QString& caption, const QString& header,
                         const std::string& app_config_key, int act_buttons);
    UnsavedChangesDialog(const QString& caption, const QString& header,
                         DynamicConfig* config, int from, int to, bool left_to_right, NozzleVolumeType nozzle);
    ~UnsavedChangesDialog() = default;

    int ShowModal() { return exec(); }

    void build(Preset::Type type, PresetCollection* dependent_presets,
               const std::string& new_selected_preset, const QString& header = {});
    void update(Preset::Type type, PresetCollection* dependent_presets,
                const std::string& new_selected_preset, const QString& header);
    void update_list(SyncExtruderParams* params = nullptr);
    std::string subreplace(std::string resource_str, std::string sub_str, std::string new_str);
    void update_tree(Preset::Type type, PresetCollection* presets);
    void update_tree(Preset::Type type, DynamicConfig* config, int from, int to);
    void show_info_line(Action action, std::string preset_name = "");
    void update_config(Action action);
    void close(Action action);
    bool save(PresetCollection* dependent_presets, bool show_save_preset_dialog = true);

    bool save_preset() const      { return m_exit_action == Action::Save;     }
    bool transfer_changes() const { return m_exit_action == Action::Transfer; }
    bool discard() const          { return m_exit_action == Action::Discard;  }

    const std::vector<PresetData>& get_names_and_types() { return names_and_types; }
    bool get_save_to_project_option() { return names_and_types[0].save_to_project; }
    std::string get_preset_name() { return names_and_types[0].name; }

    std::vector<std::string> get_unselected_options(Preset::Type /*type*/) { return {}; }
    std::vector<std::string> get_selected_options(Preset::Type type) {
        std::vector<std::string> tmp;
        for (auto& it : m_presetitems)
            if (it.type == type) tmp.push_back(it.opt_key);
        return tmp;
    }
    std::vector<std::string> get_selected_options() {
        std::vector<std::string> tmp;
        for (auto& it : m_presetitems) tmp.push_back(it.opt_key);
        return tmp;
    }
    bool has_unselected_options() { return false; }

protected:
    void on_dpi_changed(const QRect& suggested_rect) override {}
    void on_sys_color_changed() override {}
    bool check_option_valid();
};


class FullCompareDialog : public QDialog
{
    Q_OBJECT
public:
    FullCompareDialog(const QString& option_name, const QString& old_value, const QString& new_value,
                      const QString& old_value_header, const QString& new_value_header);
    ~FullCompareDialog() = default;
};


class DiffPresetDialog : public DPIDialog
{
    Q_OBJECT
    DiffViewCtrl*                   m_tree{nullptr};
    QLabel*                         m_top_info_line{nullptr};
    QLabel*                         m_bottom_info_line{nullptr};
    QCheckBox*                      m_show_all_presets{nullptr};

    Preset::Type                    m_view_type{Preset::TYPE_INVALID};
    PrinterTechnology               m_pr_technology;
    std::unique_ptr<PresetBundle>   m_preset_bundle_left;
    std::unique_ptr<PresetBundle>   m_preset_bundle_right;

    void update_tree();
    void update_bundles_from_app();
    void update_controls_visibility(Preset::Type type = Preset::TYPE_INVALID);
    void update_compatibility(const std::string& preset_name, Preset::Type type, PresetBundle* preset_bundle);

    struct DiffPresets {
        PresetComboBox* presets_left{nullptr};
        ScalableButton* equal_bmp{nullptr};
        PresetComboBox* presets_right{nullptr};
    };
    std::vector<DiffPresets> m_preset_combos;

public:
    DiffPresetDialog(MainFrame* mainframe);
    ~DiffPresetDialog() = default;

    void show(Preset::Type type = Preset::TYPE_INVALID);
    void update_presets(Preset::Type type = Preset::TYPE_INVALID);

protected:
    void on_dpi_changed(const QRect& suggested_rect) override {}
    void on_sys_color_changed() override {}

    friend class MainFrame;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_UnsavedChangesDialog_hpp_

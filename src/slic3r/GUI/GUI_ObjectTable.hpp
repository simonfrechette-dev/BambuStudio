#ifndef slic3r_GUI_ObjectTable_hpp_
#define slic3r_GUI_ObjectTable_hpp_

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <QWidget>
#include <QColor>
#include <QString>
#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "GUI_ObjectTableSettings.hpp"

namespace Slic3r {
class DynamicPrintConfig;
class ModelConfig;
class ModelObject;
class Model;
class ConfigOption;
namespace GUI {

class Plater;
class ComboBox;
class TextInput;
class ObjectTablePanel;
class ObjectTableSettings;

class ObjectGridTable
{
public:
    static std::string category_all;
    static std::string plate_outside;

    enum GridRowType { row_object = 0, row_volume = 1 };

    struct ObjectGridCol {
        std::string  key;
        std::string  category;
        int          object_id{0};
        int          volume_id{0};
        int          size{0};
        bool         b_for_object{false};
        bool         b_icon{false};
        bool         b_editable{false};
        int          choice_count{0};
        int          horizontal_align{0};
    };

    struct ObjectGridRow {
        GridRowType  type{row_object};
        int          object_id{-1};
        int          volume_id{-1};
        ModelObject* object{nullptr};
        ModelConfig* config{nullptr};
    };

    ObjectGridTable() = default;

    ObjectGridCol* get_grid_col(int col) { (void)col; return nullptr; }
    ObjectGridRow* get_grid_row(int row) { (void)row; return nullptr; }

    void sort_by_default() {}
    void sort_by_col(int col) { (void)col; }
    void SetSelection(int object_id, int volume_id) { (void)object_id; (void)volume_id; }
    void OnSelectCell(int row, int col) { (void)row; (void)col; }
    void OnRangeSelected(int row, int col, int row_count, int col_count) { (void)row; (void)col; (void)row_count; (void)col_count; }
    void OnCellValueChanged(int row, int col) { (void)row; (void)col; }
    void enable_reset_all_button(bool enable) { (void)enable; }
    void reload_cell_data(int row, const std::string& category) { (void)row; (void)category; }
};

class ObjectTablePanel : public QWidget
{
public:
    int range_select_left_col{0};
    int range_select_right_col{0};
    int range_select_top_row{0};
    int range_select_bottom_row{0};

    std::vector<QString> m_filaments_name;
    std::vector<QColor>  m_filaments_colors;
    unsigned int m_filaments_count{1};

    ObjectTablePanel(QWidget* parent, Plater* platerObj, Model* modelObj);
    ~ObjectTablePanel();

    void load_data();
    void SetSelection(int object_id, int volume_id);
    void sort_by_default();
    QSize get_init_size();
    void resetAllValuesInSideWindow(int row, bool is_object, ModelObject* object,
        ModelConfig* config, const std::string& category);
    void msw_rescale();

    void set_default_filaments_and_colors() {
        m_filaments_count = 1;
        m_filaments_colors.push_back(QColor(Qt::green));
        m_filaments_name.push_back("Generic PLA");
    }

private:
    ObjectGridTable*     m_object_grid_table{nullptr};
    ObjectTableSettings* m_object_settings{nullptr};
    Model*               m_model{nullptr};
    Plater*              m_plater{nullptr};
    int m_cur_row{-1};
    int m_cur_col{-1};
};

class ObjectTableDialog : public DPIDialog
{
    ObjectTablePanel* m_obj_panel{nullptr};
    Model*    m_model{nullptr};
    Plater*   m_plater{nullptr};

public:
    ObjectTableDialog(QWidget* parent, Plater* platerObj, Model* modelObj, QSize maxSize);
    ~ObjectTableDialog();

    void Popup(int obj_idx = -1, int vol_idx = -1);
    void OnClose();
    void OnSize();

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;
    void on_sys_color_changed();
};

} // GUI
} // Slic3r

#endif // slic3r_GUI_ObjectTable_hpp_

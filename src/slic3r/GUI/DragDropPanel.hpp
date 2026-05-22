#ifndef slic3r_DragDropPanel_hpp_
#define slic3r_DragDropPanel_hpp_
#include <QWidget>
#include <QString>

#include "GUI.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/Label.hpp"

#include <vector>


namespace Slic3r { namespace GUI {


class FilamentMapManualPanel;

QColor Hex2Color(const std::string& str);

class ColorPanel;
class DragDropPanel : public QWidget
{
public:
    DragDropPanel(QWidget *parent, const QString &label, bool is_auto, bool has_title = true, bool is_sub = false);

    void AddColorBlock(const QColor &color, const std::string &type, int filament_id, bool update_ui = true);
    void RemoveColorBlock(ColorPanel *panel, bool update_ui = true);
    void DoDragDrop(ColorPanel *panel, const QColor &color, const std::string &type, int filament_id);
    void UpdateLabel(const QString &label);

    std::vector<int> GetAllFilaments() const;

    void set_is_draging(bool is_draging) { m_is_draging = is_draging; }
    bool is_draging() const { return m_is_draging; }

    std::vector<ColorPanel *> get_filament_blocks() const { return m_filament_blocks; }

private:
    QBoxLayout *m_sizer;
    QGridLayout *m_grid_item_sizer;
    Label       *m_title_label = nullptr;
    bool         m_is_auto;

    std::vector<ColorPanel *> m_filament_blocks;

    void NotifyDragDropCompleted();
private:
    bool m_is_draging = false;
};

///////////////   ColorPanel  start ////////////////////////
// The UI panel of drag item
class ColorPanel : public QWidget
{
public:
    ColorPanel(DragDropPanel *parent, const QColor &color, int filament_id, const std::string& type);

    QColor GetColor() const { return m_color; }
    int      GetFilamentId() const { return m_filament_id; }
    std::string GetType() const { return m_type; }

private:
    void OnLeftDown(QMouseEvent &event);
    void OnLeftUp(QMouseEvent &event);
    void OnPaint(QPaintEvent &event);

    DragDropPanel *m_parent;
    QColor        m_color;
    std::string    m_type;
    int            m_filament_id;

};

class SeparatedDragDropPanel : public QWidget
{
public:
    SeparatedDragDropPanel(QWidget *parent, const QString &label, bool use_separation = false);

    void AddColorBlock(const QColor &color, const std::string &type, int filament_id, bool is_high_flow = false, bool update_ui = true);
    void RemoveColorBlock(ColorPanel *panel, bool update_ui = true);

    std::vector<int> GetAllFilaments() const;
    std::vector<int> GetHighFlowFilaments() const;
    std::vector<int> GetStandardFilaments() const;
    std::vector<int> GetTPUHighFlowFilaments() const;

    std::vector<ColorPanel *> get_filament_blocks() const;
    std::vector<ColorPanel *> get_high_flow_blocks() const;
    std::vector<ColorPanel *> get_standard_blocks() const;

    void SetUseSeparation(bool use_separation);
    bool IsUseSeparation() const { return m_use_separation; }
    void ClearAllBlocks();
    void UpdateLabel(const QString &label);

private:
    void UpdateLayout();

    QBoxLayout   *m_main_sizer;
    QWidget      *m_content_panel;
    QBoxLayout   *m_content_sizer;
    QLabel *m_label;

    DragDropPanel *m_high_flow_panel;
    DragDropPanel *m_standard_panel;

    DragDropPanel *m_unified_panel;

    bool m_use_separation;
};

}} // namespace Slic3r::GUI

#endif /* slic3r_DragDropPanel_hpp_ */

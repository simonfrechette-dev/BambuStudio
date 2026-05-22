#ifndef slic3r_GUI_ObjectLayers_hpp_
#define slic3r_GUI_ObjectLayers_hpp_

// Qt port of GUI_ObjectLayers.hpp

#include "GUI_ObjectSettings.hpp"
#include "wxExtensions.hpp"

#include <QLineEdit>
#include <QGridLayout>
#include <QLayout>
#include <QWidget>
#include <QString>
#include <functional>

namespace Slic3r {
class ModelObject;

namespace GUI {
class ConfigOptionsGroup;

typedef double                          coordf_t;
typedef std::pair<coordf_t, coordf_t>   t_layer_height_range;

class ObjectLayers;

enum EditorType
{
    etUndef         = 0,
    etMinZ          = 1,
    etMaxZ          = 2,
    etLayerHeight   = 4,
};

class LayerRangeEditor : public QLineEdit
{
    Q_OBJECT
    bool            m_enter_pressed   { false };
    bool            m_call_kill_focus { false };
    QString         m_valid_value;
    EditorType      m_type;

    std::function<void(EditorType)> m_set_focus_data;

public:
    LayerRangeEditor(   ObjectLayers* parent,
                        const QString& value = QString(),
                        EditorType type = etUndef,
                        std::function<void(EditorType)>           set_focus_data_fn = [](EditorType) {},
                        std::function<bool(coordf_t, bool, bool)> edit_fn           = [](coordf_t, bool, bool) { return false; }
                        );
    ~LayerRangeEditor() {}

    EditorType  type() const { return m_type; }
    void        set_focus_data() const { m_set_focus_data(m_type); }
    void        msw_rescale();

private:
    coordf_t    get_value();
};

class ObjectLayers : public OG_Settings
{
    ScalableBitmap  m_bmp_delete;
    ScalableBitmap  m_bmp_add;
    ModelObject*    m_object{ nullptr };

    QGridLayout*           m_grid_sizer{ nullptr };
    t_layer_height_range   m_selectable_range;
    EditorType             m_selection_type{ etUndef };

public:
    explicit ObjectLayers(QWidget* parent);
    ~ObjectLayers() {}

    // Button remembers the layer height range for which it was created.
    class PlusMinusButton : public ScalableButton
    {
    public:
        PlusMinusButton(QWidget *parent, const ScalableBitmap &bitmap, std::pair<coordf_t, coordf_t> range)
            : ScalableButton(parent, 0, bitmap), range(range) {}
        std::pair<coordf_t, coordf_t> range;
    };

    void        select_editor(LayerRangeEditor* editor, const bool is_last_edited_range);
    QLayout*    create_layer(const t_layer_height_range& range, PlusMinusButton *delete_button, PlusMinusButton *add_button);
    void        create_layers_list();
    void        update_layers_list();

    void        update_scene_from_editor_selection() const;

    void        UpdateAndShow(const bool show) override;
    void        msw_rescale();
    void        sys_color_changed();
    void        reset_selection();
    void        set_selectable_range(const t_layer_height_range& range) { m_selectable_range = range; }

    friend class LayerRangeEditor;
};

}} // namespace Slic3r::GUI

#endif // slic3r_GUI_ObjectLayers_hpp_

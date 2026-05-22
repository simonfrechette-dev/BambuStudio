#ifndef slic3r_GUI_Preview_hpp_
#define slic3r_GUI_Preview_hpp_

// Qt port of GUI_Preview.hpp

#include "libslic3r/Point.hpp"
#include "libslic3r/CustomGCode.hpp"
#include "libslic3r/PrintBase.hpp"
#include "libslic3r/GCode/GCodeProcessor.hpp"

#include <QWidget>
#include <QOpenGLWidget>
#include <string>
#include <memory>
#include <functional>

namespace Slic3r {

class DynamicPrintConfig;
class Print;
class BackgroundSlicingProcess;
class Model;

namespace GUI {

class GLCanvas3D;
class GLToolbar;
class Bed3D;
struct Camera;
class Plater;
#ifdef _WIN32
class BitmapComboBox;
#endif
namespace gcode {
    class Layers;
}

class BaseView : public QWidget
{
    Q_OBJECT
public:
    explicit BaseView(QWidget* parent = nullptr);
    virtual ~BaseView();
    QOpenGLWidget* get_wxglcanvas() { return m_canvas_widget; }
    GLCanvas3D*    get_canvas3d()   { return m_canvas; }
    bool Show(bool show);
    const std::shared_ptr<Camera>& get_override_camera() const;
protected:
    QOpenGLWidget*          m_canvas_widget{ nullptr };
    GLCanvas3D*             m_canvas{ nullptr };
    std::shared_ptr<Camera> m_p_override_camera{ nullptr };
};

class View3D : public BaseView
{
    Q_OBJECT
public:
    View3D(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process);
    virtual ~View3D();

    void set_as_dirty();
    void bed_shape_changed();
    void plates_count_changed();

    void select_view(const std::string& direction);

    void select_curr_plate_all();
    void select_object_from_idx(std::vector<int> &object_idxs);
    void remove_curr_plate_all();

    void select_all();
    void deselect_all();
    void exit_gizmo();
    void delete_selected();
    void center_selected();
    void center_selected_plate(const int plate_idx);
    void mirror_selection(Axis axis);

    bool is_layers_editing_enabled() const;
    bool is_layers_editing_allowed() const;
    void enable_layers_editing(bool enable);

    bool is_dragging() const;
    bool is_reload_delayed() const;

    void reload_scene(bool refresh_immediately, bool force_full_scene_refresh = false);
    void render();

private:
    bool init(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process);
};

class Preview : public BaseView
{
    Q_OBJECT
    DynamicPrintConfig*       m_config{ nullptr };
    BackgroundSlicingProcess* m_process{ nullptr };
    GCodeProcessorResult*     m_gcode_result{ nullptr };

#ifdef __linux__
    bool m_volumes_cleanup_required{ false };
#endif /* __linux__ */

    std::function<void()> m_schedule_background_process;

    unsigned int m_number_extruders{ 1 };
    bool         m_keep_current_preview_type{ false };

    const Slic3r::PrintBase* m_loaded_print{ nullptr };
    bool m_only_gcode{ false };
    bool m_reload_paint_after_background_process_apply{ false };

public:
    enum class OptionType : unsigned int
    {
        Travel,
        Wipe,
        Retractions,
        Unretractions,
        Seams,
        ToolChanges,
        ColorChanges,
        PausePrints,
        CustomGCodes,
        Shells,
        ToolMarker,
        Legend
    };

    Preview(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process,
        GCodeProcessorResult* gcode_result, std::function<void()> schedule_background_process = []() {});
    virtual ~Preview();

    void update_gcode_result(GCodeProcessorResult* gcode_result);
    void set_as_dirty();
    void bed_shape_changed();
    void select_view(const std::string& direction);
    void set_drop_target(void* target); // stub — drop targets handled differently in Qt

    void load_print(bool keep_z_range = false, bool only_gcode = false);
    void reload_print(bool keep_volumes = false, bool only_gcode = false);
    void refresh_print();
    void load_shells(const Print& print, bool force_previewing = false);
    void reset_shells();

    void msw_rescale();
    void sys_color_changed();

    bool is_loaded() const { return (m_loaded_print != nullptr); }
    void on_tick_changed(CustomGCode::Type type);

    void show_sliders(bool show = true);
    void show_moves_sliders(bool show = true);
    void show_layers_sliders(bool show = true);
    void set_reload_paint_after_background_process_apply(bool flag) { m_reload_paint_after_background_process_apply = flag; }
    bool get_reload_paint_after_background_process_apply() { return m_reload_paint_after_background_process_apply; }

private:
    bool init(QWidget* parent, Bed3D& bed, Model* model);

    void bind_event_handlers();
    void unbind_event_handlers();

    void resizeEvent(QResizeEvent* event) override;
    void update_layers_slider(const std::vector<double>& layers_z, bool keep_z_range = false);
    void update_layers_slider_mode();
    void update_layers_slider_from_canvas(QKeyEvent* event);
    void load_print_as_fff(bool keep_z_range = false, bool only_gcode = false);
};


class AssembleView : public BaseView
{
    Q_OBJECT
public:
    AssembleView(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process);
    ~AssembleView();

    void set_as_dirty();
    void render();

    bool is_reload_delayed() const;
    void reload_scene(bool refresh_immediately, bool force_full_scene_refresh = false);
    void select_view(const std::string& direction);

private:
    bool init(QWidget* parent, Bed3D& bed, Model* model, DynamicPrintConfig* config, BackgroundSlicingProcess* process);
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_Preview_hpp_

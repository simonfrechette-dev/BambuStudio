#pragma once
#include <QWidget>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QString>

#include "GUI_Utils.hpp"
#include "Widgets/ProgressDialog.hpp"
#include "libslic3r/TexturePainting.hpp"

#include "Widgets/PopupWindow.hpp"
#include "Widgets/SpinInput.hpp"
#include "Widgets/Button.hpp"

#include <array>
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

class GreenSlider;

namespace Slic3r { namespace GUI {


enum class TextureImportState {
    Idle,
    Computing,
    Ready,
    Error
};

struct FilamentMappingRow {
    int                        cluster_id    = -1;
    std::array<std::size_t, 3> source_color  = {0, 0, 0};
    std::string                source_hex;
    int                        target_filament_idx = 0;
    QWidget*                   source_panel  = nullptr;
    QWidget*                   target_panel  = nullptr;
};

class FilamentSelectPopup;
// Lightweight 3D preview panel using QOpenGLWidget.
// Renders: original textured, multi-color, or filament-mapped.
class TexturePreviewCanvas : public QOpenGLWidget
{
public:
    enum class RenderMode { Original, MultiColor, FilamentMap };

    TexturePreviewCanvas(QWidget* parent, const QSurfaceFormat& attrs);
    ~TexturePreviewCanvas();

    void set_mesh_data(
        const std::vector<std::array<float, 3>>& vertices,
        const std::vector<std::array<int, 3>>&   indices);

    void set_texture_data(
        const std::vector<std::array<float, 2>>& uvs,
        const unsigned char* tex_data, int tex_w, int tex_h, int tex_channels);

    void set_texture_render_data(
        const std::vector<std::vector<unsigned char>>& tex_pixels_rgb,
        const std::vector<int>& tex_widths,
        const std::vector<int>& tex_heights,
        const std::vector<std::array<std::array<float,2>, 3>>& face_uvs,
        const std::vector<int>& face_tex_ids);

    void set_painted_mesh_data(
        const std::vector<std::array<float, 3>>& vertices,
        const std::vector<std::array<int, 3>>&   indices);
    void set_face_colors(const std::vector<std::array<std::size_t, 3>>& face_colors);
    void set_original_face_colors(const std::vector<std::array<std::size_t, 3>>& face_colors);
    void set_filament_color_map(const std::map<std::array<std::size_t, 3>, std::array<float, 3>>& color_map);

    void set_render_mode(RenderMode mode);
    RenderMode get_render_mode() const { return m_mode; }
    void set_computing_overlay(bool show);
    void reset_view();

private:
    void on_paint(QPaintEvent& evt);
    void on_size(QResizeEvent& evt);
    void on_mouse(QMouseEvent& evt);
    void ensure_gl_ready();
    void render();
    void render_mesh();
    void render_textured_original();
    void render_reset_overlay(const QSize& logical_size, const QSize& viewport_size);
    void upload_reset_icon_textures();
    unsigned int upload_reset_icon_texture(const std::string& icon_name);
    QRect reset_overlay_rect() const;
    bool handle_reset_overlay_mouse(QMouseEvent& evt);
    void upload_textures();
    void compute_smooth_normals();
    void update_bounding_box();

    QOpenGLContext*  m_context        = nullptr;
    bool          m_gl_initialized = false;
    RenderMode    m_mode           = RenderMode::Original;

    float   m_zoom     = 1.0f;
    float   m_rot_x    = -30.0f;
    float   m_rot_y    = 30.0f;
    float   m_pan_x    = 0.0f;
    float   m_pan_y    = 0.0f;
    QPoint m_last_mouse_pos;
    enum class DragMode { None, Rotate, Pan };
    DragMode m_drag_mode = DragMode::None;

    std::vector<std::array<float, 3>> m_vertices;
    std::vector<std::array<int, 3>>   m_indices;
    std::vector<std::array<float, 2>> m_uvs;
    std::vector<std::array<float, 3>> m_painted_vertices;
    std::vector<std::array<int, 3>>   m_painted_indices;
    std::vector<std::array<float, 3>> m_face_colors_rgb;
    std::vector<std::array<float, 3>> m_original_face_colors_rgb;
    std::vector<std::array<float, 3>> m_filament_colors_rgb;
    std::map<std::array<std::size_t, 3>, std::array<float, 3>> m_color_map;

    unsigned int m_tex_id       = 0;
    int          m_tex_w        = 0;
    int          m_tex_h        = 0;
    int          m_tex_channels = 3;
    bool         m_tex_dirty    = false;
    std::vector<unsigned char> m_tex_data;

    std::vector<unsigned int> m_gl_tex_ids;
    std::vector<std::vector<unsigned char>> m_tex_pixels_rgb;
    std::vector<int> m_tex_widths;
    std::vector<int> m_tex_heights;
    std::vector<std::array<std::array<float,2>, 3>> m_face_uvs;
    std::vector<int> m_face_tex_ids;
    bool m_multi_tex_dirty = false;

    std::vector<std::array<float, 3>> m_vertex_normals;

    std::array<float, 3> m_center = {0, 0, 0};
    float                m_radius = 1.0f;

    unsigned int m_reset_icon_tex        = 0;
    unsigned int m_reset_icon_hover_tex  = 0;
    unsigned int m_reset_icon_dark_tex   = 0;
    unsigned int m_reset_icon_dark_hover_tex = 0;
    bool         m_reset_overlay_hovered = false;
    bool         m_reset_overlay_pressed = false;
};


class TextureImportDialog : public DPIDialog
{
public:
    TextureImportDialog(QWidget*                        parent,
                        const Slic3r::TexturedMesh&      textured_mesh,
                        const std::vector<std::string>&  filament_color_strs,
                        const std::vector<std::string>&  filament_names,
                        std::function<bool()>            initial_cancel_callback = {},
                        std::function<bool(int)>         initial_progress_callback = {});
    ~TextureImportDialog();

    int exec() override;
    void on_dpi_changed(const QRect& suggested_rect) override;

    Slic3r::PaintedMesh               get_painted_mesh() const;
    std::vector<Slic3r::FilamentMatch> get_matches() const;
    bool                               was_skipped() const { return m_skipped; }
    bool                               fallback_to_geometry_only() const { return m_fallback_to_geometry_only; }
    // Colors of virtual filaments that need to be created after dialog confirmation.
    // Index i corresponds to filament index (m_existing_filament_count + i).
    const std::vector<std::array<float, 4>>& get_new_filament_colors() const { return m_new_filament_colors; }
    const std::vector<std::string>& get_new_filament_preset_names() const { return m_new_filament_preset_names; }
    size_t get_existing_filament_count() const { return m_existing_filament_count; }

private:
    void build_ui();
    void build_preview_panel(QWidget* parent, QLayout* sizer);
    void build_params_panel(QWidget* parent, QLayout* sizer);
    void build_mapping_panel(QWidget* parent, QLayout* sizer);
    void build_bottom_buttons(QLayout* sizer);

    void set_state(TextureImportState new_state);
    void update_ui_for_state();

    void start_computation(bool auto_color = false, bool initial = false);
    void cancel_computation();
    void on_computation_complete(QEvent& evt);
    void on_computation_progress(QEvent& evt);
    void on_computation_error(QEvent& evt);

    void rebuild_mapping_rows();
    void do_auto_match();
    std::vector<Slic3r::FilamentMatch> build_matches_from_rows() const;
    void update_filament_color_map();
    void show_filament_popup(size_t row_index);
    void dismiss_filament_popup();
    void dismiss_filament_popup_on_wheel(QMouseEvent& evt);
    int  add_virtual_filament(const std::array<float, 4>& rgba, const std::string& hex,
                              const std::string& preset_name = std::string());
    size_t max_filament_count() const;
    bool can_add_virtual_filament() const;
    void show_filament_limit_warning_once();
    int  find_closest_filament_index(const std::array<std::size_t, 3>& color) const;

    void on_color_preset_clicked(QEvent& evt);
    void on_color_slider_changed(QEvent& evt);
    void on_color_spin_changed(QEvent& evt);
    void on_color_spin_text_changed(QEvent& evt);
    void on_smooth_slider_changed(QEvent& evt);
    void on_smooth_spin_changed(QEvent& evt);
    void on_smooth_spin_text_changed(QEvent& evt);
    void on_apply_clicked(QEvent& evt);
    void on_auto_merge_toggled(QEvent& evt);
    void on_view_button_clicked(QEvent& evt);
    void highlight_view_button(int view_index);
    void on_skip_clicked(QEvent& evt);
    void on_ok_clicked(QEvent& evt);

    void set_color_count_value(int value, bool update_spin);
    void set_smooth_value(int value, bool update_spin);
    void preview_spin_text_value(SpinInput* spin, GreenSlider* slider, int& param,
                                 int min_value, int max_value, const QString& text,
                                 std::function<void()> on_value_changed = {});
    void update_color_count_preset_buttons();

    bool has_valid_result() const;
    bool is_params_dirty() const;
    void update_confirm_button_state();

    Slic3r::TexturedMesh               m_textured_mesh;
    std::vector<std::string>           m_filament_color_strs;   // existing + virtual
    std::vector<std::string>           m_filament_names;        // existing + virtual
    std::vector<std::array<float, 4>>  m_filament_colors_rgba;  // existing + virtual
    size_t                             m_existing_filament_count = 0;
    std::vector<std::array<float, 4>>  m_new_filament_colors;   // only virtual (to be created)
    std::vector<std::string>           m_new_filament_preset_names; // only virtual, aligned with m_new_filament_colors
    std::string                        m_default_virtual_filament_preset_name;

    TextureImportState                 m_state = TextureImportState::Idle;
    bool                               m_skipped = false;
    bool                               m_fallback_to_geometry_only = false;
    bool                               m_filament_limit_warning_shown = false;

    Slic3r::PaintedMesh               m_painted;
    std::vector<Slic3r::FilamentMatch> m_current_matches;

    std::unique_ptr<std::thread>       m_worker;
    std::atomic<bool>                  m_cancel_flag{false};
    std::mutex                         m_result_mutex;
    Slic3r::PaintedMesh               m_pending_result;
    std::function<bool()>              m_initial_cancel_callback;
    std::function<bool(int)>           m_initial_progress_callback;
    bool                               m_current_computation_initial = false;
    bool                               m_initial_computation_pending = false;
    bool                               m_initial_computation_cancelled = false;
    bool                               m_initial_computation_failed = false;

    Button*      m_btn_color_4    = nullptr;
    Button*      m_btn_color_8    = nullptr;
    Button*      m_btn_color_16   = nullptr;
    Button*      m_btn_color_auto = nullptr;
    GreenSlider* m_color_slider   = nullptr;
    SpinInput*   m_color_spin     = nullptr;
    GreenSlider* m_smooth_slider  = nullptr;
    SpinInput*   m_smooth_spin    = nullptr;
    Button*      m_btn_apply      = nullptr;

    QCheckBox*           m_auto_merge_cb = nullptr;
    QScrollArea*     m_mapping_scroll = nullptr;
    QBoxLayout*           m_mapping_sizer  = nullptr;
    std::vector<FilamentMappingRow> m_mapping_rows;
    FilamentSelectPopup*  m_filament_popup = nullptr;
    int                   m_filament_popup_row = -1;
    int                   m_skip_next_filament_popup_row = -1;

    TexturePreviewCanvas* m_preview_canvas      = nullptr;
    QWidget*              m_tab_panel           = nullptr;
    Button*               m_btn_view_original   = nullptr;
    Button*               m_btn_view_multicolor = nullptr;
    Button*               m_btn_view_filament   = nullptr;

    ProgressDialog* m_progress_dlg = nullptr;

    Button*       m_btn_skip = nullptr;
    Button*       m_btn_ok   = nullptr;

    int   m_param_color_count = 4;
    int   m_param_smooth      = 5;

    int   m_applied_color_count = -1;
    int   m_applied_smooth      = -1;
    QLabel* m_hint_label  = nullptr;

    static const int ID_COLOR_4     = 50000 + 200;
    static const int ID_COLOR_8     = 50000 + 201;
    static const int ID_COLOR_16    = 50000 + 202;
    static const int ID_COLOR_AUTO  = 50000 + 203;
    static const int ID_BTN_APPLY   = 50000 + 204;
    static const int ID_BTN_SKIP    = 50000 + 205;
    static const int ID_VIEW_ORIGINAL   = 50000 + 206;
    static const int ID_VIEW_MULTICOLOR = 50000 + 207;
    static const int ID_VIEW_FILAMENT   = 50000 + 208;

    };

}} // namespace Slic3r::GUI

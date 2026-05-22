#ifndef _OBJ_COLOR_DIALOG_H_
#define _OBJ_COLOR_DIALOG_H_
#include <QWidget>
#include <QString>

#include "GUI_Utils.hpp"
#include "Camera.hpp"
#include "GuiColor.hpp"
#include "libslic3r/Format/OBJ.hpp"
#include "Widgets/SpinInput.hpp"
class Button;
class Label;
class ComboBox;

class ObjColorPanel : public QWidget
{
public:
    // BBS
    ObjColorPanel(QWidget *parent, Slic3r::ObjDialogInOut &in_out, const std::vector<std::string> &extruder_colours);
    ~ObjColorPanel();
    QBoxLayout *create_sizer_thumbnail(QPushButton *image_button, bool left);
    void msw_rescale();
    bool is_ok();
    void send_new_filament_to_ui();
    void cancel_paint_color();
    void update_filament_ids();
    Slic3r::ObjDialogInOut::FormatType get_input_type() const { return m_obj_in_out.input_type; }
    struct ButtonState
    {
        ComboBox*   bitmap_combox{nullptr};
        bool      is_map{false};//int id{0};
    };
    typedef std::function<void()> LayoutChanggeCallback;
    void set_layout_callback(LayoutChanggeCallback);
    void do_layout_callback();
    bool do_show(bool show);
    void clear_instance_and_revert_offset();

private:
    QBoxLayout *create_approximate_match_btn_sizer(QWidget *parent);
    QBoxLayout *create_add_btn_sizer(QWidget *parent);
    QBoxLayout *create_reset_btn_sizer(QWidget *parent);
    QBoxLayout *create_extruder_icon_and_rgba_sizer(QWidget *parent, int id, const QColor& color);
    std::string get_color_str(const QColor &color);
    QBoxLayout *create_color_icon_map_rgba_sizer(QWidget *parent, int id, const QColor &color);//for display map
    ComboBox* CreateEditorCtrl(QWidget *parent,int id);
    void draw_new_table();
    void update_new_add_final_colors();
    void show_sizer(QLayout *sizer, bool show);
    void deal_approximate_match_btn();
    bool deal_add_btn();
    void deal_reset_btn();
    void deal_algo(char cluster_number,bool redraw_ui =false);
    void deal_default_strategy();
    void deal_thumbnail();
    void generate_thumbnail();
    void generate_origin_thumbnail();
    void set_view_angle_type(int);
private:
    //view ui
    Slic3r::ObjDialogInOut &  m_obj_in_out;
    Slic3r::GUI::Camera::ViewAngleType m_camera_view_angle_type{Slic3r::GUI::Camera::ViewAngleType::Iso};
    StaticBox *               m_two_image_panel{nullptr};
    QBoxLayout *                       m_left_sizer_thumbnail{nullptr};
    QBoxLayout *                       m_right_sizer_thumbnail{nullptr};
    QPushButton *                         m_left_image_button{nullptr};
    QPushButton *                         m_right_image_button{nullptr};
    QBoxLayout *                       m_two_image_panel_sizer{nullptr};

    QWidget *                 m_page_simple  = nullptr;
    QBoxLayout *              m_sizer        = nullptr;
    QBoxLayout *              m_sizer_simple = nullptr;
    QBoxLayout *               m_sizer_current_filaments = nullptr;
    SpinInput *                m_color_cluster_num_by_user_ebox{nullptr};
    Label*                     m_warn_text{nullptr};
    Label*                     m_note_text{nullptr};
    QGridLayout *              m_new_grid_sizer{nullptr};
    QScrollArea *         m_scrolledWindow{nullptr};
    Button *    m_quick_approximate_match_btn{nullptr};
    Button *    m_quick_add_btn{nullptr};
    Button *    m_quick_reset_btn{nullptr};
    std::vector<QPushButton*> m_extruder_icon_list;
    std::vector<QPushButton*> m_color_cluster_icon_list;//need modeify
    std::vector<QLabel*> m_color_cluster_text_list;//need modeify
    std::vector<QGridLayout*> m_row_sizer_list;         // control show or not
    std::vector<QBoxLayout *> m_row_col_boxsizer_list;
    std::vector<ButtonState*> m_result_icon_list;
    int                       m_last_cluster_num{-1};
    const int               m_combox_width{50};
    int                     m_combox_icon_width;
    int                     m_combox_icon_height;
    QPushButton *              m_image_button = nullptr;
    LayoutChanggeCallback   m_layout_callback;
    //data
    char                       m_last_cluster_number{-2};
    std::vector<Slic3r::RGBA>& m_input_colors;
    int m_color_num_recommend{0};
    int m_color_cluster_num_by_algo{0};
    int m_input_colors_size{0};
    std::vector<QColor> m_colours;//from project and show right
    std::vector<int>      m_cluster_map_filaments;//show middle
    int                   m_max_filament_index = 0;
    std::vector<QColor> m_cluster_colours;//from_algo and show left
    bool                  m_can_add_filament{true};
    bool                  m_deal_thumbnail_flag{false};
    std::vector<QColor> m_new_add_colors;
    std::vector<QColor> m_new_add_final_colors;
    //algo result
    std::vector<Slic3r::RGBA> m_cluster_colors_from_algo;
    std::vector<int>          m_cluster_labels_from_algo;
    //result
    bool                        m_is_add_filament{false};
    unsigned char&             m_first_extruder_id;
    std::vector<unsigned char> &m_filament_ids;

    Slic3r::Vec3d m_thumbnail_offset;

    const int LEFT_THUMBNAIL_SIZE_WIDTH  = 100;
    const int RIGHT_THUMBNAIL_SIZE_WIDTH = 300;
};

class ObjColorDialog : public Slic3r::GUI::DPIDialog
{
public:
    ObjColorDialog(QWidget *parent, Slic3r::ObjDialogInOut &in_out, const std::vector<std::string> &extruder_colours);
    QBoxLayout *create_btn_sizer(long flags, bool exist_error);
    void on_dpi_changed(const QRect &suggested_rect) override;
    void update_layout();
    void setVisible(bool show) override;

private:
    ObjColorPanel*  m_panel_ObjColor  = nullptr;
    QBoxLayout *                      m_main_sizer     = nullptr;
    QBoxLayout *                      m_buttons_sizer   = nullptr;
    std::unordered_map<int, Button *> m_button_list;
    std::vector<unsigned char>&      m_filament_ids;
    unsigned char &                  m_first_extruder_id;
};

#endif  // _WIPE_TOWER_DIALOG_H_
#ifndef slic3r_OG_CustomCtrl_hpp_
#define slic3r_OG_CustomCtrl_hpp_


#include <map>
#include <QWidget>
#include <QFont>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QPainter>
#include <QPixmap>
#include <functional>

#include "libslic3r/Config.hpp"
#include "libslic3r/PrintConfig.hpp"

#include "OptionsGroup.hpp"
#include "I18N.hpp"

// Translate the ifdef 

namespace Slic3r { namespace GUI {

//  Static text shown among the options.
class OG_CustomCtrl :public QWidget
{
    QFont  m_font;
    int     m_v_gap;
    int     m_v_gap2;
    int     m_h_gap;
    int     m_em_unit;

    QSize  m_bmp_mode_sz;
    QSize  m_bmp_blinking_sz;

    int     m_max_win_width{0};

    struct CtrlLine {
        int           width{ -1 };
        int           height{ -1 };
        OG_CustomCtrl*    ctrl    { nullptr };
        const Line&       og_line;

        bool draw_just_act_buttons  { false };
        bool draw_mode_bitmap       { true };
        bool is_visible             { true };
        bool is_focused             { false };

        CtrlLine(   int         height,
                    OG_CustomCtrl*  ctrl,
                    const Line&     og_line,
                    bool            draw_just_act_buttons = false,
                    bool            draw_mode_bitmap = true);
        ~CtrlLine() { ctrl = nullptr; }

        int     get_max_win_width();
        void    correct_items_positions();
        void    msw_rescale();
        void    update_visibility(ConfigOptionMode mode);

        void render_separator(QPainter& dc, int v_pos);

        void    render(QPainter& dc, int h_pos, int v_pos);
        int draw_text      (QPainter& dc, QPoint pos, const QString& text, const QColor* color, int width, bool is_url = false, bool is_main = false);
        QPoint draw_blinking_bmp(QPainter& dc, QPoint pos, bool is_blinking);
        int draw_act_bmps(QPainter& dc, QPoint pos, const QPixmap& bmp_undo_to_sys, const QPixmap& bmp_undo, bool is_blinking, size_t rect_id = 0, bool skip_vertical_adjust = false);
        bool    launch_browser() const;
        bool    is_separator() const { return og_line.is_separator(); }
        void    ensure_rects_size(size_t size);
        void    update_multi_variant_height();

        std::vector<QRect> rects_undo_icon;
        std::vector<QRect> rects_undo_to_sys_icon;
        QRect              rect_label;
    };

    std::vector<CtrlLine> ctrl_lines;

public:
    OG_CustomCtrl(  QWidget* parent,
                    OptionsGroup* og,
                    const QPoint& pos = QPoint(),
                    const QSize& size = QSize(),
                    const QValidator* val = nullptr,
                    const QString& name = QString());
    ~OG_CustomCtrl() {}

    void    OnPaint(QPaintEvent&);
    void    OnMotion(QMouseEvent& event);
    void    OnLeftDown(QMouseEvent& event);
    void    OnLeaveWin(QMouseEvent& event);

    void    init_ctrl_lines();
    bool    update_visibility(ConfigOptionMode mode);
    void    correct_window_position(QWidget* win, const Line& line, Field* field = nullptr);
    void    correct_widgets_position(QLayout* widget, const Line& line, Field* field = nullptr);
    void    init_max_win_width();
    void    set_max_win_width(int max_win_width);
    int     get_max_win_width() { return m_max_win_width; }

    //BBS
    int    get_title_width();
    // BBS
    void fixup_items_positions();

    void    msw_rescale();
    void    sys_color_changed();

    QPoint get_pos(const Line& line, Field* field = nullptr);
    int     get_height(const Line& line);

    void    update_line_height_for_field(const t_config_option_key &opt_id);
    void    recalculate_and_refresh();

    OptionsGroup*  opt_group;

};

}}

#endif /* slic3r_OG_CustomCtrl_hpp_ */

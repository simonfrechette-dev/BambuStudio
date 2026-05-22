#ifndef _BaseTransparentDPIFrame_H_
#define _BaseTransparentDPIFrame_H_

#include <future>
#include <thread>
#include "GUI_App.hpp"
#include "GUI_Utils.hpp"

class Button;
class CheckBox;
namespace Slic3r { namespace GUI {
class CapsuleButton;

class BaseTransparentDPIFrame : public Slic3r::GUI::DPIFrame
{
public:
    enum DisappearanceMode {
        None,
        TimedDisappearance // unit: second
    };

    BaseTransparentDPIFrame(QWidget *        parent,
                            int               win_width,
                            QPoint           dialog_pos,
                            int               ok_button_width,
                            QString          win_text,
                            QString          ok_text,
                            QString          cancel_text        = "",
                            DisappearanceMode disappearance_mode = DisappearanceMode::None,
                            std::string       icon_name          = "completed");
    ~BaseTransparentDPIFrame() override;
    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_show();
    void on_hide();
    void clear_timer_count();
    void setVisible(bool show) override;
    void         on_full_screen(QEvent &);
    virtual void deal_ok();
    virtual void deal_cancel();
    virtual void on_timer(QTimerEvent &event);
    void         set_target_pos_and_gradual_disappearance(QPoint pos);
    void         call_start_gradual_disappearance();
    void         restart();

protected:
    Button *m_button_ok     = nullptr;
    Button *m_button_cancel = nullptr;
    Label  *m_finish_text   = nullptr;
    DisappearanceMode m_timed_disappearance_mode;
    float             m_timer_count = 0;
    QTimer *         m_refresh_timer{nullptr};
    int               m_disappearance_second  = 2500; //ANIMATION_REFRESH_INTERVAL 20  unit ms: m_disappearance_second * ANIMATION_REFRESH_INTERVAL
    bool              m_move_to_target_gradual_disappearance = false;
    QPoint           m_target_pos;

private:
    QBoxLayout *m_sizer_main{nullptr};
    QSize      m_max_size;
    QSize     m_step_size;
    QPoint    m_step_pos;
    QPoint    m_start_pos;
    float      m_time_move{6.0f};
    float      m_time_gradual_and_scale{100.0f};
    int        m_init_transparent{220};
    int        m_step_transparent;
    int        m_display_stage = 0;//0 normal //1 gradual
    bool       m_enter_window_valid{true};

private:
    void start_gradual_disappearance();
    void init_timer();
    void calc_step_transparent();
    void on_close();
    void show_sizer(QLayout *sizer, bool show);
    void hide_all();
    void begin_gradual_disappearance();
    void begin_move_to_target_and_gradual_disappearance();
};
}}     // namespace Slic3r::GUI
#endif  // _STEP_MESH_DIALOG_H_

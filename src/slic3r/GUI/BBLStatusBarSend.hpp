#ifndef BBLSTATUSBARSEND_HPP
#define BBLSTATUSBARSEND_HPP

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QBoxLayout>
#include <QFont>
#include <QPixmap>

#include <memory>
#include <string>
#include <functional>

#include "Jobs/ProgressIndicator.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"

namespace Slic3r {

class BBLStatusBarSend : public ProgressIndicator
{
    QWidget *     m_self;
    QProgressBar* m_prog;
    Label *       m_link_show_error;
    QBoxLayout*   m_sizer_status_text;
    QLabel*       m_static_bitmap_show_error;
    QPixmap       m_bitmap_show_error_close;
    QPixmap       m_bitmap_show_error_open;
    Button *      m_cancelbutton;
    QLabel *      m_status_text;
    QLabel *      m_stext_percent;
    QBoxLayout *  m_sizer;
    QBoxLayout *  m_sizer_eline;
    QWidget *     block_left;
    QWidget *     block_right;

public:
    BBLStatusBarSend(QWidget *parent = nullptr, int id = -1);
    ~BBLStatusBarSend() = default;

    int get_progress() const;
    void        set_prog_block();
    void        set_progress(int) override;
    int         get_range() const override;
    void        set_range(int = 100) override;
    void        clear_percent() override;
    void        show_error_info(const QString& msg, int code, const QString& description, const QString& extra) override;
    void        show_progress(bool);
    void        start_busy(int = 100);
    void        stop_busy();
    void        set_cancel_callback_fina(BBLStatusBarSend::CancelFn ccb);
    inline bool is_busy() const { return m_busy; }
    void        set_cancel_callback(CancelFn = CancelFn()) override;
    inline void reset_cancel_callback() { set_cancel_callback(); }
    QWidget *   get_panel();
    bool        is_english_text(QString str);
    bool        format_text(QLabel* dc, int width, const QString& text, QString& multiline_text);
    void        set_status_text(const QString& txt);
    void        set_percent_text(const QString &txt);
    void        msw_rescale();
    void        set_status_text(const std::string &txt);
    void        set_status_text(const char *txt) override;
    QString     get_status_text() const;
    void        set_font(const QFont &font);
    void        set_object_info(const QString &txt);
    void        set_slice_info(const QString &txt);
    void        show_slice_info(bool show);
    bool        is_slice_info_shown();
    bool        update_status(QString &msg, bool &was_cancel, int percent = -1, bool yield = true);
    void        reset();
    void show_cancel_button();
    void hide_cancel_button();
    void change_button_label(QString name);

    void disable_cancel_button();
    void enable_cancel_button();

    void    cancel();

private:
    bool     m_show_error_info_state = false;
    bool     m_busy = false;
    bool     m_was_cancelled = false;
    CancelFn m_cancel_cb;
    CancelFn m_cancel_cb_fina;
};

namespace GUI {
using Slic3r::BBLStatusBarSend;
}

inline const QEvent::Type EVT_SHOW_ERROR_INFO_SEND = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type EVT_SHOW_ERROR_FAIL_SEND = static_cast<QEvent::Type>(QEvent::registerEventType());

} // namespace Slic3r

#endif // BBLSTATUSBAR_HPP

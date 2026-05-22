#ifndef BBLStatusBarBind_HPP
#define BBLStatusBarBind_HPP
#include <QWidget>
#include <QString>



#include <memory>
#include <string>
#include <functional>
#include <string>
#include "Jobs/ProgressIndicator.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"

class QTimer;
class QProgressBar;
class QPushButton;
class QTimerEvent;
class QStatusBar;
class QWidget;
class QMainWindow;
class QString;
class QFont;


namespace Slic3r {
class BBLStatusBarBind : public ProgressIndicator
{
    QWidget *     m_self; // we cheat! It should be the base class but: perl!
    QProgressBar *     m_prog;
    Button *      m_cancelbutton;
    QLabel *m_status_text;
    QLabel *m_stext_percent;
    QBoxLayout *  m_sizer;
    QBoxLayout *  m_sizer_eline;

public:
    BBLStatusBarBind(QWidget *parent = nullptr, int id = -1);
    ~BBLStatusBarBind() = default;

    int get_progress() const;
    // if the argument is less than 0 it shows the last state or
    // pulses if no state was set before.
    void        set_prog_block();
    void        set_progress(int) override;
    int         get_range() const override;
    void        set_range(int = 100) override;
    void        clear_percent() override;
    void        show_error_info(QString msg, int code, QString description, QString extra);
    void        show_progress(bool);
    void        start_busy(int = 100);
    void        stop_busy();
    void        set_cancel_callback_fina(BBLStatusBarBind::CancelFn ccb);
    inline bool is_busy() const { return m_busy; }
    void        set_cancel_callback(CancelFn = CancelFn()) override;
    inline void reset_cancel_callback() { set_cancel_callback(); }
    QWidget *   get_panel();
    void        set_status_text(const QString &txt);
    void        set_percent_text(const QString &txt);
    void        msw_rescale();
    void        set_status_text(const std::string &txt);
    void        set_status_text(const char *txt) override;
    QString    get_status_text() const;
    void        set_font(const QFont &font);
    void        set_object_info(const QString &txt);
    void        set_slice_info(const QString &txt);
    void        show_slice_info(bool show);
    bool        is_slice_info_shown();
    bool        update_status(QString &msg, bool &was_cancel, int percent = -1, bool yield = true);
    void        reset();

    // Temporary methods to satisfy Perl side
    void show_cancel_button();
    void hide_cancel_button();

private:
    bool     m_busy = false;
    bool     m_was_cancelled = false;
    CancelFn m_cancel_cb;
    CancelFn m_cancel_cb_fina;
};

namespace GUI {
using Slic3r::BBLStatusBarBind;
}

} // namespace Slic3r

#endif // BBLSTATUSBAR_HPP

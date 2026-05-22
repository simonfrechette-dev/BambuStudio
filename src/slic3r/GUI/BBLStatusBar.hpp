#ifndef BBLSTATUSBAR_HPP
#define BBLSTATUSBAR_HPP

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QFont>

#include <memory>
#include <string>
#include <functional>

#include "Jobs/ProgressIndicator.hpp"

namespace Slic3r {

class BBLStatusBar : public ProgressIndicator
{
    QWidget*      m_self;
    QProgressBar* m_prog;
    QPushButton*  m_cancelbutton;
    QLabel*       m_status_text;
    QLabel*       m_object_info;
    QLabel*       m_slice_info;
    QBoxLayout*   m_slice_info_sizer;
    QBoxLayout*   m_object_info_sizer;
    QBoxLayout*   m_sizer;
public:
    BBLStatusBar(QWidget *parent = nullptr, int id = -1);
    ~BBLStatusBar() = default;

    int         get_progress() const;
    void        set_progress(int) override;
    int         get_range() const override;
    void        set_range(int = 100) override;
    void        clear_percent() override;
    void        show_error_info(const QString& msg, int code, const QString& description, const QString& extra) override;
    void        show_progress(bool);
    void        start_busy(int = 100);
    void        stop_busy();
    inline bool is_busy() const { return m_busy; }
    void        set_cancel_callback(CancelFn = CancelFn()) override;
    inline void reset_cancel_callback() { set_cancel_callback(); }
    QWidget*    get_panel();
    void        set_status_text(const QString& txt);
    void        set_status_text(const std::string& txt);
    void        set_status_text(const char *txt) override;
    QString     get_status_text() const;
    void        set_font(const QFont &font);
    void        set_object_info(const QString& txt);
    void        set_slice_info(const QString& txt);
    void        show_slice_info(bool show);
    bool        is_slice_info_shown();

    void        show_cancel_button();
    void        hide_cancel_button();

private:
    bool m_busy = false;
    CancelFn m_cancel_cb;
};

namespace GUI {
    using Slic3r::BBLStatusBar;
}

}

#endif // BBLSTATUSBAR_HPP

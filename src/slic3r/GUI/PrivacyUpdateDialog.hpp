#ifndef slic3r_GUI_PrivacyUpdateDialog_hpp_
#define slic3r_GUI_PrivacyUpdateDialog_hpp_
#include <QWidget>
#include <QString>

#include "GUI_Utils.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/WebView.hpp"

namespace Slic3r { namespace GUI {


class PrivacyUpdateDialog : public DPIDialog
{
public:
    enum ButtonStyle {
        ONLY_CONFIRM = 0,
        CONFIRM_AND_CANCEL = 1,
        MAX_STYLE_NUM = 2
    };
    PrivacyUpdateDialog(
        QWidget* parent,
        int      id = -1,
        const QString& title = QString(),
        enum ButtonStyle btn_style = CONFIRM_AND_CANCEL,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0
    );
    QWidget* CreateTipView(QWidget* parent);
    void OnNavigating(QEvent& event);
    bool ShowReleaseNote(std::string content);
    void RunScript(std::string script);
    void set_text(std::string str) { m_mkdown_text = str; };
    void on_show();
    void on_hide();
    void update_btn_label(QString ok_btn_text, QString cancel_btn_text);
    void rescale();
    ~PrivacyUpdateDialog();
    void on_dpi_changed(const QRect& suggested_rect);

    QBoxLayout* m_sizer_main;
    QWidget* m_vebview_release_note{ nullptr };
    Label* m_staticText_release_note{ nullptr };
    Button* m_button_ok;
    Button* m_button_cancel;
    std::string m_mkdown_text;
    std::string m_host_url;
};

}} // namespace Slic3r::GUI

#endif
#ifndef slic3r_params_panel_hpp_
#define slic3r_params_panel_hpp_
// Qt6 port of ParamsPanel.hpp
// Original wx version backed up to ParamsPanel.hpp.wx-backup

#include <map>
#include <vector>
#include <memory>

#include <QWidget>
#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QFrame>

#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/Button.hpp"

class SwitchButton;
class StaticBox;

namespace Slic3r {
namespace GUI {

///////////////////////////////////////////////////////////////////////////

class TipsDialog : public DPIDialog
{
    Q_OBJECT
private:
    bool m_show_again{false};
    std::string m_app_key;

public:
    TipsDialog(QWidget *parent, const QString &title, const QString &description,
               std::string app_key = "", int buttons = 1,
               std::map<int, QString> option_map = {});
    Button  *m_confirm{nullptr};
    Button  *m_cancel{nullptr};
    QWidget *m_top_line{nullptr};
    QLabel  *m_msg{nullptr};

protected:
    void on_dpi_changed(const QRect &suggested_rect) override {}
    QBoxLayout *create_item_checkbox(const QString &title, QWidget *parent,
                                     const QString &tooltip, const std::string &param);
    Button *add_button(int btn_id, const QString &label, bool set_focus = false);
};

///////////////////////////////////////////////////////////////////////////////
/// Class ParamsPanel
///////////////////////////////////////////////////////////////////////////////
class ParamsPanel : public QWidget
{
    Q_OBJECT

private:
    void free_sizers();
    void delete_subwindows();
    void refresh_tabs();

protected:
    QBoxLayout*     m_top_sizer{nullptr};
    QBoxLayout*     m_left_sizer{nullptr};
    QBoxLayout*     m_mode_sizer{nullptr};
    StaticBox*      m_top_panel{nullptr};
    ScalableButton* m_process_icon{nullptr};
    QLabel*         m_title_label{nullptr};
    SwitchButton*   m_mode_region{nullptr};
    ScalableButton* m_tips_arrow{nullptr};
    bool            m_tips_arror_blink{false};
    QLabel*         m_title_view{nullptr};
    SwitchButton*   m_mode_view{nullptr};
    QFrame*         m_staticline_print{nullptr};
    QWidget*        m_tab_print{nullptr};
    QWidget*        m_tab_print_plate{nullptr};
    QWidget*        m_tab_print_object{nullptr};
    QFrame*         m_staticline_print_object{nullptr};
    QWidget*        m_tab_print_part{nullptr};
    QWidget*        m_tab_print_layer{nullptr};
    QFrame*         m_staticline_print_part{nullptr};
    QFrame*         m_staticline_filament{nullptr};
    QWidget*        m_tab_filament{nullptr};
    QFrame*         m_staticline_printer{nullptr};
    QWidget*        m_tab_printer{nullptr};
    QBoxLayout*     m_button_sizer{nullptr};
    QWidget*        m_export_to_file{nullptr};
    QWidget*        m_import_from_file{nullptr};
    QScrollArea*    m_page_view{nullptr};
    QBoxLayout*     m_page_sizer{nullptr};

    ScalableButton* m_setting_btn{nullptr};
    ScalableButton* m_search_btn{nullptr};
    ScalableButton* m_compare_btn{nullptr};

    QPixmap m_toggle_on_icon;
    QPixmap m_toggle_off_icon;

    QWidget* m_current_tab{nullptr};

    bool m_has_object_config{false};

    struct Highlighter
    {
        void set_timer_owner(QObject *owner, int timerid = -1);
        void init(std::pair<QWidget *, bool *>, QWidget *parent = nullptr);
        void blink();
        void invalidate();

    private:
        QWidget *m_bitmap{nullptr};
        bool    *m_show_blink_ptr{nullptr};
        int      m_blink_counter{0};
        QTimer   m_timer;
        QWidget *m_parent{nullptr};
    } m_highlighter;

    void OnToggled(bool checked);

public:
    ParamsPanel(QWidget *parent, int id = 0,
                QPoint pos = QPoint(), QSize size = QSize(1800, 1080),
                Qt::WindowFlags style = Qt::Widget,
                const QString &type = {});
    ~ParamsPanel();

    void rebuild_panels();
    void create_layout();
    void clear_page();
    void OnActivate();
    void set_active_tab(QWidget *tab);
    bool is_active_and_shown_tab(QWidget *tab);
    void update_mode();
    void msw_rescale();
    void switch_to_global();
    void switch_to_object(bool with_tips = false);

    void notify_object_config_changed();
    void switch_to_object_if_has_object_configs();

    StaticBox*   get_top_panel()    { return m_top_panel; }
    QWidget*     filament_panel()   { return m_tab_filament; }
    QScrollArea* get_paged_view()   { return m_page_view; }
    QWidget*     get_current_tab()  { return m_current_tab; }
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_params_panel_hpp_

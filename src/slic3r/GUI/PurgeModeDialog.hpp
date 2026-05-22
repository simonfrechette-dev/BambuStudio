#ifndef slic3r_GUI_PurgeModeDialog_hpp_
#define slic3r_GUI_PurgeModeDialog_hpp_
#include <QWidget>
#include <QString>


#include "GUI_Utils.hpp"
#include "libslic3r/PrintConfig.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"

class QLabel;
class QBoxLayout;

namespace Slic3r {

namespace GUI {

class PurgeModeBtnPanel : public QWidget
{
public:
    PurgeModeBtnPanel(QWidget *parent, const QString &label, const QString &detail, const std::string &icon_path);
    void Select(bool selected);

protected:
    void OnPaint(QPaintEvent &event);

private:
    void OnEnterWindow(QMouseEvent &event);
    void OnLeaveWindow(QMouseEvent &evnet);

    void UpdateStatus();

    QPixmap icon;
    QPixmap check_icon;

    QLabel *m_btn;
    QLabel *m_check_btn;
    QLabel   *m_label;
    Label          *m_detail;
    std::string     m_icon_path;
    bool            m_hover{false};
    bool            m_selected{false};
};

class PurgeModeDialog : public DPIDialog
{
public:
    PurgeModeDialog(QWidget *parent);

    PrimeVolumeMode get_selected_mode() const { return m_selected_mode; }

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

private:
    void     select_option(PrimeVolumeMode mode);
    void     update_panel_selection();

    PurgeModeBtnPanel *m_standard_panel;
    PurgeModeBtnPanel *m_saving_panel;
    PrimeVolumeMode    m_selected_mode;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_PurgeModeDialog_hpp_
#ifndef slic3r_GUI_RecenterDialog_hpp_
#define slic3r_GUI_RecenterDialog_hpp_

#include "GUI_Utils.hpp"
#include "Widgets/Button.hpp"

namespace Slic3r { namespace GUI {
class RecenterDialog : public DPIDialog
{
private:
    QLabel* m_staticText_hint;
    Button* m_button_confirm;
    Button* m_button_close;
    QLabel* m_bitmap_home;
    ScalableBitmap  m_home_bmp;
    QString hint1;
    QString hint2;

    void init_bitmap();
    void OnPaint(QPaintEvent& event);
    void render(QPainter& dc);
    void on_button_confirm(QEvent& event);
    void on_button_close(QEvent& event);
    void on_dpi_changed(const QRect& suggested_rect) override;

public:
    RecenterDialog(QWidget* parent,
        int      id = -1,
        const QString& title = QString(),
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0 | 0);

    ~RecenterDialog();
};
}} // namespace Slic3r::GUI

#endif
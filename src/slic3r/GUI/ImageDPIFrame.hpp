#ifndef _ImageDPIFrame_H_
#define _ImageDPIFrame_H_

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include "GUI_App.hpp"
#include "GUI_Utils.hpp"

namespace Slic3r { namespace GUI {
class ImageDPIFrame : public Slic3r::GUI::DPIFrame
{
    Q_OBJECT
public:
    ImageDPIFrame();
    ~ImageDPIFrame() override;
    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_show();
    void on_hide();
    bool show(bool show = true);

    void set_bitmap(const QPixmap& pix);
    int  get_image_px() { return m_image_px; }

private:
    void init_timer();

private slots:
    void on_timer();

private:
    QLabel *        m_bitmap{nullptr};
    QVBoxLayout *   m_sizer_main{nullptr};
    int             m_image_px{280};
    QTimer *        m_refresh_timer{nullptr};
    int             m_timer_count{0};
};
}}     // namespace Slic3r::GUI
#endif  // _ImageDPIFrame_H_

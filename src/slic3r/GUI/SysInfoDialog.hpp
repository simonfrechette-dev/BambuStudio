#ifndef slic3r_GUI_SysInfoDialog_hpp_
#define slic3r_GUI_SysInfoDialog_hpp_


#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"

namespace Slic3r { 
namespace GUI {

class SysInfoDialog : public DPIDialog
{
    ScalableBitmap  m_logo_bmp;
    QLabel* m_logo;
    QTextBrowser*   m_opengl_info_html;
    QTextBrowser*   m_html;

    QPushButton*       m_btn_copy_to_clipboard;

public:
    SysInfoDialog();

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;
    
private:
    void onCopyToClipboard(QEvent &);
    void onCloseDialog(QEvent &);
};
} // namespace GUI
} // namespace Slic3r

#endif

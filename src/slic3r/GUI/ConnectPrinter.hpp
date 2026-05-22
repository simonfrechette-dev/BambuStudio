#ifndef slic3r_GUI_ConnectPrinter_hpp_
#define slic3r_GUI_ConnectPrinter_hpp_
#include <QWidget>
#include <QString>

#include "GUI.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/TextInput.hpp"
#include "DeviceManager.hpp"

namespace Slic3r { namespace GUI {
class ConnectPrinterDialog : public DPIDialog
{
private:
protected:
    QLabel *  m_staticText_connection_code;
    TextInput *     m_textCtrl_code;
    Button *        m_button_confirm;
    QLabel*   m_staticText_hints;
    QLabel* m_bitmap_diagram;
    QPixmap        m_diagram_bmp;
    QImage         m_diagram_img;

    MachineObject*  m_obj;
    QString        m_input_access_code;
public:
    ConnectPrinterDialog(QWidget *      parent,
                         int      id    = -1,
                         const QString &title = QString(),
                         const QPoint & pos   = QPoint(),
                         const QSize &  size  = QSize(),
                         long            style = 0 | 0);

    ~ConnectPrinterDialog();

    void end_modal(int id);
    void init_bitmap();
    void set_machine_object(MachineObject* obj);
    void on_input_enter(QEvent& evt);
    void on_button_confirm(QEvent &event);
    void on_dpi_changed(const QRect &suggested_rect) override;
};
}} // namespace Slic3r::GUI

#endif
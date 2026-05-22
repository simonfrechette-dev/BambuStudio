#pragma once
#include <QWidget>
#include <QString>

#include <unordered_set>

#include "GUI_Utils.hpp"
#include "Widgets/StateColor.hpp"

class Label;
class Button;

namespace Slic3r {

class MachineObject;//Previous definitions

namespace GUI {

class ImageMessageDialog : public DPIDialog
{
public:
    ImageMessageDialog(QWidget       *parent,
                      int  id = -1,
                      const QString& title = QString(),
                       const QString &message = QString(),
                      const QPoint& pos = QPoint(),
                      const QSize& size = QSize(),
                      long  style = 0 | 0);
    ~ImageMessageDialog();

    void on_dpi_changed(const QRect& suggested_rect);

private:
    std::unordered_set<Button*> m_used_button;

    QLabel* m_error_picture;
    Label* m_error_msg_label{ nullptr };
    Label* m_error_code_label{ nullptr };
    QBoxLayout* m_sizer_main;
    QBoxLayout* m_sizer_button;
    QWidget* m_scroll_area{ nullptr };

    std::map<int, Button*> m_button_list;
    StateColor btn_bg_white;
};
}} // namespace Slic3r::GUI

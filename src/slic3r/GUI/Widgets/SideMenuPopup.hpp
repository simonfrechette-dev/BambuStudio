#ifndef slic3r_GUI_SideMenuPopup_hpp_
#define slic3r_GUI_SideMenuPopup_hpp_

#include "PopupWindow.hpp"
#include "SideButton.hpp"
#include <vector>

class SidePopup : public PopupWindow
{
    Q_OBJECT
public:
    explicit SidePopup(QWidget *parent);
    ~SidePopup() override;

    void Create();
    void Popup(QWidget *anchor = nullptr) override;
    void Dismiss() override;

    void append_button(SideButton *btn);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<SideButton *> btn_list;
};

#endif // !slic3r_GUI_SideMenuPopup_hpp_

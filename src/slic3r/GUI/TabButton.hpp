#ifndef slic3r_GUI_TabButton_hpp_
#define slic3r_GUI_TabButton_hpp_

#include "wxExtensions.hpp"
#include "Widgets/StaticBox.hpp"

class TabButton : public StaticBox
{
    QSize   textSize;
    QSize   minSize;
    QSize   paddingSize;
    ScalableBitmap icon;
    ScalableBitmap newtag_img;

    StateColor   text_color;
    StateColor   border_color;
    bool pressedDown = false;
    bool show_new_tag = false;

public:
    TabButton();

    TabButton(QWidget *parent, QString text, ScalableBitmap &icon, long style = 0, int iconSize = 0);

    bool Create(QWidget *parent, QString text, ScalableBitmap &icon, long style = 0, int iconSize = 0);

    void setText(const QString& label);

    void setMinimumSize(const QSize& size);
    
    void SetPaddingSize(const QSize& size);

    const QSize& GetPaddingSize();
    
    void SetTextColor(StateColor const &color);

    void SetBorderColor(StateColor const &color);

    void SetBGColor(StateColor const &color);

    void SetBitmap(ScalableBitmap &bitmap);

    bool Enable(bool enable = true);

    void Rescale();

    void ShowNewTag(bool tag = false) {show_new_tag = tag; update();};
    bool GetShowNewTag() const { return show_new_tag; };

private:
    void paintEvent(QPaintEvent& evt);

    void render(QPainter& dc);

    void messureSize();

    // some useful events
    void mouseDown(QMouseEvent& event);
    void mouseReleased(QMouseEvent& event);

    void sendButtonEvent();

    
};

#endif // !slic3r_GUI_Button_hpp_

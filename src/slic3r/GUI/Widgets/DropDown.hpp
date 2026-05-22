#ifndef slic3r_GUI_DropDown_hpp_
#define slic3r_GUI_DropDown_hpp_

#include "PopupWindow.hpp"
#include "StateColor.hpp"
#include "StateHandler.hpp"
#include "../QtExtensions.hpp"

#include <QPixmap>
#include <QString>
#include <vector>
#include <chrono>

#define DD_NO_CHECK_ICON    0x0001
#define DD_NO_TEXT          0x0002
#define DD_STYLE_MASK       0x0003

#define DD_ITEM_STYLE_SPLIT_ITEM  0x0001
#define DD_ITEM_STYLE_DISABLED    0x0002
#define DD_ITEM_STYLE_DIMMED      0x0004

class DropDown : public PopupWindow
{
    Q_OBJECT
public:
    struct Item {
        QString  text;
        QString  text_static_tips;
        QPixmap  icon;
        QPixmap  icon_textctrl;
        void    *data  = nullptr;
        QString  group;
        QString  alias;
        QString  tip;
        int      flag  = 0;
        int      style = 0;
    };

    DropDown(std::vector<Item> &items);
    DropDown(QWidget *parent, std::vector<Item> &items, long style = 0);

    void init(QWidget *parent, long style = 0);

    void Invalidate(bool clear = false);

    int     GetSelection() const { return selection; }
    void    SetSelection(int n);
    QString GetValue() const;
    void    SetValue(const QString &value);

    void SetCornerRadius(double radius);
    void SetBorderColor(const StateColor &color);
    void SetSelectorBorderColor(const StateColor &color);
    void SetTextColor(const StateColor &color);
    void SetSelectorBackgroundColor(const StateColor &color);
    void SetUseContentWidth(bool use, bool limit_max = false);
    void SetAlignIcon(bool align);

    void Rescale();
    bool HasDismissLongTime() const;

signals:
    void dismissed();
    void selectionChanged(int index);

public slots:
    void Dismiss() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int  hoverIndex() const;
    int  selectedItem() const;
    void measureSize();
    void autoPosition();
    void sendDropDownEvent();

    std::vector<Item> &items;
    size_t             count       = 0;
    QString            group_filter;
    bool               need_sync   = false;
    int                selection   = -1;
    int                hover_item  = -1;

    double radius                  = 0;
    bool   use_content_width       = false;
    bool   limit_max_content_width = false;
    bool   align_icon              = false;
    bool   text_off                = false;

    QSize textSize;
    QSize iconSize;
    QSize rowSize;

    StateHandler state_handler;
    StateColor   text_color;
    StateColor   border_color;
    StateColor   selector_border_color;
    StateColor   selector_background_color;
    ScalableBitmap check_bitmap;

    bool         pressedDown = false;
    std::chrono::steady_clock::time_point dismissTime;
    int          scroll_offset = 0;

    friend class ComboBox;
};

#endif // !slic3r_GUI_DropDown_hpp_

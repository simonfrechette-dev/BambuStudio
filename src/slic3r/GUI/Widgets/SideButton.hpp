#ifndef slic3r_GUI_SideButton_hpp_
#define slic3r_GUI_SideButton_hpp_

#include "../QtExtensions.hpp"
#include "StateColor.hpp"
#include "StateHandler.hpp"

#include <QWidget>
#include <QColor>
#include <QString>
#include <vector>

class SideButton : public QWidget
{
    Q_OBJECT
public:
    enum EHorizontalOrientation : unsigned char {
        HO_Left,
        HO_Center,
        HO_Right,
        Num_Horizontal_Orientations
    };

    SideButton(QWidget *parent, const QString &text,
               const QString &icon = {}, long style = 0, int iconSize = 0);

    void SetCornerRadius(double radius);
    void SetCornerEnable(const std::vector<bool> &enable);
    void SetTextLayout(EHorizontalOrientation orient, int margin = 15);
    void SetLayoutStyle(int style);
    void setText(const QString &label);
    void setForegroundColour(QColor colour);
    void setBackgroundColour(QColor color);
    bool SetBottomColour(QColor color);
    void SetMinSize(const QSize &size);
    void SetBorderColor(const StateColor &color);
    void SetForegroundColor(const StateColor &color);
    void SetBackgroundColor(const StateColor &color);
    bool Enable(bool enable = true);
    void Rescale();
    void SetExtraSize(const QSize &size);
    void SetIconOffset(int offset);

    QSize sizeHint() const override;

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void measureSize();

    QString        m_text;
    QSize          textSize;
    QSize          minSize;
    ScalableBitmap icon;
    double         radius       = 0.0;
    QSize          extra_size;
    int            icon_offset  = 0;
    int            layout_style = 0;
    std::vector<bool> radius_enable;

    StateHandler state_handler;
    StateColor   text_color;
    StateColor   border_color;
    StateColor   background_color;
    QColor       bottom_color;
    bool         pressedDown = false;

    EHorizontalOrientation text_orientation = HO_Center;
    int text_margin = 15;
};

#endif // !slic3r_GUI_SideButton_hpp_

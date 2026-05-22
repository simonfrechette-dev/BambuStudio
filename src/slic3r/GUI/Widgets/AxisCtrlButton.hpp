#ifndef slic3r_GUI_AxisCtrlButton_hpp_
#define slic3r_GUI_AxisCtrlButton_hpp_

#include "../QtExtensions.hpp"
#include "StateColor.hpp"
#include "StateHandler.hpp"

#include <QWidget>

class AxisCtrlButton : public QWidget
{
    Q_OBJECT
public:
    enum CurrentPos : unsigned char {
        OUTER_UP    = 0,
        OUTER_LEFT  = 1,
        OUTER_DOWN  = 2,
        OUTER_RIGHT = 3,
        INNER_UP    = 4,
        INNER_LEFT  = 5,
        INNER_DOWN  = 6,
        INNER_RIGHT = 7,
        INNER_HOME  = 8,
        UNDEFINED   = 9
    };

    AxisCtrlButton(QWidget *parent, ScalableBitmap &icon, long style = 0);

    void SetMinSize(const QSize &size);
    void SetTextColor(const StateColor &color);
    void SetBorderColor(const StateColor &color);
    void SetBackgroundColor(const StateColor &color);
    void SetInnerBackgroundColor(const StateColor &color);
    void SetBitmap(ScalableBitmap &bmp);
    void Rescale();

signals:
    void buttonClicked(int pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void updateParams();
    CurrentPos posFromPoint(const QPoint &pt) const;

    QSize          minSize;
    double         stretch       = 1.0;
    double         r_outer       = 0;
    double         r_inner       = 0;
    double         r_home        = 0;
    double         r_blank       = 0;
    double         gap           = 0;
    QPoint         center;

    StateHandler   state_handler;
    StateColor     text_color;
    StateColor     border_color;
    StateColor     background_color;
    StateColor     inner_background_color;
    ScalableBitmap m_icon;

    bool           pressedDown  = false;
    CurrentPos     last_pos     = UNDEFINED;
    CurrentPos     current_pos  = UNDEFINED;
};

#endif // !slic3r_GUI_AxisCtrlButton_hpp_

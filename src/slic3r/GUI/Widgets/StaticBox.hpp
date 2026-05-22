#ifndef slic3r_GUI_StaticBox_hpp_
#define slic3r_GUI_StaticBox_hpp_

#include "../QtExtensions.hpp"
#include "StateHandler.hpp"

#include <QWidget>
#include <QPainter>
#include <QPen>

class StaticBox : public QWidget
{
    Q_OBJECT
public:
    explicit StaticBox(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    bool init(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

    void SetCornerRadius(double radius);

    void SetBorderWidth(int width);

    void SetBorderColor(StateColor const & color);

    void SetBorderColorNormal(QColor const &color);

    void SetBorderStyle(Qt::PenStyle style);

    void SetBackgroundColor(StateColor const &color);

    void SetBackgroundColorNormal(QColor const &color);

    void SetBackgroundColor2(StateColor const &color);

    static QColor GetParentBackgroundColor(QWidget *parent);

    void ShowBadge(bool show);

protected:
    void paintEvent(QPaintEvent *event) override;

    void render(QPainter &painter);

    virtual void doRender(QPainter &painter);

protected:
    double         radius       = 8;
    int            border_width = 1;
    Qt::PenStyle   border_style = Qt::SolidLine;
    StateHandler   state_handler;
    StateColor     border_color;
    StateColor     background_color;
    StateColor     background_color2;
    ScalableBitmap badge;
};

#endif // !slic3r_GUI_StaticBox_hpp_

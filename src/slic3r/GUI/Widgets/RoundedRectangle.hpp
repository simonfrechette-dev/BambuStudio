#ifndef slic3r_GUI_ROUNDEDRECTANGLE_hpp_
#define slic3r_GUI_ROUNDEDRECTANGLE_hpp_

#include <QWidget>
#include <QColor>

class RoundedRectangle : public QWidget
{
    Q_OBJECT
public:
    RoundedRectangle(QWidget *parent, QColor col, const QPoint &pos,
                     const QSize &size, double radius, int type = 0);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_radius;
    int    m_type;
    QColor m_color;
};

#endif // !slic3r_GUI_RoundedRectangle_hpp_

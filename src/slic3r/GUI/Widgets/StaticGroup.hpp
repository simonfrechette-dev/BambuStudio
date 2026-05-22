#ifndef slic3r_GUI_StaticGroup_hpp_
#define slic3r_GUI_StaticGroup_hpp_

#include "../QtExtensions.hpp"
#include <QGroupBox>
#include <QColor>

class StaticGroup : public QGroupBox
{
    Q_OBJECT
public:
    explicit StaticGroup(QWidget *parent, int id = -1);

    void ShowBadge(bool show);
    void SetBorderColor(const QColor &color);
    void setVisible(bool show) override;
    void show(bool show = true) { setVisible(show); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ScalableBitmap badge;
    QColor         borderColor_;
    bool           showBadge_ = false;
};

#endif // !slic3r_GUI_StaticGroup_hpp_

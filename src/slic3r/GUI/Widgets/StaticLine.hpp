#ifndef slic3r_GUI_StaticLine_hpp_
#define slic3r_GUI_StaticLine_hpp_

#include "../QtExtensions.hpp"
#include <QWidget>
#include <QColor>
#include <QString>

class StaticLine : public QWidget
{
    Q_OBJECT
public:
    StaticLine(QWidget *parent, bool vertical = false,
               const QString &label = {}, const QString &icon = {});

    void setText(const QString &label);
    void SetIcon(const QString &icon);
    void SetLineColour(QColor color);
    void Rescale();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void measureSize();
    void render(QPainter &painter);

    QColor         lineColor;
    bool           m_vertical;
    QString        m_text;
    ScalableBitmap icon;
};

#endif // !slic3r_GUI_StaticLine_hpp_

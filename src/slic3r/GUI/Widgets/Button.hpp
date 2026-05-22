#ifndef slic3r_GUI_Button_hpp_
#define slic3r_GUI_Button_hpp_

#include "StaticBox.hpp"

#include <QString>
#include <QSize>
#include <QRect>
#include <QFont>

class Button : public StaticBox
{
    Q_OBJECT

    QRect  textRect;       // bounding rect of current text
    QSize  minSize;        // minimum size set by caller
    QSize  paddingSize;
    ScalableBitmap active_icon;
    ScalableBitmap inactive_icon;

    StateColor text_color;

    bool pressedDown          = false;
    bool m_selected           = true;
    bool canFocus             = true;
    bool isCenter             = true;
    bool vertical             = false;

    bool m_left_corner_white  = false;
    bool m_right_corner_white = false;
    bool grayed               = false;

    QString m_text;

    static const int buttonWidth  = 200;
    static const int buttonHeight = 50;

signals:
    void clicked();

public:
    Button();
    explicit Button(QWidget *parent, const QString &text,
                    const QString &icon = {}, int iconSize = 0,
                    Qt::WindowFlags flags = {});

    bool init(QWidget *parent, const QString &text,
              const QString &icon = {}, int iconSize = 0,
              Qt::WindowFlags flags = {});

    void setText(const QString &label);
    const QString &text() const { return m_text; }

    bool setFont(const QFont &font);

    void SetIcon(const QString &icon);
    void SetInactiveIcon(const QString &icon);

    void setMinimumSize(const QSize &sz);
    void setMaximumSize(const QSize &sz);

    void SetPaddingSize(const QSize &sz);

    void SetTextColor(StateColor const &color);
    void SetTextColorNormal(QColor const &color);

    void SetSelected(bool selected = true) { m_selected = selected; }

    void setEnabled(bool enable);

    void SetCanFocus(bool cf);

    void SetValue(bool state);
    bool GetValue() const;

    void SetCenter(bool c);
    void SetVertical(bool v = true);

    void Rescale();

    void SetLeftCornerWhite(bool w = true)  { m_left_corner_white = w;  update(); }
    void SetRightCornerWhite(bool w = true) { m_right_corner_white = w; update(); }

    bool IsGrayed() { return grayed; }
    void SetGrayed(bool gray) { grayed = gray; }

    QRect GetTextRect() const { return textRect; }

    QSize sizeHint() const override;

protected:
    void doRender(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool focusNextPrevChild(bool) override { return false; }

private:
    void renderWhiteCorners(QPainter &painter);
    void measureSize();
};

#endif // !slic3r_GUI_Button_hpp_

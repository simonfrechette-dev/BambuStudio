#ifndef slic3r_GUI_CheckBox_hpp_
#define slic3r_GUI_CheckBox_hpp_

#include "../QtExtensions.hpp"

#include <QAbstractButton>

class CheckBox : public QAbstractButton
{
    Q_OBJECT
public:
    explicit CheckBox(QWidget *parent = nullptr, int id = -1);

    void setChecked(bool value);
    bool isChecked() const { return m_checked; }

    void SetHalfChecked(bool value = true);
    bool isHalfChecked() const { return m_half_checked; }

    void Rescale();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void update_bitmap();
    const QPixmap &currentPixmap() const;

    bool m_checked      = false;
    bool m_half_checked = false;
    bool m_hovered      = false;
    bool m_focused      = false;

    ScalableBitmap m_on;
    ScalableBitmap m_half;
    ScalableBitmap m_off;
    ScalableBitmap m_on_disabled;
    ScalableBitmap m_half_disabled;
    ScalableBitmap m_off_disabled;
    ScalableBitmap m_on_focused;
    ScalableBitmap m_half_focused;
    ScalableBitmap m_off_focused;
};

#endif // !slic3r_GUI_CheckBox_hpp_

#ifndef slic3r_GUI_RADIOBOX_hpp_
#define slic3r_GUI_RADIOBOX_hpp_

#include "../QtExtensions.hpp"
#include <QAbstractButton>

namespace Slic3r {
namespace GUI {

class RadioBox : public QAbstractButton
{
    Q_OBJECT
public:
    explicit RadioBox(QWidget *parent = nullptr);

    void SetValue(bool value);
    bool GetValue() const { return m_checked; }
    void Rescale();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    const QPixmap &currentPixmap() const;

    bool m_checked = false;
    ScalableBitmap m_on;
    ScalableBitmap m_off;
    ScalableBitmap m_ban;
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_CheckBox_hpp_

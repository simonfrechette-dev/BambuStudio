#ifndef slic3r_GUI_ImageSwitchButton_hpp_
#define slic3r_GUI_ImageSwitchButton_hpp_

#include "../QtExtensions.hpp"
#include "StateColor.hpp"
#include "StaticBox.hpp"
#include "Label.hpp"

class ImageSwitchButton : public StaticBox
{
    Q_OBJECT
public:
    ImageSwitchButton(QWidget *parent, ScalableBitmap &img_on,
                      ScalableBitmap &img_off, long style = 0);

    void SetLabels(const QString &lbl_on, const QString &lbl_off);
    void SetImages(ScalableBitmap &img_on, ScalableBitmap &img_off);
    void SetTextColor(const StateColor &color);
    void SetValue(bool value);
    void SetPadding(int padding);

    bool GetValue() const { return m_on_off; }
    void Rescale();

signals:
    void toggled(bool value);

protected:
    void doRender(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void measureSize();

    ScalableBitmap m_on;
    ScalableBitmap m_off;
    bool           m_on_off    = false;
    int            m_padding   = 5;
    bool           pressedDown = false;
    bool           hover       = false;
    QSize          textSize;
    QSize          minSize;
    QString        labels[2];
    StateColor     text_color;
};

class FanSwitchButton : public StaticBox
{
    Q_OBJECT
public:
    FanSwitchButton(QWidget *parent, ScalableBitmap &img_on,
                    ScalableBitmap &img_off, long style = 0);

    void SetLabels(const QString &lbl_on, const QString &lbl_off);
    void SetImages(ScalableBitmap &img_on, ScalableBitmap &img_off);
    void SetTextColor(const StateColor &color);
    void SetValue(bool value);
    void SetPadding(int padding);

    bool GetValue() const { return m_on_off; }
    void Rescale();
    void setFanValue(int val);
    void UseTextFan();
    void UseTextAirCondition();

signals:
    void toggled(bool value);

protected:
    void doRender(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void measureSize();
    void setText(const QString &text);

    ScalableBitmap m_on;
    ScalableBitmap m_off;
    bool           m_on_off    = false;
    int            m_padding   = 5;
    bool           pressedDown = false;
    bool           hover       = false;
    int            m_speed     = 0;
    QSize          textSize;
    QSize          minSize;
    QString        labels[2];
    QString        m_text;
    StateColor     text_color;
};

#endif // !slic3r_GUI_ImageSwitchButton_hpp_

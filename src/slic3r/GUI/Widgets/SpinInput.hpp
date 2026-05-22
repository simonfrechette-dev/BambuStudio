#ifndef slic3r_GUI_SpinInput_hpp_
#define slic3r_GUI_SpinInput_hpp_

#include "StaticBox.hpp"
#include "StateColor.hpp"

#include <QSpinBox>
#include <QLineEdit>
#include <QTimer>
#include <QString>

class Button;

class SpinInput : public StaticBox
{
    Q_OBJECT

    StateColor  label_color;
    StateColor  text_color;
    QSpinBox   *spin_ctrl  = nullptr;
    Button     *button_inc = nullptr;
    Button     *button_dec = nullptr;
    QTimer      timer;

    int  val   = 0;
    int  m_min = 0;
    int  m_max = 100;
    int  delta = 1;
    bool text_updating = false;

    QString m_label;

    static const int SpinInputWidth  = 200;
    static const int SpinInputHeight = 50;

public:
    SpinInput();

    SpinInput(QWidget       *parent,
              const QString  &text,
              const QString  &label  = {},
              const QPoint   &pos    = {},
              const QSize    &size   = {},
              long            style  = 0,
              int min = 0, int max = 100, int initial = 0);

    void init(QWidget       *parent,
              const QString  &text,
              const QString  &label   = {},
              const QSize    &size    = {},
              long            style   = 0,
              int min = 0, int max = 100, int initial = 0);

    void SetCornerRadius(double radius) { this->radius = radius; }
    void SetLabel(const QString &label);
    void SetLabelColor(const StateColor &color);
    void SetTextColor(const StateColor &color);
    void SetSize(const QSize &size);
    void Rescale();
    bool Enable(bool enable = true);

    QSpinBox *GetSpinCtrl() { return spin_ctrl; }

    void SetValue(const QString &text);
    void SetValue(int value);
    int  GetValue() const;
    void SetRange(int min, int max);

signals:
    void valueChanged(int value);

protected:
    void doRender(QPainter &painter) override;

private:
    void setupButtons();
    void onIncrement();
    void onDecrement();
    void onTimerTick();
};

#endif // !slic3r_GUI_SpinInput_hpp_

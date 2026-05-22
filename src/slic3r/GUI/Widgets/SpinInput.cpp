#include "SpinInput.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <QHBoxLayout>
#include <QPainter>

SpinInput::SpinInput() = default;

SpinInput::SpinInput(QWidget *parent, const QString &text,
                     const QString &label, const QPoint & /*pos*/,
                     const QSize &size, long style,
                     int min, int max, int initial)
{
    init(parent, text, label, size, style, min, max, initial);
}

void SpinInput::init(QWidget *parent, const QString &text,
                     const QString &label, const QSize &size,
                     long /*style*/, int min, int max, int initial)
{
    StaticBox::init(parent);
    m_label = label;
    m_min   = min;
    m_max   = max;
    val     = initial;

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(2);

    // Decrement button
    button_dec = new Button(this, QStringLiteral("-"), {}, 0);
    button_dec->setFixedSize(24, 24);
    layout->addWidget(button_dec);

    // Spin control (use QSpinBox but hide the native arrows)
    spin_ctrl = new QSpinBox(this);
    spin_ctrl->setRange(min, max);
    spin_ctrl->setValue(initial);
    spin_ctrl->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spin_ctrl->setFrame(false);
    spin_ctrl->setFont(Label::Body_14);
    spin_ctrl->setStyleSheet(QStringLiteral("background:transparent;"));
    spin_ctrl->setAlignment(Qt::AlignCenter);
    layout->addWidget(spin_ctrl, 1);

    // Increment button
    button_inc = new Button(this, QStringLiteral("+"), {}, 0);
    button_inc->setFixedSize(24, 24);
    layout->addWidget(button_inc);

    setLayout(layout);

    connect(button_inc, &Button::clicked, this, &SpinInput::onIncrement);
    connect(button_dec, &Button::clicked, this, &SpinInput::onDecrement);
    connect(spin_ctrl, qOverload<int>(&QSpinBox::valueChanged),
            this, [this](int v) { val = v; emit valueChanged(v); });

    if (!size.isEmpty()) setFixedSize(size);
    else setMinimumSize(SpinInputWidth, SpinInputHeight);
}

void SpinInput::SetLabel(const QString &label) { m_label = label; update(); }
void SpinInput::SetLabelColor(const StateColor &color) { label_color = color; update(); }
void SpinInput::SetTextColor(const StateColor &color)  { text_color  = color; update(); }
void SpinInput::SetSize(const QSize &size) { setFixedSize(size); }

void SpinInput::Rescale() { update(); }

bool SpinInput::Enable(bool enable)
{
    QWidget::setEnabled(enable);
    if (spin_ctrl)  spin_ctrl->setEnabled(enable);
    if (button_inc) button_inc->setEnabled(enable);
    if (button_dec) button_dec->setEnabled(enable);
    update();
    return true;
}

void SpinInput::SetValue(const QString &text)
{
    if (spin_ctrl) spin_ctrl->setValue(text.toInt());
}

void SpinInput::SetValue(int value)
{
    if (spin_ctrl) spin_ctrl->setValue(value);
    val = value;
}

int SpinInput::GetValue() const
{
    return spin_ctrl ? spin_ctrl->value() : val;
}

void SpinInput::SetRange(int min, int max)
{
    m_min = min; m_max = max;
    if (spin_ctrl) spin_ctrl->setRange(min, max);
}

void SpinInput::onIncrement()
{
    if (spin_ctrl)
        spin_ctrl->setValue(std::min(spin_ctrl->value() + delta, m_max));
}

void SpinInput::onDecrement()
{
    if (spin_ctrl)
        spin_ctrl->setValue(std::max(spin_ctrl->value() - delta, m_min));
}

void SpinInput::onTimerTick() {}

void SpinInput::doRender(QPainter &painter)
{
    StaticBox::doRender(painter);
}

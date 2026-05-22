#include "TextInput.hpp"
#include "Label.hpp"

#include <QHBoxLayout>
#include <QPainter>
#include <QFontMetrics>
#include <QResizeEvent>

TextInput::TextInput() = default;

TextInput::TextInput(QWidget *parent)
    : StaticBox(parent)
{
    init(parent, {}, {}, {}, {});
}

TextInput::TextInput(QWidget *parent, const QString &text,
                     const QString &label, const QString &iconName,
                     const QPoint & /*pos*/, const QSize &size,
                     long style, const QString &unit, const QString &prefix)
{
    init(parent, text, label, iconName, size, style, unit, prefix);
}

void TextInput::init(QWidget *parent, const QString &text,
                     const QString &label, const QString &iconName,
                     const QSize &size, long /*style*/,
                     const QString &unit, const QString &prefix)
{
    StaticBox::init(parent);
    m_label  = label;
    m_unit   = unit;
    m_prefix = prefix;

    if (!iconName.isEmpty())
        icon = ScalableBitmap(this, iconName.toStdString(), 16);

    // Build layout: [icon?] [prefix?] [QLineEdit] [unit?]
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 2, 8, 2);
    layout->setSpacing(4);

    if (!iconName.isEmpty() && icon.bmp().IsOk()) {
        // icon drawn in doRender; leave space via left margin
    }

    text_ctrl = new QLineEdit(text, this);
    text_ctrl->setFrame(false);
    text_ctrl->setFont(Label::Body_14);
    text_ctrl->setStyleSheet(QStringLiteral("background:transparent;"));
    layout->addWidget(text_ctrl);

    setLayout(layout);

    connect(text_ctrl, &QLineEdit::textChanged, this, &TextInput::textChanged);

    if (!size.isEmpty()) setFixedSize(size);
    else setMinimumSize(TextInputWidth, TextInputHeight);

    measureSize();
}

void TextInput::SetLabel(const QString &label) { m_label = label; update(); }
void TextInput::SetPrefix(const QString &prefix) { m_prefix = prefix; update(); }

void TextInput::SetStaticTips(const QString &tips, const QPixmap &bitmap)
{
    static_tips      = tips;
    static_tips_icon = bitmap;
    measureSize();
    update();
}

void TextInput::SetIcon(const QPixmap &px)
{
    icon.bmp() = BBLPixmap(px);
    update();
}

void TextInput::SetIcon(const QString &name)
{
    icon = ScalableBitmap(this, name.toStdString(), 16);
    update();
}

void TextInput::SetIcon_1(const QString &name)
{
    icon_1 = ScalableBitmap(this, name.toStdString(), 16);
    update();
}

void TextInput::SetIcon_1(const QPixmap &px)
{
    icon_1.bmp() = BBLPixmap(px);
    update();
}

void TextInput::SetLabelColor(const StateColor &color) { label_color = color; update(); }
void TextInput::SetTextColor(const StateColor &color)  { text_color  = color; update(); }

void TextInput::Rescale() { measureSize(); update(); }

bool TextInput::Enable(bool enable)
{
    QWidget::setEnabled(enable);
    if (text_ctrl) text_ctrl->setEnabled(enable);
    update();
    return true;
}

void TextInput::SetMinSize(const QSize &size) { setMinimumSize(size); }

void TextInput::SetValue(const QString &text)
{
    if (text_ctrl) text_ctrl->setText(text);
}

QString TextInput::GetValue() const
{
    return text_ctrl ? text_ctrl->text() : QString{};
}

void TextInput::addValChecker(std::shared_ptr<TextInputValChecker> checker)
{
    m_checkers.push_back(std::move(checker));
}

void TextInput::measureSize()
{
    int minH = TextInputHeight;
    if (!static_tips.isEmpty()) {
        QFontMetrics fm(font());
        minH = std::max(minH, fm.height() + 12);
    }
    setMinimumHeight(minH);
}

void TextInput::doRender(QPainter &painter)
{
    // Call base class to draw the rounded box
    StaticBox::doRender(painter);

    // Draw icon if present
    if (icon.bmp().IsOk()) {
        int iy = (height() - icon.bmp().height()) / 2;
        painter.drawPixmap(QPoint(6, iy), icon.bmp());
        // Adjust text_ctrl left margin to avoid overlapping icon
        if (auto *l = qobject_cast<QHBoxLayout *>(layout()))
            l->setContentsMargins(6 + icon.bmp().width() + 4, 2, 8, 2);
    }
}

#include "TempInput.hpp"
#include "Label.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QValidator>
#include <cctype>

TempInput::TempInput(QWidget *parent)
    : StaticBox(parent)
{}

TempInput::TempInput(QWidget *parent, int type, const QString &text,
                     TempInputType input_type, const QString &label,
                     const QString &normalIcon, const QString &activeIcon)
    : StaticBox(parent)
    , temp_type(type)
    , m_input_type(input_type)
{
    if (!normalIcon.isEmpty())
        normal_icon = ScalableBitmap(this, normalIcon.toStdString(), 20);
    if (!activeIcon.isEmpty())
        active_icon = ScalableBitmap(this, activeIcon.toStdString(), 20);
    init(parent, text, label, normalIcon, activeIcon);
}

void TempInput::init(QWidget * /*parent*/, const QString &text,
                     const QString &label,
                     const QString &normalIcon, const QString &activeIcon)
{
    if (!normalIcon.isEmpty())
        normal_icon = ScalableBitmap(this, normalIcon.toStdString(), 20);
    if (!activeIcon.isEmpty())
        active_icon = ScalableBitmap(this, activeIcon.toStdString(), 20);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(4, 2, 4, 2);
    outer->setSpacing(1);

    if (!label.isEmpty()) {
        curr_label = new QLabel(label, this);
        curr_label->setFont(Label::Body_12);
        outer->addWidget(curr_label);
    }

    text_ctrl = new QLineEdit(text, this);
    text_ctrl->setFont(Label::Body_14);
    text_ctrl->setFrame(false);
    text_ctrl->setStyleSheet(QStringLiteral("background: transparent; border: none;"));
    text_ctrl->setValidator(new QIntValidator(0, 9999, text_ctrl));
    text_ctrl->setReadOnly(m_read_only);
    outer->addWidget(text_ctrl);

    warning_text = new QLabel(this);
    warning_text->setFont(Label::Body_10);
    warning_text->setStyleSheet(QStringLiteral("color: #f00;"));
    warning_text->hide();
    outer->addWidget(warning_text);

    setLayout(outer);
    setMinimumSize(TempInputWidth, TempInputHeight);

    connect(text_ctrl, &QLineEdit::textChanged, this, &TempInput::textChanged);
    connect(text_ctrl, &QLineEdit::returnPressed, this, [this]() {
        emit tempFinished(text_ctrl->text());
    });
}

void TempInput::SetTagTemp(int temp)     { if (text_ctrl) text_ctrl->setText(QString::number(temp)); }
void TempInput::SetTagTemp(const QString &t) { if (text_ctrl) text_ctrl->setText(t); }
void TempInput::SetCurrTemp(int temp)    { if (curr_label) curr_label->setText(QString::number(temp)); }
void TempInput::SetCurrTemp(const QString &t) { if (curr_label) curr_label->setText(t); }
void TempInput::SetCurrType(TempInputType type) { m_input_type = type; }

bool TempInput::AllisNum(const std::string &str) const
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void TempInput::SetFinish()
{
    if (text_ctrl) emit tempFinished(text_ctrl->text());
}

void TempInput::Warning(bool warn, WarningType type)
{
    warning_mode = warn;
    if (warning_text) {
        warning_text->setVisible(warn);
        if (warn) {
            if (type == WARNING_TOO_HIGH)      warning_text->setText(QStringLiteral("Too high"));
            else if (type == WARNING_TOO_LOW)  warning_text->setText(QStringLiteral("Too low"));
            else                               warning_text->setText(QStringLiteral("Out of range"));
        }
    }
    update();
}

void TempInput::SetIconActive() { active = true; update(); }
void TempInput::SetIconNormal() { active = false; update(); }
void TempInput::SetReadOnly(bool ro) { m_read_only = ro; if (text_ctrl) text_ctrl->setReadOnly(ro); }
void TempInput::SetMaxTemp(int t) { max_temp = t; if (text_ctrl) if (auto *v = qobject_cast<QIntValidator*>(const_cast<QValidator*>(text_ctrl->validator()))) v->setTop(t); }
void TempInput::SetMinTemp(int t) { min_temp = t; if (text_ctrl) if (auto *v = qobject_cast<QIntValidator*>(const_cast<QValidator*>(text_ctrl->validator()))) v->setBottom(t); }
QString TempInput::GetTagTemp() const { return text_ctrl ? text_ctrl->text() : QString{}; }
QString TempInput::GetCurrTemp() const { return curr_label ? curr_label->text() : QString{}; }

void TempInput::SetLabel(const QString &label)
{
    if (!curr_label) {
        curr_label = new QLabel(label, this);
        curr_label->setFont(Label::Body_12);
        qobject_cast<QVBoxLayout*>(layout())->insertWidget(0, curr_label);
    } else {
        curr_label->setText(label);
    }
}

void TempInput::SetTextColor(const StateColor &c)  { text_color = c; update(); }
void TempInput::SetLabelColor(const StateColor &c) { label_color = c; update(); }
void TempInput::Rescale()                          { update(); }
bool TempInput::Enable(bool e)                     { setEnabled(e); return true; }
void TempInput::SetMinSize(const QSize &s)         { setMinimumSize(s); }

void TempInput::doRender(QPainter &painter)
{
    StaticBox::doRender(painter);

    const ScalableBitmap &bmp = active ? active_icon : normal_icon;
    if (bmp.bmp().IsOk()) {
        const QSize  sz  = bmp.GetBmpSize();
        const QPoint pos(rect().right() - sz.width() - 4, (rect().height() - sz.height()) / 2);
        painter.drawPixmap(pos, bmp.bmp());
    }

    if (warning_mode) {
        painter.setPen(QColor(0xff, 0x00, 0x00));
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

void TempInput::enterEvent(QEnterEvent *e) { m_hover = true; update(); StaticBox::enterEvent(e); }
void TempInput::leaveEvent(QEvent *e)      { m_hover = false; update(); StaticBox::leaveEvent(e); }

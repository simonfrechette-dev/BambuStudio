#ifndef slic3r_GUI_TextInput_hpp_
#define slic3r_GUI_TextInput_hpp_

#include "StaticBox.hpp"
#include "../QtExtensions.hpp"

#include <QLineEdit>
#include <QString>
#include <vector>
#include <memory>

class TextInputValChecker;

class TextInput : public StaticBox
{
    Q_OBJECT

    ScalableBitmap icon;
    ScalableBitmap icon_1;
    StateColor     label_color;
    StateColor     text_color;
    QLineEdit     *text_ctrl  = nullptr;
    QString        m_label;
    QString        m_unit;
    QString        m_prefix;
    QString        static_tips;
    QSize          static_tips_size;
    QPixmap        static_tips_icon;

    std::vector<std::shared_ptr<TextInputValChecker>> m_checkers;

    static const int TextInputWidth  = 200;
    static const int TextInputHeight = 50;

public:
    TextInput();
    explicit TextInput(QWidget *parent);

    TextInput(QWidget      *parent,
              const QString &text,
              const QString &label  = {},
              const QString &icon   = {},
              const QPoint  &pos    = {},
              const QSize   &size   = {},
              long           style  = 0,
              const QString &unit   = {},
              const QString &prefix = {});

    void init(QWidget      *parent,
              const QString &text,
              const QString &label  = {},
              const QString &icon   = {},
              const QSize   &size   = {},
              long           style  = 0,
              const QString &unit   = {},
              const QString &prefix = {});

    void SetCornerRadius(double radius) { this->radius = radius; }
    virtual void SetLabel(const QString &label);
    void SetPrefix(const QString &prefix);
    void SetStaticTips(const QString &tips, const QPixmap &bitmap);
    void SetIcon(const QPixmap &icon);
    void SetIcon(const QString &icon);
    void SetIcon_1(const QString &icon);
    void SetIcon_1(const QPixmap &icon);
    void SetLabelColor(const StateColor &color);
    void SetTextColor(const StateColor &color);
    virtual void Rescale();
    bool Enable(bool enable = true);
    void SetMinSize(const QSize &size);

    QLineEdit       *GetTextCtrl()       { return text_ctrl; }
    const QLineEdit *GetTextCtrl() const { return text_ctrl; }

    void SetValue(const QString &text);
    QString GetValue() const;

    void addValChecker(std::shared_ptr<TextInputValChecker> checker);

signals:
    void textChanged(const QString &text);

protected:
    void doRender(QPainter &painter) override;

private:
    void measureSize();
};

#endif // !slic3r_GUI_TextInput_hpp_

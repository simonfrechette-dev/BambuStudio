#ifndef slic3r_GUI_SwitchButton_hpp_
#define slic3r_GUI_SwitchButton_hpp_

#include "../QtExtensions.hpp"
#include "StateColor.hpp"
#include "StaticBox.hpp"
#include "Label.hpp"
#include "Button.hpp"

#include <QAbstractButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QString>
#include <QColor>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// SwitchButton — bitmap-based or text-label toggle
// ---------------------------------------------------------------------------
class SwitchButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit SwitchButton(QWidget *parent = nullptr, int id = -1);

    void SetLabels(const QString &lbl_on, const QString &lbl_off);
    void SetTextColor(const StateColor &color);
    void SetTextColor2(const StateColor &color);
    void SetTrackColor(const StateColor &color);
    void SetThumbColor(const StateColor &color);
    void SetValue(bool value);
    bool GetValue() const { return m_value; }
    void Rescale();

    QSize sizeHint() const override;

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void update();

    bool           m_value = false;
    QString        m_labels[2];
    ScalableBitmap m_on;
    ScalableBitmap m_off;
    StateColor     text_color;
    StateColor     text_color2;
    StateColor     track_color;
    StateColor     thumb_color;
};

// ---------------------------------------------------------------------------
// SwitchBoard — left/right dual-label toggle
// ---------------------------------------------------------------------------
class SwitchBoard : public QWidget
{
    Q_OBJECT
public:
    explicit SwitchBoard(QWidget *parent = nullptr,
                         const QString &leftL  = {},
                         const QString &right  = {},
                         const QSize &  size   = {});

    QString leftLabel;
    QString rightLabel;

    void updateState(const QString &target);
    void SetLabels(const QString &left, const QString &right);

    bool  switch_left  = false;
    bool  switch_right = false;
    bool  is_enable    = true;
    void *client_data  = nullptr;

    void Enable()  { is_enable = true;  update(); }
    void Disable() { is_enable = false; update(); }
    bool IsEnabled() const { return is_enable; }

    void  SetClientData(void *data) { client_data = data; }
    void *GetClientData()           { return client_data; }

    void SetAutoDisableWhenSwitch() { auto_disable_when_switch = true; }

signals:
    void switchPos(bool leftSelected);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void doRender(QPainter &p);
    bool auto_disable_when_switch = false;
};

// ---------------------------------------------------------------------------
// CustomToggleButton — icon+text toggle with primary/secondary colour
// ---------------------------------------------------------------------------
class CustomToggleButton : public QWidget
{
    Q_OBJECT
public:
    explicit CustomToggleButton(QWidget *parent, const QString &label,
                                int id = -1, const QPoint &pos = {},
                                const QSize &size = {});

    void setText(const QString &label);
    void SetSelectedIcon(const QString &iconPath);
    void SetUnSelectedIcon(const QString &iconPath);
    void SetIsSelected(bool selected);
    bool IsSelected() const { return m_isSelected; }
    void set_primary_colour(QColor col)   { m_primary_colour = col; }
    void set_secondary_colour(QColor col) { m_secondary_colour = col; }

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString  m_label;
    QPixmap  m_selected_icon;
    QPixmap  m_unselected_icon;
    QColor   m_primary_colour{QColor("#00AE42")};
    QColor   m_secondary_colour{QColor("#DEF5E7")};
    bool     m_isSelected = false;
};

// ---------------------------------------------------------------------------
// RichTooltipPopup — dark tooltip popup with optional icon
// ---------------------------------------------------------------------------
class RichTooltipPopup : public QWidget
{
    Q_OBJECT
public:
    explicit RichTooltipPopup(QWidget *parent,
                              const QString &iconName,
                              const QString &text);
    void ShowAtPosition(QWidget *anchor);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_icon;
    QString m_text;
};

// ---------------------------------------------------------------------------
// ExpandButton — small icon button with rich tooltip
// ---------------------------------------------------------------------------
class ExpandButton : public QWidget
{
    Q_OBJECT
public:
    explicit ExpandButton(QWidget *parent, const std::string &bmp,
                          int id = -1,
                          const QPoint &pos = {}, const QSize &size = {});
    ~ExpandButton() override;

    void update_bitmap(const std::string &bmp);
    void msw_rescale();
    void SetRichTooltip(const QString &iconName, const QString &text);
    void ShowRichTooltip();
    void HideRichTooltip();

signals:
    void expandClicked(int id);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    std::string         m_bmp_str;
    QPixmap             m_bmp;
    QString             m_tooltip_icon;
    QString             m_tooltip_text;
    RichTooltipPopup   *m_tooltip_popup{nullptr};
};

// ---------------------------------------------------------------------------
// ExpandButtonHolder — row container of ExpandButtons
// ---------------------------------------------------------------------------
class ExpandButtonHolder : public QWidget
{
    Q_OBJECT
public:
    explicit ExpandButtonHolder(QWidget *parent = nullptr, int id = -1,
                                const QPoint &pos = {}, const QSize &size = {});

    int  GetAvailable();
    void addExpandButton(int id, const std::string &img);
    void ShowExpandButton(int id, bool show);
    void updateExpandButtonBitmap(int id, const std::string &bitmap);
    void EnableExpandButton(int id, bool enb);
    void SetExpandButtonTooltip(int id, const QString &tooltip);
    void SetExpandButtonRichTooltip(int id, const QString &iconName, const QString &text);
    void msw_rescale();

    QHBoxLayout *hsizer{nullptr};

signals:
    void expandClicked(int id);

private:
    ExpandButton *FindExpandButton(int id);
};

// ---------------------------------------------------------------------------
// MultiSwitchButton — tab-strip selector
// ---------------------------------------------------------------------------
class MultiSwitchButton : public StaticBox
{
    Q_OBJECT
    std::vector<Button *> btns;
    QHBoxLayout          *sizer = nullptr;
    int                   sel   = -1;

public:
    explicit MultiSwitchButton(QWidget *parent = nullptr);
    ~MultiSwitchButton() override;

    int          AppendOption(const QString &option, void *clientData = nullptr);
    void         SetOptions(const std::vector<QString> &options);
    void         DeleteAllOptions();
    unsigned int GetCount() const { return (unsigned)btns.size(); }
    int          GetSelection() const { return sel; }
    void         SetSelection(int index);
    QString      GetSelectedText() const;
    Button      *GetButton(unsigned int index) const {
        return index < btns.size() ? btns[index] : nullptr;
    }
    QString      GetOptionText(unsigned int index) const;
    void         SetOptionText(unsigned int index, const QString &text);
    void        *GetOptionData(unsigned int index) const;
    void         SetOptionData(unsigned int index, void *clientData);
    void         SetBackgroundColor(const StateColor &color);
    void         SetTextColor(const StateColor &color);
    void         SetButtonTextColor(int index, const StateColor &color);
    void         SetButtonCornerRadius(double radius);
    void         SetButtonPadding(const QSize &padding);
    void         Rescale();

signals:
    void selectionChanged(int index);

protected:
    void button_clicked();
    void update_button_styles();

private:
    StateColor m_bg_color;
    StateColor m_bg_color_grayed;
    StateColor m_text_color;
    StateColor m_text_color_grayed;
    double     m_button_radius  = 10.0;
    QSize      m_button_padding = {10, 6};
    std::vector<void *> m_option_data;
};

#endif // !slic3r_GUI_SwitchButton_hpp_

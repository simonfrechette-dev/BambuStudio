#include "ComboBox.hpp"
#include "Label.hpp"
#include "../QtExtensions.hpp"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <algorithm>

ComboBox::ComboBox(QWidget *parent, const QString &value,
                   int n, const QString choices[], long style)
    : TextInput(parent)
    , drop(this, items, style)
{
    if (n > 0 && choices) {
        for (int i = 0; i < n; ++i)
            Append(choices[i]);
    }
    if (!value.isEmpty()) SetValue(value);

    connect(&drop, &DropDown::dismissed,          this, &ComboBox::onDropDismissed);
    connect(&drop, &DropDown::selectionChanged,   this, &ComboBox::onDropSelectionChanged);

    // Arrow button setup — reuse the icon slot in TextInput
    SetIcon("drop_down");
    GetTextCtrl()->setReadOnly(true);
    GetTextCtrl()->setFocusPolicy(Qt::NoFocus);
}

bool ComboBox::setFont(const QFont &f)
{
    QWidget::setFont(f);
    drop.setFont(f);
    return true;
}

int ComboBox::Append(const QString &text, const QPixmap &bitmap, int item_style)
{
    items.push_back({text, {}, bitmap, {}, nullptr, {}, {}, {}, 0, item_style});
    return (int)items.size() - 1;
}

int ComboBox::Append(const QString &text, const QPixmap &bitmap,
                     void *clientData, int item_style)
{
    items.push_back({text, {}, bitmap, {}, clientData, {}, {}, {}, 0, item_style});
    return (int)items.size() - 1;
}

int ComboBox::Append(const QString &text, const QPixmap &bitmap,
                     const QString &group, void *clientData, int item_style)
{
    items.push_back({text, {}, bitmap, {}, clientData, group, {}, {}, 0, item_style});
    return (int)items.size() - 1;
}

int ComboBox::SetItems(const std::vector<DropDown::Item> &the_items)
{
    items = the_items;
    drop.Invalidate(true);
    return (int)items.size();
}

void ComboBox::set_replace_text(const QString &text, const QString &img)
{
    replace_text = text; image_for_text = img;
}

unsigned int ComboBox::GetCount() const { return (unsigned)items.size(); }

int ComboBox::GetSelection() const { return drop.GetSelection(); }

void ComboBox::SetSelection(int n)
{
    drop.SetSelection(n);
    if (n >= 0 && n < (int)items.size())
        GetTextCtrl()->setText(items[n].alias.isEmpty() ? items[n].text : items[n].alias);
}

void ComboBox::SelectAndNotify(int n)
{
    SetSelection(n);
    sendComboBoxEvent();
}

void ComboBox::Rescale()
{
    TextInput::Rescale();
    drop.Rescale();
}

QString ComboBox::GetValue() const { return GetTextCtrl() ? GetTextCtrl()->text() : QString{}; }
void    ComboBox::SetValue(const QString &v) { drop.SetValue(v); if (GetTextCtrl()) GetTextCtrl()->setText(v); }

void    ComboBox::SetLabel(const QString &l) { TextInput::SetLabel(l); }
QString ComboBox::GetLabel() const { return GetTextCtrl() ? GetTextCtrl()->text() : QString{}; }

int  ComboBox::GetFlag(unsigned int n) const { return n < items.size() ? items[n].flag : 0; }
void ComboBox::SetFlag(unsigned int n, int v) { if (n < items.size()) { items[n].flag = v; } }

void    ComboBox::SetTextLabel(const QString &l) { if (GetTextCtrl()) GetTextCtrl()->setText(l); }
QString ComboBox::GetTextLabel() const           { return GetTextCtrl() ? GetTextCtrl()->text() : QString{}; }

QString ComboBox::GetString(unsigned int n) const
{ return n < items.size() ? items[n].text : QString{}; }
void ComboBox::SetString(unsigned int n, const QString &v)
{ if (n < items.size()) { items[n].text = v; drop.Invalidate(); } }

QString ComboBox::GetItemTooltip(unsigned int n) const { return n < items.size() ? items[n].tip : QString{}; }
void    ComboBox::SetItemTooltip(unsigned int n, const QString &v) { if (n < items.size()) items[n].tip = v; }

QString ComboBox::GetItemAlias(unsigned int n) const { return n < items.size() ? items[n].alias : QString{}; }
void    ComboBox::SetItemAlias(unsigned int n, const QString &v) { if (n < items.size()) { items[n].alias = v; drop.Invalidate(); } }

QPixmap ComboBox::GetItemBitmap(unsigned int n) const { return n < items.size() ? items[n].icon : QPixmap{}; }
void    ComboBox::SetItemBitmap(unsigned int n, const QPixmap &bmp) { if (n < items.size()) { items[n].icon = bmp; drop.Invalidate(); } }

void ComboBox::DeleteOneItem(unsigned int pos)
{
    if (pos < items.size()) { items.erase(items.begin() + pos); drop.Invalidate(); }
}
void ComboBox::DoClear() { items.clear(); drop.Invalidate(true); }

void ComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (drop_down) {
            drop.Dismiss();
            drop_down = false;
        } else if (drop.HasDismissLongTime()) {
            drop.init(this, 0);
            drop.Popup(this);
            drop_down = true;
        }
        event->accept();
    } else {
        TextInput::mousePressEvent(event);
    }
}

void ComboBox::wheelEvent(QWheelEvent *event)
{
    const int cur = drop.GetSelection();
    const int cnt = (int)items.size();
    if (cnt == 0) return;
    int next = cur + (event->angleDelta().y() > 0 ? -1 : 1);
    next = std::clamp(next, 0, cnt - 1);
    if (next != cur) SelectAndNotify(next);
    event->accept();
}

void ComboBox::keyPressEvent(QKeyEvent *event)
{
    const int cur = drop.GetSelection();
    const int cnt = (int)items.size();
    if (event->key() == Qt::Key_Up && cur > 0)          { SelectAndNotify(cur - 1); event->accept(); return; }
    if (event->key() == Qt::Key_Down && cur < cnt - 1)  { SelectAndNotify(cur + 1); event->accept(); return; }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space) {
        if (drop_down) { drop.Dismiss(); drop_down = false; }
        else { drop.Popup(this); drop_down = true; }
        event->accept(); return;
    }
    TextInput::keyPressEvent(event);
}

void ComboBox::sendComboBoxEvent() { emit selectionChanged(drop.GetSelection()); }

void ComboBox::onDropDismissed()  { drop_down = false; }

void ComboBox::onDropSelectionChanged(int idx)
{
    if (idx >= 0 && idx < (int)items.size())
        GetTextCtrl()->setText(items[idx].alias.isEmpty() ? items[idx].text : items[idx].alias);
    drop_down = false;
    sendComboBoxEvent();
}

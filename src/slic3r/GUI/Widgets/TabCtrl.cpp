#include "TabCtrl.hpp"
#include "Label.hpp"

#include <QPainter>
#include <QKeyEvent>
#include <QResizeEvent>
#include <algorithm>

#define TAB_BUTTON_SPACE    2
#define TAB_BUTTON_PADDING  2, 2

TabCtrl::TabCtrl(QWidget *parent, const QPoint & /*pos*/, const QSize & /*size*/)
    : StaticBox(parent)
{
    radius       = 1;
    border_width = 1;
    border_color.append(QColor("#CECECE"), StateColor::Normal);

    sizer = new QHBoxLayout;
    sizer->setContentsMargins(10, 0, 10, 0);
    sizer->setSpacing(TAB_BUTTON_SPACE * 2);
    sizer->addStretch(1);
    setLayout(sizer);

    setFocusPolicy(Qt::StrongFocus);
}

TabCtrl::~TabCtrl() = default;

bool TabCtrl::setFont(const QFont &font)
{
    QWidget::setFont(font);
    bold_font = font;
    bold_font.setBold(true);
    for (int i = 0; i < (int)btns.size(); ++i)
        btns[i]->setFont(i == sel ? bold_font : font);
    return true;
}

int TabCtrl::AppendItem(const QString &item, int /*image*/, int /*selImage*/,
                         void *clientData)
{
    auto *btn = new Button(this, item);
    btn->setFont(font());
    btn->SetTextColor(StateColor(
        std::make_pair(QColor(0x6B, 0x6B, 0x6C), (int)StateColor::NotChecked),
        std::make_pair(Qt::lightGray,              (int)StateColor::Normal)));
    btn->SetBackgroundColor(StateColor());
    btn->SetCornerRadius(0);
    btn->SetPaddingSize(QSize(TAB_BUTTON_PADDING));
    btns.push_back(btn);
    item_data.push_back(clientData);
    // Insert before the trailing stretch
    sizer->insertWidget(sizer->count() - 1, btn);
    connect(btn, &Button::clicked, this, &TabCtrl::buttonClicked);
    relayout();
    return (int)btns.size() - 1;
}

bool TabCtrl::DeleteItem(int /*item*/) { return false; }

void TabCtrl::DeleteAllItems()
{
    for (auto *b : btns) { sizer->removeWidget(b); delete b; }
    btns.clear();
    item_data.clear();
    if (sel >= 0) {
        sel = -1;
        emit selChanged(sel);
    }
}

unsigned int TabCtrl::GetCount() const { return (unsigned)btns.size(); }

void TabCtrl::SelectItem(int item)
{
    if (item == sel) return;
    emit selChanging(item);
    // Toggle old selection
    if (sel >= 0 && sel < (int)btns.size())
        btns[sel]->SetValue(false);
    sel = item;
    if (sel >= 0 && sel < (int)btns.size()) {
        btns[sel]->SetValue(true);
        btns[sel]->setFont(bold_font);
    }
    emit selChanged(sel);
    relayout();
    update();
}

void TabCtrl::Unselect() { SelectItem(-1); }

void TabCtrl::Rescale()
{
    for (auto *b : btns) b->Rescale();
}

QString TabCtrl::GetItemText(unsigned int i) const
{
    return i < btns.size() ? btns[i]->text() : QString{};
}
void TabCtrl::SetItemText(unsigned int i, const QString &value)
{
    if (i < btns.size()) btns[i]->setText(value);
}
bool TabCtrl::GetItemBold(unsigned int i) const
{
    return i < btns.size() && btns[i]->font().bold();
}
void TabCtrl::SetItemBold(unsigned int i, bool b)
{
    if (i >= btns.size()) return;
    QFont f = b ? bold_font : font();
    btns[i]->setFont(f);
    btns[i]->Rescale();
}
void *TabCtrl::GetItemData(unsigned int i) const
{
    return i < item_data.size() ? item_data[i] : nullptr;
}
void TabCtrl::SetItemData(unsigned int i, void *d)
{
    if (i < item_data.size()) item_data[i] = d;
}
void TabCtrl::SetItemPaddingSize(unsigned int i, const QSize &sz)
{
    if (i < btns.size()) btns[i]->SetPaddingSize(sz);
}
void TabCtrl::SetItemTextColour(unsigned int i, const StateColor &col)
{
    if (i < btns.size()) btns[i]->SetTextColor(col);
}
int TabCtrl::GetFirstVisibleItem() const { return btns.empty() ? -1 : 0; }
int TabCtrl::GetNextVisible(int item) const
{
    return ++item < (int)btns.size() ? item : -1;
}

void TabCtrl::relayout()
{
    // simple: just show all, let Qt handle overflow
    for (auto *b : btns) b->setVisible(true);
    update();
}

void TabCtrl::buttonClicked()
{
    auto *btn = qobject_cast<Button *>(sender());
    auto it   = std::find(btns.begin(), btns.end(), btn);
    SelectItem(it == btns.end() ? -1 : (int)(it - btns.begin()));
}

void TabCtrl::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        if (sel > 0) SelectItem(sel - 1);
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        if (sel + 1 < (int)btns.size()) SelectItem(sel + 1);
        break;
    default:
        StaticBox::keyPressEvent(event);
    }
}

void TabCtrl::resizeEvent(QResizeEvent *event)
{
    StaticBox::resizeEvent(event);
    relayout();
}

void TabCtrl::doRender(QPainter &painter)
{
    StaticBox::doRender(painter);

    if (sel < 0 || sel >= (int)btns.size()) return;

    const int bw  = border_width;
    const int bs2 = (1 + bw) / 2;
    const int hy  = height() - bs2;

    // Bottom separator line
    const int states = state_handler.states();
    painter.setPen(QPen(border_color.colorForStates(states), bw));
    painter.drawLine(0, hy, width(), hy);

    // Green indicator under selected tab
    const QRect btnRect = btns[sel]->geometry();
    const double r = (double)radius;
    const QRectF indicator(btnRect.x() - r,
                            hy - bw * 3,
                            btnRect.width() + r * 2,
                            bw * 3);
    painter.setBrush(QColor("#00AE42"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(indicator, r, r);
}

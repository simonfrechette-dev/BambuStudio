#include "DropDown.hpp"
#include "Label.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <algorithm>

static constexpr int ITEM_HEIGHT    = 28;
static constexpr int ITEM_PAD_H     = 8;
static constexpr int ICON_W         = 20;
static constexpr int CHECK_W        = 20;
static constexpr int MAX_VISIBLE    = 12;

DropDown::DropDown(std::vector<Item> &items_)
    : PopupWindow(nullptr)
    , items(items_)
    , state_handler(this)
{}

DropDown::DropDown(QWidget *parent, std::vector<Item> &items_, long style)
    : PopupWindow(parent)
    , items(items_)
    , state_handler(this)
{
    init(parent, style);
}

void DropDown::init(QWidget * /*parent*/, long style)
{
    text_off = (style & DD_NO_TEXT) != 0;
    check_bitmap = ScalableBitmap(this, "check_on", 14);
    setMinimumWidth(120);
    dismissTime = std::chrono::steady_clock::now();
}

void DropDown::Invalidate(bool clear)
{
    if (clear) selection = -1;
    need_sync = true;
    update();
}

void DropDown::SetSelection(int n)
{
    selection = (n >= 0 && n < (int)items.size()) ? n : -1;
    update();
    emit selectionChanged(selection);
}

QString DropDown::GetValue() const
{
    if (selection >= 0 && selection < (int)items.size())
        return items[selection].text;
    return {};
}

void DropDown::SetValue(const QString &value)
{
    for (int i = 0; i < (int)items.size(); ++i) {
        if (items[i].text == value || items[i].alias == value) {
            SetSelection(i);
            return;
        }
    }
}

void DropDown::SetCornerRadius(double r)  { radius = r; update(); }
void DropDown::SetBorderColor(const StateColor &c) { border_color = c; update(); }
void DropDown::SetSelectorBorderColor(const StateColor &c) { selector_border_color = c; update(); }
void DropDown::SetTextColor(const StateColor &c) { text_color = c; update(); }
void DropDown::SetSelectorBackgroundColor(const StateColor &c) { selector_background_color = c; update(); }
void DropDown::SetUseContentWidth(bool use, bool limit) { use_content_width = use; limit_max_content_width = limit; }
void DropDown::SetAlignIcon(bool a) { align_icon = a; }
void DropDown::Rescale() { check_bitmap = ScalableBitmap(this, "check_on", 14); update(); }

bool DropDown::HasDismissLongTime() const
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - dismissTime).count() > 300;
}

void DropDown::Dismiss()
{
    dismissTime = std::chrono::steady_clock::now();
    hide();
    emit dismissed();
}

void DropDown::measureSize()
{
    QFontMetrics fm(Label::Body_14);
    int maxW = 0;
    for (const auto &it : items) {
        const int w = fm.horizontalAdvance(it.text);
        if (w > maxW) maxW = w;
    }
    const bool hasIcon = std::any_of(items.begin(), items.end(), [](const Item &i){ return !i.icon.isNull(); });
    int w = maxW + ITEM_PAD_H * 2;
    if (hasIcon) w += ICON_W + ITEM_PAD_H;
    w += CHECK_W + ITEM_PAD_H;
    const int visN  = std::min((int)items.size(), MAX_VISIBLE);
    const int h     = visN * ITEM_HEIGHT;
    resize(w, h);
}

void DropDown::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setFont(Label::Body_14);
    QFontMetrics fm(Label::Body_14);

    // Background
    p.setBrush(Qt::white);
    const QColor bc = border_color.count() > 0
        ? border_color.colorForStates(StateColor::Normal)
        : QColor(210, 210, 210);
    p.setPen(QPen(bc, 1));
    const QRect rcWidget = rect().adjusted(0, 0, -1, -1);
    if (radius > 0)
        p.drawRoundedRect(rcWidget, radius, radius);
    else
        p.drawRect(rcWidget);

    p.setClipRect(rect());

    const bool hasIcon = std::any_of(items.begin(), items.end(),
        [](const Item &i){ return !i.icon.isNull(); });

    int y = -scroll_offset;
    for (int i = 0; i < (int)items.size(); ++i) {
        const Item &item = items[i];
        const QRect row(0, y, width(), ITEM_HEIGHT);
        if (row.bottom() < 0) { y += ITEM_HEIGHT; continue; }
        if (row.top() > height()) break;

        // Split-item separator
        if (item.style & DD_ITEM_STYLE_SPLIT_ITEM) {
            p.setPen(QColor(220, 220, 220));
            p.drawLine(0, y + ITEM_HEIGHT / 2, width(), y + ITEM_HEIGHT / 2);
            y += ITEM_HEIGHT;
            continue;
        }

        const bool hovered  = (i == hover_item);
        const bool selected = (i == selection);

        // Hover / selected highlight
        if (hovered || selected) {
            QColor bg = selector_background_color.count() > 0
                ? selector_background_color.colorForStates(StateColor::Normal)
                : QColor(0xf5, 0xf5, 0xf5);
            p.setBrush(bg);
            p.setPen(Qt::NoPen);
            p.drawRect(row);
        }

        int x = ITEM_PAD_H;

        // Check-mark column
        if (selected && check_bitmap.bmp().IsOk()) {
            const QSize csz = check_bitmap.GetBmpSize();
            p.drawPixmap(QPoint(x + (CHECK_W - csz.width()) / 2,
                                y + (ITEM_HEIGHT - csz.height()) / 2),
                         check_bitmap.bmp());
        }
        x += CHECK_W + ITEM_PAD_H;

        // Icon
        if (hasIcon) {
            if (!item.icon.isNull())
                p.drawPixmap(QPoint(x, y + (ITEM_HEIGHT - item.icon.height()) / 2), item.icon);
            x += ICON_W + ITEM_PAD_H;
        }

        // Text
        if (!text_off) {
            const bool disabled = (item.style & DD_ITEM_STYLE_DISABLED) ||
                                   (item.style & DD_ITEM_STYLE_DIMMED);
            p.setPen(disabled ? QColor(180, 180, 180) :
                     text_color.count() > 0 ? text_color.colorForStates(StateColor::Normal) :
                     Qt::black);
            const QString display = item.alias.isEmpty() ? item.text : item.alias;
            const QRect textRect(x, y, width() - x - ITEM_PAD_H, ITEM_HEIGHT);
            p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, display);
        }

        y += ITEM_HEIGHT;
    }
}

int DropDown::hoverIndex() const { return hover_item; }
int DropDown::selectedItem() const { return selection; }

void DropDown::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) { pressedDown = true; event->accept(); }
    else PopupWindow::mousePressEvent(event);
}

void DropDown::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && pressedDown) {
        pressedDown = false;
        const int idx = (event->pos().y() + scroll_offset) / ITEM_HEIGHT;
        if (idx >= 0 && idx < (int)items.size()) {
            if (!(items[idx].style & DD_ITEM_STYLE_DISABLED) &&
                !(items[idx].style & DD_ITEM_STYLE_SPLIT_ITEM)) {
                SetSelection(idx);
                sendDropDownEvent();
                Dismiss();
            }
        }
        event->accept();
    } else {
        pressedDown = false;
        PopupWindow::mouseReleaseEvent(event);
    }
}

void DropDown::mouseMoveEvent(QMouseEvent *event)
{
    const int idx = (event->pos().y() + scroll_offset) / ITEM_HEIGHT;
    if (idx != hover_item) { hover_item = idx; update(); }
    PopupWindow::mouseMoveEvent(event);
}

void DropDown::wheelEvent(QWheelEvent *event)
{
    scroll_offset -= event->angleDelta().y() / 8;
    const int maxScroll = std::max(0, (int)items.size() * ITEM_HEIGHT - height());
    scroll_offset = std::clamp(scroll_offset, 0, maxScroll);
    update();
    event->accept();
}

void DropDown::sendDropDownEvent()
{
    emit selectionChanged(selection);
}

#include "SwitchButton.hpp"
#include "../QtExtensions.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QApplication>

// ============================================================
// SwitchButton
// ============================================================

SwitchButton::SwitchButton(QWidget *parent, int /*id*/)
    : QAbstractButton(parent)
    , m_on(this,  "toggle_on",  16)
    , m_off(this, "toggle_off", 16)
    , text_color(std::make_pair(QColor(0xFF, 0xFF, 0xFE), (int)StateColor::Checked),
                 std::make_pair(QColor(0x6B, 0x6B, 0x6B), (int)StateColor::Normal))
    , track_color(QColor(0xD9, 0xD9, 0xD9))
    , thumb_color(std::make_pair(QColor(0x00, 0xAE, 0x42), (int)StateColor::Checked),
                  std::make_pair(QColor(0xD9, 0xD9, 0xD9), (int)StateColor::Normal))
{
    setCheckable(true);
    setFont(Label::Body_12);
    const QSize sz = m_on.GetBmpSize();
    setFixedSize(sz);
}

void SwitchButton::SetLabels(const QString &lbl_on, const QString &lbl_off)
{
    m_labels[0] = lbl_on;
    m_labels[1] = lbl_off;
    Rescale();
}

void SwitchButton::SetTextColor(const StateColor &color)  { text_color  = color; }
void SwitchButton::SetTextColor2(const StateColor &color) { text_color2 = color; }
void SwitchButton::SetTrackColor(const StateColor &color) { track_color = color; update(); }
void SwitchButton::SetThumbColor(const StateColor &color) { thumb_color = color; update(); }

void SwitchButton::SetValue(bool value)
{
    if (m_value != value) {
        m_value = value;
        update();
        emit toggled(m_value);
    }
}

void SwitchButton::Rescale()
{
    if (m_labels[0].isEmpty()) {
        const QSize sz = m_on.GetBmpSize();
        setFixedSize(sz);
    } else {
        // Size based on text labels
        QFontMetrics fm(font());
        const QSize t0 = fm.boundingRect(m_labels[0]).size();
        const QSize t1 = fm.boundingRect(m_labels[1]).size();
        const int thumbW = std::max(t0.width(), t1.width()) + 12;
        const int thumbH = std::max(t0.height(), t1.height()) + 6;
        const int trackW = thumbW + t1.width() + 10;
        const int trackH = thumbH + 2;
        setFixedSize(trackW, trackH);
    }
    update();
}

QSize SwitchButton::sizeHint() const { return size(); }

void SwitchButton::update()
{
    QWidget::update();
}

void SwitchButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_labels[0].isEmpty()) {
        // Bitmap mode
        const QPixmap &px = m_value ? m_on.bmp() : m_off.bmp();
        p.drawPixmap(QPoint(0, 0), px);
        return;
    }

    // Text-label mode
    const QRect rc = rect();
    const int   h  = rc.height();
    const int   r  = h / 2;
    QFontMetrics fm(font());
    const QSize t0 = fm.boundingRect(m_labels[0]).size();
    const QSize t1 = fm.boundingRect(m_labels[1]).size();
    const int   tw = std::max(t0.width(), t1.width());
    const int   th = h - 2;

    // Track
    const int state0 = StateColor::Enabled | (m_value ? StateColor::Checked : 0);
    const int state1 = StateColor::Enabled | (m_value ? 0 : StateColor::Checked);
    p.setBrush(track_color.colorForStates(m_value ? StateColor::Checked | StateColor::Enabled : StateColor::Enabled));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rc, r, r);

    // Thumb
    const int thumbX = m_value ? rc.width() - tw - 12 - 1 : 1;
    p.setBrush(thumb_color.colorForStates(StateColor::Checked | StateColor::Enabled));
    p.drawRoundedRect(thumbX, 1, tw + 12, th, (th) / 2, (th) / 2);

    // Labels
    p.setFont(font());
    // left label
    p.setPen(text_color.colorForStates(m_value ? StateColor::Checked | StateColor::Enabled : StateColor::Enabled));
    p.drawText(QRect(1, 1, tw + 12, th), Qt::AlignCenter, m_labels[0]);
    // right label
    const int tc2 = text_color2.count();
    p.setPen((tc2 ? text_color2 : text_color).colorForStates(m_value ? StateColor::Enabled : StateColor::Checked | StateColor::Enabled));
    p.drawText(QRect(rc.width() - tw - 12 - 1, 1, tw + 12, th), Qt::AlignCenter, m_labels[1]);
}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) event->accept();
    else QAbstractButton::mousePressEvent(event);
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        SetValue(!m_value);
        event->accept();
    } else {
        QAbstractButton::mouseReleaseEvent(event);
    }
}

// ============================================================
// SwitchBoard
// ============================================================

SwitchBoard::SwitchBoard(QWidget *parent, const QString &leftL,
                         const QString &right, const QSize &size)
    : QWidget(parent)
    , leftLabel(leftL)
    , rightLabel(right)
{
    if (!size.isEmpty()) setFixedSize(size);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_NoSystemBackground);
}

void SwitchBoard::updateState(const QString &target)
{
    if (target.isEmpty()) {
        if (!switch_left && !switch_right) return;
        switch_left = switch_right = false;
    } else if (target == QStringLiteral("left")) {
        if (switch_left && !switch_right) return;
        switch_left = true; switch_right = false;
    } else if (target == QStringLiteral("right")) {
        if (!switch_left && switch_right) return;
        switch_left = false; switch_right = true;
    }
    update();
}

void SwitchBoard::SetLabels(const QString &left, const QString &right)
{
    if (leftLabel == left && rightLabel == right) return;
    leftLabel = left; rightLabel = right;
    update();
}

void SwitchBoard::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    doRender(p);
}

void SwitchBoard::doRender(QPainter &p)
{
    const QColor disable_color(0xCE, 0xCE, 0xCE);
    const QRectF rc = rect();
    const double r  = 8.0;
    const int    hw = rc.width() / 2;

    // Background
    p.setPen(Qt::NoPen);
    p.setBrush(is_enable ? QColor(0xEE, 0xEE, 0xEE) : disable_color);
    p.drawRoundedRect(rc, r, r);

    // Left highlight
    if (switch_left) {
        p.setBrush(is_enable ? QColor(0, 174, 66) : disable_color);
        p.drawRoundedRect(QRectF(0, 0, hw, rc.height()), r, r);
    }
    // Right highlight
    if (switch_right) {
        p.setBrush(is_enable ? QColor(0, 174, 66) : disable_color);
        p.drawRoundedRect(QRectF(hw, 0, hw, rc.height()), r, r);
    }

    QFontMetrics fm(Label::Body_13);
    p.setFont(Label::Body_13);

    // Left text
    p.setPen(switch_left ? Qt::white : QColor(0x33, 0x33, 0x33));
    p.drawText(QRectF(0, 0, hw, rc.height()), Qt::AlignCenter, leftLabel);

    // Right text
    p.setPen(switch_right ? Qt::white : QColor(0x33, 0x33, 0x33));
    p.drawText(QRectF(hw, 0, hw, rc.height()), Qt::AlignCenter, rightLabel);
}

void SwitchBoard::mousePressEvent(QMouseEvent *event)
{
    if (!is_enable) { event->accept(); return; }
    switch_left  = event->pos().x() < width() / 2;
    switch_right = !switch_left;
    if (auto_disable_when_switch) is_enable = false;
    update();
    emit switchPos(switch_left);
}

// ============================================================
// CustomToggleButton
// ============================================================

CustomToggleButton::CustomToggleButton(QWidget *parent, const QString &label,
                                       int /*id*/, const QPoint & /*pos*/,
                                       const QSize & /*size*/)
    : QWidget(parent)
    , m_label(label)
{
    SetSelectedIcon(QStringLiteral("switch_send_mode_tag_on"));
    SetUnSelectedIcon(QStringLiteral("switch_send_mode_tag_off"));
    setAutoFillBackground(false);
    setCursor(Qt::PointingHandCursor);
}

void CustomToggleButton::setText(const QString &label) { m_label = label; update(); }

void CustomToggleButton::SetSelectedIcon(const QString &iconPath)
{
    m_selected_icon = Slic3r::GUI::create_scaled_pixmap(iconPath.toStdString(), this, 16);
    update();
}

void CustomToggleButton::SetUnSelectedIcon(const QString &iconPath)
{
    m_unselected_icon = Slic3r::GUI::create_scaled_pixmap(iconPath.toStdString(), this, 16);
    update();
}

void CustomToggleButton::SetIsSelected(bool selected) { m_isSelected = selected; update(); }

void CustomToggleButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        SetIsSelected(true);
        emit clicked();
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void CustomToggleButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF rc = rect();
    QFontMetrics fm(Label::Head_13);
    const QSize  textSz = fm.boundingRect(m_label).size();
    const QPixmap &icon = m_isSelected ? m_selected_icon : m_unselected_icon;
    const int iw = icon.isNull() ? 0 : icon.width();
    const int gap = iw > 0 ? 6 : 0;
    const int totalW = iw + gap + textSz.width();
    int left = (rc.width() - totalW) / 2;

    // Background
    if (m_isSelected) {
        p.setBrush(m_secondary_colour);
        p.setPen(QPen(m_primary_colour));
    } else {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#EEEEEE")));
    }
    p.drawRoundedRect(rc, 5, 5);

    // Icon
    if (!icon.isNull()) {
        int iy = (rc.height() - icon.height()) / 2;
        p.drawPixmap(QPoint(left, iy), icon);
        left += iw + gap;
    }

    // Text
    p.setFont(Label::Head_13);
    p.setPen(m_isSelected ? m_primary_colour : QColor("#5C5C5C"));
    int ty = (rc.height() - textSz.height()) / 2;
    p.drawText(QPoint(left, ty + fm.ascent()), m_label);
}

// ============================================================
// RichTooltipPopup
// ============================================================

RichTooltipPopup::RichTooltipPopup(QWidget *parent,
                                   const QString &iconName,
                                   const QString &text)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
    , m_text(text)
{
    if (!iconName.isEmpty())
        m_icon = Slic3r::GUI::create_scaled_pixmap(iconName.toStdString(), this, 32);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    // Compute size
    QFontMetrics fm(Label::Body_13);
    const QSize textSz = fm.boundingRect(m_text).size();
    const int iw = m_icon.isNull() ? 0 : m_icon.width() + 12;
    resize(iw + textSz.width() + 24, std::max(textSz.height() + 16,
           m_icon.isNull() ? 0 : m_icon.height() + 16));
}

void RichTooltipPopup::ShowAtPosition(QWidget *anchor)
{
    const QPoint pos = anchor->mapToGlobal(QPoint(0, anchor->height() + 4));
    const int cx = pos.x() + (anchor->width() - width()) / 2;
    move(cx, pos.y());
    show();
    raise();
}

void RichTooltipPopup::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(50, 50, 50));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect(), 4, 4);

    int x = 12;
    if (!m_icon.isNull()) {
        p.drawPixmap(QPoint(x, (height() - m_icon.height()) / 2), m_icon);
        x += m_icon.width() + 12;
    }
    p.setFont(Label::Body_13);
    p.setPen(Qt::white);
    p.drawText(QRect(x, 0, width() - x - 12, height()), Qt::AlignVCenter | Qt::AlignLeft, m_text);
}

// ============================================================
// ExpandButton
// ============================================================

ExpandButton::ExpandButton(QWidget *parent, const std::string &bmp,
                           int /*id*/, const QPoint & /*pos*/,
                           const QSize & /*size*/)
    : QWidget(parent)
    , m_bmp_str(bmp)
{
    m_bmp = Slic3r::GUI::create_scaled_pixmap(bmp, this, 18);
    setFixedSize(24, 24);
    setCursor(Qt::PointingHandCursor);
}

ExpandButton::~ExpandButton()
{
    HideRichTooltip();
}

void ExpandButton::update_bitmap(const std::string &bmp)
{
    m_bmp_str = bmp;
    m_bmp = Slic3r::GUI::create_scaled_pixmap(bmp, this, 18);
    update();
}

void ExpandButton::msw_rescale() { update_bitmap(m_bmp_str); }

void ExpandButton::SetRichTooltip(const QString &iconName, const QString &text)
{
    m_tooltip_icon = iconName;
    m_tooltip_text = text;
}

void ExpandButton::ShowRichTooltip()
{
    if (m_tooltip_text.isEmpty()) return;
    HideRichTooltip();
    m_tooltip_popup = new RichTooltipPopup(this, m_tooltip_icon, m_tooltip_text);
    m_tooltip_popup->ShowAtPosition(this);
}

void ExpandButton::HideRichTooltip()
{
    if (m_tooltip_popup) {
        m_tooltip_popup->hide();
        m_tooltip_popup->deleteLater();
        m_tooltip_popup = nullptr;
    }
}

void ExpandButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (!m_bmp.isNull()) {
        int x = (width()  - m_bmp.width())  / 2;
        int y = (height() - m_bmp.height()) / 2;
        p.drawPixmap(QPoint(x, y), m_bmp);
    }
}

void ExpandButton::enterEvent(QEnterEvent *event)
{
    ShowRichTooltip();
    QWidget::enterEvent(event);
}

void ExpandButton::leaveEvent(QEvent *event)
{
    HideRichTooltip();
    QWidget::leaveEvent(event);
}

void ExpandButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        HideRichTooltip();
        emit expandClicked(property("_bbl_id").toInt());
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

// ============================================================
// ExpandButtonHolder
// ============================================================

ExpandButtonHolder::ExpandButtonHolder(QWidget *parent, int /*id*/,
                                       const QPoint & /*pos*/,
                                       const QSize & /*size*/)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#3B4446"));
    setPalette(p);

    hsizer = new QHBoxLayout(this);
    hsizer->setContentsMargins(12, 0, 12, 0);
    hsizer->setSpacing(6);
    hsizer->addStretch(1);
    setLayout(hsizer);
}

int ExpandButtonHolder::GetAvailable()
{
    int cnt = 0;
    for (auto *c : findChildren<ExpandButton *>())
        if (c->isVisible()) ++cnt;
    return cnt;
}

void ExpandButtonHolder::addExpandButton(int id, const std::string &img)
{
    auto *btn = new ExpandButton(this, img);
    btn->setProperty("_bbl_id", id);
    connect(btn, &ExpandButton::expandClicked, this, &ExpandButtonHolder::expandClicked);
    hsizer->insertWidget(hsizer->count() - 1, btn);
}

ExpandButton *ExpandButtonHolder::FindExpandButton(int id)
{
    for (auto *c : findChildren<ExpandButton *>())
        if (c->property("_bbl_id").toInt() == id) return c;
    return nullptr;
}

void ExpandButtonHolder::ShowExpandButton(int id, bool show)
{
    if (auto *b = FindExpandButton(id)) b->setVisible(show);
}

void ExpandButtonHolder::updateExpandButtonBitmap(int id, const std::string &bitmap)
{
    if (auto *b = FindExpandButton(id)) b->update_bitmap(bitmap);
}

void ExpandButtonHolder::EnableExpandButton(int id, bool enb)
{
    if (auto *b = FindExpandButton(id)) b->setEnabled(enb);
}

void ExpandButtonHolder::SetExpandButtonTooltip(int id, const QString &tooltip)
{
    if (auto *b = FindExpandButton(id)) b->setToolTip(tooltip);
}

void ExpandButtonHolder::SetExpandButtonRichTooltip(int id, const QString &iconName, const QString &text)
{
    if (auto *b = FindExpandButton(id)) b->SetRichTooltip(iconName, text);
}

void ExpandButtonHolder::msw_rescale()
{
    for (auto *b : findChildren<ExpandButton *>()) b->msw_rescale();
}

// ============================================================
// MultiSwitchButton
// ============================================================

MultiSwitchButton::MultiSwitchButton(QWidget *parent)
    : StaticBox(parent)
    , m_bg_color(std::make_pair(QColor(0xE8, 0xE8, 0xE8), (int)StateColor::NotChecked),
                 std::make_pair(QColor(0x00, 0xAE, 0x42), (int)StateColor::Normal))
    , m_text_color(std::make_pair(QColor(0x6B, 0x6B, 0x6B), (int)StateColor::NotChecked),
                   std::make_pair(QColor(0xFF, 0xFF, 0xFE), (int)StateColor::Normal))
{
    sizer = new QHBoxLayout(this);
    sizer->setContentsMargins(2, 2, 2, 2);
    sizer->setSpacing(0);
    setLayout(sizer);
}

MultiSwitchButton::~MultiSwitchButton() = default;

int MultiSwitchButton::AppendOption(const QString &option, void *clientData)
{
    auto *btn = new Button(this, option);
    btn->SetTextColor(m_text_color);
    btns.push_back(btn);
    m_option_data.push_back(clientData);
    sizer->addWidget(btn);
    connect(btn, &Button::clicked, this, &MultiSwitchButton::button_clicked);
    update_button_styles();
    return (int)btns.size() - 1;
}

void MultiSwitchButton::SetOptions(const std::vector<QString> &options)
{
    DeleteAllOptions();
    for (auto &o : options) AppendOption(o);
}

void MultiSwitchButton::DeleteAllOptions()
{
    for (auto *b : btns) { sizer->removeWidget(b); delete b; }
    btns.clear();
    m_option_data.clear();
    sel = -1;
}

void MultiSwitchButton::SetSelection(int index)
{
    if (index < 0 || index >= (int)btns.size()) return;
    sel = index;
    update_button_styles();
    emit selectionChanged(sel);
}

void MultiSwitchButton::button_clicked()
{
    auto *btn = qobject_cast<Button *>(sender());
    for (int i = 0; i < (int)btns.size(); ++i) {
        if (btns[i] == btn) { SetSelection(i); return; }
    }
}

void MultiSwitchButton::update_button_styles()
{
    for (int i = 0; i < (int)btns.size(); ++i) {
        const bool checked = (i == sel);
        btns[i]->SetTextColor(
            checked ? StateColor(m_text_color.colorForStates(StateColor::Normal))
                    : StateColor(m_text_color.colorForStates(StateColor::NotChecked)));
        btns[i]->update();
    }
    update();
}

QString MultiSwitchButton::GetSelectedText() const
{
    return (sel >= 0 && sel < (int)btns.size()) ? btns[sel]->text() : QString{};
}

QString MultiSwitchButton::GetOptionText(unsigned int i) const
{
    return i < btns.size() ? btns[i]->text() : QString{};
}
void MultiSwitchButton::SetOptionText(unsigned int i, const QString &t)
{
    if (i < btns.size()) { btns[i]->setText(t); }
}

void *MultiSwitchButton::GetOptionData(unsigned int i) const
{
    return i < m_option_data.size() ? m_option_data[i] : nullptr;
}
void MultiSwitchButton::SetOptionData(unsigned int i, void *d)
{
    if (i < m_option_data.size()) m_option_data[i] = d;
}

void MultiSwitchButton::SetBackgroundColor(const StateColor &color) { m_bg_color = color; update(); }
void MultiSwitchButton::SetTextColor(const StateColor &color)       { m_text_color = color; update_button_styles(); }
void MultiSwitchButton::SetButtonTextColor(int i, const StateColor &color) {
    if (i >= 0 && i < (int)btns.size()) btns[i]->SetTextColor(color);
}
void MultiSwitchButton::SetButtonCornerRadius(double r) { m_button_radius = r; }
void MultiSwitchButton::SetButtonPadding(const QSize &p) { m_button_padding = p; }

void MultiSwitchButton::Rescale() { update(); }

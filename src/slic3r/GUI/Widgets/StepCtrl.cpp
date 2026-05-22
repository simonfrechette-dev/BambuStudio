#include "StepCtrl.hpp"
#include "Label.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QMouseEvent>

// ============================================================
// StepCtrlBase
// ============================================================

StepCtrlBase::StepCtrlBase(QWidget *parent) : StaticBox(parent)
{
    font_tip = Label::Body_12;
    setMinimumSize(200, 40);
}
StepCtrlBase::~StepCtrlBase() = default;

void StepCtrlBase::SetHint(const QString &h) { hint = h; update(); }
bool StepCtrlBase::SetTipFont(const QFont &f) { font_tip = f; update(); return true; }

int StepCtrlBase::AppendItem(const QString &item, const QString &tip)
{
    steps.push_back(item);
    tips.push_back(tip);
    update();
    return (int)steps.size() - 1;
}
void StepCtrlBase::DeleteAllItems() { steps.clear(); tips.clear(); step = -1; update(); }
unsigned int StepCtrlBase::GetCount() const { return (unsigned)steps.size(); }
int  StepCtrlBase::GetSelection() const { return step; }
void StepCtrlBase::Idle() { step = -1; update(); }
void StepCtrlBase::SelectItem(int item)
{
    if (item < 0 || item >= (int)steps.size()) return;
    if (sendStepCtrlEvent(true)) {
        step = item;
        sendStepCtrlEvent(false);
        update();
    }
}
QString StepCtrlBase::GetItemText(unsigned int item) const
{
    return item < steps.size() ? steps[item] : QString{};
}
int StepCtrlBase::GetItemUseText(const QString &txt) const
{
    for (int i = 0; i < (int)steps.size(); ++i)
        if (steps[i] == txt) return i;
    return -1;
}
void StepCtrlBase::SetItemText(unsigned int item, const QString &v)
{
    if (item < steps.size()) { steps[item] = v; update(); }
}
bool StepCtrlBase::sendStepCtrlEvent(bool changing)
{
    if (changing) emit stepChanging(step);
    else          emit stepChanged(step);
    return true;
}

// ============================================================
// StepCtrl
// ============================================================

StepCtrl::StepCtrl(QWidget *parent) : StepCtrlBase(parent)
{
    bmp_thumb = ScalableBitmap(this, "step_thumb", 16);
    setMouseTracking(true);
}

void StepCtrl::Rescale() { bmp_thumb = ScalableBitmap(this, "step_thumb", 16); update(); }

void StepCtrl::doRender(QPainter &p)
{
    StaticBox::doRender(p);
    if (steps.empty()) return;

    p.setRenderHint(QPainter::Antialiasing);
    const QRect rc = rect();
    const int n = (int)steps.size();
    const int itemW = rc.width() / n;
    const int barY  = rc.height() / 2;

    // Background bar
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(220, 220, 220));
    p.drawRoundedRect(QRect(0, barY - bar_width / 2, rc.width(), bar_width), bar_width / 2, bar_width / 2);

    // Filled portion
    if (step >= 0) {
        const int filled = (step + 1) * itemW;
        QColor fc = clr_bar.count() > 0 ? clr_bar.colorForStates(StateColor::Normal) : QColor(0, 0xae, 0x42);
        p.setBrush(fc);
        p.drawRoundedRect(QRect(0, barY - bar_width / 2, filled, bar_width), bar_width / 2, bar_width / 2);
    }

    // Step circles
    QFont f = Label::Body_12;
    p.setFont(f);
    QFontMetrics fm(f);
    for (int i = 0; i < n; ++i) {
        const int cx = itemW * i + itemW / 2;
        const bool done = (i <= step);
        const QColor cc = done ? QColor(0, 0xae, 0x42) : QColor(180, 180, 180);
        p.setPen(Qt::NoPen);
        p.setBrush(cc);
        p.drawEllipse(QPoint(cx, barY), radius, radius);
        p.setPen(done ? Qt::white : Qt::black);
        const QString num = QString::number(i + 1);
        const QRect tr = fm.boundingRect(num);
        p.drawText(QPoint(cx - tr.width() / 2, barY + tr.height() / 2 - fm.descent()), num);
        // Label
        p.setPen(QColor(60, 60, 60));
        p.setFont(font_tip);
        p.drawText(QRect(itemW * i, barY + radius + 2, itemW, 20), Qt::AlignCenter, steps[i]);
        p.setFont(f);
    }
}

void StepCtrl::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) { dragging = true; drag_offset = e->pos(); }
    StaticBox::mousePressEvent(e);
}
void StepCtrl::mouseMoveEvent(QMouseEvent *) {}
void StepCtrl::mouseReleaseEvent(QMouseEvent *e)
{
    if (dragging) {
        dragging = false;
        const int n = (int)steps.size();
        if (n > 0) {
            const int itemW = rect().width() / n;
            int idx = e->pos().x() / itemW;
            if (idx >= 0 && idx < n) SelectItem(idx);
        }
    }
    StaticBox::mouseReleaseEvent(e);
}

// ============================================================
// StepIndicator
// ============================================================

StepIndicator::StepIndicator(QWidget *parent) : StepCtrlBase(parent)
{
    bmp_ok = ScalableBitmap(this, "step_ok", 16);
}
void StepIndicator::Rescale() { bmp_ok = ScalableBitmap(this, "step_ok", 16); update(); }
void StepIndicator::SelectNext() { SelectItem(step + 1); }

void StepIndicator::doRender(QPainter &p)
{
    StaticBox::doRender(p);
    if (steps.empty()) return;
    p.setRenderHint(QPainter::Antialiasing);
    const QRect rc = rect();
    const int n    = (int)steps.size();
    const int itemW = rc.width() / n;
    const int barY  = rc.height() / 2;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(220, 220, 220));
    p.drawRoundedRect(QRect(0, barY - bar_width / 2, rc.width(), bar_width), bar_width / 2, bar_width / 2);

    QFontMetrics fm(font_tip);
    for (int i = 0; i < n; ++i) {
        const int cx = itemW * i + itemW / 2;
        const bool done = (i < step);
        const bool cur  = (i == step);
        QColor cc = done ? QColor(0, 0xae, 0x42) : (cur ? QColor(50, 120, 250) : QColor(180, 180, 180));
        p.setPen(Qt::NoPen);
        p.setBrush(cc);
        p.drawEllipse(QPoint(cx, barY), radius, radius);
        if (done && bmp_ok.bmp().IsOk()) {
            const QSize  sz = bmp_ok.GetBmpSize();
            p.drawPixmap(QPoint(cx - sz.width() / 2, barY - sz.height() / 2), bmp_ok.bmp());
        }
        p.setPen(QColor(60, 60, 60));
        p.setFont(font_tip);
        p.drawText(QRect(itemW * i, barY + radius + 2, itemW, 20), Qt::AlignCenter, steps[i]);
    }
}

// ============================================================
// FilamentStepIndicator
// ============================================================

FilamentStepIndicator::FilamentStepIndicator(QWidget *parent) : StepCtrlBase(parent)
{
    bmp_ok = ScalableBitmap(this, "step_ok", 16);
}
void FilamentStepIndicator::Rescale() { bmp_ok = ScalableBitmap(this, "step_ok", 16); update(); }
void FilamentStepIndicator::SelectNext() { SelectItem(step + 1); }
void FilamentStepIndicator::SetSlotInformation(const QString &slot)
{
    m_slot_information = slot; update();
}

void FilamentStepIndicator::doRender(QPainter &p)
{
    // Same as StepIndicator but with slot info overlaid
    StepIndicator tmp(parentWidget()); // proxy — just delegate to base rendering
    StaticBox::doRender(p);
    p.setRenderHint(QPainter::Antialiasing);
    const QRect rc = rect();
    const int n    = (int)steps.size();
    if (n == 0) return;
    const int itemW = rc.width() / n;
    const int barY  = rc.height() / 2;

    QFontMetrics fm(font_tip);
    for (int i = 0; i < n; ++i) {
        const int cx = itemW * i + itemW / 2;
        const bool done = (i < step);
        QColor cc = done ? QColor(0, 0xae, 0x42) : (i == step ? QColor(50, 120, 250) : QColor(180, 180, 180));
        p.setPen(Qt::NoPen);
        p.setBrush(cc);
        p.drawEllipse(QPoint(cx, barY), radius, radius);
        p.setPen(QColor(60, 60, 60));
        p.setFont(font_tip);
        p.drawText(QRect(itemW * i, barY + radius + 2, itemW, 20), Qt::AlignCenter, steps[i]);
    }
    if (!m_slot_information.isEmpty()) {
        p.setPen(QColor(90, 90, 90));
        p.setFont(Label::Body_10);
        p.drawText(rc, Qt::AlignBottom | Qt::AlignHCenter, m_slot_information);
    }
}

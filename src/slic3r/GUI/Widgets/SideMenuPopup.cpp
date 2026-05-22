#include "SideMenuPopup.hpp"
#include <QVBoxLayout>
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>

SidePopup::SidePopup(QWidget *parent)
    : PopupWindow(parent)
{}

SidePopup::~SidePopup() = default;

void SidePopup::Create()
{
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) {
        layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(2);
        setLayout(layout);
    }
    // Clear old buttons from layout, then re-add
    while (QLayoutItem *item = layout->takeAt(0))
        delete item;
    for (SideButton *btn : btn_list) {
        layout->addWidget(btn);
        connect(btn, &SideButton::clicked, this, &SidePopup::Dismiss);
    }
}

void SidePopup::Popup(QWidget *anchor)
{
    Create();
    PopupWindow::Popup(anchor);
}

void SidePopup::Dismiss()
{
    PopupWindow::Dismiss();
}

void SidePopup::append_button(SideButton *btn)
{
    btn_list.push_back(btn);
    btn->setParent(this);
}

void SidePopup::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(255, 255, 255));
    p.setPen(QColor(210, 210, 210));
    p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 4, 4);
    QFrame::paintEvent(event);
}

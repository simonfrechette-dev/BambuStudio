#include "PopupWindow.hpp"

#include <QEvent>
#include <QFocusEvent>
#include <QApplication>

void PopupWindow::init()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameShape(QFrame::NoFrame);
}

void PopupWindow::Popup(QWidget *anchor)
{
    if (anchor) {
        const QPoint pt = anchor->mapToGlobal(QPoint(0, anchor->height()));
        move(pt);
    }
    show();
    raise();
    activateWindow();
}

void PopupWindow::Dismiss()
{
    hide();
}

bool PopupWindow::event(QEvent *e)
{
    // Qt::Popup windows dismiss on click-outside automatically,
    // but we also want to dismiss when the top-level window loses focus.
    if (e->type() == QEvent::WindowDeactivate ||
        e->type() == QEvent::Hide) {
        // don't recurse
        if (e->type() == QEvent::WindowDeactivate)
            Dismiss();
    }
    return QFrame::event(e);
}

void PopupWindow::focusOutEvent(QFocusEvent *e)
{
    QFrame::focusOutEvent(e);
    // Qt::Popup already handles auto-hide, but keep this for subclasses
    // that might use other window flags.
}

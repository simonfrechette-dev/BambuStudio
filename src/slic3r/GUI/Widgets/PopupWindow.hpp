#ifndef slic3r_GUI_PopupWindow_hpp_
#define slic3r_GUI_PopupWindow_hpp_

#include <QFrame>

/**
 * Lightweight popup window — shown as a frameless window/tool-window.
 * Equivalent of QWidget: auto-dismisses when parent loses
 * focus, user clicks outside, or the top-level window is minimised/hidden.
 */
class PopupWindow : public QFrame
{
    Q_OBJECT
public:
    PopupWindow() : QFrame(nullptr) { init(); }
    explicit PopupWindow(QWidget *parent, Qt::WindowFlags flags = Qt::Popup)
        : QFrame(parent, flags) { init(); }

    // Show anchored below `anchor` (bottom-left of anchor by default).
    virtual void Popup(QWidget *anchor = nullptr);
    // Hide the popup.
    virtual void Dismiss();

protected:
    bool event(QEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    void init();
};

#endif // !slic3r_GUI_PopupWindow_hpp_

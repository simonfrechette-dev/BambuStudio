#ifndef slic3r_GUI_StateHandler_hpp_
#define slic3r_GUI_StateHandler_hpp_

#include <QObject>
#include <memory>
#include <vector>

#include "StateColor.hpp"

class QWidget;
class QEvent;

class StateHandler : public QObject
{
    Q_OBJECT
public:
    enum State {
        Enabled    = 1,
        Checked    = 2,
        Focused    = 4,
        Hovered    = 8,
        Pressed    = 16,
        Disabled   = 1 << 16,
        NotChecked = 2 << 16,
        NotFocused = 4 << 16,
        NotHovered = 8 << 16,
        NotPressed = 16 << 16,
    };

public:
    explicit StateHandler(QWidget *owner, QObject *parent = nullptr);

    ~StateHandler();

public:
    void attach(StateColor const &color);

    void attach(std::vector<StateColor const *> const &colors);

    void attach_child(QWidget *child);

    void remove_child(QWidget *child);

    void update_binds();

    int states() const { return states_ | states2_; }

    void set_state(int state, int mask);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    StateHandler(StateHandler *parent, QWidget *owner);

    void triggerRefresh();

private:
    QWidget                                  *owner_;
    std::vector<StateColor const *>           colors_;
    int                                       bind_states_ = 0;
    int                                       states_      = 0;
    int                                       states2_     = 0; // accumulated from children
    std::vector<std::unique_ptr<StateHandler>> children_;
    StateHandler                             *parent_      = nullptr;
};

#endif // !slic3r_GUI_StateHandler_hpp_

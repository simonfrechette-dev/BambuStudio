#include "StateHandler.hpp"

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

StateHandler::StateHandler(QWidget *owner, QObject *parent)
    : QObject(parent)
    , owner_(owner)
{
    owner_->installEventFilter(this);
    if (owner->isEnabled())
        states_ |= Enabled;
    if (owner->hasFocus())
        states_ |= Focused;
}

StateHandler::~StateHandler() { owner_->removeEventFilter(this); }

void StateHandler::attach(StateColor const &color)
{
    colors_.push_back(&color);
}

void StateHandler::attach(std::vector<StateColor const *> const & colors)
{
    colors_.insert(colors_.end(), colors.begin(), colors.end());
}

void StateHandler::attach_child(QWidget *child)
{
    auto ch = new StateHandler(this, child);
    children_.emplace_back(ch);
    ch->update_binds();
    states2_ |= ch->states();
}

void StateHandler::remove_child(QWidget *child)
{
    children_.erase(std::remove_if(children_.begin(), children_.end(),
            [child](auto &c) { return c->owner_ == child; }), children_.end());
    states2_ = 0;
    for (auto & c : children_) states2_ |= c->states();
}

void StateHandler::update_binds()
{
    int bind_states = parent_ ? (parent_->bind_states_ & ~Enabled) : 0;
    for (auto c : colors_)
        bind_states |= c->states();
    bind_states   = bind_states | (bind_states >> 16);
    bind_states_ = bind_states;
    for (auto &c : children_) c->update_binds();
}

void StateHandler::set_state(int state, int mask)
{
    if ((states_ & mask) == (state & mask)) return;
    int old = states_;
    states_ = (states_ & ~mask) | (state & mask);
    if (old != states_ && (old | states2_) != (states_ | states2_))
        triggerRefresh();
}

bool StateHandler::eventFilter(QObject *watched, QEvent *event)
{
    int old = states_;

    switch (event->type()) {
    case QEvent::EnabledChange:
        if (watched == owner_) {
            states_ = owner_->isEnabled()
                          ? (states_ | Enabled)
                          : (states_ & ~Enabled);
        }
        break;
    case QEvent::FocusIn:
        if (watched == owner_)
            states_ |= Focused;
        break;
    case QEvent::FocusOut:
        if (watched == owner_)
            states_ &= ~Focused;
        break;
    case QEvent::Enter:
        if (watched == owner_)
            states_ |= Hovered;
        break;
    case QEvent::Leave:
        if (watched == owner_)
            states_ &= ~Hovered;
        break;
    case QEvent::MouseButtonPress:
        if (watched == owner_) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton)
                states_ |= Pressed;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (watched == owner_) {
            auto *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton)
                states_ &= ~Pressed;
        }
        break;
    default:
        break;
    }

    if (old != states_ && (old | states2_) != (states_ | states2_))
        triggerRefresh();

    return false; // never consume the event
}

void StateHandler::triggerRefresh()
{
    if (parent_) {
        // Propagate upward: rebuild parent's states2_ from all children.
        parent_->states2_ = 0;
        for (auto &c : parent_->children_)
            parent_->states2_ |= c->states();
        parent_->triggerRefresh();
    } else {
        owner_->update();
    }
}

StateHandler::StateHandler(StateHandler *parent, QWidget *owner)
    : StateHandler(owner)
{
    states_ &= ~Enabled;
    parent_ = parent;
}

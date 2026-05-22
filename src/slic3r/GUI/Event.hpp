#ifndef slic3r_Events_hpp_
#define slic3r_Events_hpp_

// Qt port of Event.hpp
// Custom event types inherit from ::QEvent directly.

#include <array>
#include <QEvent>
#include <QObject>

namespace Slic3r {
namespace GUI {

// SimpleEvent — a QEvent carrying no payload
struct SimpleEvent : public ::QEvent {
    static ::QEvent::Type eventType() {
        static ::QEvent::Type t = static_cast<::QEvent::Type>(::QEvent::registerEventType());
        return t;
    }
    explicit SimpleEvent(::QEvent::Type type = eventType()) : ::QEvent(type) {}
};

// IntEvent — a QEvent carrying a single int
struct IntEvent : public ::QEvent {
    explicit IntEvent(::QEvent::Type type, int data) : ::QEvent(type), m_data(data) {}
    int get_data() const { return m_data; }
private:
    int m_data;
};

// ArrayEvent — a QEvent carrying a std::array<T,N>
template<class T, size_t N>
struct ArrayEvent : public ::QEvent {
    std::array<T, N> data;
    ArrayEvent(::QEvent::Type type, std::array<T, N> d)
        : ::QEvent(type), data(std::move(d)) {}
};

// Event<T> — a QEvent carrying a single value of type T
template<class T>
struct Event : public ::QEvent {
    T data;
    Event(::QEvent::Type type, const T &d) : ::QEvent(type), data(d) {}
    Event(::QEvent::Type type, T &&d)      : ::QEvent(type), data(std::move(d)) {}
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_Events_hpp_

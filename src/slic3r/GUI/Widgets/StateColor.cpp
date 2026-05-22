#include "StateColor.hpp"

#include <QRgb>

static bool gDarkMode = false;

// std::map needs operator< for QColor keys (same translation unit only).
static bool operator<(QColor const &l, QColor const &r) { return l.rgba() < r.rgba(); }

static std::map<QColor, QColor> buildDarkColors()
{
    static const std::pair<const char *, const char *> entries[] = {
        {"#00AE42", "#21A452"}, /*green*/
        {"#1F8EEA", "#2778D2"}, /*blue*/
        {"#FF6F00", "#D15B00"},
        {"#D01B1B", "#BB2A3A"}, /*red*/
        {"#262E30", "#EFEFF0"}, /*black*/
        {"#2C2C2E", "#B3B3B4"}, /*black*/
        {"#E5E7EB", "#374151"}, /*gray200 -> gray800*/
        {"#6B6B6B", "#818183"},
        {"#ACACAC", "#54545A"},
        {"#EEEEEE", "#4C4C55"},
        {"#E8E8E8", "#3E3E45"},
        {"#323A3D", "#E5E5E4"},
        {"#FFFFFF", "#2D2D31"},
        {"#F8F8F8", "#36363C"},
        {"#F1F1F1", "#36363B"},
        {"#3B4446", "#2D2D30"},
        {"#CECECE", "#54545B"},
        {"#DBFDD5", "#3B3B40"},
        {"#000000", "#FFFFFE"},
        {"#F4F4F4", "#36363D"},
        {"#F7F7F7", "#333337"},
        {"#DBDBDB", "#4A4A51"},
        {"#EDFAF2", "#283232"},
        {"#323A3C", "#E5E5E6"},
        {"#6B6B6A", "#B3B3B5"},
        {"#303A3C", "#E5E5E5"},
        {"#FEFFFF", "#242428"},
        {"#A6A9AA", "#2D2D29"},
        {"#363636", "#B2B3B5"},
        {"#F0F0F1", "#404040"},
        {"#9E9E9E", "#53545A"},
        {"#D7E8DE", "#1F2B27"},
        {"#2B3436", "#808080"},
        {"#ABABAB", "#ABABAB"},
        {"#D9D9D9", "#2D2D32"},
        {"#EBF9F0", "#293F34"},
        {"#DBFDE7", "#1F3529"},
    };
    std::map<QColor, QColor> m;
    for (auto &e : entries)
        m[QColor(e.first)] = QColor(e.second);
    return m;
}

static std::map<QColor, QColor> gDarkColors = buildDarkColors();

std::map<QColor, QColor> const &StateColor::GetDarkMap()
{
    return gDarkColors;
}

void StateColor::SetDarkMode(bool dark) { gDarkMode = dark; }

static QColor darkModeColorFor2(QColor const &color)
{
    if (!gDarkMode)
        return color;
    auto iter = gDarkColors.find(color);
    if (iter != gDarkColors.end()) return iter->second;
    return color;
}

static std::map<QColor, QColor> revert(std::map<QColor, QColor> const &map)
{
    std::map<QColor, QColor> map2;
    for (auto &p : map) map2.emplace(p.second, p.first);
    return map2;
}

QColor StateColor::lightModeColorFor(QColor const &color)
{
    static std::map<QColor, QColor> gLightColors = revert(gDarkColors);
    auto iter = gLightColors.find(color);
    if (iter != gLightColors.end()) return iter->second;
    return color;
}

QColor StateColor::darkModeColorFor(QColor const &color) { return darkModeColorFor2(color); }

StateColor::StateColor(QColor const &color) { append(color, 0); }

StateColor::StateColor(QString const &color) { append(color, 0); }

StateColor::StateColor(unsigned long color) { append(color, 0); }

void StateColor::append(QColor const &color, int states)
{
    statesList_.push_back(states);
    colors_.push_back(color);
}

void StateColor::append(QString const &color, int states)
{
    append(QColor(color), states);
}

void StateColor::append(unsigned long color, int states)
{
    // Input is 0x00RRGGBB; add full opacity, giving 0xAARRGGBB == QRgb format.
    if ((color & 0xff000000) == 0)
        color |= 0xff000000;
    append(QColor::fromRgba(static_cast<QRgb>(color)), states);
}

void StateColor::clear()
{
    statesList_.clear();
    colors_.clear();
}

int StateColor::states() const
{
    int states = 0;
    for (auto s : statesList_) states |= s;
    states = (states & 0xffff) | (states >> 16);
    if (takeFocusedAsHovered_ && (states & Hovered))
        states |= Focused;
    return states;
}

QColor StateColor::defaultColor()
{
    return colorForStates(0);
}

QColor StateColor::colorForStates(int states)
{
    bool focused = takeFocusedAsHovered_ && (states & Focused);
    for (int i = 0; i < (int)statesList_.size(); ++i) {
        int s   = statesList_[i];
        int on  = s & 0xffff;
        int off = s >> 16;
        if ((on & states) == on && (off & ~states) == off) {
            return darkModeColorFor2(colors_[i]);
        }
        if (focused && (on & Hovered)) {
            on |= Focused;
            on &= ~Hovered;
            if ((on & states) == on && (off & ~states) == off) {
                return darkModeColorFor2(colors_[i]);
            }
        }
    }
    return QColor(0, 0, 0, 0);
}

QColor StateColor::colorForStatesNoDark(int states)
{
    bool focused = takeFocusedAsHovered_ && (states & Focused);
    for (int i = 0; i < (int)statesList_.size(); ++i) {
        int s   = statesList_[i];
        int on  = s & 0xffff;
        int off = s >> 16;
        if ((on & states) == on && (off & ~states) == off) {
            return colors_[i];
        }
        if (focused && (on & Hovered)) {
            on |= Focused;
            on &= ~Hovered;
            if ((on & states) == on && (off & ~states) == off) {
                return colors_[i];
            }
        }
    }
    return QColor(0, 0, 0, 0);
}

int StateColor::colorIndexForStates(int states)
{
    for (int i = 0; i < (int)statesList_.size(); ++i) {
        int s   = statesList_[i];
        int on  = s & 0xffff;
        int off = s >> 16;
        if ((on & states) == on && (off & ~states) == off) return i;
    }
    return -1;
}

bool StateColor::setColorForStates(QColor const &color, int states)
{
    for (int i = 0; i < (int)statesList_.size(); ++i) {
        if (statesList_[i] == states) {
            colors_[i] = color;
            return true;
        }
    }
    return false;
}

void StateColor::setTakeFocusedAsHovered(bool set) { takeFocusedAsHovered_ = set; }

StateColor StateColor::createButtonStyleGray()
{
    return StateColor(
        std::pair<QColor, int>(QColor(206, 206, 206), StateColor::Pressed),
        std::pair<QColor, int>(Qt::white,             StateColor::Focused),
        std::pair<QColor, int>(QColor(238, 238, 238), StateColor::Hovered),
        std::pair<QColor, int>(Qt::white,             StateColor::Normal));
}

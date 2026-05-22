#include "GuiColor.hpp"

namespace Slic3r { namespace GUI {
QColor convert_to_QColor(const RGBA &color)
{
    auto     r = std::clamp((int) (color[0] * 255.f), 0, 255);
    auto     g = std::clamp((int) (color[1] * 255.f), 0, 255);
    auto     b = std::clamp((int) (color[2] * 255.f), 0, 255);
    auto     a = std::clamp((int) (color[3] * 255.f), 0, 255);
    QColor wx_color(r, g, b, a);
    return wx_color;
}

RGBA convert_to_rgba(const QColor &color)
{
    RGBA rgba;
    rgba[0] = std::clamp(color.red() / 255.f, 0.f, 1.f);
    rgba[1] = std::clamp(color.green() / 255.f, 0.f, 1.f);
    rgba[2] = std::clamp(color.blue() / 255.f, 0.f, 1.f);
    rgba[3] = std::clamp(color.alpha() / 255.f, 0.f, 1.f);
    return rgba;
}

float calc_color_distance(QColor c1, QColor c2)
{
    float lab[2][3];
    RGB2Lab(c1.red(), c1.green(), c1.blue(), &lab[0][0], &lab[0][1], &lab[0][2]);
    RGB2Lab(c2.red(), c2.green(), c2.blue(), &lab[1][0], &lab[1][1], &lab[1][2]);

    return DeltaE76(lab[0][0], lab[0][1], lab[0][2], lab[1][0], lab[1][1], lab[1][2]);
}

float calc_color_distance(RGBA c1, RGBA c2)
{
    float lab[2][3];
    RGB2Lab(c1[0], c1[1], c1[2], &lab[0][0], &lab[0][1], &lab[0][2]);
    RGB2Lab(c2[0], c2[1], c2[2], &lab[1][0], &lab[1][1], &lab[1][2]);

    return DeltaE76(lab[0][0], lab[0][1], lab[0][2], lab[1][0], lab[1][1], lab[1][2]);
}

} }

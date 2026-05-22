#ifndef slic3r_GUI_Color_hpp_
#define slic3r_GUI_Color_hpp_
#include <QColor>
#include "libslic3r/Color.hpp"
#include "slic3r/Utils/ColorSpaceConvert.hpp"

struct ColorDistValue
{
    int   id;
    float distance;
};

namespace Slic3r { namespace GUI {
QColor convert_to_QColor(const RGBA &color);
RGBA     convert_to_rgba(const QColor &color);
float    calc_color_distance(QColor c1, QColor c2);
float    calc_color_distance(RGBA c1, RGBA c2);
} // namespace GUI
} // namespace Slic3r

#endif /* slic3r_GUI_Color_hpp_ */

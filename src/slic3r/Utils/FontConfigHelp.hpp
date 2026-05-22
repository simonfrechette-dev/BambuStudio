
#ifndef slic3r_FontConfigHelp_hpp_
#define slic3r_FontConfigHelp_hpp_

#ifdef __linux__
#define EXIST_FONT_CONFIG_INCLUDE
#endif

#ifdef EXIST_FONT_CONFIG_INCLUDE
#include <string>
#include <QFont>
namespace Slic3r::GUI {

/// <summary>
/// Initialize font config and convert a QFont to its file path.
/// </summary>
std::string get_font_path(const QFont &font, bool reload_fonts = false);

/// <summary>
/// Look up font file path by family name using FontConfig (no wx dependency).
/// </summary>
std::string get_font_path_by_name(const std::string &family_name, bool reload_fonts = false);

} // namespace Slic3r
#endif // EXIST_FONT_CONFIG_INCLUDE
#endif // slic3r_FontConfigHelp_hpp_

#ifndef slic3r_WxFontUtils_hpp_
#define slic3r_WxFontUtils_hpp_

#include <memory>
#include <optional>
#include <string_view>
#include <boost/bimap.hpp>
#include <QFont>
#include "libslic3r/Emboss.hpp"

namespace Slic3r {
namespace GUI {

/// Helper class for Qt font operations used by the Emboss/text gizmo system.
/// Replaces the old wx-based WxFontUtils.
class WxFontUtils
{
public:
    WxFontUtils() = delete;

    /// Check if a loadable font file exists for this font.
    static bool can_load(const QFont &font);
    /// Load font file for the given QFont.
    static std::unique_ptr<::Slic3r::Emboss::FontFile> create_font_file(const QFont &font);

    static EmbossStyle::Type get_current_type();
    static EmbossStyle       create_emboss_style(const QFont &font, const std::string &name = "");

    static std::string get_human_readable_name(const QFont &font);

    /// Serialize a QFont to a string suitable for storage in 3mf.
    static std::string store_wxFont(const QFont &font);
    /// Deserialize a QFont from a stored string.
    static QFont load_wxFont(const std::string &font_descriptor);

    /// Try to create a QFont from the stored EmbossStyle.
    static QFont create_wxFont(const EmbossStyle &style);
    /// Update FontProp from a QFont (excluding emboss depth and font size).
    static void update_property(FontProp &font_prop, const QFont &font);

    static bool is_italic(const QFont &font);
    static bool is_bold(const QFont &font);

    /// Try to find an italic variant; returns new FontFile if a different file
    /// is found, or nullptr when no italic variant exists.
    static std::unique_ptr<::Slic3r::Emboss::FontFile> set_italic(QFont &font, const ::Slic3r::Emboss::FontFile &prev_font_file);

    /// Try to find a bold variant; returns new FontFile or nullptr.
    static std::unique_ptr<::Slic3r::Emboss::FontFile> set_bold(QFont &font, const ::Slic3r::Emboss::FontFile &font_file);

    // Style-hint bimap: QFont::StyleHint ↔ string_view
    static const boost::bimap<QFont::StyleHint, std::string_view> type_to_family;
    // Style bimap: QFont::Style ↔ string_view
    static const boost::bimap<QFont::Style, std::string_view> type_to_style;
    // Weight bimap: int (Qt weight 100-900) ↔ string_view
    static const boost::bimap<int, std::string_view> type_to_weight;
};

}
} // namespace Slic3r::GUI
#endif // slic3r_WxFontUtils_hpp_

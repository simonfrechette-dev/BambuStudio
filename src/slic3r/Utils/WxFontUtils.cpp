#include "WxFontUtils.hpp"
#include <boost/assign.hpp>
#include <boost/log/trivial.hpp>
#include "libslic3r/Utils.hpp"
#include <QFont>
#include <QFontDatabase>

#if defined(__linux__)
#include "slic3r/Utils/FontConfigHelp.hpp"
#endif

using namespace Slic3r;
using namespace Slic3r::GUI;

bool WxFontUtils::can_load(const QFont &font)
{
    if (font.family().isEmpty()) return false;
#if defined(__linux__)
    std::string path = Slic3r::GUI::get_font_path(font);
    return !path.empty();
#else
    return true;
#endif
}

std::unique_ptr<Emboss::FontFile> WxFontUtils::create_font_file(const QFont &font)
{
#if defined(__linux__)
    std::string font_path = Slic3r::GUI::get_font_path(font);
    if (font_path.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Cannot find font file for '"
                                 << font.family().toStdString() << "'";
        return nullptr;
    }
    return Emboss::create_font_file(font_path.c_str());
#elif defined(_WIN32)
    // On Windows, Qt can give us the font path via QFontDatabase
    // Fallback: load by family name
    std::string family = font.family().toStdString();
    if (family.empty()) return nullptr;
    // WxFontUtils on Windows previously used HFONT; for now stub out
    BOOST_LOG_TRIVIAL(error) << "WxFontUtils::create_font_file not implemented for Windows Qt build";
    return nullptr;
#else
    // macOS: use QFontDatabase to find file (best effort)
    BOOST_LOG_TRIVIAL(error) << "WxFontUtils::create_font_file not implemented for this platform";
    return nullptr;
#endif
}

EmbossStyle::Type WxFontUtils::get_current_type()
{
#ifdef _WIN32
    return EmbossStyle::Type::wx_win_font_descr;
#elif defined(__APPLE__)
    return EmbossStyle::Type::wx_mac_font_descr;
#elif defined(__linux__)
    return EmbossStyle::Type::wx_lin_font_descr;
#else
    return EmbossStyle::Type::undefined;
#endif
}

std::string WxFontUtils::get_human_readable_name(const QFont &font)
{
    return font.family().toStdString();
}

std::string WxFontUtils::store_wxFont(const QFont &font)
{
    return font.toString().toStdString();
}

QFont WxFontUtils::load_wxFont(const std::string &font_descriptor)
{
    BOOST_LOG_TRIVIAL(trace) << "load_wxFont: '" << font_descriptor << "'";
    QFont font;
    if (!font.fromString(QString::fromStdString(font_descriptor))) {
        BOOST_LOG_TRIVIAL(warning) << "QFont::fromString failed for: '" << font_descriptor << "'";
    }
    return font;
}

// Bimaps
using TypeToFamily = boost::bimap<QFont::StyleHint, std::string_view>;
const TypeToFamily WxFontUtils::type_to_family =
    boost::assign::list_of<TypeToFamily::relation>
        (QFont::AnyStyle,   "default")
        (QFont::Decorative, "decorative")
        (QFont::Serif,      "roman")
        (QFont::Cursive,    "script")
        (QFont::SansSerif,  "swiss")
        (QFont::TypeWriter, "modern")
        (QFont::Monospace,  "teletype");

using TypeToStyle = boost::bimap<QFont::Style, std::string_view>;
const TypeToStyle WxFontUtils::type_to_style =
    boost::assign::list_of<TypeToStyle::relation>
        (QFont::StyleItalic,  "italic")
        (QFont::StyleOblique, "slant")
        (QFont::StyleNormal,  "normal");

using TypeToWeight = boost::bimap<int, std::string_view>;
const TypeToWeight WxFontUtils::type_to_weight =
    boost::assign::list_of<TypeToWeight::relation>
        (QFont::Thin,       "thin")
        (QFont::ExtraLight, "extraLight")
        (QFont::Light,      "light")
        (QFont::Normal,     "normal")
        (QFont::Medium,     "medium")
        (QFont::DemiBold,   "semibold")
        (QFont::Bold,       "bold")
        (QFont::ExtraBold,  "extraBold")
        (QFont::Black,      "heavy")
        (QFont::Black,      "extraHeavy"); // map extraHeavy to Black

EmbossStyle WxFontUtils::create_emboss_style(const QFont &font, const std::string &name)
{
    std::string name_item = name.empty() ? get_human_readable_name(font) : name;
    std::string fontDesc = store_wxFont(font);
    EmbossStyle::Type type = get_current_type();

    FontProp font_prop;
    font_prop.size_in_mm = font.pointSizeF() > 0 ? font.pointSizeF() : 10.0f;
    update_property(font_prop, font);
    return { name_item, fontDesc, type, font_prop };
}

QFont WxFontUtils::create_wxFont(const EmbossStyle &style)
{
    const FontProp &fp = style.prop;
    QFont font;
    font.setPointSizeF(fp.size_in_mm > 0 ? fp.size_in_mm : 10.0);

    if (fp.family.has_value()) {
        auto it = type_to_family.right.find(*fp.family);
        if (it != type_to_family.right.end())
            font.setStyleHint(it->second);
    }
    if (fp.style.has_value()) {
        auto it = type_to_style.right.find(*fp.style);
        if (it != type_to_style.right.end())
            font.setStyle(it->second);
    }
    if (fp.weight.has_value()) {
        auto it = type_to_weight.right.find(*fp.weight);
        if (it != type_to_weight.right.end())
            font.setWeight(static_cast<QFont::Weight>(it->second));
    }
    if (fp.face_name.has_value())
        font.setFamily(QString::fromStdString(*fp.face_name));

    // Check if we can load this font
    std::unique_ptr<Emboss::FontFile> ff = create_font_file(font);
    if (ff == nullptr) return {};
    return font;
}

void WxFontUtils::update_property(FontProp &font_prop, const QFont &font)
{
    std::string face_name = font.family().toStdString();
    if (!face_name.empty()) font_prop.face_name = face_name;

    QFont::StyleHint hint = font.styleHint();
    if (hint != QFont::AnyStyle) {
        auto it = type_to_family.left.find(hint);
        if (it != type_to_family.left.end()) font_prop.family = it->second;
    }

    QFont::Style style = font.style();
    if (style != QFont::StyleNormal) {
        auto it = type_to_style.left.find(style);
        if (it != type_to_style.left.end()) font_prop.style = it->second;
    }

    int w = font.weight();
    if (w != QFont::Normal) {
        auto it = type_to_weight.left.find(w);
        if (it != type_to_weight.left.end()) font_prop.weight = it->second;
    }
}

bool WxFontUtils::is_italic(const QFont &font)
{
    return font.style() == QFont::StyleItalic || font.style() == QFont::StyleOblique;
}

bool WxFontUtils::is_bold(const QFont &font)
{
    return font.weight() > QFont::Normal;
}

std::unique_ptr<Emboss::FontFile> WxFontUtils::set_italic(QFont &font, const Emboss::FontFile &font_file)
{
    static const std::vector<QFont::Style> italic_styles = {
        QFont::StyleItalic,
        QFont::StyleOblique
    };
    QFont::Style orig_style = font.style();
    for (QFont::Style style : italic_styles) {
        font.setStyle(style);
        std::unique_ptr<Emboss::FontFile> new_font_file = WxFontUtils::create_font_file(font);
        if (new_font_file == nullptr) continue;
        if (font_file == *new_font_file) continue;
        return new_font_file;
    }
    font.setStyle(orig_style);
    return nullptr;
}

std::unique_ptr<Emboss::FontFile> WxFontUtils::set_bold(QFont &font, const Emboss::FontFile &font_file)
{
    static const std::vector<int> bold_weights = {
        QFont::Bold, QFont::ExtraBold, QFont::Black
    };
    int orig_weight = font.weight();
    for (int weight : bold_weights) {
        font.setWeight(static_cast<QFont::Weight>(weight));
        std::unique_ptr<Emboss::FontFile> new_font_file = WxFontUtils::create_font_file(font);
        if (new_font_file == nullptr) continue;
        if (font_file == *new_font_file) continue;
        return new_font_file;
    }
    font.setWeight(static_cast<QFont::Weight>(orig_weight));
    return nullptr;
}

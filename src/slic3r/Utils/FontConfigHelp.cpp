#include "FontConfigHelp.hpp"

#ifdef EXIST_FONT_CONFIG_INCLUDE

#include <fontconfig/fontconfig.h>
#include "libslic3r/Utils.hpp"
#include <QFont>
#include <QFileInfo>
#include <QString>

using namespace Slic3r::GUI;
using Slic3r::ScopeGuard;


// @Vojta suggest to make static variable global
// Guard for finalize Font Config
// Will be finalized on application exit
// It seams that it NOT work
static std::optional<Slic3r::ScopeGuard> finalize_guard;
// cache for Loading of the default configuration file and building information about the available fonts.
static FcConfig *fc = nullptr;

static void ensure_fc_initialized(bool reload_fonts)
{
    if (!finalize_guard.has_value()) {
        FcInit();
        fc = FcInitLoadConfigAndFonts();
        finalize_guard.emplace([]() { FcConfigDestroy(fc); });
    } else if (reload_fonts) {
        FcConfigDestroy(fc);
        fc = FcInitLoadConfigAndFonts();
    }
}

static std::string fc_file_path_from_pattern(FcPattern *matchPattern)
{
    FcConfigSubstitute(NULL, matchPattern, FcMatchPattern);
    FcDefaultSubstitute(matchPattern);

    FcResult res;
    FcPattern *resultPattern = FcFontMatch(NULL, matchPattern, &res);
    if (resultPattern == nullptr) return {};
    ScopeGuard sg_rp([resultPattern]() { FcPatternDestroy(resultPattern); });

    FcChar8 *fileName = nullptr;
    if (FcPatternGetString(resultPattern, FC_FILE, 0, &fileName) != FcResultMatch
        || fileName == nullptr)
        return {};

    std::string fontFileName(reinterpret_cast<const char*>(fileName));
    if (fontFileName.empty()) return {};

    QFileInfo fileInfo(QString::fromStdString(fontFileName));
    if (!fileInfo.exists() || !fileInfo.isReadable()) return {};
    return fileInfo.absoluteFilePath().toStdString();
}

std::string Slic3r::GUI::get_font_path(const QFont &font, bool reload_fonts)
{
    ensure_fc_initialized(reload_fonts);
    if (fc == nullptr) return {};

    std::string familyName = font.family().toStdString();
    const char *fontFamily = familyName.c_str();

    // Map QFont style → FC slant
    int slant = FC_SLANT_ROMAN;
    if (font.style() == QFont::StyleOblique)
        slant = FC_SLANT_OBLIQUE;
    else if (font.style() == QFont::StyleItalic)
        slant = FC_SLANT_ITALIC;

    // Map QFont weight (1-1000) → FC weight
    int weight = FC_WEIGHT_NORMAL;
    int qw = font.weight();
    if (qw <= 150)       weight = FC_WEIGHT_THIN;
    else if (qw <= 250)  weight = FC_WEIGHT_ULTRALIGHT;
    else if (qw <= 325)  weight = FC_WEIGHT_LIGHT;
    else if (qw <= 375)  weight = FC_WEIGHT_BOOK;
    else if (qw <= 450)  weight = FC_WEIGHT_NORMAL;
    else if (qw <= 550)  weight = FC_WEIGHT_MEDIUM;
    else if (qw <= 650)  weight = FC_WEIGHT_DEMIBOLD;
    else if (qw <= 750)  weight = FC_WEIGHT_BOLD;
    else if (qw <= 850)  weight = FC_WEIGHT_ULTRABOLD;
    else                 weight = FC_WEIGHT_BLACK;

    // Map QFont stretch → FC width
    int width = FC_WIDTH_NORMAL;
    int qs = font.stretch();
    if (qs > 0 && qs <= 56)       width = FC_WIDTH_ULTRACONDENSED;
    else if (qs <= 69)            width = FC_WIDTH_EXTRACONDENSED;
    else if (qs <= 81)            width = FC_WIDTH_CONDENSED;
    else if (qs <= 93)            width = FC_WIDTH_SEMICONDENSED;
    else if (qs <= 106)           width = FC_WIDTH_NORMAL;
    else if (qs <= 118)           width = FC_WIDTH_SEMIEXPANDED;
    else if (qs <= 137)           width = FC_WIDTH_EXPANDED;
    else if (qs <= 175)           width = FC_WIDTH_EXTRAEXPANDED;
    else if (qs > 175)            width = FC_WIDTH_ULTRAEXPANDED;

    FcPattern *matchPattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString,
                                             (FcChar8 *)fontFamily, NULL);
    ScopeGuard sg_mp([matchPattern]() { FcPatternDestroy(matchPattern); });
    FcPatternAddInteger(matchPattern, FC_SLANT, slant);
    FcPatternAddInteger(matchPattern, FC_WEIGHT, weight);
    FcPatternAddInteger(matchPattern, FC_WIDTH, width);

    return fc_file_path_from_pattern(matchPattern);
}

std::string Slic3r::GUI::get_font_path_by_name(const std::string &family_name, bool reload_fonts)
{
    if (family_name.empty()) return {};
    ensure_fc_initialized(reload_fonts);
    if (fc == nullptr) return {};

    FcPattern *matchPattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString,
                                             (FcChar8 *)family_name.c_str(), NULL);
    ScopeGuard sg([matchPattern]() { FcPatternDestroy(matchPattern); });
    return fc_file_path_from_pattern(matchPattern);
}

#endif // EXIST_FONT_CONFIG_INCLUDE
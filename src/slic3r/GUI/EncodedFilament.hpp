#ifndef slic3r_ENCODED_FILAMENT_hpp_
#define slic3r_ENCODED_FILAMENT_hpp_

// Qt port: QColor->QColor, QString->std::string

#include <vector>
#include <map>
#include <math.h>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>

#include <QColor>

namespace Slic3r
{

class FilamentColorCode;
class FilamentColorCodes;
class FilamentColorCodeQuery;

struct ColourHSV { double h, s, v; };

inline ColourHSV QColorToHSV(const QColor &c)
{
    double r = c.red() / 255.0, g = c.green() / 255.0, b = c.blue() / 255.0;
    double maxc = std::max({r, g, b}), minc = std::min({r, g, b});
    double delta = maxc - minc;
    double h = 0, s = 0, v = maxc;
    if (delta > 0.00001) {
        if (maxc == r)      h = 60.0 * fmod((g - b) / delta, 6.0);
        else if (maxc == g) h = 60.0 * ((b - r) / delta + 2.0);
        else                h = 60.0 * ((r - g) / delta + 4.0);
        if (h < 0) h += 360.0;
        s = delta / maxc;
    }
    return {h, s, v};
}

struct QColorSorter {
    bool operator()(const QColor &a, const QColor &b) const noexcept
    {
        ColourHSV ha = QColorToHSV(a), hb = QColorToHSV(b);
        if (ha.h != hb.h) return ha.h < hb.h;
        if (ha.s != hb.s) return ha.s < hb.s;
        if (ha.v != hb.v) return ha.v < hb.v;
        return a.alpha() < b.alpha();
    }
};

struct FilamentColor
{
    enum class ColorType : char { SINGLE_CLR = 0, MULTI_CLR, GRADIENT_CLR };

    ColorType m_color_type = ColorType::SINGLE_CLR;
    std::set<QColor, QColorSorter> m_colors;

    size_t ColorCount() const noexcept { return m_colors.size(); }

    void EndSet(int ctype)
    {
        if (m_colors.size() < 2) {
            m_color_type = ColorType::SINGLE_CLR;
        } else {
            m_color_type = (ctype == 0) ? ColorType::GRADIENT_CLR : ColorType::MULTI_CLR;
        }
    }

    bool operator<(const FilamentColor &other) const
    {
        if (ColorCount() != other.ColorCount()) return ColorCount() < other.ColorCount();
        if (m_color_type != other.m_color_type) return m_color_type < other.m_color_type;
        if (m_colors == other.m_colors) return false;
        auto li = m_colors.begin(), ri = other.m_colors.begin();
        while (li != m_colors.end()) {
            ColourHSV ha = QColorToHSV(*li), hb = QColorToHSV(*ri);
            if (ha.h != hb.h) return ha.h < hb.h;
            if (ha.s != hb.s) return ha.s < hb.s;
            if (ha.v != hb.v) return ha.v < hb.v;
            if (li->alpha() != ri->alpha()) return li->alpha() < ri->alpha();
            ++li; ++ri;
        }
        return false;
    }
};

struct EncodedFilaColorEqual {
    bool operator()(const FilamentColor &a, const FilamentColor &b) const noexcept { return a < b; }
};
using FilamentColor2CodeMap = std::map<FilamentColor, FilamentColorCode*, EncodedFilaColorEqual>;


class FilamentColorCodeQuery
{
public:
    FilamentColorCodeQuery();
    virtual ~FilamentColorCodeQuery();

    FilamentColorCodes* GetFilaInfoMap(const std::string &fila_id) const;
    std::string GetFilaColorName(const std::string &fila_id, const FilamentColor &colors) const;
    FilamentColorCode* GetFilaInfo(const std::string &fila_id, const FilamentColor &colors) const;

protected:
    void LoadFromLocal();

public:
    void CreateFilaCode(const std::string &fila_id,
                        const std::string &fila_type,
                        const std::string &fila_color_code,
                        FilamentColor &&fila_color,
                        std::unordered_map<std::string, std::string> &&fila_color_names);

private:
    std::string m_fila_path;
    std::unordered_map<std::string, FilamentColorCodes*> *m_fila_id2colors_map;
};


class FilamentColorCodes
{
public:
    FilamentColorCodes(const std::string &fila_id, const std::string &fila_type);
    virtual ~FilamentColorCodes();

    std::string GetFilaCode() const { return m_fila_id; }
    std::string GetFilaType() const { return m_fila_type; }

    FilamentColor2CodeMap *GetFilamentColor2CodeMap() const { return m_fila_colors_map; }
    FilamentColorCode *GetColorCode(const FilamentColor &colors) const;

    void Debug(const char *prefix);
    void AddColorCode(FilamentColorCode *code);

private:
    std::string m_fila_id;
    std::string m_fila_type;
    FilamentColor2CodeMap *m_fila_colors_map;
};


class FilamentColorCode
{
public:
    FilamentColorCode() = delete;
    FilamentColorCode(const std::string &color_code, FilamentColorCodes *owner,
                      FilamentColor &&color,
                      std::unordered_map<std::string, std::string> &&name_map);
    ~FilamentColorCode() {}

    std::string GetFilaCode()       const { return m_owner->GetFilaCode(); }
    std::string GetFilaType()       const { return m_owner->GetFilaType(); }
    std::string GetFilaColorCode()  const { return m_fila_color_code; }
    FilamentColor GetFilaColor()    const { return m_fila_color; }
    std::string GetFilaColorName()  const;

    void Debug(const char *prefix);

private:
    FilamentColorCodes *m_owner;
    std::string m_fila_color_code;
    FilamentColor m_fila_color;
    std::unordered_map<std::string, std::string> m_fila_color_names;
};

} // namespace Slic3r
#endif // slic3r_ENCODED_FILAMENT_hpp_

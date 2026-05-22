#ifndef SLIC3R_GUI_BITMAP_CACHE_HPP
#define SLIC3R_GUI_BITMAP_CACHE_HPP

#include <map>
#include <vector>

#include <QPixmap>
#include <imgui/imgui.h>

struct NSVGimage;

namespace Slic3r { namespace GUI {

class BitmapCache
{
public:
	BitmapCache();
	~BitmapCache() { clear(); }
	void 			clear();
	double			scale() { return m_scale; }

	QPixmap* 		find(const std::string &name) 		{ auto it = m_map.find(name); return (it == m_map.end()) ? nullptr : it->second; }
	const QPixmap* find(const std::string &name) const { return const_cast<BitmapCache*>(this)->find(name); }

	QPixmap*       insert(const std::string &name, size_t width, size_t height);
	QPixmap* 		insert(const std::string &name, const QPixmap &bmp);
	QPixmap* 		insert(const std::string &name, const QPixmap &bmp, const QPixmap &bmp2);
	QPixmap* 		insert(const std::string &name, const QPixmap &bmp, const QPixmap &bmp2, const QPixmap &bmp3);
	QPixmap* 		insert(const std::string &name, const std::vector<QPixmap> &bmps) { return this->insert(name, &bmps.front(), &bmps.front() + bmps.size()); }
	QPixmap* 		insert(const std::string &name, const QPixmap *begin, const QPixmap *end);
	QPixmap* 		insert_raw_rgba(const std::string &bitmap_key, unsigned width, unsigned height, const unsigned char *raw_data, const bool grayscale = false);

	// BBS: support resize by fill border  (scale_in_center)
	// Load png from resources/icons. bitmap_key is given without the .png suffix. Bitmap will be rescaled to provided height/width if nonzero.
    QPixmap* 		load_png(const std::string &bitmap_key, unsigned width = 0, unsigned height = 0, const bool grayscale = false, const float scale_in_center = 0.f);

	// Parses SVG file from a file, returns SVG image as paths.
	// And makes replases befor parsing
	// replace_map containes old_value->new_value
	static NSVGimage* nsvgParseFromFileWithReplace(const char* filename, const char* units, float dpi, const std::map<std::string, std::string>& replaces);
	// Load svg from resources/icons. bitmap_key is given without the .svg suffix. SVG will be rasterized to provided height/width.
    QPixmap* 		load_svg(const std::string &bitmap_key, unsigned width = 0, unsigned height = 0, const bool grayscale = false, const bool dark_mode = false, const std::string& new_color = "", const float scale_in_center = 0.f);
    //Load background image of semi transparent material with color,
    QPixmap* 		load_svg2(const std::string &bitmap_key, unsigned width = 0, unsigned height = 0, const bool grayscale = false, const bool dark_mode = false, const std::vector<std::string>& array_new_color = std::vector<std::string>(), const float scale_in_center = 0.0f);

	QPixmap 		mksolid(size_t width, size_t height, unsigned char r, unsigned char g, unsigned char b, unsigned char transparency, bool suppress_scaling = false, size_t border_width = 0, bool dark_mode = false);
	QPixmap 		mksolid(size_t width, size_t height, const unsigned char rgb[3], bool suppress_scaling = false, size_t border_width = 0, bool dark_mode = false) { return mksolid(width, height, rgb[0], rgb[1], rgb[2], static_cast<unsigned char>(255), suppress_scaling, border_width, dark_mode); }
	QPixmap 		mkclear(size_t width, size_t height) { return mksolid(width, height, static_cast<unsigned char>(0), static_cast<unsigned char>(0), static_cast<unsigned char>(0), static_cast<unsigned char>(0)); }

	static bool     parse_color(const std::string& scolor, unsigned char* rgb_out);
	static bool     parse_color4(const std::string& scolor, unsigned char* rgba_out);

	static bool load_from_svg_file_change_color(const std::string &filename, unsigned width, unsigned height, ImTextureID &texture_id, const char *hexColor);


private:
    std::map<std::string, QPixmap*>	m_map;
    double	m_gs	= 0.2;	// value, used for image.ConvertToGreyscale(m_gs, m_gs, m_gs)
	double	m_scale = 1.0;	// value, used for correct scaling of SVG icons on Retina display
};

} // GUI
} // Slic3r

#endif // SLIC3R_GUI_BITMAP_CACHE_HPP

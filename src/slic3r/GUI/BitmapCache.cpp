// Qt port of BitmapCache.cpp
// Original wx implementation backed up to BitmapCache.cpp.wx-backup

#include "BitmapCache.hpp"

#include "libslic3r/Utils.hpp"
#include "../Utils/MacDarkMode.hpp"
#include "GUI.hpp"
#include "GUI_Utils.hpp"

#include <boost/filesystem.hpp>
#include <boost/nowide/cstdio.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/system/detail/error_code.hpp>
#include <unordered_set>

#include <GL/glew.h>
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/nanosvgrast.h"
#include "3DScene.hpp"

#include <QImage>
#include <QPainter>

namespace Slic3r { namespace GUI {

static std::string dark_version(const std::string &bitmap_name)
{
    static std::unordered_set<std::string> cache;
    auto dark_bitmap_name = bitmap_name + "_dark";
    auto it = cache.find(dark_bitmap_name);
    if (it != cache.end()) return *it;
    boost::system::error_code ec;
    if (boost::filesystem::exists(Slic3r::var(dark_bitmap_name) + ".svg", ec)) {
        cache.insert(dark_bitmap_name);
        return dark_bitmap_name;
    }
    return {};
}

BitmapCache::BitmapCache()
{
#ifdef __APPLE__
    m_scale = mac_max_scaling_factor();
#endif
}

void BitmapCache::clear()
{
    for (auto &pair : m_map)
        delete pair.second;
    m_map.clear();
}

QPixmap* BitmapCache::insert(const std::string &bitmap_key, size_t width, size_t height)
{
    QPixmap *pixmap = nullptr;
    auto it = m_map.find(bitmap_key);
    if (it == m_map.end()) {
        pixmap = new QPixmap(int(width), int(height));
        pixmap->fill(Qt::transparent);
        m_map[bitmap_key] = pixmap;
    } else {
        pixmap = it->second;
        if (size_t(pixmap->width()) != width || size_t(pixmap->height()) != height) {
            *pixmap = QPixmap(int(width), int(height));
            pixmap->fill(Qt::transparent);
        }
    }
    return pixmap;
}

QPixmap* BitmapCache::insert(const std::string &bitmap_key, const QPixmap &bmp)
{
    auto it = m_map.find(bitmap_key);
    if (it == m_map.end()) {
        auto *pixmap = new QPixmap(bmp);
        m_map[bitmap_key] = pixmap;
        return pixmap;
    }
    *(it->second) = bmp;
    return it->second;
}

QPixmap* BitmapCache::insert(const std::string &bitmap_key, const QPixmap &bmp, const QPixmap &bmp2)
{
    const QPixmap bmps[2] = { bmp, bmp2 };
    return this->insert(bitmap_key, bmps, bmps + 2);
}

QPixmap* BitmapCache::insert(const std::string &bitmap_key, const QPixmap &bmp, const QPixmap &bmp2, const QPixmap &bmp3)
{
    const QPixmap bmps[3] = { bmp, bmp2, bmp3 };
    return this->insert(bitmap_key, bmps, bmps + 3);
}

QPixmap* BitmapCache::insert(const std::string &bitmap_key, const QPixmap *begin, const QPixmap *end)
{
    int total_w = 0, max_h = 0;
    for (const QPixmap *p = begin; p != end; ++p) {
        total_w += p->width();
        max_h    = std::max(max_h, p->height());
    }
    QPixmap *result = this->insert(bitmap_key, total_w, max_h);
    QPainter painter(result);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    int x = 0;
    for (const QPixmap *p = begin; p != end; ++p) {
        if (p->width() > 0) { painter.drawPixmap(x, 0, *p); x += p->width(); }
    }
    painter.end();
    return result;
}

QPixmap* BitmapCache::insert_raw_rgba(const std::string &bitmap_key, unsigned width, unsigned height,
                                       const unsigned char *raw_data, const bool grayscale)
{
    QImage image(int(width), int(height), QImage::Format_RGBA8888);
    unsigned char *dst = image.bits();
    unsigned int pixels = width * height;
    for (unsigned int i = 0; i < pixels; ++i) {
        *dst++ = *raw_data++;
        *dst++ = *raw_data++;
        *dst++ = *raw_data++;
        *dst++ = *raw_data++;
    }
    if (grayscale)
        image = image.convertToFormat(QImage::Format_Grayscale8).convertToFormat(QImage::Format_RGBA8888);
    return this->insert(bitmap_key, QPixmap::fromImage(std::move(image)));
}

QPixmap* BitmapCache::load_png(const std::string &bitmap_name, unsigned width, unsigned height,
                                 const bool grayscale, const float scale_in_center)
{
    std::string bitmap_key = bitmap_name
        + (height != 0 ? "-h" + std::to_string(height) : "-w" + std::to_string(width))
        + (grayscale ? "-gs" : "");
    auto it = m_map.find(bitmap_key);
    if (it != m_map.end()) return it->second;

    QImage image;
    if (!image.load(from_u8(Slic3r::var(bitmap_name + ".png"))) || image.width() == 0 || image.height() == 0)
        return nullptr;

    if (height == 0 && width == 0) height = image.height();
    if (height != 0 && unsigned(image.height()) != height)
        width = unsigned(0.5f + float(image.width()) * height / image.height());
    else if (width != 0 && unsigned(image.width()) != width)
        height = unsigned(0.5f + float(image.height()) * width / image.width());

    if (height != 0 && width != 0) {
        if (scale_in_center > 0) {
            QImage canvas(int(width), int(height), QImage::Format_RGBA8888);
            canvas.fill(Qt::transparent);
            QPainter p(&canvas);
            p.drawImage((int(width) - image.width()) / 2, (int(height) - image.height()) / 2, image);
            p.end();
            image = canvas;
        } else {
            image = image.scaled(int(width), int(height), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    if (grayscale)
        image = image.convertToFormat(QImage::Format_Grayscale8).convertToFormat(QImage::Format_RGBA8888);
    return this->insert(bitmap_key, QPixmap::fromImage(std::move(image)));
}

NSVGimage* BitmapCache::nsvgParseFromFileWithReplace(const char* filename, const char* units, float dpi,
                                                      const std::map<std::string, std::string>& replaces)
{
    std::string str;
    FILE* fp = nullptr; size_t size; char* data = nullptr; NSVGimage* image = nullptr;
    fp = boost::nowide::fopen(filename, "rb");
    if (!fp) goto error;
    fseek(fp, 0, SEEK_END); size = ftell(fp); fseek(fp, 0, SEEK_SET);
    data = (char*)malloc(size + 1);
    if (!data) goto error;
    if (fread(data, 1, size, fp) != size) goto error;
    data[size] = '\0'; fclose(fp);
    if (replaces.empty()) {
        image = nsvgParse(data, units, dpi);
    } else {
        str.assign(data);
        for (const auto &v : replaces) boost::replace_all(str, v.first, v.second);
        image = nsvgParse(str.data(), units, dpi);
    }
    free(data); return image;
error:
    if (fp) fclose(fp); if (data) free(data); if (image) nsvgDelete(image); return nullptr;
}

QPixmap* BitmapCache::load_svg(const std::string &light_bitmap_name, unsigned target_width, unsigned target_height,
                                 const bool grayscale, const bool dark_mode,
                                 const std::string& new_color, const float scale_in_center)
{
    const std::string dark_bitmap_name = dark_mode ? dark_version(light_bitmap_name) : "";
    const std::string bitmap_name      = dark_bitmap_name.empty() ? light_bitmap_name : dark_bitmap_name;
    std::string bitmap_key = bitmap_name
        + (target_height != 0 ? "-h" + std::to_string(target_height) : "-w" + std::to_string(target_width))
        + (m_scale != 1.0f ? "-s" + float_to_string_decimal_point(m_scale) : "")
        + (dark_mode ? "-dm" : "") + (grayscale ? "-gs" : "") + new_color;
    auto it = m_map.find(bitmap_key);
    if (it != m_map.end()) return it->second;

    std::map<std::string, std::string> replaces;
    if (dark_mode && dark_bitmap_name.empty()) {
        replaces["\"#262E30\""] = "\"#EFEFF0\""; replaces["\"#323A3D\""] = "\"#B3B3B5\"";
        replaces["\"#808080\""] = "\"#818183\""; replaces["\"#CECECE\""] = "\"#54545B\"";
        replaces["\"#6B6B6B\""] = "\"#818182\""; replaces["\"#909090\""] = "\"#FFFFFF\"";
        replaces["\"#00FF00\""] = "\"#FF0000\""; replaces["\"#F1F1F1\""] = "\"#36363B\"";
        replaces["\"#DBDBDB\""] = "\"#4A4A51\"";
    }
    if (!new_color.empty()) replaces["\"#00AE42\""] = "\"" + new_color + "\"";

    NSVGimage *image = nullptr;
    if (strstr(bitmap_name.c_str(), "printer_thumbnail") == nullptr)
        image = nsvgParseFromFileWithReplace(Slic3r::var(bitmap_name + ".svg").c_str(), "px", 96.0f, replaces);
    else
        image = nsvgParseFromFileWithReplace(Slic3r::var(bitmap_name + ".svg").c_str(), "px", 96.0f, {});
    if (!image) return nullptr;

    if (target_height == 0 && target_width == 0) target_height = (unsigned)image->height;
    target_height != 0 ? target_height *= (unsigned)m_scale : target_width *= (unsigned)m_scale;
    float svg_scale = target_height != 0 ? (float)target_height / image->height
                    : target_width  != 0 ? (float)target_width  / image->width : 1.f;
    int w = (int)(svg_scale * image->width + 0.5f), h = (int)(svg_scale * image->height + 0.5f);
    if (w * h <= 0) { nsvgDelete(image); return nullptr; }

    NSVGrasterizer *rast = nsvgCreateRasterizer();
    if (!rast) { nsvgDelete(image); return nullptr; }
    std::vector<unsigned char> data(w * h * 4, 0);
    if (scale_in_center > 0 && scale_in_center < svg_scale) {
        int sw = (int)(image->width * scale_in_center), sh = (int)(image->height * scale_in_center);
        nsvgRasterize(rast, image, 0, 0, scale_in_center,
            data.data() + (h-sh)/2*w*4 + (w-sw)/2*4, sw, sh, w*4);
    } else {
        nsvgRasterize(rast, image, 0, 0, svg_scale, data.data(), w, h, w*4);
    }
    nsvgDeleteRasterizer(rast); nsvgDelete(image);
    return this->insert_raw_rgba(bitmap_key, w, h, data.data(), grayscale);
}

QPixmap* BitmapCache::load_svg2(const std::string& bitmap_name, unsigned target_width, unsigned target_height,
                                  const bool grayscale, const bool dark_mode,
                                  const std::vector<std::string>& array_new_color, const float scale_in_center)
{
    std::map<std::string, std::string> replaces;
    if (array_new_color.size() == 2) {
        replaces["#D9D9D9"] = array_new_color[0];
        replaces["fill-opacity=\"1.0"] = array_new_color[1];
    }
    NSVGimage* image = nsvgParseFromFileWithReplace(Slic3r::var(bitmap_name + ".svg").c_str(), "px", 96.0f, replaces);
    if (!image) return nullptr;

    if (target_height == 0 && target_width == 0) target_height = (unsigned)image->height;
    target_height != 0 ? target_height *= (unsigned)m_scale : target_width *= (unsigned)m_scale;
    float svg_scale = target_height != 0 ? (float)target_height / image->height
                    : target_width  != 0 ? (float)target_width  / image->width : 1.f;
    int w = (int)(svg_scale * image->width + 0.5f), h = (int)(svg_scale * image->height + 0.5f);
    if (w * h <= 0) { nsvgDelete(image); return nullptr; }

    NSVGrasterizer* rast = nsvgCreateRasterizer();
    if (!rast) { nsvgDelete(image); return nullptr; }
    std::vector<unsigned char> data(w * h * 4, 0);
    if (scale_in_center > 0 && scale_in_center < svg_scale) {
        int sw = (int)(image->width * scale_in_center), sh = (int)(image->height * scale_in_center);
        nsvgRasterize(rast, image, 0, 0, scale_in_center,
            data.data() + (h-sh)/2*w*4 + (w-sw)/2*4, sw, sh, w*4);
    } else {
        nsvgRasterize(rast, image, 0, 0, svg_scale, data.data(), w, h, w*4);
    }
    nsvgDeleteRasterizer(rast); nsvgDelete(image);

    QImage qimage(data.data(), w, h, w * 4, QImage::Format_RGBA8888);
    if (grayscale)
        qimage = qimage.convertToFormat(QImage::Format_Grayscale8).convertToFormat(QImage::Format_RGBA8888);
    return new QPixmap(QPixmap::fromImage(qimage));
}

QPixmap BitmapCache::mksolid(size_t width, size_t height,
                               unsigned char r, unsigned char g, unsigned char b, unsigned char transparency,
                               bool suppress_scaling, size_t border_width, bool dark_mode)
{
    double scale = suppress_scaling ? 1.0 : m_scale;
    size_t sw = size_t(width * scale), sh = size_t(height * scale);
    QImage image(int(sw), int(sh), QImage::Format_RGBA8888);
    unsigned char* bits = image.bits();
    for (size_t i = 0; i < sw * sh; ++i) {
        bits[i*4+0]=r; bits[i*4+1]=g; bits[i*4+2]=b; bits[i*4+3]=transparency;
    }
    if (border_width > 0) {
        if (border_width > sh) border_width = sh - 1;
        if (border_width > sw) border_width = sw - 1;
        unsigned char bc = dark_mode ? 245u : 110u;
        for (size_t x = 0; x < sw; ++x) {
            for (size_t y = 0; y < sh; ++y) {
                if (x < border_width || y < border_width ||
                    x >= sw - border_width || y >= sh - border_width) {
                    size_t idx = (x + y * sw) * 4;
                    bits[idx]=bits[idx+1]=bits[idx+2]=bc; bits[idx+3]=255u;
                }
            }
        }
    }
    return QPixmap::fromImage(std::move(image));
}

bool BitmapCache::parse_color(const std::string& scolor, unsigned char* rgb_out)
{
    if (scolor.size() == 9) {
        unsigned char rgba[4]; parse_color4(scolor, rgba);
        rgb_out[0]=rgba[0]; rgb_out[1]=rgba[1]; rgb_out[2]=rgba[2]; return true;
    }
    rgb_out[0]=rgb_out[1]=rgb_out[2]=0;
    if (scolor.size() != 7 || scolor.front() != '#') return false;
    const char* c = scolor.data() + 1;
    for (size_t i = 0; i < 3; ++i) {
        int d1=hex_digit_to_int(*c++), d2=hex_digit_to_int(*c++);
        if (d1==-1||d2==-1) return false;
        rgb_out[i] = (unsigned char)(d1*16+d2);
    }
    return true;
}

bool BitmapCache::parse_color4(const std::string& scolor, unsigned char* rgba_out)
{
    rgba_out[0]=rgba_out[1]=rgba_out[2]=0; rgba_out[3]=255;
    if ((scolor.size()!=7 && scolor.size()!=9) || scolor.front()!='#') return false;
    const char* c = scolor.data() + 1;
    for (size_t i = 0; i < scolor.size()/2; ++i) {
        int d1=hex_digit_to_int(*c++), d2=hex_digit_to_int(*c++);
        if (d1==-1||d2==-1) return false;
        rgba_out[i] = (unsigned char)(d1*16+d2);
    }
    return true;
}

bool BitmapCache::load_from_svg_file_change_color(const std::string &filename, unsigned width, unsigned height,
                                                    ImTextureID &texture_id, const char *hexColor)
{
    NSVGimage* image = nsvgParseFromFile(filename.c_str(), "px", 96.0f);
    if (!image) return false;
    char temp_color[8]; strncpy(temp_color, hexColor, 7); temp_color[7]='\0';
    unsigned int change_color = nsvg__parseColorHex(temp_color);
    change_color |= (unsigned int)(1.0f * 255) << 24;
    unsigned int green_color = 4282560000u;
    for (NSVGshape* shape = image->shapes; shape; shape = shape->next)
        if (shape->fill.color == green_color) shape->fill.color = change_color;

    float scale = (float)width / image->width;
    int n_pixels = int(width) * int(height);
    if (n_pixels <= 0) { nsvgDelete(image); return false; }

    NSVGrasterizer* rast = nsvgCreateRasterizer();
    if (!rast) { nsvgDelete(image); return false; }
    std::vector<unsigned char> data(n_pixels * 4, 0);
    nsvgRasterize(rast, image, 0, 0, scale, data.data(), width, height, width*4);

    GLint last_texture; unsigned m_image_texture{0};
    glsafe(::glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture));
    glsafe(::glGenTextures(1, &m_image_texture));
    glsafe(::glBindTexture(GL_TEXTURE_2D, m_image_texture));
    glsafe(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glsafe(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glsafe(::glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
    glsafe(::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
    texture_id = (ImTextureID)(intptr_t)m_image_texture;
    glsafe(::glBindTexture(GL_TEXTURE_2D, last_texture));

    nsvgDeleteRasterizer(rast); nsvgDelete(image);
    return true;
}

} // namespace GUI
} // namespace Slic3r

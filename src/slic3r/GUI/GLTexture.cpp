// Qt6 stub for GLTexture.cpp
#include "GLTexture.hpp"

namespace Slic3r { namespace GUI {

GLTexture::GLTexture() : m_compressor(*this) {}
GLTexture::~GLTexture() {}

void GLTexture::Compressor::reset() {}

bool GLTexture::load_from_svg_files_as_sprites_array(
    const std::vector<std::string>&, const std::vector<std::pair<int,bool>>&,
    unsigned int, bool)
{ return false; }

void GLTexture::reset() {}

void GLTexture::shutdown()
{
    // stub
}

}} // namespace Slic3r::GUI


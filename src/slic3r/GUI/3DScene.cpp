// Qt6 stub for 3DScene.cpp
#include "3DScene.hpp"

// global namespace — matches the extern declaration in 3DScene.hpp
void glAssertRecentCallImpl(const char* /*file_name*/, unsigned int /*line*/, const char* /*function_name*/) {}

namespace Slic3r {

void GLIndexedVertexArray::release_geometry() {}

void GLVolume::set_color(const std::array<float, 4>& rgba)
{
    color = rgba;
}

void GLVolume::set_render_color(float r, float g, float b, float a)
{
    render_color = { r, g, b, a };
}

void GLVolume::set_render_color(const std::array<float, 4>& rgba)
{
    render_color = rgba;
}

void GLVolume::set_render_color()
{
    render_color = color;
}

void GLVolumeCollection::clear()
{
    for (GLVolume *v : volumes)
        delete v;
    volumes.clear();
}

int GLVolumeCollection::load_object_volume(
    const ModelObject* /*model_object*/,
    int                /*obj_idx*/,
    int                /*volume_idx*/,
    int                /*instance_idx*/,
    const std::string& /*color_by*/,
    bool               /*opengl_initialized*/,
    bool               /*in_assemble_view*/,
    bool               /*use_loaded_id*/,
    bool               /*lod_enabled*/)
{
    return 0;
}

} // namespace Slic3r

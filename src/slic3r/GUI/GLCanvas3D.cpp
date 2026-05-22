// Qt6 stub for GLCanvas3D.cpp
#include "GLCanvas3D.hpp"

namespace Slic3r { namespace GUI {

void GLCanvas3D::render_thumbnail_framebuffer(
    const std::shared_ptr<OpenGLManager>& /*p_ogl_manager*/,
    ThumbnailData& /*thumbnail_data*/,
    unsigned int /*w*/,
    unsigned int /*h*/,
    const ThumbnailsParams& /*thumbnail_params*/,
    PartPlateList& /*partplate_list*/,
    ModelObjectPtrs& /*model_objects*/,
    const GLVolumeCollection& /*volumes*/,
    std::vector<std::array<float, 4>>& /*extruder_colors*/,
    const std::shared_ptr<GLShaderProgram>& /*shader*/,
    Camera::EType /*camera_type*/,
    Camera::ViewAngleType /*camera_view_angle_type*/,
    bool /*for_picking*/,
    bool /*ban_light*/,
    const ExtraThumbData& /*extra_thumb_data*/)
{
    // stub
}

}} // namespace Slic3r::GUI


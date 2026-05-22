// Qt6 stub for GLShadersManager.cpp
#include "GLShadersManager.hpp"

namespace Slic3r {

std::pair<bool, std::string> GLShadersManager::init()
{
    return { true, {} };
}

void GLShadersManager::shutdown()
{
    m_shaders.clear();
    m_current_shader.reset();
}

const std::shared_ptr<GLShaderProgram>& GLShadersManager::get_shader(const std::string& /*shader_name*/) const
{
    static std::shared_ptr<GLShaderProgram> null_shader;
    return null_shader;
}

void GLShadersManager::bind_shader(const std::shared_ptr<GLShaderProgram>& p_shader)
{
    m_current_shader = p_shader;
}

void GLShadersManager::unbind_shader()
{
    m_current_shader.reset();
}

} // namespace Slic3r

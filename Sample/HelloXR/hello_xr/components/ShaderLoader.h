//
// Created by hy on 2023/6/14.
//

#pragma once

#include <memory>
#include <string>

namespace vr
{

    class ShaderProgram;

    class ShaderLoader {
    public:

        static std::shared_ptr<ShaderProgram> LoadShaderForPath(const std::string& v_gl_path, const std::string& f_gl_path);
        static std::shared_ptr<ShaderProgram> LoadShaderSource(const std::string& v_gl, const std::string& f_gl);
        static std::shared_ptr<ShaderProgram> LoadShaderSource(const std::string& v_gl, const std::string& g_gl, const std::string& f_gl);

    };

}

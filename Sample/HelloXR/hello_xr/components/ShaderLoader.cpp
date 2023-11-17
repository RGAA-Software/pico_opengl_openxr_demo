//
// Created by hy on 2023/6/14.
//

#include "ShaderLoader.h"
#include "ShaderProgram.h"
#include "NLogger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace vr
{

    std::shared_ptr<ShaderProgram> ShaderLoader::LoadShaderForPath(const std::string &v_gl_path, const std::string &f_gl_path) {
        // Read the Vertex Shader code from the file
        std::string vertex_shader_code;
        std::ifstream vertex_shader_stream(v_gl_path, std::ios::in);
        if (vertex_shader_stream.is_open()) {
            std::stringstream sstr;
            sstr << vertex_shader_stream.rdbuf();
            vertex_shader_code = sstr.str();
            vertex_shader_stream.close();
        } else {
            std::cerr << "failed to open : " << v_gl_path << std::endl;
            return nullptr;
        }

        // Read the Fragment Shader code from the file
        std::string fragment_shader_code;
        std::ifstream fragment_shader_stream(f_gl_path, std::ios::in);
        if (fragment_shader_stream.is_open()) {
            std::stringstream sstr;
            sstr << fragment_shader_stream.rdbuf();
            fragment_shader_code = sstr.str();
            fragment_shader_stream.close();
        } else {
            std::cerr << "failed to open : " << v_gl_path << std::endl;
            return nullptr;
        }

        return LoadShaderSource(vertex_shader_code, fragment_shader_code);
    }

    std::shared_ptr<ShaderProgram> ShaderLoader::LoadShaderSource(const std::string& v_gl, const std::string& g_gl, const std::string& f_gl) {
        if (v_gl.empty() || f_gl.empty()) {
            return nullptr;
        }
        auto result = std::make_shared<ShaderProgram>();
        GLint compile_result = GL_FALSE;
        int log_len;

        // Create the shaders
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);

        // Compile Vertex Shader
        char const *vertex_source = v_gl.c_str();
        glShaderSource(vertex_shader_id, 1, &vertex_source, NULL);
        glCompileShader(vertex_shader_id);

        // Check Vertex Shader
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compile_result);
        glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            std::vector<char> err_msg(log_len + 1);
            glGetShaderInfoLog(vertex_shader_id, log_len, NULL, &err_msg[0]);
            printf("%s\n", &err_msg[0]);
            NLog::Write(vr::Level::Info, Fmt("Shader result: %s", err_msg.data()));
        }

        if (!g_gl.empty()) {
            // Compile Geometry Shader
            char const *geometry_source = g_gl.c_str();
            glShaderSource(geometry_shader_id, 1, &geometry_source, NULL);
            glCompileShader(geometry_shader_id);

            // Check Vertex Shader
            glGetShaderiv(geometry_shader_id, GL_COMPILE_STATUS, &compile_result);
            glGetShaderiv(geometry_shader_id, GL_INFO_LOG_LENGTH, &log_len);
            if (log_len > 0) {
                std::vector<char> err_msg(log_len + 1);
                glGetShaderInfoLog(geometry_shader_id, log_len, NULL, &err_msg[0]);
                printf("%s\n", &err_msg[0]);
                NLog::Write(vr::Level::Info, Fmt("Shader result: %s", err_msg.data()));
            }
        }

        // Compile Fragment Shader
        char const *fragment_source = f_gl.c_str();
        glShaderSource(fragment_shader_id, 1, &fragment_source, NULL);
        glCompileShader(fragment_shader_id);

        // Check Fragment Shader
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compile_result);
        glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            std::vector<char> err_msg(log_len + 1);
            glGetShaderInfoLog(fragment_shader_id, log_len, NULL, &err_msg[0]);
            printf("%s\n", &err_msg[0]);
            NLog::Write(vr::Level::Info, Fmt("Shader result: %s", err_msg.data()));
        }

        // Link the program
        GLuint program_id = glCreateProgram();

        glAttachShader(program_id, vertex_shader_id);
        if (!g_gl.empty()) {
            glAttachShader(program_id, geometry_shader_id);
        }
        glAttachShader(program_id, fragment_shader_id);
        glLinkProgram(program_id);


        // Check the program
        glGetProgramiv(program_id, GL_LINK_STATUS, &compile_result);
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            std::vector<char> err_msg(log_len + 1);
            glGetProgramInfoLog(program_id, log_len, NULL, &err_msg[0]);
            printf("%s\n", &err_msg[0]);
            NLog::Write(vr::Level::Info, Fmt("Program result: %s", err_msg.data()));
        }

        glDetachShader(program_id, vertex_shader_id);
        glDetachShader(program_id, fragment_shader_id);
        glDetachShader(program_id, geometry_shader_id);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
        glDeleteShader(geometry_shader_id);

        NLog::Write(vr::Level::Info, Fmt("Shader success, program id: %d", program_id));
        result->program_id = program_id;
        return result;
    }

    std::shared_ptr<ShaderProgram> ShaderLoader::LoadShaderSource(const std::string &v_gl, const std::string &f_gl) {
        return LoadShaderSource(v_gl, "", f_gl);
    }

}
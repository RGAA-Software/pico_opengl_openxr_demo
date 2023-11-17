//
// Created by hy on 2023/6/14.
//

#pragma once

#include <memory>
#include <string>

#include "GLHeader.h"

namespace vr
{

    class ShaderProgram {
    public:

        ShaderProgram() {

        }

        ~ShaderProgram() {

        }

        void Use() {
             glUseProgram(program_id);
        }

        void Unuse() {
            glUseProgram(0);
        }

        void Release() {
            if (program_id) {
                glDeleteProgram(program_id);
            }
        }

        int GetAttribLocation(const std::string& name) {
            return  glGetAttribLocation(program_id, name.c_str());
        }

        int GetUniformLocation(const std::string& name) {
            return  glGetUniformLocation(program_id, name.c_str());
        }

        void SetUniform1i(const std::string& name, int value) {
             glUniform1i(GetUniformLocation(name), value);
        }

        void SetUniform1f(const std::string& name, float value) {
             glUniform1f(GetUniformLocation(name), value);
        }

        void SetUniform2fv(const std::string& name, const glm::vec2& vec) {
             glUniform2f(GetUniformLocation(name), vec.x, vec.y);
        }

        void SetUniform3fv(const std::string& name, const glm::vec3& vec) {
             glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
        }

        void SetUniformMatrix(const std::string& name, const glm::mat4& m) {
            auto loc = GetUniformLocation(name);
            glUniformMatrix4fv(loc, 1, false, glm::value_ptr(m));
        }

        void SetUniformMatrix(const std::string& name, const float* m) {
            auto loc = GetUniformLocation(name);
            glUniformMatrix4fv(loc, 1, false, m);
        }

    public:
        GLuint program_id;

    };

}
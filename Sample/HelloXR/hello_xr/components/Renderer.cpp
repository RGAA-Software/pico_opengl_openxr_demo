//
// Created by hy on 2023/6/14.
//

#include "Renderer.h"
#include "ShaderLoader.h"
#include "ShaderProgram.h"
#include "Director.h"

namespace vr
{

    Renderer::Renderer(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs)
        : Releasable(director), AABB(director) {
        this->director = director;
        shader = ShaderLoader::LoadShaderSource(vs, fs);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    Renderer::~Renderer() {
        director->RemoveRenderer(GetResId());
    }

    bool Renderer::Render(float delta) {
        if (!Ready()) {
            return false;
        }

        glm::mat4 mvp = vp * model;

        glBindVertexArray(vao);
        shader->Use();
        shader->SetUniformMatrix("MVP", mvp);

        return true;
    }

    void Renderer::RenderCompleted() {
        if (shader) {
            shader->Unuse();
        }
        if (vao) {
            glBindVertexArray(0);
        }
        if (vertex_buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void Renderer::Release() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }
        if (vertex_buffer) {
            glDeleteBuffers(1, &vertex_buffer);
        }
        if (shader) {
            shader->Release();
        }
    }

    bool Renderer::Ready() {
        return shader && shader->program_id > 0 && vao > 0;
    }

    void Renderer::UpdateSingleColor(const glm::vec3& color) {
        single_color = color;
    }

    void Renderer::UpdateViewProjectionMatrix(const glm::mat4& m) {
        this->vp = m;
    }

    void Renderer::UpdateModelMatrix(const glm::mat4& m) {
        this->model = m;
    }

    void Renderer::SetDebugEnabled(bool e) {
        debug_enabled = e;
    }

    std::string_view Renderer::Name() {
        return name;
    }
}
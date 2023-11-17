//
// Created by hy on 2023/6/14.
//

#pragma once

#include <memory>
#include "GLHeader.h"
#include "Releasable.h"
#include "AABB.h"

namespace vr
{

    class Director;
    class ShaderProgram;

    class Renderer : public Releasable, public AABB {
    public:

        Renderer(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs);
        virtual ~Renderer();

        virtual bool Render(float delta);
        virtual void RenderCompleted();
        virtual void Release() override;
        virtual void UpdateViewProjectionMatrix(const glm::mat4& m);
        // calculate new position for intersection checking
        virtual void UpdateModelMatrix(const glm::mat4& m);
        virtual std::string_view Name();

        bool Ready();
        void UpdateSingleColor(const glm::vec3& color);
        void SetDebugEnabled(bool e);

    protected:

        std::shared_ptr<Director> director = nullptr;
        std::shared_ptr<ShaderProgram> shader = nullptr;
        GLuint vao = 0;
        GLuint vertex_buffer = 0;
        glm::vec3 single_color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::mat4 vp = glm::mat4(1.0);
        glm::mat4 model = glm::mat4(1.0);
        uint32_t z_order = 0;
        bool debug_enabled = false;
        std::string name{};

    };

}

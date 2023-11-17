//
// Created by hy on 2023/6/14.
//

#include "Rectangle.h"
#include "BasicShaders.h"
#include "ShaderProgram.h"
#include "Director.h"
#include "NLogger.h"

namespace vr
{

    std::shared_ptr<Rectangle> Rectangle::MakeAttribColor(const std::shared_ptr<Director>& director) {
        auto instance = std::make_shared<Rectangle>(director, kBasicVertexShader, kBasicColorFragmentShader);
        director->AddRenderer(instance);
        return instance;
    }

    std::shared_ptr<Rectangle> Rectangle::MakeSingleColor(const std::shared_ptr<Director>& director, const glm::vec3& color) {
        auto instance = std::make_shared<Rectangle>(director, kBasicVertexShader, kBasicSingleColorFragmentShader);
        instance->UpdateSingleColor(color);
        director->AddRenderer(instance);
        return instance;
    }

    Rectangle::Rectangle(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs)
        : Renderer(director, vs, fs) {

        this->name = "Rectangle";

        float NH = -1.0f;
        float PH = 1.0f;
        float ZERO = 0.0f;
        float ONE = 1.0f;
        float Z = -2.5f;

        float vertices[] = {
            NH, NH, Z, ONE, ONE, ONE, ZERO, ZERO,
            PH, NH, Z, ONE, ONE, ONE, ONE,  ZERO,
            PH, PH, Z, ONE, ONE, ONE, ONE,  ONE,

            PH, PH, Z, ONE, ONE, ONE, ONE,  ONE,
            NH, PH, Z, ONE, ONE, ONE, ZERO, ONE,
            NH, NH, Z, ONE, ONE, ONE, ZERO, ZERO
        };

        SetBorderPoints(glm::vec3(NH, NH, Z), glm::vec3(PH, PH, Z));

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    Rectangle::~Rectangle() {
        Renderer::~Renderer();
    }

    bool Rectangle::Render(float delta) {
        if (!Renderer::Render(delta)) {
            return false;
        }

        shader->SetUniform3fv("UniformColor", single_color);
        if (IsIntersectionPointIn()) {
            shader->SetUniform3fv("UniformColor", glm::vec3(1.0, 0.5, 0.8));
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

        RenderCompleted();
        return true;
    }

    void Rectangle::UpdateModelMatrix(const glm::mat4 &m) {
        Renderer::UpdateModelMatrix(m);

        auto new_left_bottom = m * glm::vec4(origin_left_bottom, 0.0f, 1.0f);
        auto new_right_top = m * glm::vec4(origin_right_top, 0.0f, 1.0f);

        left_bottom = glm::vec2(new_left_bottom.x, new_left_bottom.y);
        right_top = glm::vec2(new_right_top.x, new_right_top.y);
        if (debug_enabled) {
            //auto msg = glm::to_string(m);
            //vr::NLog::Write(Level::Info, Fmt("Id: %llu,  %s", GetResId(), msg.c_str()));
            //vr::NLog::Write(Level::Info, Fmt("Id: %llu,  %f, %f", GetResId(), new_left_bottom.x, new_left_bottom.y));
        }
    }

    void Rectangle::Release() {
        Renderer::Release();
    }

}

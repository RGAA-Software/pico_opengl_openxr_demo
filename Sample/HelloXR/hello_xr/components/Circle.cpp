//
// Created by hy on 2023/6/15.
//

#include "Circle.h"
#include "Director.h"
#include "BasicShaders.h"

namespace vr
{

    std::shared_ptr<Circle> Circle::MakeAttribColor(const std::shared_ptr<Director>& director) {
        auto instance = std::make_shared<Circle>(director, kBasicVertexShader, kBasicColorFragmentShader);
        director->AddRenderer(instance);
        return instance;
    }

    std::shared_ptr<Circle> Circle::MakeSingleColor(const std::shared_ptr<Director>& director, const glm::vec3& color) {
        auto instance = std::make_shared<Circle>(director, kBasicVertexShader, kBasicSingleColorFragmentShader);
        director->AddRenderer(instance);
        return instance;
    }

    Circle::Circle(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs)
        : Renderer(director, vs, fs) {

        this->name = "Circle";

        //float Z = -2.49f;
        float Z = .0f;

        int slice = 30;
        int line_float_size = 8;
        int vertices_size = (slice + 1) * line_float_size;
        float* vertices = new float[vertices_size]{0};
        float cr = 1.0f;
        float cg = 0.5f;
        float cb = 0.3f;
        float radius = 0.025f;
        auto add_point = [&](int line, float x, float y, float z, float r, float g, float b, float u, float v, bool center) {
            auto base_idx = line * line_float_size;
            *(vertices + base_idx + 0) = x;
            *(vertices + base_idx + 1) = y;
            *(vertices + base_idx + 2) = z;

            *(vertices + base_idx + 3) = r;
            *(vertices + base_idx + 4) = g;
            *(vertices + base_idx + 5) = b;

            *(vertices + base_idx + 6) = u;
            *(vertices + base_idx + 7) = v;
            //PrintLog("x: %f, y: %f", hw + hw * x, hh + hh * y);
        };
        add_point(0, 0.0f, 0.0f, Z, cr, cg, cb, 0.5, 0.5, true);

        auto item_angel = 360.0f/(slice - 1);
        for (int i = 0; i < slice; i++) {
            auto angel = i * item_angel;
            auto cos = glm::cos(glm::radians(angel)) * radius;
            auto sin = glm::sin(glm::radians(angel)) * radius;

            auto x = cos;
            auto y = sin;
            auto z = Z;

            auto u = (cos + 1)/2;
            auto v = (sin + 1)/2;

            add_point(i + 1, x, y, z, cr, cg, cb, u, v, false);
        }

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_size, vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    Circle::~Circle() {
        Renderer::~Renderer();
    }

    bool Circle::Render(float delta) {
        if (!Renderer::Render(delta)) {
            return false;
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, 31);

        RenderCompleted();
        return true;
    }

    void Circle::Release() {
        Renderer::Release();
    }
}
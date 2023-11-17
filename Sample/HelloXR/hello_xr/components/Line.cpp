//
// Created by hy on 2023/6/15.
//

#include "Line.h"
#include "Director.h"
#include "BasicShaders.h"

namespace vr
{

    std::shared_ptr<Line> Line::Make(const std::shared_ptr<Director>& d,
                                          const glm::vec3& start,
                                          const glm::vec3& end,
                                          const glm::vec3& sc,
                                          const glm::vec3& ec) {
        auto line = std::make_shared<Line>(d, kBasicVertexShader, kBasicColorFragmentShader);
        line->UpdateEnds(start, end);
        line->UpdateEndsColor(sc, ec);
        d->AddRenderer(line);
        return line;
    }

    Line::Line(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs)
        : Renderer(director, vs, fs) {

        this->name = "Line";

        float vertex[] = {
            start.x, start.y, start.z, start_color.x, start_color.y, start_color.z,
            end.x, end.y, end.z, end_color.x, end_color.y, end_color.z,
        };

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Line::~Line() {
        Renderer::~Renderer();
    }

    void Line::UpdateEnds(const glm::vec3& s, const glm::vec3& e) {
        start = s;
        end = e;
    }

    void Line::UpdateEndsColor(const glm::vec3& sc, const glm::vec3& ec) {
        start_color = sc;
        end_color = ec;
    }

    bool Line::Render(float delta) {
        if (!Renderer::Render(delta)) {
            return false;
        }

        float vertex[] = {
            start.x, start.y, start.z, start_color.x, start_color.y, start_color.z,
            end.x, end.y, end.z, end_color.x, end_color.y, end_color.z,
        };

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, 2);

        RenderCompleted();
        return true;
    }

    void Line::Release() {
        Renderer::Release();
    }

}
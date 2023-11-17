//
// Created by hy on 2023/6/15.
//

#pragma once

#include "Renderer.h"

namespace vr
{

    class Line : public Renderer {
    public:

        static std::shared_ptr<Line> Make(const std::shared_ptr<Director>& d,
                                              const glm::vec3& start,
                                              const glm::vec3& end,
                                              const glm::vec3& sc,
                                              const glm::vec3& ec);


        Line(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs);
        ~Line();

        bool Render(float delta) override;
        void Release() override;

        void UpdateEnds(const glm::vec3& s, const glm::vec3& e);
        void UpdateEndsColor(const glm::vec3& sc, const glm::vec3& ec);

    private:

        glm::vec3 start = glm::vec3(0, 0, 0);
        glm::vec3 end = glm::vec3(0, 0, 0);

        glm::vec3 start_color = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 end_color = glm::vec3(1.0, 1.0, 1.0);

    };

}

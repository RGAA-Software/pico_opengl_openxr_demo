//
// Created by hy on 2023/6/15.
//

#pragma once

#include "Renderer.h"

namespace vr
{

    class Circle : public Renderer {
    public:

        static std::shared_ptr<Circle> MakeAttribColor(const std::shared_ptr<Director>& director);
        static std::shared_ptr<Circle> MakeSingleColor(const std::shared_ptr<Director>& director, const glm::vec3& color);

        Circle(const std::shared_ptr<Director>& director, const std::string& vs, const std::string& fs);
        ~Circle();

        bool Render(float delta) override;
        void Release() override;

    private:

    };

}

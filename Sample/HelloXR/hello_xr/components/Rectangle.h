//
// Created by hy on 2023/6/14.
//

#pragma once

#include "Renderer.h"

namespace vr
{

    class Rectangle : public Renderer {
    public:

        static std::shared_ptr<Rectangle> MakeAttribColor(const std::shared_ptr<Director>& director);
        static std::shared_ptr<Rectangle> MakeSingleColor(const std::shared_ptr<Director>&, const glm::vec3& color);

        Rectangle(const std::shared_ptr<Director>&, const std::string& vs, const std::string& fs);
        ~Rectangle();

        bool Render(float delta) override;
        void Release() override;
        void UpdateModelMatrix(const glm::mat4 &m) override;

    private:

    };

}

//
// Created by hy on 2023/6/15.
//

#pragma once

#include <memory>
#include <limits>

#include "GLHeader.h"

namespace vr
{

    class HandEvent;
    class Director;

    class AABB {
    public:

        AABB(const std::shared_ptr<Director>& director);
        virtual ~AABB();

        void SetBorderPoints(const glm::vec2& lb, const glm::vec2& rt);
        bool IsIntersectionPointIn() const;

    protected:

        std::shared_ptr<Director> director;

        //最初的值
        glm::vec2 origin_left_bottom{0, 0};
        glm::vec2 origin_right_top{0, 0};
        //model * origin 后得到的值，参与计算
        glm::vec2 left_bottom{0, 0};
        glm::vec2 right_top{0, 0};

    };

}
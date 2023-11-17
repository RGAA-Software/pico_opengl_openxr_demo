//
// Created by hy on 2023/6/15.
//

#include "AABB.h"
#include "Event.h"
#include "Director.h"

namespace vr
{

    AABB::AABB(const std::shared_ptr<Director>& director) {
        this->director = director;
    }

    AABB::~AABB() {
    }

    void AABB::SetBorderPoints(const glm::vec2& lb, const glm::vec2& rt) {
        origin_left_bottom = lb;
        origin_right_top = rt;
        left_bottom = lb;
        right_top = rt;
    }

    bool AABB::IsIntersectionPointIn() const {
        auto left_hand = director->GetLeftHand();
        auto right_hand = director->GetRightHand();
        bool left_in = left_hand && left_hand->active &&
                left_hand->intersection.x >= left_bottom.x &&left_hand->intersection.x <= right_top.x &&
                left_hand->intersection.y >= left_bottom.y && left_hand->intersection.y <= right_top.y;
        bool right_in = right_hand && right_hand->active &&
                right_hand->intersection.x >= left_bottom.x && right_hand->intersection.x <= right_top.x &&
                right_hand->intersection.y >= left_bottom.y && right_hand->intersection.y <= right_top.y;
        return left_in || right_in;
    }

}
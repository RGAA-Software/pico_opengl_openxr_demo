//
// Created by hy on 2023/6/16.
//

#pragma once

#include <limits>

#include "GLHeader.h"

namespace vr
{

    enum class HandSide {
        kLeft,
        kRight
    };

    class Event {
    public:


    };

    class HandEvent : public Event {
    public:
        static std::shared_ptr<HandEvent> Make(const HandSide& hand);

    public:
        HandSide hand;
        bool active = false;
        float scale = 1.0f;
        glm::vec2 intersection = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        glm::vec3 position = glm::vec3(0,0,0);
        glm::vec3 orientation = glm::vec3(0,0,0);
    };
}
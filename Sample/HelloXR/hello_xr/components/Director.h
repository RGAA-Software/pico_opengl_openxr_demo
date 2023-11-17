//
// Created by hy on 2023/6/14.
//

#pragma once

#include <unordered_map>
#include <memory>

#include "Event.h"
#include "GLHeader.h"

namespace vr
{

    class Renderer;

    class Director {
    public:

        Director();
        ~Director();

        void UpdateMVP(const glm::mat4& m);
        glm::mat4 GetMVP();
        const float* GetMVPMatrix() const;

        void AddRenderer(const std::shared_ptr<Renderer>& renderer);
        void RemoveRenderer(uint64_t res_id);
        void UpdateHandInfo(const HandSide& hand, const glm::vec3& pos, const glm::vec3& orientation, float scale);
        void UpdateIntersectionPoint(const HandSide& hand, const glm::vec2& point);
        void UpdateViewProjectionMatrix(const glm::mat4& m);
        uint32_t NextResourceId();

        std::shared_ptr<HandEvent> GetLeftHand();
        std::shared_ptr<HandEvent> GetRightHand();

    private:

        glm::mat4 mvp;
        std::unordered_map<uint64_t, std::shared_ptr<Renderer>> renderers;

        std::shared_ptr<HandEvent> left_hand_event = nullptr;
        std::shared_ptr<HandEvent> right_hand_event = nullptr;
    };

}
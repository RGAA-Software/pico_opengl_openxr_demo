//
// Created by hy on 2023/6/14.
//

#include "Director.h"
#include "NLogger.h"
#include "Renderer.h"
#include "Event.h"

namespace vr
{

    Director::Director() {
        left_hand_event = HandEvent::Make(HandSide::kLeft);
        right_hand_event = HandEvent::Make(HandSide::kRight);
    }

    Director::~Director() {

    }

    void Director::UpdateMVP(const glm::mat4& m) {
        mvp = m;
    }

    glm::mat4 Director::GetMVP() {
        return mvp;
    }

    const float* Director::GetMVPMatrix() const {
        return glm::value_ptr(mvp);
    }

    void Director::AddRenderer(const std::shared_ptr<Renderer>& renderer) {
        renderers.insert(std::make_pair(renderer->GetResId(), renderer));
    }

    void Director::RemoveRenderer(uint64_t res_id) {
        renderers.erase(res_id);
    }

    void Director::UpdateHandInfo(const HandSide& hand, const glm::vec3& pos, const glm::vec3& orientation, float scale) {
        auto target_hand = (hand == HandSide::kLeft ? left_hand_event : right_hand_event);
        target_hand->active = true;
        target_hand->position = pos;
        target_hand->orientation = orientation;
        target_hand->scale = scale;
    }

    void Director::UpdateIntersectionPoint(const HandSide& side, const glm::vec2& point) {
        if (side == HandSide::kLeft) {
            left_hand_event->intersection = point;
        }
        else if (side == HandSide::kRight) {
            right_hand_event->intersection = point;
        }
    }

    void Director::UpdateViewProjectionMatrix(const glm::mat4& m) {
        for (auto& [_, renderer] : renderers) {
            renderer->UpdateViewProjectionMatrix(m);
            NLog::Write(Level::Info, Fmt("%llu -> %s", renderer->GetResId(), renderer->Name().data()));
        }
        NLog::Write(Level::Info, Fmt("<- -> "));
    }

    uint32_t Director::NextResourceId() {
        return renderers.size() + 1;
    }

    std::shared_ptr<HandEvent> Director::GetLeftHand() {
        return left_hand_event;
    }

    std::shared_ptr<HandEvent> Director::GetRightHand() {
        return right_hand_event;
    }

}
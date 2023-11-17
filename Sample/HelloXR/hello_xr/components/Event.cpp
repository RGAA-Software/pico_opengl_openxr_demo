//
// Created by hy on 2023/6/16.
//

#include "Event.h"

namespace vr
{

    std::shared_ptr<HandEvent> HandEvent::Make(const HandSide& hand) {
        auto event = std::make_shared<HandEvent>();
        event->hand = hand;
        return event;
    }

}
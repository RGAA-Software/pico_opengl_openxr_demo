//
// Created by hy on 2023/6/15.
//
#include "Releasable.h"
#include "Director.h"

namespace vr
{

    Releasable::Releasable(const std::shared_ptr<Director>& director) {
        res_id = director->NextResourceId();
    }

    Releasable::~Releasable() {}

    uint64_t Releasable::GetResId() {
        return res_id;
    }
}
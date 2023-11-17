//
// Created by hy on 2023/6/15.
//

#pragma once

#include <memory>

namespace vr
{

    class Director;

    class Releasable {
    public:

        Releasable(const std::shared_ptr<Director>& director);
        virtual ~Releasable();

        virtual void Release() = 0;
        uint64_t GetResId();

    protected:

        uint64_t res_id = 0;

    };

}

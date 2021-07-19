//
// Created by stephen on 2021-02-10.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Shark {
    static ECS::Entity createShark(vec2 pos, float speed);
};


//
// Created by austi on 2021-02-02.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"


struct Island {
    static ECS::Entity createIsland(vec2 pos, int type);
};


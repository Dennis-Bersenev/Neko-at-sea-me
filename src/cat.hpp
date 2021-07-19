//
// Created by stephen on 2021-03-13.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Cat {
    static ECS::Entity createCat(vec2 pos, float frames, float rows, std::string path, std::string name);
};

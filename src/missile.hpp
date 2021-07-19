#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Missile
{
    // Creates all the associated render resources and default transform
    static ECS::Entity createMissile(vec2 pos, float power, ECS::Entity target);
};

//
// Created by austi on 2021-02-02.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"


struct Boat {
    static ECS::Entity createBoat(vec2 position, int type, int player_number, bool isAI);
    ECS::Entity health;
};


//
// Created by stephen on 2021-03-03.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Player {
    static ECS::Entity createPlayer(int player_number);
};


//
// Created by yichen on 2021-03-17.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct AIPlayer {
    static ECS::Entity createAIPlayer(int player_number);
};

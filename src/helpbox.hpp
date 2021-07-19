//
// Created by mandy77 on 2021-02-06.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"


struct Helpbox {
    static ECS::Entity createHelpbox(vec2 pos,std::string name);
};

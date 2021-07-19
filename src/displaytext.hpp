//
// Created by mandy77 on 2021-02-06.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"


struct DisplayText {
    static ECS::Entity createDisplayText(vec2 pos,std::string name, std::string path, float scale);
};

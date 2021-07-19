//
// Created by mandy77 on 2021-02-06.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"


struct Button {
    static ECS::Entity createButton(vec2 pos,std::string name);

    static ECS::Entity createSkillButton(vec2 pos, std::string name, std::string path, ECS::Entity self);
};

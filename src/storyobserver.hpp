//
// Created by yichen on 2021-02-07.
//
#pragma once
#include "tiny_ecs.hpp"
#include "world.hpp"
#include "text.hpp"

struct StorySystem
{

    int chapter_1_b; 
    int chapter_1_e;

    int chapter_2_b;
    int chapter_2_e;

    int chapter_3_b;
    int chapter_3_e;

    int chapter_4_b;
    int chapter_4_e;
 
    
    std::vector<ECS::Entity> curr_cutscene;
    std::shared_ptr<Font> font;
    ECS::Entity skipbutton;

    StorySystem(WorldSystem& world);

    void on_next_text(ECS::Entity entity);
    void restart_scene();
   
};
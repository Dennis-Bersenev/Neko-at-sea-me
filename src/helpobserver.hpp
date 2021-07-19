//
// Created by yichen on 2021-02-07.
//
#pragma once
#include "tiny_ecs.hpp"
#include "world.hpp"
#include "text.hpp"

struct HelpSystem
{

    int menu_tip; 
    int selection_tip;
    int game_tip;
    int level_tip;
   // ECS::Entity textEntityAchievements;
    
    std::vector<ECS::Entity> curr_help_information;
    std::vector<ECS::Entity> curr_help_tool;
    std::shared_ptr<Font> font;
    ECS::Entity skipbutton;
    ECS::Entity advance_text;

    HelpSystem(WorldSystem& world);

    void ongetHelp(ECS::Entity entity);
    void onhelpScene(int scene);
    void hovergetHelp(ECS::Entity entity);
    void handle_other_help(ECS::Entity entity);
};
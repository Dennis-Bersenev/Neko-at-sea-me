//
// Created by yichen on 2021-02-07.
//
#pragma once
#include "tiny_ecs.hpp"
#include "world.hpp"
#include "text.hpp"

struct AchievementsSystem
{
    int num_gamestart;
    int num_attack;
    bool ach_gamestart_1;
    bool ach_gamestart_5;
    bool ach_attack_1;
    ECS::Entity textEntityAchievements;

    std::shared_ptr<Font> font;

    AchievementsSystem(WorldSystem& world);

    void onGamestart();
    void onAttack(ECS::Entity& entity);
    void setAchievementTimer(); 
};
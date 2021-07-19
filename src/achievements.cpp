//
// Created by yichen on 2021-02-07.
//
#include <functional>
#include <iostream>
#include <string>

#include "achievements.hpp"
#include "observer_pattern.hpp"
#include "world.hpp"
#include "shark.hpp"
#include "boat.hpp"

#include "text.hpp"

AchievementsSystem::AchievementsSystem(WorldSystem& world)
{
    // initialize local variables
    num_gamestart = 0;
    num_attack = 0;
    ach_gamestart_1 = false;
    ach_gamestart_5 = false;
    ach_attack_1 = false;
    textEntityAchievements = ECS::Entity();
    font = Font::load("data/fonts/Noto/NotoSans-Regular.ttf");//placeholder text until we figure out what we want

    // add observer
    world.subj_game_start.add_observer([&] {onGamestart(); });
    // world.subj_shark.add_observer_entity([&](ECS::Entity& entity) {onAttack(entity); });
}

void AchievementsSystem::onGamestart()
{
    num_gamestart++;
    if (num_gamestart == 1 && ach_gamestart_1 == false)
    {
        ach_gamestart_1 = true;
        std::cout << "===============================" << std::endl;
        std::cout << "Achievement Unlock: First Game!" << std::endl;
        std::cout << "===============================" << std::endl;

        if (ECS::registry<Text>.has(textEntityAchievements)) {
            ECS::registry<Text>.remove(textEntityAchievements);
        }
            ECS::registry<Text>.insert(
                textEntityAchievements,
                Text("Achievement Unlock: First Game!", font, { 100.0f, 700.0f })
            );
        //setting text to be smaller
        ECS::registry<Text>.get(textEntityAchievements).scale = 0.7;
        //set to yellow
        ECS::registry<Text>.get(textEntityAchievements).colour = { 1.0f, 1.0f, 0.0f };
        setAchievementTimer();
    }
    if (num_gamestart == 5 && ach_gamestart_5 == false)
    {
        ach_gamestart_5 = true;
        std::cout << "=====================================" << std::endl;
        std::cout << "Achievement Unlock: 5 times in a row!" << std::endl;
        std::cout << "=====================================" << std::endl;

        if (ECS::registry<Text>.has(textEntityAchievements)) {
            ECS::registry<Text>.remove(textEntityAchievements);
        }
        ECS::registry<Text>.insert(
            textEntityAchievements,
            Text("Achievement Unlock: 5 times in a row!", font, { 100.0f, 700.0f })
        );
        //setting text to be smaller
        ECS::registry<Text>.get(textEntityAchievements).scale = 0.7;
        //set to yellow
        ECS::registry<Text>.get(textEntityAchievements).colour = { 1.0f, 1.0f, 0.0f };
        setAchievementTimer();
    }
}

void AchievementsSystem::onAttack(ECS::Entity& entity)
{
    num_attack++;
    if (num_attack == 1 && ach_attack_1 == false)
    {
        std::string attacker;
        if (ECS::registry<Shark>.has(entity))
        {
            attacker = "Shark";
        }
        else if (ECS::registry<Boat>.has(entity))
        {
            attacker = "Boat";
        }
        else
        {
            attacker = "NULL";
        }
        ach_attack_1 = true;
        std::cout << "===========================================" << std::endl;
        std::cout << "Achievement Unlock: First Attack by " << attacker << "!" << std::endl;
        std::cout << "===========================================" << std::endl;

        std::string text = "Achievement Unlock : First Attack by " + attacker + "!";

        if (ECS::registry<Text>.has(textEntityAchievements)) {
            ECS::registry<Text>.remove(textEntityAchievements);
        }
        ECS::registry<Text>.insert(
            textEntityAchievements,
            Text(text, font, { 100.0f, 700.0f })
        );
        //setting text to be smaller
        ECS::registry<Text>.get(textEntityAchievements).scale = 0.7;
        //set to yellow
        ECS::registry<Text>.get(textEntityAchievements).colour = { 1.0f, 1.0f, 0.0f };
        setAchievementTimer();
    }

}

void AchievementsSystem::setAchievementTimer()
{
    if (!ECS::registry<AchievementTimer>.has(textEntityAchievements)) {
        ECS::registry<AchievementTimer>.emplace(textEntityAchievements);
    }
}




//
// Created by stephen on 2021-03-03.
//

// Header
#include "player.hpp"

ECS::Entity Player::createPlayer(int player_number)
{
    auto entity = ECS::Entity();

    // Initialize empty fleet for on player creation
    auto& fleet = ECS::registry<Fleet>.emplace(entity);
    fleet.player_number = player_number;
    
    // Create a Boat component
    ECS::registry<Player>.emplace(entity);

    return entity;
}
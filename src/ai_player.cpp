#include "ai_player.hpp"

ECS::Entity AIPlayer::createAIPlayer(int ai_player_number)
{
    auto entity = ECS::Entity();

    auto& fleet = ECS::registry<Fleet>.emplace(entity);
    fleet.player_number = ai_player_number;

    ECS::registry<AIPlayer>.emplace(entity);

    return entity;
}

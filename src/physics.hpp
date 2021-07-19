#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
    void step(float elapsed_ms, vec2 window_size_in_game_units);
    bool box_colliding(Motion& box1, Motion& box2);
    bool wall_colliding(Motion& box1, vec2 window_size_in_game_units);
    bool precise_colliding(ECS::Entity entity1, ECS::Entity entity2);

    // Stucture to store collision information
    struct Collision
    {
        // Note, the first object is stored in the ECS container.entities
        ECS::Entity other; // the second object involved in the collision
        Collision(ECS::Entity& other);
    };
};

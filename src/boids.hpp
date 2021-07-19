//
// Created by yichen on 2021-04-06.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Boids {
    static void createBoid(vec2 position);
    static void updateBoids(vec2 window);
    static void stepMoveBoids();
    static void clearBoids();
};
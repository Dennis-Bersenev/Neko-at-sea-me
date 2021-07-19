//
// Created by yichen on 2021-04-01.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "boat.hpp"

struct SkillSystem
{
	static void reinit_stats();
	static void init_skills();

	static void recon_s1(ECS::Entity self); // boost self spd
	static void recon_s2(ECS::Entity self); // enlarge self range

	static void attack_s1(ECS::Entity self); // fire missiles to all the enemies in range
	static void attack_s2(ECS::Entity self); // boost damage; can't move

	static void regular_s1(ECS::Entity self); // emergency repair
	static void regular_s2(ECS::Entity self); // risky group repair

	static void big_s1(ECS::Entity self); // spawn boids
	static void big_s2(ECS::Entity self); // reguide the enemy missiles
};
#include "skill.hpp"
#include "player.hpp"
#include "ai_player.hpp"
#include "boat.hpp"
#include "missile.hpp"
#include "motion_spline.hpp"
#include "boids.hpp"
#include <set>
#include <iostream>
#include "nlohmann/json.hpp"
using namespace nlohmann;

void inner_reinit_recon(ECS::Entity recon)
{
	auto& recon_status = recon.get<Status>();
	auto& recon_movement = recon.get<FixedMovement>();
	// can be optimized by doing it once in world
	std::string name = "data/boats/recon_boat.json";
	std::ifstream i(name);
	json j;
	i >> j;
	// recon modifiable stats are range and speed
	recon_status.view_range = j["stats"]["view_range"];
	recon_status.speed = j["stats"]["speed"];
	recon_movement.speed = recon_status.speed;
}

void inner_reinit_attack(ECS::Entity attack)
{
	auto& attack_status = attack.get<Status>();
	auto& attack_movement = attack.get<FixedMovement>();
	std::string name = "data/boats/attack_boat.json";
	std::ifstream i(name);
	json j;
	i >> j;
	// recon modifiable stats are damage and speed
	attack_status.weapon_damage = j["stats"]["weapon_damage"];
	attack_status.speed = j["stats"]["speed"];
	attack_movement.speed = attack_status.speed;
}

void SkillSystem::reinit_stats()
{
	for (auto& boat : ECS::registry<Boat>.entities) {
		auto& status = boat.get<Status>();
		switch (status.rank) {
		case 10:
			inner_reinit_recon(boat);
			break;
		case 20:
			inner_reinit_attack(boat);
			break;
		}
	}
}

void inner_init_skills_helper(ECS::Entity boat, void(*s)(ECS::Entity), int skill)
{
	auto& status = boat.get<Status>();
	if (skill == 1) {
		if (status.s1_cd > 0) {
			status.s1_cd -= 1;
		}
		else {
			if (status.s1_selected) {
				// std::cout << "skill 1 activated" << std::endl;
				(*s)(boat);
			}
		}
	}
	else if (skill == 2) {
		if (status.s2_cd > 0) {
			status.s2_cd -= 1;
		}
		else {
			if (status.s2_selected) {
				// std::cout << "skill 2 activated" << std::endl;
				(*s)(boat);
			}
		}
	}
}

void inner_init_skill_1(ECS::Entity boat)
{
	auto& status = boat.get<Status>();
	switch (status.rank) {
	case 10:
		inner_init_skills_helper(boat, SkillSystem::recon_s1, 1);
		break;
	case 20:
		inner_init_skills_helper(boat, SkillSystem::attack_s1, 1);
		break;
	case 30:
		inner_init_skills_helper(boat, SkillSystem::regular_s1, 1);
		break;
	case 40:
		inner_init_skills_helper(boat, SkillSystem::big_s1, 1);
	}
}

void inner_init_skill_2(ECS::Entity boat)
{
	auto& status = boat.get<Status>();
	switch (status.rank) {
	case 10:
		inner_init_skills_helper(boat, SkillSystem::recon_s2, 2);
		break;
	case 20:
		inner_init_skills_helper(boat, SkillSystem::attack_s2, 2);
		break;
	case 30:
		inner_init_skills_helper(boat, SkillSystem::regular_s2, 2);
		break;
	case 40:
		inner_init_skills_helper(boat, SkillSystem::big_s2, 2);
	}
}

void SkillSystem::init_skills()
{
	// init 2nd skills first for self buff or team utility
	for (auto& boat : ECS::registry<Boat>.entities) {
		inner_init_skill_2(boat);
	}
	// init 1st skills, to avoid firing missiles without self buff
	// or to avoid do not attack enemies in larger view range
	for (auto& boat : ECS::registry<Boat>.entities) {
		inner_init_skill_1(boat);
	}
	// after the skills init, switch off the skill selection
	// to avoid the situation that player skill switched off before AI skill check
	for (auto& boat : ECS::registry<Boat>.entities) {
		boat.get<Status>().s1_selected = false;
		boat.get<Status>().s2_selected = false;
	}

}

void SkillSystem::recon_s1(ECS::Entity self)
{
	// Engine Boost; boost speed *2; 1cd;
	auto& self_status = ECS::registry<Status>.get(self);
	self_status.speed *= 2.f;
	auto& self_movement = ECS::registry<FixedMovement>.get(self);
	self_movement.speed = self_status.speed;
	// cd
	self_status.s1_cd = 1;
}

void SkillSystem::recon_s2(ECS::Entity self)
{
	// Flares; enlarge range; 1cd;
	auto& self_status = ECS::registry<Status>.get(self);
	self_status.view_range *= 1.5f;
	// cd
	self_status.s2_cd = 1;
}

void SkillSystem::attack_s1(ECS::Entity self)
{
	// Open Fire; fire missiles to all enemies in fleet's range; 2cd;
	// interact with: attack_s2(self dmg buff), recon_s2(view range buff)
	auto cmp = [](ECS::Entity a, ECS::Entity b) { return a.id < b.id; };
	std::set<ECS::Entity, decltype(cmp)> foe_boats_set(cmp);
	auto& self_status = ECS::registry<Status>.get(self);
	auto& self_motion = ECS::registry<Motion>.get(self);
	auto& player = ECS::registry<Player>.entities[0];
	auto& ai_player = ECS::registry<AIPlayer>.entities[0];
	auto& fleet = ECS::registry<Fleet>.get(player);
	auto& ai_fleet = ECS::registry<Fleet>.get(ai_player);
	auto& my_fleet = (self_status.player_number > 0) ? fleet : ai_fleet;
	auto& foe_fleet = (self_status.player_number > 0) ? ai_fleet : fleet; // if self fleet is player, then enemy is ai, vice versa.
	for (auto& my_boat : my_fleet.boats) {
		auto& my_status = my_boat.get<Status>();
		for (auto& foe_boat : foe_fleet.boats) {
			auto& my_motion = my_boat.get<Motion>();
			auto& foe_motion = foe_boat.get<Motion>();
			if (distance(my_motion.position, foe_motion.position) <= my_status.view_range) {
				foe_boats_set.insert(foe_boat);
				// std::cout << "The in-range boat is " << foe_boat.get<Status>().name << " and the dist is " << distance(my_motion.position, foe_motion.position) << " and the th is " << my_status.attack_range << std::endl;
			}
		}
	}
	// Creating a iterator pointing to start of set
	std::set<ECS::Entity>::iterator it = foe_boats_set.begin();
	// Iterate till the end of set
	while (it != foe_boats_set.end())
	{
		// do stuff
		auto& foe_boat = *it;
		ECS::Entity missile = Missile::createMissile(self_motion.position, self_status.weapon_damage, foe_boat);
		// std::cout << "Missile created" << std::endl;
		auto& missile_spline = ECS::registry<MotionSpline>.get(missile);
		auto& defender_position = ECS::registry<Motion>.get(foe_boat).position;
		missile_spline.setPoints({ self_motion.position, defender_position }, true);
		// increment the iterator
		it++;
	}
	// cd
	self_status.s1_cd = 2;
}

void SkillSystem::attack_s2(ECS::Entity self)
{
	// Fire Power Boost; boost damage *2; immobilize; 2cd;
	auto& self_status = ECS::registry<Status>.get(self);
	self_status.weapon_damage *= 2.f;
	self_status.speed = 0.01f;
	auto& self_movement = ECS::registry<FixedMovement>.get(self);
	self_movement.speed = self_status.speed;
	self_status.s2_cd = 2;
}

void SkillSystem::regular_s1(ECS::Entity self)
{
	// emergency repair; heal the boat with lowest hp and lowest spd; 2 cd;
	// interact with: attack_s2(set speed to 0.01f)
	auto& self_status = ECS::registry<Status>.get(self);
	auto& player = ECS::registry<Player>.entities[0];
	auto& ai_player = ECS::registry<AIPlayer>.entities[0];
	auto& fleet = ECS::registry<Fleet>.get(player);
	auto& ai_fleet = ECS::registry<Fleet>.get(ai_player);
	auto& my_fleet = (self_status.player_number > 0) ? fleet : ai_fleet;
	// find lowest hp boat and slowest boat in my fleet
	ECS::Entity lowest_hp_boat = self;
	float lowest_hp = self_status.health;
	ECS::Entity lowest_spd_boat = self;
	float lowest_spd = self_status.speed;
	for (auto& boat : my_fleet.boats) {
		auto boat_hp = boat.get<Status>().health;
		if (boat_hp < lowest_hp) {
			lowest_hp = boat_hp;
			lowest_hp_boat = boat;
		}
		auto boat_spd = boat.get<Status>().speed;
		if (boat_spd < lowest_spd) {
			lowest_spd = boat_spd;
			lowest_spd_boat = boat;
		}
	}
	// add 20 hp, around 20% of small boats and 10% of large boat
	auto& lowest_health = lowest_hp_boat.get<Status>().health;
	auto& lowest_max_health = lowest_hp_boat.get<Status>().maxHealth;
	lowest_health += 20;
	lowest_health = (lowest_health > lowest_max_health) ? lowest_max_health : lowest_health; // can't above max
	auto& lowest_spd_health = lowest_spd_boat.get<Status>().health;
	auto& lowest_spd_max_health = lowest_spd_boat.get<Status>().maxHealth;
	lowest_spd_health += 20;
	lowest_spd_health = (lowest_spd_health > lowest_spd_max_health) ? lowest_spd_max_health : lowest_spd_health; // can't above max
	// cd
	self_status.s1_cd = 2;
}

void inner_regular_s2_helper(Fleet& fleet, float per_hp) { // + per_hp to heal, - per_hp to damage
	for (auto& boat : fleet.boats) {
		auto& boat_health = boat.get<Status>().health;
		auto& boat_max_health = boat.get<Status>().maxHealth;
		boat_health += boat_max_health * per_hp;
		boat_health = (boat_health > boat_max_health) ? boat_max_health : boat_health;
	}
}

void SkillSystem::regular_s2(ECS::Entity self)
{
	// risky neutron pulse; if enemy does not use this skill, heal fleet 20%; else damage fleet 20%;
	// interact with: enemy_regular_s2
	auto& self_status = ECS::registry<Status>.get(self);
	auto& player = ECS::registry<Player>.entities[0];
	auto& ai_player = ECS::registry<AIPlayer>.entities[0];
	auto& fleet = ECS::registry<Fleet>.get(player);
	auto& ai_fleet = ECS::registry<Fleet>.get(ai_player);
	auto& my_fleet = (self_status.player_number > 0) ? fleet : ai_fleet;
	auto& foe_fleet = (self_status.player_number > 0) ? ai_fleet : fleet;
	// find enemy regular boat
	bool found_foe_reg = false;
	for (auto& boat : foe_fleet.boats) {
		if (boat.get<Status>().rank == 30) {
			found_foe_reg = boat.get<Status>().s2_selected; // if no foe reg or s2 not selected, bool false; if foe reg s2 selected, bool true;
			break;
		}
	}
	if (found_foe_reg) { // foe regular also used this skill
		// deal damage to fleet
		inner_regular_s2_helper(my_fleet, -0.2f);
	}
	else { // either no foe regular or it didn't use this skill
		// heal fleet
		inner_regular_s2_helper(my_fleet, 0.2f);
	}
	// cd
	self_status.s2_cd = 2;
}

void SkillSystem::big_s1(ECS::Entity self)
{
	// missile defense mk2
	auto& self_status = ECS::registry<Status>.get(self);
	auto& self_motion = ECS::registry<Motion>.get(self);
	// create a flock of boids; fire-and-forget destroy will be handled by tick_comat() in world.cpp;
	// max num is 50 with decent performance
	auto boids_margin = 50 - ECS::registry<Boids>.entities.size();
	int boids_num = (boids_margin >= 25) ? 25 : boids_margin;
	if (boids_num > 0) {
		for (int i = 0; i < boids_num; i++) {
			float rand_x = rand() % 100 - 50; // -50 to 49
			float rand_y = rand() % 100 - 50; // -50 to 49
			Boids::createBoid(self_motion.position + vec2{ rand_x * 0.01 , rand_y * 0.01 });
		}
	}
	// cd
	self_status.s1_cd = 3;
}

void SkillSystem::big_s2(ECS::Entity self)
{
	// missile targeting override;
	auto& self_status = ECS::registry<Status>.get(self);
	for (auto& missile_comp : ECS::registry<MissileComponent>.components) {
		if (missile_comp.target.get<Status>().player_number == self_status.player_number) { // if this missile's target is my fleet, re-guides to me
			missile_comp.target = self;
			// std::cout << "Missile retargeted" << std::endl;
		}
	}
	// cd
	self_status.s2_cd = 1;
}

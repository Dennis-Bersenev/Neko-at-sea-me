//
// Created by yichen on 2021-03-02.
// Reference: https://www.redblobgames.com/pathfinding/a-star/implementation.html
// Reference: https://www.youtube.com/watch?v=FQx4dAcb8ug&ab_channel=HelgeRhodin 
// && https://cdn-uploads.piazza.com/paste/k1mhfhwc32o6gc/0f130f0b2e8421a9f429fd4e5375b71ec6455ccfbe516cd6c8f41ebd49ac5c8b/btree_demo.cpp
//
#include "ai.hpp"
#include "boat.hpp"
#include "shark.hpp"
#include "motion_spline.hpp"
#include "ai_player.hpp"
#include "player.hpp"

#include <math.h>
#include <iostream>
#include <queue>

AISystem::AISystem(vec2 window_size_in_game_units, WorldSystem& world, Map* map_graph)
{
	// hook the game restart
	world.subj_game_restart.add_observer([&] {on_game_restart(); });
	world.subj_change_to_planning.add_observer([&] {master_mind(); });
	m_graph_ptr = map_graph;
	// 
	auto bottom_right = m_graph_ptr->get_pos(88);
	auto top_right = m_graph_ptr->get_pos(18);
	auto top_left = m_graph_ptr->get_pos(11);
	auto bottom_left = m_graph_ptr->get_pos(81);
	std::shared_ptr <BTNode> patrol_br = std::make_unique<FleetPatrol>(window_size_in_game_units, bottom_right, map_graph);
	std::shared_ptr <BTNode> patrol_tr = std::make_unique<FleetPatrol>(window_size_in_game_units, top_right, map_graph);
	std::shared_ptr <BTNode> patrol_tl = std::make_unique<FleetPatrol>(window_size_in_game_units, top_left, map_graph);
	std::shared_ptr <BTNode> patrol_bl = std::make_unique<FleetPatrol>(window_size_in_game_units, bottom_left, map_graph);
	std::shared_ptr <BTNode> hold_still = std::make_unique<FleetHold>();
	std::shared_ptr <BTNode> shark_rand_swim = std::make_unique<SharkRandSwim>(window_size_in_game_units, map_graph);
	std::shared_ptr <BTNode> shark_swim_repeater_1 = std::make_unique<BTRepeater>(shark_rand_swim, 1);
	std::shared_ptr <BTNode> shark_swim_repeater_3 = std::make_unique<BTRepeater>(shark_rand_swim, 3);
	std::shared_ptr <BTNode> shark_hunt = std::make_unique<SharkHunt>(49.f);
	root_sharks = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		shark_swim_repeater_3 }));

	// build ai player behavior tree
	// phase change behavior will be handled in the main step
	// fleet composition: big boat as center, recon as scout, attack as dps, regular boat as escort
	// fleet behavoir: check if there's an enemies insight if no
	//		big boat: decide a direction (90 deg front) + depend on the pos of map try to patrol clockwise
	//		recon boat: go forward (x units) 
	//		attack boat: hide behind the big boat
	//		regular boat: beside the attack boat
	// if yes
	//		big boat: move to the middle point of cloest enemy and attack boat
	//		recon boat: hide behind the big boat
	//		attack boat: get out of attacker's range
	//		regular boat: beside the attack boat, if the attacker is really close, intercept the attacker
	/*root_ai_player = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		 patrol_br, patrol_tr, patrol_tl, patrol_bl }));*/
	
	root_ai_patrol_cw_tl = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		 patrol_tl, patrol_tr, patrol_br, patrol_bl }));

	root_ai_patrol_cw_tr = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		 patrol_tr, patrol_br, patrol_bl, patrol_tl }));

	root_ai_patrol_cw_bl = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		 patrol_bl, patrol_tl, patrol_tr, patrol_br }));

	root_ai_patrol_cw_br = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		 patrol_br, patrol_bl, patrol_tl, patrol_tr }));

	root_ai_hold = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({
		  hold_still }));
}

BTState SharkRandSwim::process(ECS::Entity e)
{
	if (ECS::registry<Motion>.entities.size() >= 1 && ECS::registry<MotionSpline>.entities.size() >= 1)
	{
		auto& shark_motion = ECS::registry<Motion>.get(e);
		auto& shark_motionspline = ECS::registry<MotionSpline>.get(e);
		if (shark_motionspline.isMoving())
		{
			return BTState::Running;
		}
		else
		{
			// dereferencing pointer
			auto& m_graph = *m_graph_ptr;
			// m_graph.print_map();
			bool isInvalid = true;
			int rand_pos_vert = -1;
			while (isInvalid || rand_pos_vert < 0 || rand_pos_vert > 99) {
				rand_pos_vert = std::floor(uniform_dist(rng) * 100);
				auto& v = m_graph.removed_vert;
				isInvalid = std::find(v.begin(), v.end(), rand_pos_vert) != v.end() ? true : false;
			}
			shark_motionspline.setPoints({ shark_motion.position, shark_motion.position }, true); // kick start
			std::vector<vec2> shortest_path = AISystem::get_waypoints(m_graph, shark_motion.position, m_graph.get_pos(rand_pos_vert), AISystem::astar_goal2start);
			for (vec2 pos : shortest_path)
			{
				// add some randomness within the vert
				vec2 rand_pos_in_vert = { pos.x + (uniform_dist(rng) - 0.5f) * (m_window.x * 0.075f), pos.y + (uniform_dist(rng) - 0.5f) * (m_window.y * 0.075f) };
				shark_motionspline.addPoint(rand_pos_in_vert);
			}
			// std::cout << "The next success goal is " << shark_motionspline.points.back().x << ' ' << shark_motionspline.points.back().y << std::endl;
			return BTState::Success;
		}
	}
	return BTState::Failure;
}

BTState SharkHunt::process(ECS::Entity e)
{
	if (ECS::registry<Motion>.entities.size() >= 2 && ECS::registry<MotionSpline>.entities.size() >= 2 && !prey_ptr) // initialize prey
	{
		float min_dist = INFINITY;
		auto shark_pos = ECS::registry<Motion>.get(e).position;
		for (auto& boat : ECS::registry<Boat>.entities)
		{
			auto boat_pos = ECS::registry<Motion>.get(boat).position;
			auto dist = distance(boat_pos, shark_pos);
			if (dist < min_dist)
			{
				min_dist = dist;
				prey_ptr = &boat;
			}
		}
	}
	if (ECS::registry<Motion>.entities.size() >= 2 && ECS::registry<MotionSpline>.entities.size() >= 2 && prey_ptr) // hunt to death
	{
		auto& shark_motion = ECS::registry<Motion>.get(e);
		auto& shark_motionspline = ECS::registry<MotionSpline>.get(e);
		auto& prey_motion = ECS::registry<Motion>.get(*prey_ptr);
		if (distance(shark_motion.position, prey_motion.position) < 50.f)
		{
			return BTState::Success;
		}
		else if (shark_motionspline.isMoving())
		{
			return BTState::Running;
		}
		else
		{
			shark_motionspline.setPoints({ shark_motion.position, prey_motion.position }, true);
		}
	}
	return BTState::Failure;
}

BTState FleetPatrol::process(ECS::Entity e)
{
	if (ECS::registry<Motion>.entities.size() >= 1 && ECS::registry<MotionSpline>.entities.size() >= 1)
	{
		auto& ai_fleet = ECS::registry<Fleet>.get(e);
		int cur_fleet_size = ai_fleet.boats.size();
		if (cur_fleet_size <= 0)
			return BTState::Failure; // if there's no boats in the fleet, AI failed

		if (pre_fleet_size != cur_fleet_size) {
			pre_fleet_size = cur_fleet_size; // update fleet size
			auto& status = ECS::registry<Status>;
			int highest_rank = -1;
			for (auto& boat : ai_fleet.boats) {
				auto rank = status.get(boat).rank;
				// std::cout << "Cur boat rank is " << rank << std::endl;
				if (rank > highest_rank) {
					highest_rank = rank;
					leader_ptr = &boat;
				}
			}
		}

		assert(leader_ptr);

		auto& leader_motion = ECS::registry<Motion>.get(*leader_ptr);
		auto& leader_motionspline = ECS::registry<MotionSpline>.get(*leader_ptr);
		int leader_id = leader_ptr->id;
		auto& m_graph = *m_graph_ptr;
		if (m_graph.get_vert(leader_motion.position) == waypoint_vert)
		{
			return BTState::Success;
		}
		else if (leader_motionspline.isMoving()) // since leader is always the slowest boat, speed: big < regular = attack < recon
		{
			// follower follow the leader
			float diff_x = waypoint.x - leader_motion.position.x;
			float diff_y = waypoint.y - leader_motion.position.y;
			float shift_unit = 70.f;
			vec2 regular_shift;
			vec2 attack_shift;
			vec2 recon_shift;
			if (abs(diff_x) > abs(diff_y))
			{
				// boat moving along x dir, delata should be in y
				shift_unit *= (diff_x < 0) ? -1 : 1;
				regular_shift = { 0.f, -shift_unit }; // only escort big boat
				attack_shift = { -shift_unit, 0.f }; // escort big or regular
				recon_shift = { shift_unit, 0.f }; // escort big or regualr or attack
			}
			else
			{
				// y dir
				shift_unit *= (diff_y < 0) ? -1 : 1;
				regular_shift = { shift_unit, 0.f }; // only escort big boat
				attack_shift = { 0.f, -shift_unit }; // escort big or regular
				recon_shift = { 0.f, shift_unit }; // escort big or regualr or attack
			}
			for (auto& boat : ai_fleet.boats) {
				if (boat.id != leader_id) {
					// follower behavior
					auto& boat_motion = ECS::registry<Motion>.get(boat);
					auto& boat_motionspline = ECS::registry<MotionSpline>.get(boat);
					auto boat_rank = ECS::registry<Status>.get(boat).rank;
					auto boat_vert = m_graph.get_vert(boat_motion.position);
					/*int sign = ((boat_vert + boat_vert/10) % 2 == 0) ? -1 : 1;
					vec2 periodic_pos = leader_motion.position + vec2{ sign, sign } *delta;*/
					switch (boat_rank) {
					case (10):
						boat_motionspline.setPoints({ boat_motion.position, leader_motion.position + recon_shift * vec2{1.5, 1.5} }, true);
						break;
					case (20):
						boat_motionspline.setPoints({ boat_motion.position, leader_motion.position + attack_shift }, true);
						break;
					case (30):
						boat_motionspline.setPoints({ boat_motion.position, leader_motion.position + regular_shift }, true);
						break;
					}
				}
			}
			return BTState::Running;
		}
		else
		{
			leader_motionspline.setPoints({ leader_motion.position, leader_motion.position }, true); // kick start
			// std::cout << "\nLeader ";
			std::vector<vec2> shortest_path = AISystem::get_waypoints(m_graph, leader_motion.position, waypoint, AISystem::bfs_goal2start);
			for (vec2 pos : shortest_path)
			{

				leader_motionspline.addPoint(pos);
			}
			return BTState::Running;
		}
	}
	return BTState::Failure;
}

BTState FleetHold::process(ECS::Entity e)
{
	if (ECS::registry<Motion>.entities.size() > 0 && ECS::registry<MotionSpline>.entities.size() > 0) {
		for (auto ai_boat : ECS::registry<AIComponent>.entities) {
			ai_boat.get<MotionSpline>().reset(ai_boat.get<Motion>().position);
		}
		return BTState::Success;
	}
	return BTState::Failure;
}

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	next_ai -= elapsed_ms;
	if (next_ai < 0.f)
	{
		// reset next_ai timer;
		next_ai = ai_waiting_time;
		// update sharks' behavior
		sharks_step(elapsed_ms);
		// update ai player' behavior 
		ai_player_step(elapsed_ms);
	}
	
}

void AISystem::sharks_step(float elapsed_ms)
{
	/*
	1. if not started yet start init the behavior tree
	2. if already started keep processesing
	3. In the process, the main is And Sequence, it will process the current child. If get Running just return. If get Success it will increase child index and process.
	4. Generally the And Sequence return running.
	5. If it returns Success, the sequence is finished. We may start again.
	6. TODO: Failure needs to be handle properly. Right now it will re-init the root.
	*/
	
	if (ECS::registry<Shark>.entities.size() >= 1) // TODO: what if there's a shark dead?
	{
		auto& sharks = ECS::registry<Shark>.entities;

		if (shark_num != sharks.size()) { // if sharks size changes, rebuild bool vec
			shark_num = sharks.size();
			shark_started.clear();
			for (unsigned int i = 0; i < shark_num; i++) {
				shark_started.push_back(false);
			}
		}
		// sharks.size() == shark_num == shark_started.size() should
		for (unsigned int i = 0; i < shark_started.size(); i++) {
			auto shark_i = sharks[i];
			if (!shark_started[i]) {
				root_sharks->init(shark_i);
				shark_started[i] = true;
			}
			if (root_sharks->process(shark_i) == BTState::Success) // process and check the ret status
				shark_started[i] = false; // simple loop
		}
	}
}

void AISystem::ai_player_step(float elapsed_ms)
{
	if (ECS::registry<AIPlayer>.entities.size() > 0)
	{
		auto& ai_player = ECS::registry<AIPlayer>.entities[0];
		//// auto& ai_fleet = ECS::registry<Fleet>.get(ai_player_e);
		//if (!ai_player_started) 
		//{
		//	ai_player_started = true;
		//	root_ai_player->init(ai_player);
		//}
		//auto ret_ai_player = root_ai_player->process(ai_player);
		//if (ret_ai_player == BTState::Success) // simple root loop
		//{
		//	ai_player_started = false;
		//}
		if (cur_command_ptr != NULL) {
			auto ret_ai_player = (*cur_command_ptr)->process(ai_player);
			if (ret_ai_player == BTState::Success) // simple root loop
			{
				ai_player_started = false;
			}
		}
	}
}

void AISystem::on_game_restart()
{
	auto& sharks = ECS::registry<Shark>.entities;
	shark_num = sharks.size();
	shark_started.clear();
	for (unsigned int i = 0; i < shark_num; i++) {
		root_sharks->init(sharks[i]);
		shark_started.push_back(true);
	}

	auto& ai_player = ECS::registry<AIPlayer>.entities[0];
	round = 0;
	ai_player_started = false;
	master_mind();
}

int AISystem::find_nearest_patrol_point(vec2 current_pos)
{
	int curr_vert = m_graph_ptr->get_vert(current_pos);
	int nearest_dist = 1000;
	int nearest_vert = 0;
	if (abs(curr_vert - 0) < nearest_dist) {
		nearest_dist = abs(curr_vert - 0);
		nearest_vert = 0;
	}
	if (abs(curr_vert - 9) < nearest_dist) {
		nearest_dist = abs(curr_vert - 9);
		nearest_vert = 9;
	}
	if (abs(curr_vert - 90) < nearest_dist) {
		nearest_dist = abs(curr_vert - 90);
		nearest_vert = 90;
	}
	if (abs(curr_vert - 99) < nearest_dist) {
		nearest_dist = abs(curr_vert - 99);
		nearest_vert = 99;
	}
	return nearest_vert;
}

void AISystem::master_mind()
{
	round++;
	// std::cout << "round: " << round << std::endl;
	if (ECS::registry<AIPlayer>.entities.size() > 0 && ECS::registry<Player>.entities.size() > 0) {
		auto& ai_player = ECS::registry<AIPlayer>.entities[0];
		auto& player = ECS::registry<Player>.entities[0];
		boats_in_range.clear();
		// 1. find all player boats in view range
		for (auto boat : ECS::registry<PlayerComponent>.entities) {
			for (auto ai_boat : ECS::registry<AIComponent>.entities) {
				if (distance(boat.get<Motion>().position, ai_boat.get<Motion>().position) <= ai_boat.get<Status>().view_range)
					boats_in_range.push_back(boat);
			}
		}
		// 2. know my fleet composition and compute pre-3
		auto boats_in_range_size = boats_in_range.size();
		auto fleet_size = ECS::registry<PlayerComponent>.entities.size();
		auto lowest_health_per = 1.f;
		auto total_health = 0.f;
		auto total_max_health = 0.f;
		std::vector<ECS::Entity> recon_vec;
		std::vector<ECS::Entity> attack_vec;
		std::vector<ECS::Entity> reg_vec;
		std::vector<ECS::Entity> big_vec;
		for (auto boat : ECS::registry<AIComponent>.entities) {
			float health = boat.get<Status>().health;
			float max_health = boat.get<Status>().maxHealth;
			float health_per = health / max_health;
			total_health += health;
			total_max_health += max_health;
			if (health_per < lowest_health_per) {
				lowest_health_per = health_per;
			}
			auto rank = boat.get<Status>().rank;
			switch (rank) {
			case 10:
				recon_vec.push_back(boat);
				break;
			case 20:
				attack_vec.push_back(boat);
				break;
			case 30:
				reg_vec.push_back(boat);
				break;
			case 40:
				big_vec.push_back(boat);
			}
		}
		float fleet_health_per = total_health / total_max_health;
		// 3. call skill selections with info prepared in 2
		for (auto boat : ECS::registry<AIComponent>.entities) {
			auto rank = boat.get<Status>().rank;
			switch (rank) {
			case 10:
				recon_skill_selection(boat, boats_in_range_size);
				break;
			case 20:
				attack_skill_selection(boat, boats_in_range_size, fleet_size, recon_vec);
				break;
			case 30:
				reg_skill_selection(boat, lowest_health_per, fleet_health_per, attack_vec);
				break;
			case 40:
				big_skill_selection(boat);
			}
		}
		// 4. based on the condition to init preset nodes
		bool attack_hold = false;
		for (auto attack : attack_vec) {
			if (attack.get<Status>().s2_selected)
				attack_hold = true;
		}
		if (!ai_player_started) {
			ai_player_started = true;
			vec2 cur_pos = { 0.f, 0.f };
			if (big_vec.size() > 0) {
				cur_pos = big_vec[0].get<Motion>().position;
			}
			else if (reg_vec.size() > 0) {
				cur_pos = reg_vec[0].get<Motion>().position;
			}
			else if (attack_vec.size() > 0) {
				cur_pos = attack_vec[0].get<Motion>().position;
			}
			else if (recon_vec.size() > 0) {
				cur_pos = recon_vec[0].get<Motion>().position;
			}
			int nearest_vert = find_nearest_patrol_point(cur_pos);
			switch (nearest_vert) {
			case 0:
				root_ai_patrol_cw_tl->init(ai_player);
				cur_command_ptr = &root_ai_patrol_cw_tl;
				break;
			case 9:
				root_ai_patrol_cw_tr->init(ai_player);
				cur_command_ptr = &root_ai_patrol_cw_tr;
				break;
			case 90:
				root_ai_patrol_cw_bl->init(ai_player);
				cur_command_ptr = &root_ai_patrol_cw_bl;
			case 99:
				root_ai_patrol_cw_br->init(ai_player);
				cur_command_ptr = &root_ai_patrol_cw_br;
			}
		}
		if (attack_hold) {
			root_ai_hold->init(ai_player);
			ai_player_started = false;
			cur_command_ptr = &root_ai_hold;
		}
	}
}

void AISystem::recon_skill_selection(ECS::Entity recon, int boats_in_range_size)
{
	auto& status = recon.get<Status>();
	// flares - if spots at least one boat, enlarge view range
	if (status.s2_cd == 0 && boats_in_range_size > 0)
	{
		status.s2_selected = true;
	}
	// engine boost - will flee if health is low
	if (status.s1_cd == 0 && status.health <= status.maxHealth * 0.5f)
	{
		status.s1_selected = true;
	}
}

void AISystem::attack_skill_selection(ECS::Entity attack, int boats_in_range_size, int fleet_size, std::vector<ECS::Entity> recon_vec)
{
	auto& status = attack.get<Status>();
	bool flares_selected = false;
	if (recon_vec.size() > 0) {
		for (auto recon : recon_vec) {
			if (recon.get<Status>().s2_selected)
				flares_selected = true;
		}
	}
	// fire power boost - if 1. recon flare selected or 2. boats_in_range_size > 1 or 3. boats_in_range_size > 0.5 * fleet_size
	if (status.s2_cd == 0 && (flares_selected || boats_in_range_size > 1 || boats_in_range_size >= 0.5f * fleet_size))
	{
		status.s2_selected = true;
	}
	// open fire - will fire if 1. fire power boost selected or 2. boats_in_range_size > 0
	if (status.s1_cd == 0 && (status.s2_selected || boats_in_range_size > 0))
	{
		status.s1_selected = true;
	}
}

void AISystem::reg_skill_selection(ECS::Entity reg, float lowest_health_per, float fleet_health_per, std::vector<ECS::Entity> attack_vec)
{
	auto& status = reg.get<Status>();
	bool station_selected = false;
	float attack_hp_per = 1.f;
	if (attack_vec.size() > 0) {
		for (auto attack : attack_vec) {
			if (attack.get<Status>().s2_selected) {
				station_selected = true;
				attack_hp_per = attack.get<Status>().health / attack.get<Status>().maxHealth;
			}
		}
	}
	// risky heal - 1. lowest_health per < 0.2 or 2. overall health < 0.6
	if (status.s2_cd == 0 && (lowest_health_per <= 0.2f || fleet_health_per <= 0.6f)) {
		status.s2_selected = true;
	}
	// emergency heal - 1. risky heal selected or 2. lowest heath < 0.6 or 3. attack power-up selected && its health < 0.8
	if (status.s1_cd == 0 && (status.s2_selected || lowest_health_per <= 0.6 || (station_selected && attack_hp_per <= 0.8))) {
		status.s1_selected = true;
	}
}

void AISystem::big_skill_selection(ECS::Entity big)
{
	auto& status = big.get<Status>();
	auto missile_size = ECS::registry<MissileComponent>.entities.size();
	int enemy_missile_size = 0;
	if (missile_size > 2) {
		for (auto missile_comp : ECS::registry<MissileComponent>.components) {
			if (missile_comp.target.get<Status>().player_number == -1)
				enemy_missile_size++;
		}
	}
	// missile reguidance - if enemy_missile_size > 2
	if (status.s2_cd == 0 && enemy_missile_size > 2) {
		status.s2_selected = true;
	}
	// boids - if enemy_missile_size > 3
	if (status.s1_cd == 0 && enemy_missile_size > 3) {
		status.s1_selected = true;
	}
}

// static
std::vector<int> AISystem::build_path(std::unordered_map<int, int> parent, int start, int goal)
{
	std::vector<int> ret;
	int cur = goal;
	// std::cout << "\nbacktrace - start" << std::endl;
	int guard = 10000; // 10k
	while (cur != start && guard-- > 0)
	{
		ret.push_back(cur);
		// std::cout << cur << ' ';
		int bef_size = parent.size();
		int child = parent[cur];
		int aft_size = parent.size();
		if (aft_size == bef_size)
		{
			cur = child; // found the parent
		}
		else
		{
			ret.clear(); // reinit the vector
			ret.push_back(start); // idle
			std::cout << "Debug: Can't find path from " << start << " to " << goal << std::endl;
			break;
		}
	}
	//std::cout << '\n' << "bactrace - end\n" << std::endl;

	if (ret.size() == 0) // skip the wholse while loop since goal == start
	{
		ret.push_back(goal);
		std::cout << "Debug: Start == Goal, this can happen but shouldn't spam." << std::endl;
	}

	return ret; // always returns a verctor with at least one element
}

std::vector<int> AISystem::bfs_goal2start(Map map_graph, vec2 start_pos, vec2 goal_pos)
{
	int start = map_graph.get_vert(start_pos); // convert the arguments from position to vertex
	int goal = map_graph.get_vert(goal_pos);

	if (start < 0 || start > 99) // out of the map
		start = 0; // reset the pos to top left corner
	
	std::queue<int> frontier; // initialize the queue
	frontier.push(start);
	
	std::unordered_map<int, int> parent; // key: child, value: parent
	parent[start] = start; // initialize the path system to trace back to

	// std::cout << "\nfrontier - start" << std::endl;
	int guard = 100000; // .1 million
	while (!frontier.empty() && guard-- > 0)
	{
		int current = frontier.front();
		frontier.pop();  // gets the current and removes it
		
		if (current == goal) // exits early
			break;

		for (int next : map_graph.get_adj(current))
		{
			if (parent.find(next) == parent.end()) // only accepts unvisited nodes
			{
				frontier.push(next);
				parent[next] = current;
			}
		}
	}
	// std::cout << "frontier - end\n" << std::endl;

	return build_path(parent, start, goal);
}

std::vector<int> AISystem::astar_goal2start(Map map_graph, vec2 start_pos, vec2 goal_pos)
{
	int start = map_graph.get_vert(start_pos); // convert the arguments from position to vertex
	int goal = map_graph.get_vert(goal_pos);

	if (start < 0 || start > 99) // out of the map
		start = 0; // reset the pos to top left corner


	std::unordered_map<int, int> parent; // key: child, value: parent
	std::unordered_map<int, float> cost_so_far; // key: vert, value: cost
	PriorityQueue<int, double> frontier;
	frontier.put(start, map_graph.heuristic(start, goal));

	parent[start] = start;
	cost_so_far[start] = 0;

	int guard = 100000; // .1 million
	while (!frontier.empty() && guard-- > 0) {
		int current = frontier.get(); // customized fun get

		if (current == goal) // exits early
			break;

		for (int next : map_graph.get_adj(current)) {
			float new_cost = cost_so_far[current] + map_graph.cost(current, next);
			if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) { // next !in cost_so_far || next in new_cost but the cost is lower
				cost_so_far[next] = new_cost;
				float priority = new_cost + map_graph.heuristic(next, goal);
				frontier.put(next, priority);
				parent[next] = current;
			}
		}
	}

	return build_path(parent, start, goal);
}

std::vector<vec2> AISystem::get_waypoints(Map map_graph, vec2 start, vec2 goal, std::function<std::vector<int>(Map, vec2, vec2)> func) // can also pass functor
{
	std::vector<int> reverse_path = func(map_graph, start, goal);
	std::vector<vec2> ret;
	for (std::vector<int>::reverse_iterator rit = reverse_path.rbegin(); rit != reverse_path.rend(); ++rit) // reverse traversal or we can std::reverse(vec.begin(), vec.end())
	{
		// std::cout << *rit << ' ';
		vec2 pos = map_graph.get_pos(*rit);
		ret.push_back(pos);
	}
	ret.pop_back(); // remove the center pos converted from vert
	ret.push_back(goal); // push back the real goal
	// std::cout << '\n' << std::endl;
	return ret;
}

void AISystem::debug_bfs_path(Map map_graph, vec2 start, vec2 goal)
{
	std::vector<int> reverse_path = bfs_goal2start(map_graph, start, goal);
	for (std::vector<int>::reverse_iterator rit = reverse_path.rbegin(); rit != reverse_path.rend(); ++rit) // reverse traversal
	{
		std::cout << *rit << ' ';
	}
	std::cout << '\n' << std::endl;
}

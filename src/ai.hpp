//
// Created by yichen on 2021-03-02.
//

#pragma once

#include <cassert>
#include <memory>
#include <functional>
#include <random>

#include "tiny_ecs.hpp"
#include "common.hpp"
#include "world.hpp"
#include "map.hpp"

// BTState; BTNode; BTAndSequence; BTIfCondition; retrieved from btree_demo.cpp
// https://cdn-uploads.piazza.com/paste/k1mhfhwc32o6gc/0f130f0b2e8421a9f429fd4e5375b71ec6455ccfbe516cd6c8f41ebd49ac5c8b/btree_demo.cpp

// The return type of behaviour tree processing
enum class BTState {
    Running,
    Success,
    Failure
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
	virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

	virtual void init(ECS::Entity e) {};

	virtual BTState process(ECS::Entity e) = 0;
};

// A composite node that loops through all children and exits when one fails
class BTAndSequence : public BTNode {
public:
	BTAndSequence(std::vector<std::shared_ptr<BTNode>> children)
		: m_children(std::move(children)), m_index(0) {
	}

private:
	void init(ECS::Entity e) override
	{
		m_index = 0;
		assert(m_index < m_children.size());
		// initialize the first child
		const auto& child = m_children[m_index];
		assert(child);
		child->init(e);
	}

	BTState process(ECS::Entity e) override {
		if (m_index >= m_children.size())
			return BTState::Success;

		// process current child
		const auto& child = m_children[m_index];
		assert(child);
		BTState state = child->process(e);

		// select a new active child and initialize its internal state
		if (state == BTState::Success) {
			++m_index;
			if (m_index >= m_children.size()) {
				return BTState::Success;
			}
			else {
				const auto& nextChild = m_children[m_index];
				assert(nextChild);
				nextChild->init(e);
				return BTState::Running;
			}
		}
		else {
			return state;
		}
	}

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

// A general decorator with lambda condition
class BTIfCondition : public BTNode
{
public:
	BTIfCondition(std::shared_ptr<BTNode> child, std::function<bool(ECS::Entity)> condition)
		: m_child(std::move(child)), m_condition(condition) {
	}

	virtual void init(ECS::Entity e) override {
		m_child->init(e);
	}

	virtual BTState process(ECS::Entity e) override {
		if (m_condition(e))
			return m_child->process(e);
		else
			return BTState::Success;
	}
private:
	std::shared_ptr<BTNode> m_child;
	std::function<bool(ECS::Entity)> m_condition;
};

// Repeater
class BTRepeater : public BTNode
{
public:
	BTRepeater(std::shared_ptr<BTNode> child, int repeat_counter)
		: m_child(std::move(child)), m_repeater(repeat_counter), m_counter(repeat_counter) {
	}

	virtual void init(ECS::Entity e) override {
		m_child->init(e);
		m_counter = m_repeater;
	}

	virtual BTState process(ECS::Entity e) override {
		
		if (m_counter > 0)
		{
			auto state = m_child->process(e);
			if (state == BTState::Success)
				--m_counter;
			return BTState::Running;
		}
		else
		{
			return BTState::Success;
		}
	}

private:
	std::shared_ptr<BTNode> m_child;
	int m_repeater; // fixed
	int m_counter;  // changing
};

// Leaf nodes
// shark randomly swims in the world
class SharkRandSwim : public BTNode
{
public:
	SharkRandSwim(vec2 window, Map* map_graph) noexcept
		: m_window(window), m_graph_ptr(map_graph) {
		rng = std::default_random_engine(std::random_device()());
	};

private:
	void init(ECS::Entity e) override {
		rng = std::default_random_engine(std::random_device()());
	};
	BTState process(ECS::Entity e) override;

private:
	vec2 m_window;
	Map* m_graph_ptr;
	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};

// shark will find the closed boat and chase it till death (distance < given radius)
class SharkHunt : public BTNode
{
public:
	SharkHunt(float rad) noexcept {
		prey_ptr = NULL;
	};

private:
	void init(ECS::Entity e) override {
		prey_ptr = NULL;
	};
	BTState process(ECS::Entity e) override;

private:
	ECS::Entity* prey_ptr;
};

// fleet will patrol to the given waypoint
class FleetPatrol : public BTNode
{
public:
	FleetPatrol(vec2 window, vec2 pos, Map* map_graph) noexcept
		: m_window(window), waypoint(pos), m_graph_ptr(map_graph) {
		waypoint_vert = (*m_graph_ptr).get_vert(waypoint);
		pre_fleet_size = -1;
		leader_ptr = NULL;
	};

private:
	void init(ECS::Entity e) override {
		pre_fleet_size = -1;
		leader_ptr = NULL;
	};
	BTState process(ECS::Entity e) override;

private:
	vec2 m_window;
	vec2 waypoint;
	int waypoint_vert;
	Map* m_graph_ptr;
	int pre_fleet_size;
	ECS::Entity* leader_ptr;
};

// hold position
class FleetHold : public BTNode
{
public:
	FleetHold() {};

private:
	void init(ECS::Entity e) override {};
	BTState process(ECS::Entity e) override;
};


// std::priority_queue wrapper class for a_start / dijkstra
template<typename T, typename priority_t>
struct PriorityQueue {
	// https://www.cplusplus.com/reference/queue/priority_queue/priority_queue/
	typedef std::pair<priority_t, T> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

	inline bool empty() const {
		return elements.empty();
	}

	inline void put(T item, priority_t priority) {
		elements.emplace(priority, item);
	}

	T get() {
		T best_item = elements.top().second;
		elements.pop();
		return best_item;
	}
};

class AISystem
{
public:
	AISystem(vec2 window_size_in_game_units, WorldSystem& world, Map* map_graph);
	void step(float elapsed_ms, vec2 window_size_in_game_units);

	static std::vector<int> bfs_goal2start(Map map_graph, vec2 start_pos, vec2 goal_pos);
	static std::vector<int> astar_goal2start(Map map_graph, vec2 start_pos, vec2 goal_pos);
	static std::vector<vec2> get_waypoints(Map map_graph, vec2 start, vec2 goal, std::function<std::vector<int> (Map, vec2, vec2)> func);
	static void debug_bfs_path(Map map_graph, vec2 start, vec2 goal);
	static std::vector<int> build_path(std::unordered_map<int, int> parent, int start, int goal);

private:
	// internal methods
	void sharks_step(float elapsed_ms);
	void ai_player_step(float elapsed_ms);
	void on_game_restart();
	void master_mind();
	void recon_skill_selection(ECS::Entity recon, int boats_in_range_size);
	void attack_skill_selection(ECS::Entity attack, int boats_in_range_size, int fleet_size, std::vector<ECS::Entity> recon_vec);
	void reg_skill_selection(ECS::Entity reg, float lowest_health_per, float fleet_health_per, std::vector<ECS::Entity> attack_vec);
	void big_skill_selection(ECS::Entity big);
	int find_nearest_patrol_point(vec2 current_pos);
	
private:
	// Game state
	float ai_waiting_time = 0.f; // temp real time AI
	float next_ai = 0.f;
	// bool sharks_started = false;
	int shark_num = -1;
	std::shared_ptr <BTNode> root_sharks;
	std::vector<bool> shark_started;
	int prey_id = -1;
	bool ai_player_started = false;
	std::shared_ptr <BTNode> root_ai_patrol_cw_tl;
	std::shared_ptr <BTNode> root_ai_patrol_cw_tr;
	std::shared_ptr <BTNode> root_ai_patrol_cw_bl;
	std::shared_ptr <BTNode> root_ai_patrol_cw_br;
	std::shared_ptr <BTNode> root_ai_hold;
	vec2 new_waypoint = { 0.f, 0.f };
	vec2* waypoint_ptr = &new_waypoint;
	std::shared_ptr <BTNode>* cur_command_ptr = NULL;
	std::vector<ECS::Entity> boats_in_range;
	std::default_random_engine rng = std::default_random_engine(std::random_device()());;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	int round = 0;
	Map* m_graph_ptr;
};
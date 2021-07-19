//
// Created by austi on 2021-02-02.
//

using namespace std;
// Header
#include "boat.hpp"
#include "convexhull.hpp"
#include "render_components.hpp"
#include "render.hpp"
#include "selectable.hpp"
#include "motion_spline.hpp"
#include "health.hpp"
#include "ai_player.hpp"
#include <iostream>
#include "nlohmann/json.hpp"
using namespace nlohmann;

string STATS = "stats";
string NAME = "name";
string COST = "cost";
string RANK = "rank";
string PLAYER_NUM = "player_number";
string WEAPON_DMG = "weapon_damage";
string ATTACK_RANGE = "attack_range";
string VIEW_RANGE = "view_range";
string HEALTH = "health";
string MAX_HEALTH = "maxHealth";
string SPEED = "speed";
string MAXSPEED = "maxSpeed";

ECS::Entity Boat::createBoat(vec2 position, int type, int player_number, bool isAI)
{
	auto entity = ECS::Entity();

	std::string key = "boat" + type;
	ShadedMesh& resource = cache_resource(key);

    string boat_name;

    // Initialize the status component of each boat depending on type
    switch (type) {
        case 0:
            boat_name = "regular_boat";
            break;
        case 1:
            // Big boat (could we not have had a better name?)
            boat_name = "big_boat";
            break;
        case 2:
            // Recon boat
            boat_name = "recon_boat";
            break;
        case 3:
            // Attack boat
            boat_name = "attack_boat";
            break;
    }

	if (resource.mesh.vertices.size() == 0)
	{
	    resource.mesh.loadFromOBJFile(mesh_path(boat_name + ".obj"));
		RenderSystem::createColoredMesh(resource, "boat");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);

	auto& boat_status = ECS::registry<Status>.emplace(entity);

    string name = "data/boats/" + boat_name + ".json";
    ifstream i(name);
    json j;
    i >> j;

    std::cout << j[STATS][WEAPON_DMG] << std::endl;

    boat_status.name = j[STATS][NAME];
    boat_status.cost = j[STATS][COST];
    boat_status.rank = j[STATS][RANK];
    boat_status.player_number = player_number;
    boat_status.weapon_damage = j[STATS][WEAPON_DMG];
    boat_status.attack_range = j[STATS][ATTACK_RANGE];
    boat_status.view_range = j[STATS][VIEW_RANGE];
    boat_status.health = j[STATS][HEALTH];
    boat_status.maxHealth = j[STATS][MAX_HEALTH];
    boat_status.speed = j[STATS][SPEED];
    boat_status.maxSpeed = j[STATS][MAXSPEED];

    // Initialize the position, scale, and physics components
	auto& motion = ECS::registry<Motion>.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ 50.f, 50.f });

    if (isAI) {
        ECS::registry<AIComponent>.emplace(entity);
    }
    else {
        ECS::registry<PlayerComponent>.emplace(entity);
    }

	auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
	std::vector<ColoredVertex> boat_vertices = texmesh.mesh.vertices;

	auto& convex_hull = ECS::registry<ConvexComponent>.emplace(entity);
	convex_hull.convex_hull = ConvexHull::getConvexhull(boat_vertices, boat_vertices.size(),motion);


	auto& movement = ECS::registry<FixedMovement>.emplace(entity);
	movement.speed = boat_status.speed;

	// Make boat selectable
	ECS::registry<Selectable>.emplace(entity);

	// Give boat collision
	ECS::registry<Collider>.emplace(entity);

	// Add a motion spline
	ECS::registry<MotionSpline>.emplace(entity, position);

	// Add a bounding box
	auto&& bb = ECS::registry<BoundingBox>.emplace(entity);
	bb.x_dim = abs(motion.scale.x) / 2.0f;
	bb.y_dim = abs(motion.scale.y) / 2.0f;

	// Create a Boat component
	ECS::registry<Boat>.emplace(entity);

	auto health_entity = ECS::Entity();
	std::string health_key = "health";
    if (isAI) {
        health_key = "enemy_health";
    }
	ShadedMesh& health_resource = cache_resource(health_key);
    if (isAI) {
        health_resource.mesh.loadFromOBJFile(mesh_path("enemy_health.obj"));

    } else {
        health_resource.mesh.loadFromOBJFile(mesh_path("health.obj"));
    }
    RenderSystem::createColoredMesh(health_resource, "health");


    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(health_entity, health_resource);

	ECS::registry<Health>.emplace(health_entity);

	// Add a motion spline
	ECS::registry<MovementPath>.emplace(entity);

	entity.get<Boat>().health = health_entity;

	return entity;
}
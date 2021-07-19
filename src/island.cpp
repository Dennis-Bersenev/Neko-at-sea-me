//
// Created by austi on 2021-02-02.
//

// Header
#include "island.hpp"
#include "render.hpp"
#include "convexhull.hpp"
#include "selectable.hpp"
#include "motion_spline.hpp"

ECS::Entity Island::createIsland(vec2 position, int type)
{
	auto entity = ECS::Entity();

	std::string key = "island" + type;
	ShadedMesh& resource = cache_resource(key);
	if (resource.mesh.vertices.size() == 0)
	{
		switch (type) {
		case 0:
			resource.mesh.loadFromOBJFile(mesh_path("island1.obj"));
			break;
		case 1:
			resource.mesh.loadFromOBJFile(mesh_path("island2.obj"));
			break;
		case 2:
			resource.mesh.loadFromOBJFile(mesh_path("island3.obj"));
			break;
		}
		RenderSystem::createColoredMesh(resource, "boat");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);

	
	// Initialize the position, scale, and physics components
	auto& motion = ECS::registry<Motion>.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = vec2({ 150.f, 150.f });

	auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
	std::vector<ColoredVertex> boat_vertices = texmesh.mesh.vertices;

	auto& convex_hull = ECS::registry<ConvexComponent>.emplace(entity);
	convex_hull.convex_hull = ConvexHull::getConvexhull(boat_vertices, boat_vertices.size(), motion);

	auto& movement = ECS::registry<FixedMovement>.emplace(entity);
	movement.speed = 0.f;

	// Make boat selectable
	ECS::registry<Selectable>.emplace(entity);

	// Give boat collision
	ECS::registry<Collider>.emplace(entity);

	// Add a bounding box
	auto&& bb = ECS::registry<BoundingBox>.emplace(entity);
	bb.x_dim = abs(motion.scale.x) / 2.0f;
	bb.y_dim = abs(motion.scale.y) / 2.0f;

	// Create a Boat component
	ECS::registry<Island>.emplace(entity);

	return entity;
}
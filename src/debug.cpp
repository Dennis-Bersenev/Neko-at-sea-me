// Header
#include "debug.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace DebugSystem 
{
	void createLine(vec2 position, vec2 scale) {
		auto entity = ECS::Entity();

		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;
			vec3 red = { 0.8,0.1,0.1 };

			// Corner points
			ColoredVertex v;
			v.position = {-0.5,-0.5,z};
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);

			// Two triangles
			resource.mesh.vertex_indices.push_back(0);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(3);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(2);
			resource.mesh.vertex_indices.push_back(3);

			RenderSystem::createColoredMesh(resource, "boat");
		}

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ECS::registry<ShadedMeshRef>.emplace(entity, resource);

		// Create motion
		auto& motion = ECS::registry<Motion>.emplace(entity);
		auto& movement = ECS::registry<DynamicMovement>.emplace(entity);
		motion.angle = 0.f;
		motion.position = position;
		motion.scale = scale;
		movement.velocity = { 0, 0 };

		ECS::registry<DebugComponent>.emplace(entity);
	}

	void createColoredLine(vec2 position, vec2 scale, vec3 color)
	{
		auto entity = ECS::Entity();

		std::string key = "thick_line" + std::to_string(color.x) + std::to_string(color.y) + std::to_string(color.z);
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;

			// Corner points
			ColoredVertex v;
			v.position = { -0.5,-0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = color;
			resource.mesh.vertices.push_back(v);

			// Two triangles
			resource.mesh.vertex_indices.push_back(0);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(3);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(2);
			resource.mesh.vertex_indices.push_back(3);

			RenderSystem::createColoredMesh(resource, "boat");
		}

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ECS::registry<ShadedMeshRef>.emplace(entity, resource);

		// Create motion
		auto& motion = ECS::registry<Motion>.emplace(entity);
		auto& movement = ECS::registry<DynamicMovement>.emplace(entity);
		motion.angle = 0.f;
		motion.position = position;
		motion.scale = scale;
		movement.velocity = { 0, 0 };

		ECS::registry<DebugComponent>.emplace(entity);
	}

	void clearDebugComponents() {
		// Clear old debugging visualizations
		while (ECS::registry<DebugComponent>.entities.size() > 0) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<DebugComponent>.entities.back());
        }
	}

	bool in_debug_mode = false;
}

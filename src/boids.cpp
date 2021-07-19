#include "boids.hpp"
#include "missile.hpp"
#include "render.hpp"

void Boids::createBoid(vec2 position)
{
    auto entity = ECS::Entity();

    // render
	std::string key = "triangle";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0) {
		// create a procedural circle
		constexpr float z = -0.1f;
		auto color = vec3{ 1.0, 1.0, 1.0 };

		// Corner points
		ColoredVertex v;
		v.position = { -0.5, -0.5, z };
		v.color = color;
		resource.mesh.vertices.push_back(v);
		v.position = { 0.0, 1.0, z };
		v.color = color;
		resource.mesh.vertices.push_back(v);
		v.position = { 0.5, -0.5, z };
		v.color = color;
		resource.mesh.vertices.push_back(v);

		// 1 triangles
		resource.mesh.vertex_indices.push_back(0);
		resource.mesh.vertex_indices.push_back(1);
		resource.mesh.vertex_indices.push_back(2);

		RenderSystem::createColoredMesh(resource, "boat");
	}

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);

	// Create motion
	auto& motion = ECS::registry<Motion>.emplace(entity);
	auto& movement = ECS::registry<DynamicMovement>.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = { 10, 10 };
	float rand_x = (rand() % 100 - 50) * 0.01f * 5;
	float rand_y = (rand() % 100 - 50) * 0.01f * 5;
	// std::cout << rand_x << ' ' << rand_y << std::endl;
	movement.velocity = { rand_x, rand_y };
    
    // register in Boids
    ECS::registry<Boids>.emplace(entity);
}

vec2 rules(ECS::Entity self, float cohesion_level, float separation_level, float alignment_level, vec2 window, vec2 target)
{
	// https://www.red3d.com/cwr/boids/
	// http://www.kfish.org/boids/pseudocode.html
	vec2 ret = { 0.f, 0.f };
	vec2 perceivedCenter = { 0.f, 0.f };
	vec2 moveAway = { 0.f, 0.f };
	vec2 perceivedVel = { 0.f, 0.f };
	vec2 toTarget = { 0.f, 0.f };
	for (auto boid : ECS::registry<Boids>.entities) {
		if (boid.id != self.id) {
			// cohesion
			perceivedCenter += boid.get<Motion>().position;
			// separation
			if (distance(boid.get<Motion>().position, self.get<Motion>().position) < 10.f) {
				moveAway -= (boid.get<Motion>().position - self.get<Motion>().position);
			}
			// alignment
			perceivedVel += boid.get<DynamicMovement>().velocity;
			// towards the target
			toTarget = target - boid.get<Motion>().position;
		}
	}
	int flock_size_sub_1 = ECS::registry<Boids>.entities.size() - 1;
	flock_size_sub_1 = (flock_size_sub_1 == 0) ? 1 : flock_size_sub_1;
	// cohesion
	perceivedCenter /= flock_size_sub_1;
	// alignment
	perceivedVel /= flock_size_sub_1;
	// bound
	vec2 bounce = vec2{ 0.f, 0.f };
	auto& pos = self.get<Motion>().position;
	float offset = 20.f;
	float x_min = offset;
	float x_max = window.x - offset;
	float y_min = offset;
	float y_max = window.y - offset;
	if (pos.x < x_min) {
		bounce.x = 10.f;
	}
	else if (pos.x > x_max) {
		bounce.x = -10.f;
	}
	if (pos.y < y_min) {
		bounce.y = 10.f;
	}
	else if (pos.y > y_max) {
		bounce.y = -10.f;
	}
	// final
	ret += ((perceivedCenter - self.get<Motion>().position) * vec2 { 0.01f * cohesion_level, 0.01f * cohesion_level});
	ret += (moveAway * vec2{ separation_level, separation_level });
	ret += ((perceivedVel - self.get<DynamicMovement>().velocity) * vec2 { 0.1f * alignment_level, 0.1f * alignment_level});
	ret += (toTarget * vec2{ 0.005f * cohesion_level, 0.005f * cohesion_level });
	ret += bounce;
	return ret;
}

void limit_velocity(ECS::Entity self, float limit)
{
	auto& vel = self.get<DynamicMovement>().velocity;
	auto speed = length(vel);
	if (speed > limit) {
		vel = normalize(vel) * limit;
	}
}

void Boids::updateBoids(vec2 window)
{
	// check if this skill is activated
	if (ECS::registry<Boids>.entities.size() > 0) {
		vec2 target = window * vec2{ 0.5f, 0.5f };
		if (ECS::registry<Missile>.entities.size() > 0) {
			target = ECS::registry<Missile>.entities[0].get<Motion>().position;
		}
		for (auto boid : ECS::registry<Boids>.entities) {
			auto& boid_motion = boid.get<Motion>();
			auto& boid_dmovement = boid.get<DynamicMovement>();
			// update with rules
			auto v = rules(boid, 0.2f, 0.5f, 1.f, window, target);
			// update the final resulst
			boid_dmovement.velocity += v;
			limit_velocity(boid, 10.f);
		}
	}
}

void Boids::stepMoveBoids()
{
	if (ECS::registry<Boids>.entities.size() > 0) {
		for (auto boid : ECS::registry<Boids>.entities) {
			auto& boid_motion = boid.get<Motion>();
			auto& boid_dmovement = boid.get<DynamicMovement>();
			boid_motion.position += boid_dmovement.velocity;
			boid_motion.angle = atan2f(boid_dmovement.velocity.x, boid_dmovement.velocity.y);
			boid_motion.angle = boid_motion.angle * 180 / PI - 180;
		}
	}
}

void Boids::clearBoids()
{
	while (ECS::registry<Boids>.entities.size() > 0) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<Boids>.entities.back());
	}
}

// internal
#include <iostream>
#include "physics.hpp"
#include "render_components.hpp"
#include <boat.hpp>
#include "convexhull.hpp"
#include <shark.hpp>
#include <island.hpp>
#include <crate.hpp>
#include "tiny_ecs.hpp"
#include "debug.hpp"
#include "motion_spline.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

vec2 get_vertex_points(vec2 vertex, Transform transform) {

	mat4 mat = transform.mat;
	vec4 position = vec4{ vertex.x,vertex.y, 0 ,1 };
	vec4 transformed_pos =
		vec4(mat[0][0] * position.x + mat[1][0] * position.y + mat[2][0] * position.z + mat[3][0] * 1.f,
			mat[0][1] * position.x + mat[1][1] * position.y + mat[2][1] * position.z + mat[3][1] * 1.f,
			mat[0][2] * position.x + mat[1][2] * position.y + mat[2][2] * position.z + mat[3][2] * 1.f,
			mat[0][3] * position.x + mat[1][3] * position.y + mat[2][3] * position.z + mat[3][3] * 1.f);

	return vec2(transformed_pos.x, transformed_pos.y);

}

//https://gamedev.stackexchange.com/questions/586/what-is-the-fastest-way-to-work-out-2d-bounding-box-intersection
bool PhysicsSystem::box_colliding(Motion& box1, Motion& box2) {
	vec2 bounding_box1 = get_bounding_box(box1);
	vec2 bounding_box2 = get_bounding_box(box2);

	float top_box1 = box1.position.y - bounding_box1.y / 2.f;
	float bottom_box1 = box1.position.y + bounding_box1.y / 2.f;
	float left_box1 = box1.position.x - bounding_box1.y / 2.f;
	float right_box1 = box1.position.x + bounding_box1.y / 2.f;

	float top_box2 = box2.position.y - bounding_box2.y / 2.f;
	float bottom_box2 = box2.position.y + bounding_box2.y / 2.f;
	float left_box2 = box2.position.x - bounding_box2.y / 2.f;
	float right_box2 = box2.position.x + bounding_box2.y / 2.f;


	return !(left_box2 > right_box1 ||
		right_box2 < left_box1 ||
		top_box2 > bottom_box1 ||
		bottom_box2 < top_box1);
}

bool PhysicsSystem::wall_colliding(Motion& box, vec2 window_size_in_game_units) {
	vec2 bounding_box = get_bounding_box(box);

	float top_box = box.position.y - bounding_box.y / 2.f;
	float bottom_box = box.position.y + bounding_box.y / 2.f;
	float left_box = box.position.x - bounding_box.y / 2.f;
	float right_box = box.position.x + bounding_box.y / 2.f;

	if (left_box < 0) {
		box.position.x += 2;
		return true;
	}
	 else if (right_box > window_size_in_game_units.x) {
		box.position.x -= 2;
		return true;
	}
	else if (top_box < 0) {
		box.position.y += 2;
		return true;
	}
	else if (bottom_box > window_size_in_game_units.y) {
		box.position.y -= 2;
		return true;
	}
	else {
		return false;
	}
}

std::vector<std::vector<vec2>> get_edge_vector_and_normal(std::vector<vec2> points, vec2 center) {
	std::vector<vec2> reference_points;
	std::vector<vec2> edges;
	std::vector<vec2> normals;
	for (int x = 0; x < points.size(); x++)
	{
		//check if it is not last point 
		if (x != points.size() - 1) {
			reference_points.push_back(points[x + 1]);
			vec2 edge = points[x + 1] - points[x];
			edges.push_back(edge);

			vec2 n_edge = normalize(edge);
			vec2 normal = vec2{ -n_edge.y,n_edge.x };

			//check that they are prependicular
			float check = dot(normal,normalize(edge));

			normals.push_back(normal);
		}
		else {
			reference_points.push_back(points[0]);
			vec2 edge = points[0] - points[x];
			edges.push_back(edge);

			vec2 n_edge = normalize(edge);
			vec2 normal = vec2{ -n_edge.y,n_edge.x };

			//check that they are prependicular
			float check = dot(normal, normalize(edge));

			normals.push_back(normal);
		}

	}
	std::vector<std::vector<vec2>> result;
	result.push_back(reference_points);
	result.push_back(edges);
	result.push_back(normals);
	return result;
}

Transform get_transform(Motion& motion) {
	Transform transform;
	// transform.translate(vec2(motion.position.x - ConvexHull::window_dims.x, ConvexHull::window_dims.y - motion.position.y));
	transform.translate(vec2(motion.position.x, motion.position.y));
	transform.rotate(motion.angle);
	transform.scale(vec2{ motion.scale.x,-motion.scale.y });

	return transform;

}

std::vector<vec2> get_transform_points(std::vector<vec2> vertices, Transform transform) {
	std::vector<vec2> transformed_points;
	for (unsigned int x = 0; x < vertices.size(); x++)
	{
		transformed_points.push_back(get_vertex_points(vertices[x], transform));
	}
	return transformed_points;
}

bool PhysicsSystem::precise_colliding(ECS::Entity entity1, ECS::Entity entity2) {
	//calculating the SAT to check if it is precisely collding 

	ConvexComponent& convexhull_1 = ECS::registry<ConvexComponent>.get(entity1);
	ConvexComponent& convexhull_2 = ECS::registry<ConvexComponent>.get(entity2);

	Motion& motion_1 = ECS::registry<Motion>.get(entity1);
    Motion& motion_2 = ECS::registry<Motion>.get(entity2);

	Transform transform_1 = get_transform(motion_1);
	Transform transform_2 = get_transform(motion_2);

	std::vector<vec2> transformed_points_1 = get_transform_points(convexhull_1.convex_hull, transform_1);
	std::vector<vec2> transformed_points_2 = get_transform_points(convexhull_2.convex_hull, transform_2);

	//this is to get reference points[0], edge vector[1] and normals[2]
	std::vector<std::vector<vec2>>info_1 = get_edge_vector_and_normal(transformed_points_1,motion_1.position);
	std::vector<std::vector<vec2>> info_2 = get_edge_vector_and_normal(transformed_points_2,motion_2.position);

	//check shape 1 to 2 
	for (int i = 0; i < info_1[1].size(); i++) {
		std::vector<vec2> left_side; //this is the side the normal is point at 
		std::vector<vec2> right_side; //this is the side opposite to normal point

		vec2 curr_ref_point = info_1[0][i];
		vec2 curr_normal = info_1[2][i];
		for (int j = 0; j < transformed_points_2.size(); j++) {
			vec2 curr_point = transformed_points_2[j];
			float dot_result = dot(curr_normal, normalize(curr_point - curr_ref_point));

			if (dot_result > 0) {
				left_side.push_back(curr_point);
			}
			else {
				right_side.push_back(curr_point);
			}
		}
		if (left_side.size() + right_side.size() == transformed_points_2.size()) {
			if (left_side.size() == transformed_points_2.size()) {
				return true;//there is a seperating axis !!!!!
			}
		}
	}
	//check shape 2 to 1 
	for (int i = 0; i < info_2[1].size(); i++) {
		std::vector<vec2> left_side; //this is the side the normal is point at 
		std::vector<vec2> right_side; //this is the side opposite to normal point

		vec2 curr_ref_point = info_2[0][i];
		vec2 curr_normal = info_2[2][i];
		for (int j = 0; j < transformed_points_1.size(); j++) {
			vec2 curr_point = transformed_points_1[j];
			float dot_result = dot(curr_normal, normalize(curr_point - curr_ref_point));

			if (dot_result > 0) {
				left_side.push_back(curr_point);
			}
			else {
				right_side.push_back(curr_point);
			}
		}
		if (left_side.size() + right_side.size() == transformed_points_2.size()) {
			if (left_side.size() == transformed_points_2.size()) {
				return true;//there is a seperating axis !!!!!
			}
		}
	}

	return false;


}


void build_bounding_box(const Motion& motion)
{
	vec2 bounding_box_horizontal = get_bounding_box(motion);
	vec2 bounding_box_vertical = get_bounding_box(motion);
	bounding_box_horizontal.y *= 0.05f;
	bounding_box_vertical.x *= 0.05f;
	float half_horizontal_length = bounding_box_horizontal.x / 2.f;
	float half_vertical_length = bounding_box_vertical.y / 2.f;

	DebugSystem::createLine(vec2(motion.position.x, motion.position.y + half_vertical_length), bounding_box_horizontal);
	DebugSystem::createLine(vec2(motion.position.x, motion.position.y - half_vertical_length), bounding_box_horizontal);
	DebugSystem::createLine(vec2(motion.position.x + half_horizontal_length, motion.position.y), bounding_box_vertical);
	DebugSystem::createLine(vec2(motion.position.x - half_horizontal_length, motion.position.y), bounding_box_vertical);
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool collides(const Motion& motion1, const Motion& motion2)
{
	auto dp = motion1.position - motion2.position;
	float dist_squared = dot(dp, dp);
	float other_r = std::sqrt(std::pow(get_bounding_box(motion1).x / 2.0f, 2.f) + std::pow(get_bounding_box(motion1).y / 2.0f, 2.f));
	float my_r = std::sqrt(std::pow(get_bounding_box(motion2).x / 2.0f, 2.f) + std::pow(get_bounding_box(motion2).y / 2.0f, 2.f));
	float r = max(other_r, my_r);
	if (dist_squared < r * r)
		return true;
	return false;
}



void PhysicsSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	// Check for collisions between all moving entities
	auto& collider_container = ECS::registry<Collider>;
	// for (auto [i, motion_i] : enumerate(motion_container.components)) // in c++ 17 we will be able to do this instead of the next three lines
	for (unsigned int i = 0; i < collider_container.components.size(); i++)
	{
		ECS::Entity entity_i = collider_container.entities[i];
		auto& motion_i = entity_i.get<Motion>();

		for (unsigned int j = i + 1; j < collider_container.components.size(); j++)
		{
			ECS::Entity entity_j = collider_container.entities[j];
			auto& motion_j = entity_j.get<Motion>();

			if (collides(motion_i, motion_j))
			{
				// Create a collision event
				// Note, we are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity, hence, emplace_with_duplicates
				ECS::registry<Collision>.emplace_with_duplicates(entity_i, entity_j);
				ECS::registry<Collision>.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	if (DebugSystem::in_debug_mode)
	{
		auto& registry = ECS::registry<Motion>;
		for (unsigned int i = 0; i < registry.components.size(); i++)
		{
			Motion& motion = registry.components[i];

			Transform transform;
			// transform.translate(vec2(motion.position.x - ConvexHull::window_dims.x, ConvexHull::window_dims.y - motion.position.y));
			transform.translate(vec2(motion.position.x, motion.position.y));
			transform.rotate(motion.angle);
			transform.scale(vec2{ motion.scale.x,-motion.scale.y });

			ECS::Entity entity = registry.entities[i];
			if (ECS::registry<Boat>.has(entity) || ECS::registry<Shark>.has(entity) ||
				ECS::registry<Crate>.has(entity) || ECS::registry<Island>.has(entity)) {
				ConvexComponent& convex = ECS::registry<ConvexComponent>.get(entity);
				std::vector<vec2> vertices = convex.convex_hull;
				for (unsigned int x = 0; x < vertices.size(); x++)
				{

					DebugSystem::createLine(get_vertex_points(vertices[x], transform), vec2{ 5,5 });
				}
			}
			if (ECS::registry<Boat>.has(entity) || ECS::registry<Shark>.has(entity) ||
				ECS::registry<Crate>.has(entity) || ECS::registry<Island>.has(entity)) {
				build_bounding_box(motion);
			}
		}
	}

	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	// Check for collisions, bounce boats/sharks towards the direction they came in
	for (auto entity1 : ECS::registry<Motion>.entities) {
		for (auto entity2 : ECS::registry<Motion>.entities) {

			if (entity1.id != entity2.id) {
                auto& motion1 = entity1.get<Motion>();
                auto& motion2 = entity2.get<Motion>();

                if (motion1.isColliding == true && motion2.isColliding == true) continue;
				if (ECS::registry<AIComponent>.has(entity1) && ECS::registry<AIComponent>.has(entity2)) continue;

				if (box_colliding(motion1, motion2)) {
				    if ((ECS::registry<Boat>.has(entity1) || ECS::registry<Shark>.has(entity1) ||ECS::registry<Crate>.has(entity1) || ECS::registry<Island>.has(entity1)) &&(
				        ECS::registry<Boat>.has(entity2) || ECS::registry<Shark>.has(entity2) ||ECS::registry<Crate>.has(entity2) || ECS::registry<Island>.has(entity2))) {
                        if (!precise_colliding(entity1, entity2)) {
                            motion1.isColliding = true;
                            motion2.isColliding = true;
							float bounce_factor = -20.f; // aaftdf
							float repulse_factor = 5.f; // reposition after collision to avoid sticky effect

                            // Both entities have motion spline
                            if (ECS::registry<MotionSpline>.has(entity1) && ECS::registry<MotionSpline>.has(entity2)) {
                                auto &motionTemp1 = motion1;
                                auto &motionTemp2 = motion2;
                                auto &spline1 = entity1.get<MotionSpline>();
                                auto &spline2 = entity2.get<MotionSpline>();

                                auto &movement1 = entity1.get<FixedMovement>();
                                auto &movement2 = entity2.get<FixedMovement>();
								
                                float step_speed1 = movement1.speed * step_seconds * bounce_factor;
                                float step_speed2 = movement2.speed * step_seconds * bounce_factor;

								float offset_x1 = repulse_factor;
								float offset_x2 = -repulse_factor;
								float offset_y1 = repulse_factor;
								float offset_y2 = -repulse_factor;

								if (motion1.position.x - motion2.position.x < 0) {
									offset_x1 *= -1.f;
									offset_x2 *= -1.f;
								}

								if (motion1.position.y - motion2.position.y < 0) {
									offset_y1 *= -1.f;
									offset_y2 *= -1.f;
								}

                                // Set bounce for entity 1
                                vec2 bounce_target_pos1 = spline1.getLastPoint();
                                if (bounce_target_pos1 != motion1.position) {
                                    vec2 heading1 = bounce_target_pos1 - motion1.position;
                                    motion1.angle = atan2f(heading1.y, heading1.x);
									motion1.position += vec2{ offset_x1, offset_y1 };
                                    vec2 bounce_point1 = motion1.position + step_speed1 * normalize(heading1);
                                    spline1.reset(motion1.position);
                                    spline1.setPoints({motion1.position, bounce_point1}, true);
                                }

                                // Set bounce for entity 2
                                vec2 bounce_target_pos2 = spline2.getLastPoint();
                                if (bounce_target_pos2 != motion2.position) {
                                    vec2 heading2 = bounce_target_pos2 - motion2.position;
                                    motion2.angle = atan2f(heading2.y, heading2.x);
									motion2.position += vec2{ offset_x2, offset_y2 };
                                    vec2 bounce_point2 = motion2.position + step_speed2 * normalize(heading2);
                                    spline2.reset(motion2.position);
                                    spline2.setPoints({motion2.position, bounce_point2}, true);
                                }

                                // After collision calculation if both entitites have status component (boats and sharks)
                                if (ECS::registry<Status>.has(entity1) && ECS::registry<Status>.has(entity2)) {
                                    auto &status1 = entity1.get<Status>();
                                    auto &status2 = entity2.get<Status>();

                                    status1.health -= status2.collision_damage;
                                    status2.health -= status1.collision_damage;
                                }
                            } else {
                                // Entity 1 has motion spline (Boat or Shark)
                                if (ECS::registry<MotionSpline>.has(entity1)) {
                                    auto &status1 = entity1.get<Status>();

                                    // Collides into Island (lose all health)
                                    if (ECS::registry<Island>.has(entity2)) {
                                        if (status1.player_number != 0) // a hack to not call ECS::registry<Boat>.has(entity1)
											status1.health = 0;
                                    } else if (ECS::registry<Crate>.has(entity2)) {
                                        // Collides into Crate (gain 100 health)
                                        status1.health += 100;
										if (status1.health > status1.maxHealth)
											status1.health = status1.maxHealth;
                                        // Destroy crate
										entity2.get<CrateComponent>().isUsed = true;
                                    }
                                } else if (ECS::registry<MotionSpline>.has(entity2)) {
                                    // Entity 2 has motion spline (Boat or Shark)
                                    auto &status2 = entity2.get<Status>();

                                    // Collides into Island (lose all health)
                                    if (ECS::registry<Island>.has(entity1)) {
										if (status2.player_number != 0) // a hack to not call ECS::registry<Boat>.has(entity1)
											status2.health = 0;
                                    } else if (ECS::registry<Crate>.has(entity1)) {
                                        // Collides into Crate (gain 100 health)
										status2.health += 100;
										if (status2.health > status2.maxHealth)
											status2.health = status2.maxHealth;
                                        // Destroy crate
										entity1.get<CrateComponent>().isUsed = true;
                                    }
                                }
                            }
                        }
                    }
					
				}
			}
		}
	}

	// Reset motion colliding boolean
	for (auto entity : ECS::registry<Motion>.entities) {
		auto& motion = entity.get<Motion>();
		motion.isColliding = false;
	}

	// Check to see if boats collide into wall, if yes, clear the motion spline
	for (auto entity : ECS::registry<MotionSpline>.entities) {
		if (ECS::registry<Boat>.has(entity)) {
			auto& motion = entity.get<Motion>();
			if (wall_colliding(motion, window_size_in_game_units)) {
				auto& spline = entity.get<MotionSpline>();
				spline.reset(motion.position);
			}
		}
	}
}

PhysicsSystem::Collision::Collision(ECS::Entity& other)
{
	this->other = other;
}

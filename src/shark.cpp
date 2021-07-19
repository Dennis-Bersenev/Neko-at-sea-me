//
// Created by austi on 2021-02-02.
//

// Header
#include "shark.hpp"
#include "render.hpp"
#include "selectable.hpp"
#include "convexhull.hpp"
#include "motion_spline.hpp"

ECS::Entity Shark::createShark(vec2 position, float speed)
{
    auto entity = ECS::Entity();

    std::string key = "shark";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource.mesh.loadFromOBJFile(mesh_path("new_shark.obj"));
        RenderSystem::createColoredMesh(resource, "boat");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // add status
    auto& shark_status = ECS::registry<Status>.emplace(entity);
    // Regular boat
    shark_status.name = "Bruce the Shark";
    shark_status.weapon_damage = 5.f;
    shark_status.collision_damage = 0.f;
    shark_status.attack_range = 50.f;
    shark_status.health = 200.f;
    shark_status.maxHealth = 200.f;
    shark_status.speed = 200.f;
    shark_status.maxSpeed = 200.f;

    // Initialize the position, scale, and physics components
    auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& movement = ECS::registry<FixedMovement>.emplace(entity);
    motion.angle = 0.f;
    movement.speed = speed; // TODO use the status or input?
    motion.position = position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ 50.f,50.f });

    auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
    std::vector<ColoredVertex> shark_vertices = texmesh.mesh.vertices;

    auto& convex_hull = ECS::registry<ConvexComponent>.emplace(entity);
    convex_hull.convex_hull = ConvexHull::getConvexhull(shark_vertices, shark_vertices.size(), motion);

    // Add motionspline
    ECS::registry<MotionSpline>.emplace(entity, position);

    // Add collision
    ECS::registry<Collider>.emplace(entity);

    // Add a bounding box
    auto&& bb = ECS::registry<BoundingBox>.emplace(entity);
    bb.x_dim = abs(motion.scale.x) / 2.0f;
    bb.y_dim = abs(motion.scale.y) / 2.0f;

    // Create a Shark component
    ECS::registry<Shark>.emplace(entity);

    return entity;
}
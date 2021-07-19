// Header
#include "missile.hpp"
#include "render.hpp"

ECS::Entity Missile::createMissile(vec2 pos, float power, ECS::Entity target)
{
    auto entity = ECS::Entity();

    std::string key = "missile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.size() == 0)
    {
        resource.mesh.loadFromOBJFile(mesh_path("big_missile.obj"));
        RenderSystem::createColoredMesh(resource, "boat");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Motion
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.position = pos;
	motion.scale = vec2({ 30.f, 70.f });

    // Add a bounding box
    auto&& bb = ECS::registry<BoundingBox>.emplace(entity);
    bb.x_dim = abs(motion.scale.x) / 2.0f;
    bb.y_dim = abs(motion.scale.y) / 2.0f;

//    auto& missile_component = ECS::registry<MissileComponent>.emplace(entity);
//    missile_component.power = power;
//    missile_component.target_pos = target_pos;
//    missile_component.isMoving = true;

    auto& movement = ECS::registry<FixedMovement>.emplace(entity);
    movement.speed = 150.f;

    ECS::registry<MotionSpline>.emplace(entity, pos);

    ECS::registry<Missile>.emplace(entity);
    auto& missle_comp = ECS::registry<MissileComponent>.emplace(entity);
    missle_comp.damage = power;
    missle_comp.target = target;
    
    return entity;
}

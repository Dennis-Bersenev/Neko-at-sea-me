//
// Created by stephen on 2021-03-13.
//

// Header
#include "cat.hpp"
#include "render.hpp"
// stlib
#include <iostream>

ECS::Entity Cat::createCat(vec2 position, float frames, float rows, std::string path,std::string name)
{
    auto entity = ECS::Entity();

    std::string key = "cat_" + name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSpriteSheet(resource, textures_path(path), "cat_textured", frames, rows);
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache) 
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    float animation_frame_width = (resource.texture.size.x / frames) / resource.texture.size.x;
    float animation_frame_height = (resource.texture.size.y / rows) / resource.texture.size.y;

    //std::cout << animation_frame_width << "\n";
    //std::cout << animation_frame_height << "\n";

    // Initialize the position, scale, and physics components
    auto& catComponent = ECS::registry<CatComponent>.emplace(entity);
    catComponent.position = position;
    catComponent.angle = 0.f;
    catComponent.scale = vec2({ -0.5f, 0.5f }) * static_cast<vec2>(resource.texture.size.x / frames, resource.texture.size.y / rows);
    catComponent.frame = 0.f;
    catComponent.frames = frames;
    catComponent.row = 0.f;
    catComponent.rows = rows;
    catComponent.animation_frame_width = animation_frame_width;
    catComponent.animation_frame_height = animation_frame_height;
    catComponent.animationSpeed = 100.f;
    catComponent.elapsedTime = 0.f;
    catComponent.texture_x_Position_UV = 0.f;
    catComponent.texture_y_Position_UV = 0.f;

    //ECS::registry<DebugModeComponent>.emplace(entity);
    ECS::registry<Cat>.emplace(entity);

    return entity;
}
//
// Created by mandy77 on 2021-02-07.
//

// Header
#include "displaytext.hpp"
#include "render.hpp"
ECS::Entity DisplayText::createDisplayText(vec2 position, std::string name, std::string path, float scale)
{
    auto entity = ECS::Entity();

    std::string key = "text" + name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(path), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale, and physics components
    auto& dtcomponent = ECS::registry<DisplayTextComponent>.emplace(entity);
    dtcomponent.angle = 0.f;
    dtcomponent.position = position;
    // Setting initial values, scale is negative to make it face the opposite way
    dtcomponent.scale = scale*vec2({ 0.2f, 0.2f }) * static_cast<vec2>(resource.texture.size);

    //adding the name of the button which will display
    auto& text = ECS::registry<TextComponent>.emplace(entity);
    text.text = std::move(name);

    // Create and (empty) Button component
    ECS::registry<DisplayText>.emplace(entity);


    return entity;
}
//
// Created by mandy77 on 2021-02-07.
//

// Header
#include "helpbox.hpp"
#include "render.hpp"

ECS::Entity Helpbox::createHelpbox(vec2 position,std::string name)
{
    auto entity = ECS::Entity();

    std::string key = "help_box";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("help_box.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale, and physics components
    auto& helpcomponent = ECS::registry<HelpComponent>.emplace(entity);
    helpcomponent.angle = 0.f;
    helpcomponent.position = position;
    // Setting initial values, scale is negative to make it face the opposite way
    helpcomponent.scale = vec2({ 0.2f, 0.2f }) * static_cast<vec2>(resource.texture.size);

    //adding the name of the button which will display
    auto& text = ECS::registry<TextComponent>.emplace(entity);
    text.text = std::move(name);

    // Create and (empty) Button component
    ECS::registry<Helpbox>.emplace(entity);


    return entity;
}
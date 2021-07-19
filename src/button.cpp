//
// Created by mandy77 on 2021-02-07.
//

// Header
#include "button.hpp"
#include "render.hpp"

ECS::Entity Button::createButton(vec2 position,std::string name)
{
    auto entity = ECS::Entity();
    
    std::string key = "button";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("s_buttons.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale, and physics components
    auto& buttonComponent = ECS::registry<ButtonComponent>.emplace(entity);
    buttonComponent.position = position;
    buttonComponent.angle = 0.f;
    // Setting initial values, scale is negative to make it face the opposite way
    buttonComponent.scale = vec2({ -0.2f, 0.2f }) * static_cast<vec2>(resource.texture.size);

    //adding the name of the button which will display
    auto& text = ECS::registry<TextComponent>.emplace(entity);
    text.text = std::move(name);

    // Create and (empty) Button component
    ECS::registry<Button>.emplace(entity);


    return entity;
}

ECS::Entity Button::createSkillButton(vec2 pos, std::string name, std::string path, ECS::Entity self)
{
    auto entity = ECS::Entity();

    std::string key = "skill_" + name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(path), "textured");
    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    // Initialize the position, scale, and physics components
    auto& buttonComponent = ECS::registry<ButtonComponent>.emplace(entity);
    buttonComponent.position = pos;
    buttonComponent.angle = 0.f;
    // Setting initial values, scale is negative to make it face the opposite way
    buttonComponent.scale = vec2({ -0.4f, 0.4f }) * static_cast<vec2>(resource.texture.size);
    //adding the name of the button which will display
    auto& text = ECS::registry<TextComponent>.emplace(entity);
    text.text = std::move(name);
    // add self to skill component
    auto& skill_button_comp = ECS::registry<SkillButtonComponent>.emplace(entity);
    skill_button_comp.selfboat = self;
    // Create and (empty) Button component
    ECS::registry<Button>.emplace(entity);
    return entity;
}

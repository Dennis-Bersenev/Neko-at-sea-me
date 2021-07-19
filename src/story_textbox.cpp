//
// Created by mandy77 on 2021-02-07.
//

// Header
#include "story_textbox.hpp"
#include "render.hpp"

ECS::Entity StoryTextbox::createTextbox(vec2 pos, std::string name, bool talking, int type)
{
    auto entity = ECS::Entity();

    std::string key = "story_text_box";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        if (type == 0) {
            //this is when a cat is talking
            RenderSystem::createSprite(resource, textures_path("speech_box.png"), "textured");
        }
        else if (type == 1) {
            //this is for just moving the story along
            RenderSystem::createSprite(resource, textures_path("story_box.png"), "textured");
        }
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale, and physics components
    auto& storycomponent = ECS::registry<StoryComponent>.emplace(entity);
    storycomponent.angle = 0.f;
    storycomponent.position = pos;
    // Setting initial values, scale is negative to make it face the opposite way
    if (talking) {
        storycomponent.scale = vec2({ 0.6f, 0.6f }) * static_cast<vec2>(resource.texture.size);
    }
    else {
        storycomponent.scale = vec2({ -0.6f, 0.6f }) * static_cast<vec2>(resource.texture.size);
    }

    //adding the name of the button which will display
    auto& text = ECS::registry<TextComponent>.emplace(entity);
    text.text = std::move(name);

    // Create and (empty) Button component
    ECS::registry<StoryTextbox>.emplace(entity);


    return entity;
}
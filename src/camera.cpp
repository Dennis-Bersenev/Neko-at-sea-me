#pragma once
#include "camera.hpp"

camera::camera(vec2 window_size_px) :
    window_size_px{ window_size_px }, window_size{ window_size_px }, upper_left_corner{ vec2(-window_size_px.x / 2, window_size_px.y / 2) }
{
    //This will be (screen.y /2) since we keep vfov fixed at 90deg.
    max_zoom = window_size_px.y / 2.f;
    curr_zoom = max_zoom;
    origin = glm::vec3(0.0f, 0.0f, max_zoom);
    lookat = glm::vec3(0.0f, 0.0f, 0.0f);
    vup = glm::vec3(0.0f, 1.0f, 0.0f);
    mat4 vcs = build_vcs();
    view_matrix = inverse(vcs);
}

glm::mat4 camera::build_vcs() const
{
    vec3 w = glm::normalize(origin - lookat);
    vec3 u = glm::normalize(glm::cross(vup, w));
    vec3 v = glm::normalize(glm::cross(w, u));

    mat4 vcs = glm::mat4(vec4(u, 0.f), vec4(v, 0.f), vec4(w, 0.f), vec4(origin, 1.f));
    return vcs;
}

void camera::zoom(double scroll)
{
    //TODO: better conditionals!
    float shift = 0;
    if ((scroll > 0) && (curr_zoom > 10))
    {
        shift = -10;
    }
    else if ((scroll < 0) && (curr_zoom < max_zoom))
    {
        shift = 10;
    }

    if (!inside_world_bounds(vec2(origin.x, origin.y), curr_zoom + shift))
    { 
        handle_zoom(curr_zoom + shift);
        lookat.x = origin.x;
        lookat.y = origin.y;
    }
        

    curr_zoom += shift;
    origin.z = curr_zoom;
    float zoom_ratio = (curr_zoom / max_zoom);
    window_size = zoom_ratio * vec2(window_size_px.x, window_size_px.y);
    upper_left_corner = vec2(origin.x, origin.y) + vec2(-window_size.x / 2, window_size.y / 2);
    mat4 updated_vcs = build_vcs();
    view_matrix = inverse(updated_vcs);
}

void camera::pan(vec2 offset)
{
    if (!inside_world_bounds(vec2(origin.x, origin.y) + offset, curr_zoom))
        return;

    origin.x += offset.x;
    origin.y += offset.y;

    lookat.x += offset.x;
    lookat.y += offset.y;
    mat4 updated_vcs = build_vcs();
    view_matrix = inverse(updated_vcs);
    upper_left_corner = vec2(origin.x, origin.y) + vec2(-window_size.x / 2, window_size.y / 2);
}

void camera::orient(float u, float t)
{
    vec2 new_target = upper_left_corner + vec2(u * window_size.x, -t * window_size.y);

    if (!inside_world_bounds(vec2(new_target.x, new_target.y), curr_zoom))
        return;

    origin = vec3(new_target.x, new_target.y, curr_zoom);
    lookat = vec3(new_target.x, new_target.y, 0.f);
    mat4 updated_vcs = build_vcs();
    view_matrix = inverse(updated_vcs);
    upper_left_corner = vec2(origin.x, origin.y) + vec2(-window_size.x / 2, window_size.y / 2);
}
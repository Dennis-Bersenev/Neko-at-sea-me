#pragma once
#include "common.hpp"

class camera {
public:
    glm::vec3 origin, lookat, vup;
    glm::vec3 dir;
    glm::mat4 view_matrix;
    float max_zoom;
    float curr_zoom;
    vec2 window_size;
    const vec2 window_size_px;
    vec2 upper_left_corner;

    camera(vec2 window_size_px);

    glm::mat4 build_vcs() const;
    
    void zoom(double scroll);

    void pan(vec2 offset);

    //Center cam at point given
    void orient(float u, float t);

    inline bool inside_world_bounds(vec2 new_origin, float new_zoom) const
    {
        float zoom_ratio = (new_zoom / max_zoom);
        vec2 dims = 0.5f * zoom_ratio * vec2(window_size_px.x, window_size_px.y);
        bool x_in = (-window_size_px.x / 2 <= (new_origin.x - dims.x)) && ((new_origin.x + dims.x) <= window_size_px.x / 2);
        bool y_in = (-window_size_px.y / 2 <= (new_origin.y - dims.y)) && ((new_origin.y + dims.y) <= window_size_px.y / 2);
        return x_in && y_in;
    }

    inline void handle_zoom(float new_zoom)
    {
        //lookat/look from have to be the same
        float zoom_ratio = (new_zoom / max_zoom);
        if (zoom_ratio >= 1.f - 0.1)
        {
            origin.x = 0;
            origin.y = 0;
            return;
        }

        vec2 dims = 0.5f * zoom_ratio * vec2(window_size_px.x, window_size_px.y);
        float x_diff_left = (-window_size_px.x / 2) - (origin.x - dims.x); // shift origin right by this amount
        float x_diff_right = (origin.x + dims.x) - (window_size_px.x / 2); // shift origin left by this amount
        float y_diff_bot = (-window_size_px.y / 2) - (origin.y - dims.y); // shift origin up by this amount
        float y_diff_top = (origin.y + dims.y) - (window_size_px.y / 2); // shift origin down by this amount
        
        if (x_diff_left > 0)
            origin.x += x_diff_left;
        else
            origin.x -= x_diff_right;

        if (y_diff_bot > 0)
            origin.y += y_diff_bot;
        else
            origin.y -= y_diff_top;
    }

    inline void reset_camera()
    {
        window_size = window_size_px;
        upper_left_corner = vec2(-window_size_px.x / 2, window_size_px.y / 2);
        curr_zoom = max_zoom;
        origin = glm::vec3(0.0f, 0.0f, max_zoom);
        lookat = glm::vec3(0.0f, 0.0f, 0.0f);
        vup = glm::vec3(0.0f, 1.0f, 0.0f);
        mat4 vcs = build_vcs();
        view_matrix = inverse(vcs);
    }
};
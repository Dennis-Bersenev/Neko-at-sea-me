//
// Created by austi on 2021-02-02.
//

#include "motion_spline.hpp"
#include "render.hpp"

#include <utility>





MotionSpline::MotionSpline(vec2 starting_pos) {
    target_point_idx = -1;
    points = {starting_pos};
    last_point = starting_pos;
}

void MotionSpline::setPoints(std::vector<vec2> new_points, bool start_moving) {
    points = std::move(new_points);
    if (start_moving) {
        target_point_idx = 1;
    }
}

std::vector<vec2> MotionSpline::getPoints() {
    return points;
}

vec2 MotionSpline::getNextPoint() {

    
    if (target_point_idx >= points.size() || points.size() < 2) { // hopefully this will eliminate random getNextPoint() bug
        target_point_idx = -1;
        last_point = vec2{ 0.f, 0.f };
        return vec2{ 0.f, 0.f };
    }
    last_point = points[target_point_idx - 1];
    return points[target_point_idx];
}

vec2 MotionSpline::getEndPoint() {
    return points.back();
}

vec2 MotionSpline::getLastPoint() {
    return last_point;
}

void MotionSpline::advanceTargetPoint() {
    if (target_point_idx == -1) {
        throw std::runtime_error("Cannot advance target point of inactive MotionSpline");
    }
    target_point_idx += 1;
    if (target_point_idx >= points.size()) {
        target_point_idx = -1;
    }
}

void MotionSpline::addPoint(vec2 point) {
    points.push_back(point);
}

bool MotionSpline::isMoving() {
    return target_point_idx != -1;
}

void MotionSpline::reset(vec2 starting_pos) {
    target_point_idx = -1;
    points = {starting_pos};
    last_point = starting_pos;
}


float MotionSpline::getTotalLength() {
    if (points.size() < 2) {
        return 0;
    }
    float total_length = 0;
    for (int i = 1; i < points.size(); i++) {
        total_length += distance(points[i], points[i-1]);
    }
    return total_length;
}

ShadedMesh& MotionSpline::getShadedMesh(vec2 start_pos) {
    float LINE_WIDTH = 5;

    std::string key = "motion_spline";
    ShadedMesh& resource = cache_resource(key);
    resource.mesh.vertices.clear();
    resource.mesh.vertex_indices.clear();
    constexpr float z = -0.1f;
    vec3 red = { 0.8,0.1,0.1 };

    ColoredVertex v;
    vec2 a;
    vec2 b;
    vec2 c;
    vec2 d;
    int idx_a = 0;
    int idx_b = 1;
    int idx_c;
    int idx_d;

    // Create 1 vertex "above" spline point and 1 "below"
    vec2 current_pos = start_pos;
    vec2 heading = points[target_point_idx] - current_pos;
    vec2 ortho = {heading.y, -heading.x};
    ortho = normalize(ortho);
    a = current_pos - (ortho * LINE_WIDTH);
    b = current_pos + (ortho * LINE_WIDTH);
    v.position = {a.x, a.y, z};
    v.color = red;
    resource.mesh.vertices.push_back(v);
    v.position = {b.x, b.y, z};
    resource.mesh.vertices.push_back(v);

    // Create 2 triangles for each segment
    int next_point = target_point_idx;
    while (next_point < points.size()) {
        current_pos = points[next_point];
        c = current_pos - (ortho * LINE_WIDTH);
        d = current_pos + (ortho * LINE_WIDTH);
        v.position = {c.x, c.y, z};
        resource.mesh.vertices.push_back(v);
        idx_c = idx_b + 1;
        v.position = {d.x, d.y, z};
        resource.mesh.vertices.push_back(v);
        idx_d = idx_c + 1;
        // Triangle 1
        resource.mesh.vertex_indices.push_back(idx_a);
        resource.mesh.vertex_indices.push_back(idx_b);
        resource.mesh.vertex_indices.push_back(idx_c);
        // Triangle 2
        resource.mesh.vertex_indices.push_back(idx_b);
        resource.mesh.vertex_indices.push_back(idx_d);
        resource.mesh.vertex_indices.push_back(idx_c);
        next_point++;
        // Update indices for next segment
        idx_a = idx_c;
        idx_b = idx_d;
        if (next_point < points.size()) {
            heading = points[next_point] - current_pos;
            ortho = normalize(vec2(heading.y, -heading.x) );
        }
    }

    RenderSystem::createColoredMesh(resource, "colored_mesh");

    return resource;
}





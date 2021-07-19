//
// Created by austi on 2021-02-02.
//

#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

// A curve representing a path for an entity
struct MotionSpline {
    explicit MotionSpline(vec2 starting_pos);

    void setPoints(std::vector<vec2> new_points, bool start_moving=false);

    std::vector<vec2> getPoints();

    void advanceTargetPoint();

    void addPoint(vec2 point);

    vec2 getNextPoint();

    vec2 getEndPoint();

    vec2 getLastPoint();

    bool isMoving();

    float getTotalLength();

    ShadedMesh& getShadedMesh(vec2 start_pos);

    void reset(vec2 starting_pos);
    std::vector<vec2> points;

private:
    int target_point_idx;
    vec2 last_point;
};

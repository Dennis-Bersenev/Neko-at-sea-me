#pragma once

#include "common.hpp"
#include "render_components.hpp"
#include "render.hpp"

// Data structure for pebble-specific information
namespace ConvexHull {

	extern vec2 window_dims;

	vec2 getvertexpoints(ColoredVertex vertex, Transform transform);
	int orientation(vec2 p, vec2 q, vec2 r);
	std::vector<vec2> getConvexhull(std::vector<ColoredVertex> vertices, int n, Motion& motion);
	void set_window(vec2 window);

};

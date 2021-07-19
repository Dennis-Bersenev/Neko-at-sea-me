#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
	void createLine(vec2 position, vec2 scale);

	// draw a line with given color
	void createColoredLine(vec2 position, vec2 scale, vec3 color);

	// !!! TODO A2: implement debugging of bounding boxes
	void createBox(vec2 position, vec2 scale);

	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearDebugComponents();
};

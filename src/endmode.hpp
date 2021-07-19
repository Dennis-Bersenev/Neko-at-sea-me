#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace EndModeSystem {
	extern bool in_end_mode_menu;
	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearEndModeComponents();
}

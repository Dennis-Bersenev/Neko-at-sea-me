#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace PauseModeSystem {
	extern bool in_pause_mode_menu;


	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearPauseModeComponents();
	//Change from pause to not puase and visa vera 
	void change_mode();
};

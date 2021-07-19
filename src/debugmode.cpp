// Header
#include "debugmode.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace DebugModeSystem 
{

	void clearDebugModeComponents() {
		// Clear old debugging visualizations
		while (ECS::registry<DebugModeComponent>.entities.size() > 0) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<DebugModeComponent>.entities.back());
        }
	}

	bool in_debug_mode_menu = false;
}

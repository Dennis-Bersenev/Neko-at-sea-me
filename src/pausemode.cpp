// Header
#include "pausemode.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace PauseModeSystem 
{

	void clearPauseModeComponents() {
		// Clear old debugging visualizations
		while (ECS::registry<PauseModeComponent>.entities.size() > 0) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<PauseModeComponent>.entities.back());
        }
	}

	void change_mode() {
		if (in_pause_mode_menu == true) {
			in_pause_mode_menu = false;
			std::cout << "this is false" << std::endl;
		}
		else {
			in_pause_mode_menu = true;
			std::cout << "this is true" << std::endl;
		}
	}

	bool in_pause_mode_menu = false;
}

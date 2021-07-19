
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"
#include "debug.hpp"
#include "debugmode.hpp"
#include "endmode.hpp"
#include "convexhull.hpp"
#include "pausemode.hpp"
#include "optionmode.hpp"
#include "world.hpp"
#include "achievements.hpp"
#include "physics.hpp"
#include "ai.hpp"
#include "helpobserver.hpp"
#include "storyobserver.hpp"
#include "level_loader.hpp"
#include "map.hpp"

using Clock = std::chrono::high_resolution_clock;

const ivec2 window_size_in_px = {1500, 950};
const vec2 window_size_in_game_units = { 1500, 950 };
const float vfov = 90.f;
float max_zoom;
// Note, here the window will show a width x height part of the game world, measured in px. 
// You could also define a window to show 1.5 x 1 part of your game world, where the aspect ratio depends on your window size.

struct Description {
	std::string name;
	Description(const char* str) : name(str) {};
};

// Entry point
int main()
{
	// Initialize the main systems
	WorldSystem world(window_size_in_px);
	RenderSystem renderer(*world.window);
    PhysicsSystem physics;

	Map map_graph(window_size_in_game_units, world);
	AISystem ai(window_size_in_game_units, world, &map_graph);

	AchievementsSystem achievements(world);
	HelpSystem helpsystem(world);
	StorySystem storysystem(world);
    LevelSystem level_loader;
	ConvexHull::set_window(vec2{ window_size_in_game_units.x/2,window_size_in_game_units.y/2});

    bool isDebug;

	// Set all states to default
	world.restart();
	auto t = Clock::now();
	glm::mat4 o_proj = glm::ortho(0, window_size_in_px.x, 0, window_size_in_px.y);


	// Variable timestep loop
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;

		DebugSystem::clearDebugComponents();
		DebugModeSystem::clearDebugModeComponents();
		PauseModeSystem::clearPauseModeComponents();
		OptionModeSystem::clearOptionModeComponents();
		EndModeSystem::clearEndModeComponents();
        world.step(elapsed_ms, window_size_in_game_units);
		ai.step(elapsed_ms, window_size_in_game_units);
        physics.step(elapsed_ms, window_size_in_game_units);
		world.handle_collisions();
		world.death_check(elapsed_ms);


		// Camera stuff
		camera c = world.cam;
		float aspect = (float)window_size_in_px.x / window_size_in_px.y;
		mat4 projection_matrix = glm::perspective(glm::radians(vfov), aspect, 0.1f, (float)window_size_in_px.y);

		renderer.draw(window_size_in_game_units, c.view_matrix, projection_matrix, o_proj, elapsed_ms, world.get_current_scene());
	}

	//ECS::ContainerInterface::remove_all_components_of(ECS::registry<Text>.entities.back());


	return EXIT_SUCCESS;
}

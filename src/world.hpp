#pragma once

// internal
#include "common.hpp"
#include "tiny_ecs.hpp"
#include "observer_pattern.hpp"
#include "text.hpp"
#include "camera.hpp"
// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <stack>

enum GamePhase {menu, setup, planning, action};
enum GameMode {freeplay,story};
//enum Scene { title, selection, pause, game};

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class WorldSystem
{
public:
	// Window into our world
	camera cam;

	// Creates a window
	WorldSystem(ivec2 window_size_px);

	// Releases all associated resources
	~WorldSystem();

	// restart level
	void restart();

	// Steps the game ahead by ms milliseconds
    void step(float elapsed_ms, vec2 window_size_in_game_units);

	// Check for collisions
	void handle_collisions();

	void death_check(float elapsed_ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	//To move from one scene to another
	void scene_update(int scene_number);

	ivec2 get_window_size();

	//keep track of mode
	GameMode mode;

	// OpenGL window handle
	GLFWwindow* window;

	// game start subject
	Subject subj_game_start;

	// game restart subject
	Subject subj_game_restart;

	// action -> planning
	Subject subj_change_to_planning;

	// level loading finished
	Subject subj_level_done;

	Subject subj_help_listener;
	//helptool
	Subject subj_helptool_listener;

	//this is to go through textboxes
	Subject subj_story_listener;
	//this is to restart the story if needed
	Subject subj_restart_story_listener;

	//Needed for correct game background rendering
	int get_current_scene() const { return current_scene; }
	ivec2 get_window_dims() const { return window_dimensions; }

	std::shared_ptr<Font> font;
private:
	// Input callback functions
	void on_key(int key, int, int key_action, int mod);
	void on_mouse_move(vec2 mouse_pos);
	bool check_button_click(vec2 mouse_pos, vec2 scale, vec2 button_pos);
	void on_mouse_skill_helper(ECS::Entity boat, int skill_num, Mix_Chunk* act, Mix_Chunk* dea);
	void on_mouse_button(int button, int mouse_action, int mods);
	void is_game_mode(ECS::Entity entity, int scene);
	void on_mouse_scroll(vec2 scroll_input);

	bool handle_selection();
    void handle_movement(bool queue);

	void step_movement(float elapsed_ms);
	void step_debug(float elapsed_ms);
	void step_pause(float elapsed_ms);
	void step_end(float elapsed_ms);
	// void step_ai(float elapsed_ms);
    void combat_resolver(ECS::Entity attacker, ECS::Entity defender);
	void step_option(float elapsed_ms);
	void tick_combat_helper(ECS::Entity cat1, ECS::Entity cat2);
	void tick_combat(float elapsed_ms);
	void guide_missile(float elapsed_ms);
	void clear_scene_stack();

	//start a timer
    void getTimer();

	// Loads the audio
	void init_audio();

	// Game functions
	void set_mouse_pos();
	vec2 get_game_pos() const;
	void shift_at_boundry();
	bool in_window_bounds();
	vec2 window_offset(const vec2& v) const;
	void handle_movement_arrow();
	void start_movement_arrow();
	void end_movement_arrow();

	ivec2 window_dimensions;
	vec2 mouse_pos;
	bool shift_mod;
	bool ctrl_mod;

	// Game state
	std::vector<ECS::Entity> boats;
	std::vector<ECS::Entity> buttons;
	std::vector<ECS::Entity> pause_buttons;
	std::vector<ECS::Entity> option_buttons;
	std::vector<ECS::Entity> sharks;
	std::vector<ECS::Entity> players;
	std::vector<ECS::Entity> texts;
	std::vector<bool> chapters_completed;
	ECS::Entity warning_text;
	std::vector<ECS::Entity> cats;
	int current_scene;
	std::string level_selected;
	std::stack<int> scene_stack;
	bool exit;
	bool result;
	bool started_story;
	float action_phase_time = 5000.f; // 10s action phase
	float remaining_action_time = 5000.f; // initialization = action_phase_time
	float combat_cd = 2000.f;
	float combat_timer = 0.f;
	float boids_timer = 0.f;
	//this is for sound
	float current_bg_volume;
	float current_effect_volume;


	GamePhase phase;


	// music references
	Mix_Music* background_music;
	Mix_Chunk* button_sound;
	Mix_Chunk* missile_sound;

	Mix_Chunk* selection_sound;
	
	// https://ttsmp3.com/
	Mix_Chunk* sk_recon_a1;
	Mix_Chunk* sk_recon_d1;
	Mix_Chunk* sk_recon_a2;
	Mix_Chunk* sk_recon_d2;
	Mix_Chunk* sk_attack_a1;
	Mix_Chunk* sk_attack_d1;
	Mix_Chunk* sk_attack_a2;
	Mix_Chunk* sk_attack_d2;
	Mix_Chunk* sk_reg_a1;
	Mix_Chunk* sk_reg_d1;
	Mix_Chunk* sk_reg_a2;
	Mix_Chunk* sk_reg_d2;
	Mix_Chunk* sk_big_a1;
	Mix_Chunk* sk_big_d1;
	Mix_Chunk* sk_big_a2;
	Mix_Chunk* sk_big_d2;

	Mix_Chunk* sk_cd_1;
	Mix_Chunk* sk_cd_2;
	Mix_Chunk* sk_cd_3;

	Mix_Chunk* name_recon;
	Mix_Chunk* name_attack;
	Mix_Chunk* name_reg;
	Mix_Chunk* name_big;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
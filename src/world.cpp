// Header
#include "world.hpp"
#include "debug.hpp"
#include "debugmode.hpp"
#include "pausemode.hpp"
#include "optionmode.hpp"
#include "render_components.hpp"
#include "cutscene.hpp"
#include "boat.hpp"
#include "crate.hpp"
#include "island.hpp"
#include "shark.hpp"
#include "button.hpp"
#include "displaytext.hpp"
#include "selectable.hpp"
#include "motion_spline.hpp"
#include "physics.hpp"
#include "text.hpp"
#include "helpbox.hpp"
#include "story_textbox.hpp"
#include "helptool.hpp"
#include "player.hpp"
#include "level_loader.hpp"
#include "cat.hpp"
#include "missile.hpp"
#include "ai_player.hpp"
#include "endmode.hpp"
#include "skill.hpp"
#include "boids.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>

// Game configuration

//cursor position
vec2 curr;

ivec2 window_size;
int lastTime;
int curr_round;
int curr_num_enemies;

string curr_phase;
string PLANNING = "Planning";
string COMBAT = "Combat";

float health_bar_padding = 30.f;

// Create the world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
WorldSystem::WorldSystem(ivec2 window_size_px) :
	current_scene(4),
	window_dimensions(window_size_px),
	mouse_pos({ 0.f,0.f }),
	cam(window_size_px),
	shift_mod(false),
	ctrl_mod(false),
	exit(false),
	result(false),//set this to false - check if the game has been won or not
	level_selected("1"),
	started_story(false),
	chapters_completed({ false,false,false }),
	current_bg_volume(30),//this is 50
	current_effect_volume(50),
	mode(freeplay),
	phase(planning) // TODO: implement "menu" and "setup" phases if required
{
	window_size = window_size_px;
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

	///////////////////////////////////////
	// Initialize GLFW
	auto glfw_err_callback = [](int error, const char* desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
	glfwSetErrorCallback(glfw_err_callback);
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization, needs to be set before glfwCreateWindow
	// Core Opengl 3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_size_px.x, window_size_px.y, "Neko At-Sea-Me", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to glfwCreateWindow");

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) {
		((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3);
	};
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) {
		((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 });
	};
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) {
		((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2);
	};
	auto scroll_redirect = [](GLFWwindow* wnd, double _0, double _1) {
		((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_scroll({ _0, _1 });
	};
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);
	glfwSetScrollCallback(window, scroll_redirect);

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	curr.x = (float)x;
	curr.y = (float)y;
	// Playing background music indefinitely
	init_audio();
	Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";
}

ivec2 WorldSystem::get_window_size() {
	return window_size;
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

	// Close the window
	glfwDestroyWindow(window);
}

void WorldSystem::init_audio() {
	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("Failed to initialize SDL Audio");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		throw std::runtime_error("Failed to open audio device");

	background_music = Mix_LoadMUS(audio_path("bossa.wav").c_str());
	button_sound = Mix_LoadWAV(audio_path("171521__leszek-szary__button.wav").c_str());
	missile_sound = Mix_LoadWAV(audio_path("51468__smcameron__missile-launch-2.wav").c_str());
	selection_sound = Mix_LoadWAV(audio_path("413310__tieswijnen__select.wav").c_str());

	Mix_VolumeChunk(missile_sound, current_effect_volume / 8.f);
	Mix_VolumeChunk(button_sound, current_effect_volume / 8.f);
	Mix_VolumeChunk(selection_sound, current_effect_volume / 8.f);

	sk_recon_a1 = Mix_LoadWAV(audio_path("sk_recon_a1.wav").c_str());
	sk_recon_d1 = Mix_LoadWAV(audio_path("sk_recon_d1.wav").c_str());
	sk_recon_a2 = Mix_LoadWAV(audio_path("sk_recon_a2.wav").c_str());
	sk_recon_d2 = Mix_LoadWAV(audio_path("sk_recon_d2.wav").c_str());
	sk_attack_a1 = Mix_LoadWAV(audio_path("sk_attack_a1.wav").c_str());
	sk_attack_d1 = Mix_LoadWAV(audio_path("sk_attack_d1.wav").c_str());
	sk_attack_a2 = Mix_LoadWAV(audio_path("sk_attack_a2.wav").c_str());
	sk_attack_d2 = Mix_LoadWAV(audio_path("sk_attack_d2.wav").c_str());
	sk_reg_a1 = Mix_LoadWAV(audio_path("sk_reg_a1.wav").c_str());
	sk_reg_d1 = Mix_LoadWAV(audio_path("sk_reg_d1.wav").c_str());
	sk_reg_a2 = Mix_LoadWAV(audio_path("sk_reg_a2.wav").c_str());
	sk_reg_d2 = Mix_LoadWAV(audio_path("sk_reg_d2.wav").c_str());
	sk_big_a1 = Mix_LoadWAV(audio_path("sk_big_a1.wav").c_str());
	sk_big_d1 = Mix_LoadWAV(audio_path("sk_big_d1.wav").c_str());
	sk_big_a2 = Mix_LoadWAV(audio_path("sk_big_a2.wav").c_str());
	sk_big_d2 = Mix_LoadWAV(audio_path("sk_big_d2.wav").c_str());
	sk_cd_1 = Mix_LoadWAV(audio_path("sk_cd_1.wav").c_str());
	sk_cd_2 = Mix_LoadWAV(audio_path("sk_cd_2.wav").c_str());
	sk_cd_3 = Mix_LoadWAV(audio_path("sk_cd_3.wav").c_str());
	name_recon = Mix_LoadWAV(audio_path("name_recon.wav").c_str());
	name_attack = Mix_LoadWAV(audio_path("name_attack.wav").c_str());
	name_reg = Mix_LoadWAV(audio_path("name_reg.wav").c_str());
	name_big = Mix_LoadWAV(audio_path("name_big.wav").c_str());

	Mix_VolumeMusic(current_bg_volume / 2.f);

	if (background_music == nullptr || button_sound == nullptr ||
		missile_sound == nullptr || selection_sound == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
			audio_path("bossa.wav"));

}

void WorldSystem::step_movement(float elapsed_ms) {
	remaining_action_time -= elapsed_ms;

	//check game end
	auto& player = ECS::registry<Player>.entities[0];
	auto& ai = ECS::registry<AIPlayer>.entities[0];
	auto& fleet = ECS::registry<Fleet>.get(player);
	auto& ai_fleet = ECS::registry<Fleet>.get(ai);
	if (fleet.boats.size() <= 0) {
		result = false;
		if (mode == freeplay) {
			EndModeSystem::in_end_mode_menu = true;
		}
		else if (mode == story) {
			EndModeSystem::in_end_mode_menu = true;
		}

	}
	else if (ai_fleet.boats.size() <= 0) {
		result = true;
		if (mode == freeplay) {
			EndModeSystem::in_end_mode_menu = true;
		}
		else if (mode == story) {
			scene_stack.push(current_scene);
			//go to after battle cutscenes
			if (level_selected == "1s") {
				chapters_completed[0] = true;

			}
			else if (level_selected == "2s") {
				chapters_completed[1] = true;
			}
			else {
				chapters_completed[2] = true;
			}int next_scene = 5;
			scene_update(next_scene);
			if ((ECS::registry<TransitionTimer>.components.size() < 1)) {
				ECS::Entity entity;
				subj_restart_story_listener.notify_all();
				if (!ECS::registry<TransitionTimer>.has(entity)) {
					ECS::registry<TransitionTimer>.emplace(entity);
					auto& t_time = ECS::registry<TransitionTimer>.get(entity);
					t_time.counter_ms = 500;
				}
			}
		}
		return;
	}

	if (remaining_action_time < 0.f) {

		//TODO: erase this when collision is implemented
		//erase all missiles
		/*for (auto entity : ECS::registry<Missile>.entities) {
			ECS::ContainerInterface::remove_all_components_of(entity);
		}*/

		remaining_action_time = action_phase_time;
		phase = planning;
		// std::cout << "Planning" << std::endl;
		subj_change_to_planning.notify_all();
		lastTime = (int)(action_phase_time / 1000.f);
		curr_phase = PLANNING;
		curr_round += 1;

		float padding = 100.f;
		ECS::Entity p_button = Button::createButton({ padding , window_size.y - padding * 0.75f }, "game_pause");
		auto& p_bcom = ECS::registry<ButtonComponent>.get(p_button);
		p_bcom.scale = 0.5f * p_bcom.scale;
		WorldSystem::buttons = {
				Button::createButton({window_size.x - padding * 2.f, window_size.y - padding}, "End Turn"), p_button};
				//,Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };
		WorldSystem::texts = {
				DisplayText::createDisplayText({window_size.x - padding * 2.f, window_size.y - padding}, "end_turn","end_turn.png", 0.7),
				DisplayText::createDisplayText({padding , window_size.y - padding * 0.75f}, "pause_icon","pause_icon.png", 0.9)};
				//,DisplayText::createDisplayText({padding * 2.f, window_size.y - padding}, "back", "back.png", 1) };

		for (auto& boat : ECS::registry<Player>.entities[0].get<Fleet>().boats) {
			auto& path = ECS::registry<MovementPath>.get(boat);
			path.position_coords.clear();

			auto& spline = boat.get<MotionSpline>();
			auto& motion = boat.get<Motion>();
			// spline.points.clear();
			spline.setPoints({ motion.position, motion.position }, true);
		}
	}
	else {
		for (auto& boat : ECS::registry<Boat>.entities) {
			if (ECS::registry<Selectable>.get(boat).selected)
				ECS::registry<Selectable>.get(boat).selected = false;
		}

		for (auto entity : ECS::registry<Button>.entities) {
			ECS::ContainerInterface::remove_all_components_of(entity);
		}
		for (auto entity : ECS::registry<DisplayTextComponent>.entities) {
			ECS::ContainerInterface::remove_all_components_of(entity);
		}

		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		if (curr_phase != COMBAT) {
			curr_phase = COMBAT;
		}

		if (lastTime != (int)(remaining_action_time / 1000.f) + 1) {
			lastTime = (int)(remaining_action_time / 1000.f) + 1;
		}

		bool still_moving = false;

		// Move all entities with MotionSplines towards the next point on those splines
		for (auto entity : ECS::registry<MotionSpline>.entities) {
			auto& spline = entity.get<MotionSpline>();
			if (!spline.isMoving()) continue;
			still_moving = true;

			auto& motion = entity.get<Motion>();
			auto& movement = entity.get<FixedMovement>();

			float step_speed = movement.speed * step_seconds;
			vec2 target_pos = spline.getNextPoint();
			vec2 heading = target_pos - motion.position;

			motion.angle = atan2f(heading.x, heading.y);
			motion.angle = motion.angle * 180 / PI - 180;

			vec2 capped_heading = step_speed * normalize(heading);

			if (length(heading) <= step_speed) {
				motion.position = target_pos;
				spline.advanceTargetPoint();
			}
			else {
				motion.position += capped_heading;
			}

		}

		// update boids
		boids_timer -= elapsed_ms;
		if (boids_timer < 0.f) {
			boids_timer = 200.f;
			Boids::updateBoids(window_size);
		}
		Boids::stepMoveBoids();
	}

	// Timer text
	for (ECS::Entity e : ECS::registry<Text>.entities) {
		ECS::registry<Text>.remove(e);
	}
	curr_num_enemies = ai_fleet.boats.size();

	getTimer();
}

void WorldSystem::step_end(float elapsed_ms) {
	if (EndModeSystem::in_end_mode_menu) {
		ECS::Entity end_mode = DisplayText::createDisplayText({ window_size.x / 2.f, window_size.y / 2.f }, "pause_mode", "pause_screen.png", 2.0);
		ECS::registry<EndModeComponent>.emplace(end_mode);
		ECS::Entity logo = DisplayText::createDisplayText({ window_size.x / 2.f, window_size.y / 4.f - 25.f }, "result_logo", "empty_icon.png", 3.0);
		ECS::registry<EndModeComponent>.emplace(logo);

		ECS::Entity e1;
		ECS::registry<EndModeComponent>.emplace(e1);

		ECS::Entity game_result;
		ECS::registry<EndModeComponent>.emplace(game_result);
		std::vector<ECS::Entity> end_buttons;
		if (WorldSystem::result == true) {
			ECS::registry<Text>.insert(
				game_result,
				Text("WIN...", font, { window_size.x / 2.f - 80.f, window_size.y / 4.f - 10.f }));
		}
		else {
			ECS::registry<Text>.insert(
				game_result,
				Text("LOSE...", font, { window_size.x / 2.f - 80.f, window_size.y / 4.f - 10.f }));
		}
		if (mode == freeplay) {
			end_buttons = { Button::createButton({ window_size.x / 2, window_size.y * 0.25f + 100.f }, "end_again"),//boat selection
													Button::createButton({ window_size.x / 2, window_size.y * 0.50f}, "end_map"),//map selection
													Button::createButton({ window_size.x / 2, window_size.y * 0.75f - 100.f  }, "end_return") };//main

			if (WorldSystem::result == false) {
				ECS::registry<Text>.insert(end_buttons[0], Text("Try Again", font, { window_size.x / 2 - 50.f, window_size.y * 0.25f + 100.f }));
				ECS::registry<Text>.get(end_buttons[0]).scale = 0.5;
			}
			else {
				ECS::registry<Text>.insert(end_buttons[0], Text("Replay", font, { window_size.x / 2 - 50.f, window_size.y * 0.25f + 100.f }));
				ECS::registry<Text>.get(end_buttons[0]).scale = 0.5;
			}
			ECS::registry<Text>.insert(end_buttons[1], Text("Back to", font, { window_size.x / 2 - 50.f, window_size.y * 0.5f - 10.f }));
			ECS::registry<Text>.get(end_buttons[1]).scale = 0.35;

			ECS::registry<Text>.insert(e1, Text("map selection", font, { window_size.x / 2 - 50.f, window_size.y * 0.5f + 10.f }));
			ECS::registry<Text>.get(e1).scale = 0.35;

			ECS::registry<Text>.insert(end_buttons[2], Text("Back to main", font, { window_size.x / 2 - 50.f, window_size.y * 0.75f - 100.f }));
			ECS::registry<Text>.get(end_buttons[2]).scale = 0.35;
		}
		else if (mode == story) {
			end_buttons = { Button::createButton({ window_size.x / 2, window_size.y * 0.25f + 100.f }, "end_map"),//chapter
												   Button::createButton({ window_size.x / 2, window_size.y * 0.50f }, "end_return") };//main

			ECS::registry<Text>.insert(end_buttons[0], Text("Back to", font, { window_size.x / 2 - 50.f, window_size.y * 0.25f + 100.f - 10.f }));
			ECS::registry<Text>.get(end_buttons[0]).scale = 0.35;

			ECS::registry<Text>.insert(e1, Text("ch. selection", font, { window_size.x / 2 - 50.f, window_size.y * 0.25f + 100.f + 10.f }));
			ECS::registry<Text>.get(e1).scale = 0.35;

			ECS::registry<Text>.insert(end_buttons[1], Text("Back to main", font, { window_size.x / 2 - 50.f, window_size.y * 0.5f }));
			ECS::registry<Text>.get(end_buttons[1]).scale = 0.35;

		}
		for (int i = 0; i < end_buttons.size(); i++) {
			ECS::registry<EndModeComponent>.emplace(end_buttons[i]);
			auto& bcom = ECS::registry<ButtonComponent>.get(end_buttons[i]);
			bcom.scale = 0.9f * bcom.scale;
		}

	}
}

void WorldSystem::step_pause(float elapsed_ms) {
	if (PauseModeSystem::in_pause_mode_menu) {
		ECS::Entity pause_mode = DisplayText::createDisplayText({ window_size.x / 2.f, window_size.y / 2.f }, "pause_mode", "pause_screen.png", 3);
		ECS::Entity pause_logo = DisplayText::createDisplayText({ window_size.x / 2, window_size.y * 0.25f - 50 }, "pause_logo", "pause_logo.png", 1);

		ECS::Entity e1;

		ECS::registry<PauseModeComponent>.emplace(pause_mode);
		ECS::registry<PauseModeComponent>.emplace(pause_logo);
		std::vector<ECS::Entity> pause_buttons = { Button::createButton({ window_size.x / 2, window_size.y * 0.25f + 100.f }, "pause_resume"),
													Button::createButton({ window_size.x / 2, window_size.y * 0.50f}, "pause_option"),
													Button::createButton({ window_size.x / 2, window_size.y * 0.75f - 100.f }, "pause_selection"),
													Button::createButton({ window_size.x / 2, window_size.y - 200.f }, "pause_return"), e1 };
		for (int i = 0; i < pause_buttons.size(); i++) {
			ECS::registry<PauseModeComponent>.emplace(pause_buttons[i]);
		}

		ECS::registry<Text>.insert(pause_buttons[0], Text("Resume", font, { window_size.x / 2 - 60.f, window_size.y * 0.25f + 100.f }));
		ECS::registry<Text>.get(pause_buttons[0]).scale = 0.6;


		ECS::registry<Text>.insert(pause_buttons[1], Text("Options", font, { window_size.x / 2 - 60.f, window_size.y * 0.5f }));
		ECS::registry<Text>.get(pause_buttons[1]).scale = 0.6;
		ECS::registry<Text>.insert(pause_buttons[2], Text("Back to ", font, { window_size.x / 2 - 60.f, window_size.y * 0.75f - 100.f }));
		if (mode == story) {
			ECS::registry<Text>.insert(e1, Text("ch. selection", font, { window_size.x / 2 - 60.f, window_size.y * 0.75f - 80.f }));
		}
		else {
			ECS::registry<Text>.insert(e1, Text("map selection", font, { window_size.x / 2 - 60.f, window_size.y * 0.75f - 80.f }));
		}
		ECS::registry<Text>.get(e1).scale = 0.4;
		ECS::registry<Text>.get(pause_buttons[2]).scale = 0.4;

		ECS::registry<Text>.insert(pause_buttons[3], Text("Back to main ", font, { window_size.x / 2 - 60.f, window_size.y - 200.f }));
		ECS::registry<Text>.get(pause_buttons[3]).scale = 0.4;
	}
}

void WorldSystem::step_option(float elapsed_ms) {
	if (OptionModeSystem::in_option_mode_menu) {
		float padding = 100.f;
		ECS::Entity option_mode = DisplayText::createDisplayText({ window_size.x / 2.f, window_size.y / 2.f }, "option_mode", "pause_screen.png", 3);
		ECS::Entity option_logo = DisplayText::createDisplayText({ window_size.x / 2, window_size.y * 0.25f - 50 }, "option_logo", "option_logo.png", 1);
		ECS::registry<OptionModeComponent>.emplace(option_mode);
		ECS::registry<OptionModeComponent>.emplace(option_logo);

		WorldSystem::option_buttons = {
			// Fleet selection buttons left arrow
			Button::createButton({window_size.x / 2.f - padding * 2.f,300 + 0.5f * padding }, "volume_left_0"),
			Button::createButton({window_size.x / 2.f - padding * 2.f, 450 + padding}, "volume_left_1"),
			// Fleet selection middle number
			Button::createButton({window_size.x / 2.f, 300 + 0.5f * padding}, "background_music"),
			Button::createButton({window_size.x / 2.f,  450 + padding}, "effect_sound"),
			// Fleet selection buttons right number
			Button::createButton({window_size.x / 2.f + padding * 2.f, 300 + 0.5f * padding}, "volume_right_0"),
			Button::createButton({window_size.x / 2.f + padding * 2.f,  450 + padding}, "volume_right_1"),
			Button::createButton({window_size.x / 2.f,  650 + padding}, "exit_options")
		};
		std::vector<ECS::Entity> option_texts = {
			// Fleet selection texts left arrow
			DisplayText::createDisplayText({window_size.x / 2.f - padding * 2.f,300 + 0.5f * padding}, "volume_left_0", "left_arrow.png", 1),
			DisplayText::createDisplayText({window_size.x / 2.f - padding * 2.f,  450 + padding}, "volume_left_1", "left_arrow.png", 1),
			// Fleet selection texts right arrow
			DisplayText::createDisplayText({window_size.x / 2.f + padding * 2.f,300 + 0.5f * padding}, "volume_right_0", "right_arrow.png", 1),
			DisplayText::createDisplayText({window_size.x / 2.f + padding * 2.f,  450 + padding}, "volume_right_1", "right_arrow.png", 1),
		};
		for (int i = 0; i < option_buttons.size(); i++) {
			ECS::registry<OptionModeComponent>.emplace(option_buttons[i]);
		}

		for (int j = 0; j < option_texts.size(); j++) {
			ECS::registry<OptionModeComponent>.emplace(option_texts[j]);
		}


		// Button text 
		std::string bg_sound = std::to_string((int)(current_bg_volume));
		std::string effect_sound = std::to_string((int)(current_effect_volume));

		ECS::registry<Text>.insert(option_buttons[2], Text(bg_sound, font, { window_size.x / 2.f, 300 + 0.5f * padding })),
			ECS::registry<Text>.get(option_buttons[2]).scale = 0.6;
		ECS::registry<Text>.insert(option_buttons[3], Text(effect_sound, font, { window_size.x / 2.f,  450 + padding })),
			ECS::registry<Text>.get(option_buttons[3]).scale = 0.6;
		ECS::registry<Text>.insert(option_buttons[6], Text("Close", font, { window_size.x / 2.f - 0.5 * padding,  650 + padding })),
			ECS::registry<Text>.get(option_buttons[6]).scale = 0.6;

		ECS::Entity textEntity0;
		ECS::registry<OptionModeComponent>.emplace(textEntity0);
		ECS::Entity textEntity1;
		ECS::registry<OptionModeComponent>.emplace(textEntity1);

		ButtonComponent& dtcomp = ECS::registry<ButtonComponent>.get(option_buttons[0]);
		float x_padding = abs(dtcomp.scale.x) / 2.f;

		ECS::registry<Text>.insert(textEntity0, Text("Background Music Volume", font, { window_size.x / 2.f - padding * 2.f - x_padding, 350 - 0.75f * padding }));
		ECS::registry<Text>.get(textEntity0).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity0);
		ECS::registry<Text>.insert(textEntity1, Text("Sound Effect Volume", font, { window_size.x / 2.f - padding * 2.f - x_padding,  550 - 0.75f * padding }));
		ECS::registry<Text>.get(textEntity1).scale = 0.5;
	}
}

void WorldSystem::step_debug(float elapsed_ms) {
	if (DebugModeSystem::in_debug_mode_menu)
	{
		ECS::Entity debug_mode = DisplayText::createDisplayText({ window_size.x / 2.f, window_size.y / 2.f }, "debug_mode", "debug_mode.png", 3);
		ECS::registry<DebugModeComponent>.emplace(debug_mode);
		auto& dtcomponent = ECS::registry<DisplayTextComponent>.get(debug_mode);
		vec2 scale = { abs(dtcomponent.scale.x),abs(dtcomponent.scale.y) };

		ECS::Entity textEntity0;
		ECS::Entity textEntity1;
		ECS::Entity textEntity2;
		ECS::Entity textEntity3;
		ECS::Entity textEntity4;
		ECS::Entity textEntity5;
		ECS::Entity textEntity6;
		ECS::Entity textEntity7;
		ECS::Entity textEntity8;

		float padding = 50.f;

		ECS::registry<Text>.insert(textEntity0, Text("Hold Shift - Activate Debug Menu", font, { window_size.x / 4.f, window_size.y / 4.f }));
		ECS::registry<Text>.get(textEntity0).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity0);
		ECS::registry<Text>.insert(textEntity1, Text("Shift + S - Set all Tutorial as skipped", font, { window_size.x / 4.f, window_size.y / 4.f + padding }));
		ECS::registry<Text>.get(textEntity1).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity1);
		ECS::registry<Text>.insert(textEntity2, Text("Shift + A - Restart all Tutorial", font, { window_size.x / 4.f, window_size.y / 4.f + 2 * padding }));
		ECS::registry<Text>.get(textEntity2).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity2);
		ECS::registry<Text>.insert(textEntity3, Text("Shift + Scene Number - Go to Scene Number", font, { window_size.x / 4.f, window_size.y / 4.f + 3 * padding }));
		ECS::registry<Text>.get(textEntity3).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity3);
		ECS::registry<Text>.insert(textEntity4, Text("( 0 - Title | 1 - Boat Selection | 2 - Game | 3 - Level Selection | 4 - Opening | 5 - cutscene)", font, { window_size.x / 4.f, window_size.y / 4.f + 4 * padding }));
		ECS::registry<Text>.get(textEntity4).scale = 0.35;
		ECS::registry<DebugModeComponent>.emplace(textEntity4);
		ECS::registry<Text>.insert(textEntity5, Text("Hold D - Debugging bounding boxes (Only in Game)", font, { window_size.x / 4.f, window_size.y / 4.f + 5 * padding }));
		ECS::registry<Text>.get(textEntity5).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity5);
		ECS::registry<Text>.insert(textEntity6, Text("Shift + W - auto win battle (Only in Game)", font, { window_size.x / 4.f, window_size.y / 4.f + 6 * padding }));
		ECS::registry<Text>.get(textEntity6).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity6);
		ECS::registry<Text>.insert(textEntity7, Text("Shift + L - auto lose battle (Only in Game)", font, { window_size.x / 4.f, window_size.y / 4.f + 7 * padding }));
		ECS::registry<Text>.get(textEntity7).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity7);
		ECS::registry<Text>.insert(textEntity8, Text("Shirt + T - unlock all chapters (Only in Chapter select)", font, { window_size.x / 4.f, window_size.y / 4.f + 8 * padding }));
		ECS::registry<Text>.get(textEntity8).scale = 0.5;
		ECS::registry<DebugModeComponent>.emplace(textEntity8);
	}
}

void WorldSystem::combat_resolver(ECS::Entity attacker, ECS::Entity defender) {

	auto& attacker_motion = ECS::registry<Motion>.get(attacker);
	auto& attacker_status = ECS::registry<Status>.get(attacker);
	auto& defender_motion = ECS::registry<Motion>.get(defender);
	auto& defender_status = ECS::registry<Status>.get(defender);

	auto& attacker_position = attacker_motion.position;
	auto& attacker_damage = attacker_status.weapon_damage;
	auto& defender_position = defender_motion.position;

	if (ECS::registry<Shark>.has(attacker)) { // shark does not fire missile
		defender_status.health -= attacker_damage;
	}
	else { // only boat does
		float rnd_x = uniform_dist(rng) * 100.f - 50.f;
		float rnd_y = uniform_dist(rng) * 100.f - 50.f;
		ECS::Entity missile = Missile::createMissile(attacker_position + vec2{ rnd_x, rnd_y }, attacker_damage, defender);
		// std::cout << "Attacker: " << attacker_status.name << " Target: " << defender_status.name << std::endl;
		auto& missile_spline = ECS::registry<MotionSpline>.get(missile);
		missile_spline.setPoints({ attacker_position, defender_position }, true);
		Mix_PlayChannel(-1, missile_sound, 0);
	}
}

void WorldSystem::tick_combat_helper(ECS::Entity cat1, ECS::Entity cat2) {
	auto& cat1_motion = ECS::registry<Motion>.get(cat1);
	auto& cat1_status = ECS::registry<Status>.get(cat1);
	auto& cat2_motion = ECS::registry<Motion>.get(cat2);
	auto& cat2_status = ECS::registry<Status>.get(cat2);
	auto dist = distance(cat1_motion.position, cat2_motion.position);
	if (dist <= cat1_status.attack_range) {
		combat_resolver(cat1, cat2);
	}
	if (dist <= cat2_status.attack_range)
	{
		combat_resolver(cat2, cat1);
	}
}

void WorldSystem::tick_combat(float elapsed_ms) {
	combat_timer -= elapsed_ms;
	if (combat_timer < 0.f) {
		combat_timer = combat_cd; // reset combat timer
		// start to find potential enemy in the range for each boat and shark
		auto& player = ECS::registry<Player>.entities[0];
		auto& ai_player = ECS::registry<AIPlayer>.entities[0];
		auto& fleet = ECS::registry<Fleet>.get(player);
		auto& ai_fleet = ECS::registry<Fleet>.get(ai_player);
		auto& sharks = ECS::registry<Shark>.entities;
		for (auto boat : fleet.boats) {
			for (auto ai_boat : ai_fleet.boats) {
				tick_combat_helper(boat, ai_boat); // bi-directional
			}
		}
		for (auto boat : fleet.boats) {
			for (auto shark : sharks) {
				tick_combat_helper(boat, shark);
			}
		}
		for (auto ai_boat : ai_fleet.boats) {
			for (auto shark : sharks) {
				tick_combat_helper(ai_boat, shark);
			}
		}
		// good place for combat debug print
		/*for (auto& missile_comp : ECS::registry<MissileComponent>.components) {
			std::cout << "Missile damage is " << missile_comp.damage << " and the target is " << missile_comp.target.get<Status>().name << std::endl;
		}*/
		// deal with boids if there's any
		if (ECS::registry<Boids>.entities.size() > 0) {
			// find the center of mass
			vec2 center = { 0.f, 0.f };
			for (auto boid : ECS::registry<Boids>.entities) {
				center += boid.get<Motion>().position;
			}
			center /= ECS::registry<Boids>.entities.size();
			if (ECS::registry<Missile>.entities.size() > 0) {
				auto boids_size = ECS::registry<Boids>.entities.size();
				auto damage_ms = boids_size * 100.f; // min 1; max 50;
				auto range = boids_size * 10.f; // min 10; max 500;
				int lost = combat_cd / 1000; // -1 naturally every second
				// deal time damage if missile is close enough to the center of boids
				for (auto missile : ECS::registry<Missile>.entities) {
					auto& missile_motion = missile.get<Motion>();
					if (distance(center, missile_motion.position) < range) {
						missile.get<MissileComponent>().remaining_ms -= damage_ms;
						lost++; // if hit a missile -1
					}
				}
				// deal damage to boids, lost num may be larger than the boids size
				if (lost > ECS::registry<Boids>.entities.size()) {
					Boids::clearBoids();
				}
				else {
					for (int i = 0; i < lost; i++) {
						ECS::ContainerInterface::remove_all_components_of(ECS::registry<Boids>.entities[0]);
					}
				}
			}
		}
	}
}

void WorldSystem::guide_missile(float elapsed_ms)
{
	auto& missiles = ECS::registry<Missile>.entities;
	for (auto& missile : missiles) {
		auto& missile_comp = missile.get<MissileComponent>();
		auto& target = missile_comp.target;
		bool target_alive = ECS::registry<Boat>.has(target) || ECS::registry<Shark>.has(target);
		missile_comp.remaining_ms -= elapsed_ms;
		if (missile_comp.remaining_ms < 0.f || !target_alive) {
			// aoe damage or check by collision?
			ECS::ContainerInterface::remove_all_components_of(missile); // self-destruct
		}
		else {
			auto& missile_motion = missile.get<Motion>();
			auto& target_motion = target.get<Motion>(); // this will not crash since target_alive is true
			auto dist = distance(missile_motion.position, target_motion.position);
			if (dist <= 50.f) {
				target.get<Status>().health -= missile_comp.damage; // this will also not crash since target_alive is true
				ECS::ContainerInterface::remove_all_components_of(missile); // self-destruct
			}
			else {
				auto& missile_motionspline = missile.get<MotionSpline>();
				missile_motionspline.setPoints({ missile_motion.position, target_motion.position }, true);
			}

		}
	}
}

void WorldSystem::death_check(float elapsed_ms) {
	for (ECS::Entity e : ECS::registry<Status>.entities) { // check boat and shark
		auto& status = ECS::registry<Status>.get(e);
		bool isBoat = ECS::registry<Boat>.has(e);
		bool isShark = ECS::registry<Shark>.has(e);
		if (status.health <= 0 && (isBoat || isShark)) {
			// notify the other system ?

			// check if it's a boat or a shark, boat is in a vector, shark can be removed directly
			// if a boat
			if (isBoat) { // e is a boat entity
				auto& e_fleets = ECS::registry<Fleet>.entities; // don't know which fleets it belongs to, but do have player_number
				for (auto& e_fleet : e_fleets) {
					auto& fleet = ECS::registry<Fleet>.get(e_fleet);
					if (fleet.player_number == status.player_number) { // found the fleet the boat belongs to
						auto& vec_boats = fleet.boats; // found the vector the boat belongs to
						for (int i = 0; i < vec_boats.size(); i++) { // since vector is dynamic and we can't directly compare entity
							if (vec_boats.at(i).id == e.id) { // have to check the entity id
								vec_boats.erase(vec_boats.begin() + i);
								// https://stackoverflow.com/questions/1257744
								goto safe_break; // since e.id is unique, we can safely break the whole loop
							}
						}
					}
				}

			}
		safe_break: // both boat and shark will goto here, boat needs another step to be removed from vector
			// destroy
			ECS::ContainerInterface::remove_all_components_of(e);
		}
	}

	for (ECS::Entity e : ECS::registry<Crate>.entities) {
		if (e.get<CrateComponent>().isUsed)
			ECS::ContainerInterface::remove_all_components_of(e);
	}
}

void WorldSystem::clear_scene_stack() {
	while (!scene_stack.empty())
		scene_stack.pop();
}

void remove_help() {
	if (ECS::registry<Helptool>.components.size() > 0) {
		for (auto entity : ECS::registry<Helptool>.entities) {
			ECS::ContainerInterface::remove_all_components_of(entity);
		}
		for (auto entity : ECS::registry<Text>.entities) {
			if (ECS::registry<TextComponent>.has(entity)) {
				std::string text = ECS::registry<TextComponent>.get(entity).text;
				if (text == "help_tool") {
					ECS::ContainerInterface::remove_all_components_of(entity);
				}
			}
		}
	}
}

// Update our game world
void WorldSystem::step(float elapsed_ms, vec2 window_size_in_game_units) {
	// Processing the enter click
	assert(ECS::registry<ScreenState>.components.size() <= 1);
	auto& screen = ECS::registry<ScreenState>.components[0];

	if ((current_scene != 2) && PauseModeSystem::in_pause_mode_menu) {
		PauseModeSystem::in_pause_mode_menu = false;
	}

	if (current_scene == 0) {
		clear_scene_stack();
	}

	// Camera handling
	set_mouse_pos();
	shift_at_boundry();

	step_debug(elapsed_ms);
	step_pause(elapsed_ms);
	step_option(elapsed_ms);
	//if the game ends
	step_end(elapsed_ms);

	if (current_scene == 4) {
		if (Cutscene::font == NULL) {
			Cutscene::font = Font::load("data/fonts/Noto/NotoSans-Regular.ttf");
		}
		Cutscene::get_cutscene_word(elapsed_ms, window_size_in_game_units);
	}

	//Checking entities for TransitionTimer
	for (auto entity : ECS::registry<TransitionTimer>.entities) {
		// Progress timer
		auto& counter = ECS::registry<TransitionTimer>.get(entity);
		counter.counter_ms -= elapsed_ms;

		// Reduce window brightness when transitioning
		screen.darken_screen_factor = 1 - counter.counter_ms / 3000.f;

		// Restart the game once the transition timer expired
		if (counter.counter_ms < 0) {
			if (current_scene == 4) {
				scene_update(0);
			}
			ECS::registry<TransitionTimer>.remove(entity);
			screen.darken_screen_factor = 0;
			restart();
			return;
		}
	}
	//Checking entities for AchievementTimer
	for (auto entity : ECS::registry<AchievementTimer>.entities) {
		// Progress timer
		auto& counter = ECS::registry<AchievementTimer>.get(entity);
		counter.counter_ms -= elapsed_ms;

		// Restart the game once the achievement timer expired
		if (counter.counter_ms < 0) {
			ECS::registry<AchievementTimer>.remove(entity);
			ECS::registry<Text>.remove(entity);//then remove the text entity

		}
	}

	//Sprite animation
	for (auto cat_entity : ECS::registry<Cat>.entities)
	{
		auto& catComponent = ECS::registry<CatComponent>.get(cat_entity);
		catComponent.elapsedTime += elapsed_ms;
		if (catComponent.elapsedTime > catComponent.animationSpeed) {
			// TEST multi row animations for all cats, comment when done, this does not affect sprite sheets with 1 row
			//if (fmod((catComponent.frame + 1.0), catComponent.frames) == 0) {
				//catComponent.row = fmod((catComponent.row + 1.0), catComponent.rows);
			catComponent.texture_y_Position_UV = catComponent.row * catComponent.animation_frame_height;
			//}
			// -----------------------------------------------------------------------------------------

			// Change to the next frame in the same row
			catComponent.frame = fmod((catComponent.frame + 1.0), catComponent.frames);
			catComponent.texture_x_Position_UV = catComponent.frame * catComponent.animation_frame_width;

			// Get texmesh and send uniform
			auto& texmesh = *ECS::registry<ShadedMeshRef>.get(cat_entity).reference_to_cache;
			glUseProgram(texmesh.effect.program);
			GLint in_sprite_texcoord_loc = glGetUniformLocation(texmesh.effect.program, "texOffset");
			glUniform2f(in_sprite_texcoord_loc, catComponent.texture_x_Position_UV, catComponent.texture_y_Position_UV);

			catComponent.elapsedTime = 0;

			//std::cout << catComponent.frame << "\n";
			//std::cout << catComponent.texture_x_Position_UV << "\n";
		}
	}

	//Checking entities for warning timer
	for (auto entity : ECS::registry<WarningTimer>.entities)
	{
		// Progress timer
		auto& counter = ECS::registry<WarningTimer>.get(entity);
		counter.counter_ms -= elapsed_ms;

		// Restart the game once the achievement timer expired
		if (counter.counter_ms < 0)
		{
			ECS::registry<WarningTimer>.remove(entity);
			ECS::registry<Text>.remove(entity);//then remove the text entity

		}
	}

	//Checking entities for skill UI removeable timer
	for (auto entity : ECS::registry<SkillUIRemoveTimerComp>.entities)
	{
		auto& counter = ECS::registry<SkillUIRemoveTimerComp>.get(entity).timer;
		counter -= elapsed_ms;

		// Restart the game once the achievement timer expired
		if (counter < 0)
		{
			ECS::ContainerInterface::remove_all_components_of(entity);
		}
	}

	// Drawing attack range indicators
	for (auto& selected : ECS::registry<Selectable>.entities)
	{
		if (!ECS::registry<Selectable>.get(selected).selected)
			continue;
		else
		{
			float range = ECS::registry<Status>.get(selected).attack_range - 1.f;
			auto& motion = ECS::registry<Motion>.get(selected);

			for (int theta = 0; theta <= 360; theta += 5)
			{
				float rad = (PI / 180.f) * theta;
				float x = range * sin(rad);
				float y = range * cos(rad);
				vec2 pos = (motion.position + vec2(x, y));
				DebugSystem::createLine(pos, vec2(5, 5));
			}
		}
	}

	// Drawing view range indicators
	for (auto& selected : ECS::registry<Selectable>.entities)
	{
		if (!ECS::registry<Selectable>.get(selected).selected)
			continue;
		else
		{
			float range = ECS::registry<Status>.get(selected).view_range - 1.f; // slightly smaller to avoid any xyz-fighting
			auto& motion = ECS::registry<Motion>.get(selected);

			for (int theta = 2; theta <= 360; theta += 5)
			{
				float rad = (PI / 180.f) * theta;
				float x = range * sin(rad);
				float y = range * cos(rad);
				vec2 pos = (motion.position + vec2(x, y));
				DebugSystem::createColoredLine(pos, vec2(5, 5), vec3{ 1.0, 1.0, 0.0 });
			}
		}
	}


	if (phase != action) {
		// no motion if we're not in the action phase
		return;
	}
	else {
		remove_help();
	}

	if (!PauseModeSystem::in_pause_mode_menu) {
		death_check(elapsed_ms);
		step_movement(elapsed_ms);
		tick_combat(elapsed_ms);
		guide_missile(elapsed_ms);
	}

}

void clearHelptool() {
	for (auto entity : ECS::registry<Text>.entities) {
		if (ECS::registry<TextComponent>.has(entity)) {
			std::string text = ECS::registry<TextComponent>.get(entity).text;
			if (text == "help_tool") {
				ECS::ContainerInterface::remove_all_components_of(entity);
			}
		}
	}
}

// Reset the world state to its initial state
void WorldSystem::restart() {
	std::cout << "Restarting\n";
	curr_round = 1;

	font = Font::load("data/fonts/Noto/NotoSans-Regular.ttf");//placeholder text until we figure out what we want

	// Remove all entities that we created
	while (!ECS::registry<Motion>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());
	}

	while (!ECS::registry<Text>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<Text>.entities.back());
	}

	if (current_scene != 2) {
		while (!ECS::registry<Player>.entities.empty()) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<Player>.entities.back());
		}
		while (!ECS::registry<AIPlayer>.entities.empty()) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<AIPlayer>.entities.back());
		}
	}

	while (!ECS::registry<ButtonComponent>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<ButtonComponent>.entities.back());
	}

	while (!ECS::registry<HelpComponent>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<HelpComponent>.entities.back());
	}

	while (!ECS::registry<StoryComponent>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<StoryComponent>.entities.back());
	}

	while (!ECS::registry<DisplayTextComponent>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<DisplayTextComponent>.entities.back());
	}

	while (!ECS::registry<CatComponent>.entities.empty()) {
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<CatComponent>.entities.back());
	}


	// reset phase and timer
	remaining_action_time = action_phase_time;
	phase = planning;
	//BOATS!
	//cutscene scene 
	//TODO
	if (current_scene == 5) {
		int padding = 200.f;
		ECS::Entity bg = DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "b_background", "black_bg.png", 4.0);
		WorldSystem::buttons = { Button::createButton({ window_size.x - padding, window_size.y / 2 - 2.f * padding }, "skip_story") };//this will have to depend on progress
		ECS::registry<Text>.insert(buttons[0], Text("Skip", font, { window_size.x - padding * 1.130, window_size.y / 2 - 2.f * padding }));
		ECS::registry<Text>.get(buttons[0]).scale = 0.7;


		ECS::Entity e;
		StoryLevel sl;
		sl.level = level_selected;
		sl.after_battle = result;
		ECS::registry<StoryLevel>.emplace(e, sl);
		subj_restart_story_listener.notify_all();
		subj_story_listener.notify_all_entity(e);
		ECS::ContainerInterface::remove_all_components_of(e);
		clearHelptool();

	}

	//story scene
	if (current_scene == 4) {
		int padding = 200.f;
		ECS::Entity bg = DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "b_bg", "black_bg.png", 4.0);
		WorldSystem::buttons = { Button::createButton({ window_size.x - padding, window_size.y / 2 - 2.f * padding }, "skip_cutscene") };
		ECS::registry<Text>.insert(buttons[0], Text("Skip", font, { window_size.x - padding * 1.130, window_size.y / 2 - 2.f * padding }));
		ECS::registry<Text>.get(buttons[0]).scale = 0.7;

		CutsceneComponent& cs = ECS::registry<CutsceneComponent>.emplace(bg);
		cs.counter_ms = 16000;//10 seconds;
		ECS::Entity e;
		Scene s;
		s.scene = current_scene;
		ECS::registry<Scene>.emplace(e, s);
		subj_help_listener.notify_all_entity(e);
		clearHelptool();
	}

	//this is the level selection scene
	if (current_scene == 3)
	{
		if (mode == freeplay) {
			float padding = 100.f;
			float text_padding = 60.f;
			DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "l_background", "selection_bg.png", 3.0);
			WorldSystem::buttons = { Button::createButton({window_size.x * 0.25f, window_size.y / 2}, "map_1"),//keep this to go to next scene
									Button::createButton({window_size.x / 2.f, window_size.y / 2}, "map_2"),
									Button::createButton({window_size.x * 0.75f, window_size.y / 2 }, "map_3"),
									Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };
			WorldSystem::texts = { DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 - (padding * 1.5)}, "map_title", "map_selection.png", 1),
									DisplayText::createDisplayText({padding * 2.f, window_size.y - padding}, "back", "back.png", 1) };

			ECS::registry<Text>.insert(buttons[0], Text("Silent Waters", font, { window_size.x * 0.25f - text_padding, window_size.y / 2 }));
			ECS::registry<Text>.get(buttons[0]).scale = 0.4;


			ECS::registry<Text>.insert(buttons[1], Text("Distant Waves", font, { window_size.x * 0.50f - text_padding, window_size.y / 2 }));
			ECS::registry<Text>.get(buttons[1]).scale = 0.4;

			ECS::registry<Text>.insert(buttons[2], Text("Rising Depths", font, { window_size.x * 0.75f - text_padding, window_size.y / 2 }));
			ECS::registry<Text>.get(buttons[2]).scale = 0.4;

			int random_num = rand() % 4 + 1;

			if (random_num == 3) {
				cats = { Cat::createCat({ window_size.x / 4, 3 * window_size.y / 4}, 6.0, 4.0,"robot_cat_expression.png","robo_map") };
				ECS::registry<Text>.insert(cats[0], Text("BEEP BOP...I WILL FIND THE ONE PUSS, BUT FIRST I MUST DESTROY YOU!!!!", font, { window_size.x / 4 + padding, 3 * window_size.y / 4 - padding / 2.f }));
				ECS::registry<Text>.get(cats[0]).scale = 0.4;
			}
			else if (random_num == 2) {
				cats = { Cat::createCat({ window_size.x / 4, 3 * window_size.y / 4}, 6.0, 4.0,"business_cat_expression.png","business_map") };
				ECS::registry<Text>.insert(cats[0], Text("I do say... the treasure will be exquisite but first I need to eliminate my target", font, { window_size.x / 4 + padding, 3 * window_size.y / 4 - padding / 2.f }));
				ECS::registry<Text>.get(cats[0]).scale = 0.4;
			}
			else if (random_num == 4) {
				cats = { Cat::createCat({ window_size.x / 4, 3 * window_size.y / 4}, 6.0, 4.0,"princess_cat_expression.png","princess_map") };
				ECS::registry<Text>.insert(cats[0], Text("I'm a purrate and a princess... THE TREASURE WILL BE MINE!!!", font, { window_size.x / 4 + padding, 3 * window_size.y / 4 - padding / 2.f }));
				ECS::registry<Text>.get(cats[0]).scale = 0.4;

			}
			else {
				cats = { Cat::createCat({ window_size.x / 4, 3 * window_size.y / 4}, 6.0, 4.0, "sailor_cat_expression.png","sailor_map") };
				ECS::registry<Text>.insert(cats[0], Text("No matter what I will be the one to find the one puss... I hope", font, { window_size.x / 4 + padding, 3 * window_size.y / 4 - padding / 2.f }));
				ECS::registry<Text>.get(cats[0]).scale = 0.4;

			}

			ECS::Entity e;
			Scene s;
			s.scene = current_scene;
			ECS::registry<Scene>.emplace(e, s);
			subj_help_listener.notify_all_entity(e);
			clearHelptool();
		}

		if (mode == story) {
			float padding = 100.f;
			float d_padding = 19.f;
			DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "l_background", "selection_bg.png", 3.0);
			WorldSystem::buttons = { Button::createButton({(2.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_1s"),//keep this to go to next scene
									Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };
			if (chapters_completed[0]) {
				WorldSystem::buttons = { Button::createButton({(2.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_1s"),//keep this to go to next scene
			Button::createButton({(3.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_2s"),
			Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };

			}
			if (chapters_completed[1]) {
				WorldSystem::buttons = { Button::createButton({(2.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_1s"),//keep this to go to next scene
						Button::createButton({(3.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_2s"),
						Button::createButton({(4.f * window_size.x) / 6.f - padding, window_size.y / 2 }, "story_3s"),
						Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };

			}
			if (chapters_completed[2]) {
				WorldSystem::buttons = { Button::createButton({(2.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_1s"),//keep this to go to next scene
						Button::createButton({(3.f * window_size.x) / 6.f - padding, window_size.y / 2}, "story_2s"),
			Button::createButton({(4.f * window_size.x) / 6.f - padding, window_size.y / 2 }, "story_3s"),
			Button::createButton({(5.f * window_size.x) / 6.f - padding, window_size.y / 2 }, "story_4s"),
						Button::createButton({padding * 2.f, window_size.y - padding}, "Back") };

			}


			ECS::registry<Text>.insert(buttons[0], Text("Chapter 1", font, { (2.f * window_size.x) / 6.f - 1.5f * padding, window_size.y / 2 }));
			ECS::registry<Text>.get(buttons[0]).scale = 0.5;

			if (chapters_completed[0]) {
				ECS::registry<Text>.insert(buttons[1], Text("Chapter 2", font, { (3.f * window_size.x) / 6.f - 1.5f * padding, window_size.y / 2 }));
				ECS::registry<Text>.get(buttons[1]).scale = 0.5;
			}

			if (chapters_completed[1]) {
				ECS::registry<Text>.insert(buttons[2], Text("Chapter 3", font, { (4.f * window_size.x) / 6.f - 1.5f * padding, window_size.y / 2 }));
				ECS::registry<Text>.get(buttons[2]).scale = 0.5;
			}

			if (chapters_completed[2]) {
				ECS::registry<Text>.insert(buttons[3], Text("Chapter 4", font, { (5.f * window_size.x) / 6.f - 1.5f * padding, window_size.y / 2 }));
				ECS::registry<Text>.get(buttons[3]).scale = 0.5;
			}

			WorldSystem::texts = { DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 - (padding * 1.5)}, "story_title", "story_logo.png", 1),
						DisplayText::createDisplayText({padding * 2.f, window_size.y - padding}, "back", "back.png", 1)
				/*,DisplayText::createDisplayText({ (4.f * window_size.x) / 6.f - padding + d_padding , window_size.y / 2 + d_padding }, "dne_1", "unavailable_button.png", 1),
				DisplayText::createDisplayText({ (5.f * window_size.x) / 6.f - padding + d_padding , window_size.y / 2 + d_padding }, "dne_2", "unavailable_button.png", 1)*/ };

				//commented out until story is completed
				/*ECS::Entity e;
				Scene s;
				s.scene = current_scene;
				ECS::registry<Scene>.emplace(e, s);
				subj_help_listener.notify_all_entity(e);
				clearHelptool();*/

		}

	}

	if (current_scene == 1) {
		float padding = 100.f;
		DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "s_background", "selection_bg.png", 3.0);
		ECS::Entity player1 = Player::createPlayer(1);
		ECS::Entity ai_player1 = AIPlayer::createAIPlayer(-1);

		WorldSystem::buttons = {
			// Start and back buttons
			Button::createButton({window_size.x - padding * 2.f, window_size.y - padding}, "Start"),
			Button::createButton({padding * 2.f, window_size.y - padding}, "Back"),
			// Tokens

				// Fleet selection buttons left arrow
				Button::createButton({950, 250}, "allocation_left_0"),
				Button::createButton({950, 400}, "allocation_left_1"),
				Button::createButton({950, 550}, "allocation_left_2"),
				Button::createButton({950, 700}, "allocation_left_3"),
				// Fleet selection middle number
				Button::createButton({window_size.x + 300, window_size.y + 300}, "tokens"),//keeping this fir changing the number but out of scene
				Button::createButton({1100, 250}, "regular boats"),
				Button::createButton({1100, 400}, "big boats"),
				Button::createButton({1100, 550}, "recon boats"),
				Button::createButton({1100, 700}, "attack boats"),
				// Fleet selection buttons right number
				Button::createButton({1250, 250}, "allocation_right_0"),
				Button::createButton({1250, 400}, "allocation_right_1"),
				Button::createButton({1250, 550}, "allocation_right_2"),
				Button::createButton({1250, 700}, "allocation_right_3"),
				// right and left on character
				Button::createButton({300, 500}, "chara_left"),
				Button::createButton({500, 500}, "chara_right")
		};
		WorldSystem::texts = {
				DisplayText::createDisplayText({window_size.x - padding * 2.f, window_size.y - padding}, "start",
											   "start.png", 1),
				DisplayText::createDisplayText({padding * 2.f, window_size.y - padding}, "back", "back.png", 1),
				// Fleet selection texts left arrow
				DisplayText::createDisplayText({950, 250}, "allocation_left_0", "left_arrow.png", 0.8),
				DisplayText::createDisplayText({950, 400}, "allocation_left_1", "left_arrow.png", 0.8),
				DisplayText::createDisplayText({950, 550}, "allocation_left_2", "left_arrow.png", 0.8),
				DisplayText::createDisplayText({950, 700}, "allocation_left_3", "left_arrow.png", 0.8),
				// Fleet selection texts right arrow
				DisplayText::createDisplayText({1250, 250}, "allocation_right_0", "right_arrow.png", 0.8),
				DisplayText::createDisplayText({1250, 400}, "allocation_right_1", "right_arrow.png", 0.8),
				DisplayText::createDisplayText({1250, 550}, "allocation_right_2", "right_arrow.png", 0.8),
				DisplayText::createDisplayText({1250, 700}, "allocation_right_3", "right_arrow.png", 0.8),
				//display icon
				DisplayText::createDisplayText({750, 250}, "regular_boat_icon", "regular_boat_icon.png", 1.5),
				DisplayText::createDisplayText({750, 400}, "big_boat_icon", "big_boat_icon.png", 1.5),
				DisplayText::createDisplayText({750, 550}, "recon_boat_icon", "recon_boat_icon.png", 1.5),
				DisplayText::createDisplayText({750, 700}, "attack_boat_icon", "attack_boat_icon.png", 1.5),
				//display title
				DisplayText::createDisplayText({1100, 150}, "token_icon", "token_icon.png", 2.5),
				DisplayText::createDisplayText({750, 150}, "boats_icon", "boats_icon.png", 2.5),
				DisplayText::createDisplayText({400, 325}, "cat_icon", "pause_screen.png", 0.9),
				DisplayText::createDisplayText({400, 150}, "cats_i_icon", "cats_icon.png", 2.5),
				//chara right and left
				DisplayText::createDisplayText({300, 500}, "chara_left", "left_arrow.png", 0.7),
				DisplayText::createDisplayText({500, 500}, "chara_right", "right_arrow.png", 0.7)

		};
		// Fleet selection texts number of specific boats
		ECS::registry<Text>.insert(buttons[6], Text("20", font, { 1150, 165 })),
			ECS::registry<Text>.get(buttons[6]).scale = 0.9;
		ECS::registry<Text>.insert(buttons[7], Text("0", font, { 1100, 250 })),
			ECS::registry<Text>.get(buttons[7]).scale = 0.6;
		ECS::registry<Text>.insert(buttons[8], Text("0", font, { 1100, 400 })),
			ECS::registry<Text>.get(buttons[8]).scale = 0.6;
		ECS::registry<Text>.insert(buttons[9], Text("0", font, { 1100, 550 })),
			ECS::registry<Text>.get(buttons[9]).scale = 0.6;
		ECS::registry<Text>.insert(buttons[10], Text("0", font, { 1100, 700 })),
			ECS::registry<Text>.get(buttons[10]).scale = 0.6;


		for (int i = 0; i < buttons.size(); i++) {
			auto& text = ECS::registry<TextComponent>.get(buttons[i]);
			if (text.text != "Start" && text.text != "Back") {
				auto& bcom = ECS::registry<ButtonComponent>.get(buttons[i]);
				bcom.scale = 0.8f * bcom.scale;
			}
			if (text.text == "chara_right" || text.text == "chara_left") {
				auto& bcom = ECS::registry<ButtonComponent>.get(buttons[i]);
				bcom.scale = 0.7f * bcom.scale;
			}
		}


		// Default cat that is selected
		auto& fleet = ECS::registry<Fleet>.get(player1);
		fleet.catType = "story";
		cats = { Cat::createCat({400, 325}, 6.0, 1.0, "story_cat_talking.png", "story") };

		ECS::Entity e;
		Scene s;
		s.scene = current_scene;
		ECS::registry<Scene>.emplace(e, s);
		subj_help_listener.notify_all_entity(e);
		clearHelptool();
	}

	if (current_scene == 2) {
		if (DebugModeSystem::in_debug_mode_menu || mode == story)//this is just for place holding when set up have been given
		{
			if (ECS::registry<Player>.components.size() <= 0) {
				Player::createPlayer(1);
				AIPlayer::createAIPlayer(-1);
			}
			ECS::Entity player1 = ECS::registry<Player>.entities[0];
			auto& fleet = ECS::registry<Fleet>.get(player1);

			if (level_selected == "1s") {
				fleet.boat_selections[0] = 2;
			}
			else if (level_selected == "2s") {
				fleet.boat_selections[2] = 1;
				fleet.boat_selections[3] = 1;
			}
			else if (level_selected == "3s") {
				fleet.boat_selections[0] = 1;
				fleet.boat_selections[1] = 1;
				fleet.boat_selections[2] = 1;
				fleet.boat_selections[3] = 1;
			}
			else if (level_selected == "4s") {
				fleet.boat_selections[0] = 1;
				fleet.boat_selections[1] = 1;
				fleet.boat_selections[2] = 1;
				fleet.boat_selections[3] = 1;
			}
			else {
				fleet.boat_selections[0] = 1;
				fleet.boat_selections[1] = 1;
				fleet.boat_selections[2] = 1;
				fleet.boat_selections[3] = 1;
			}
		}

		//Island::createIsland({ window_size.x/2.f, window_size.y/2.f },2);
		float padding = 100.f;

		ECS::Entity p_button = Button::createButton({ padding , window_size.y - padding * 0.75f }, "game_pause");
		auto& p_bcom = ECS::registry<ButtonComponent>.get(p_button);
		p_bcom.scale = 0.5f * p_bcom.scale;

		WorldSystem::buttons = {
				Button::createButton({window_size.x - padding * 2.f, window_size.y - padding}, "End Turn"), p_button };
		WorldSystem::texts = {
				DisplayText::createDisplayText({window_size.x - padding * 2.f, window_size.y - padding}, "end_turn","end_turn.png", 0.7),
				DisplayText::createDisplayText({padding , window_size.y - padding * 0.75f}, "pause_icon","pause_icon.png", 0.9) };


		// Test Token Allocation
		// Get Player Entity
		auto& player1 = ECS::registry<Player>.entities[0];
		// Get fleet
		auto& fleet = ECS::registry<Fleet>.get(player1);
		// Loop through boat selections in fleet and create the respective boat entities
		for (auto& x : fleet.boat_selections) {
			for (int i = 0; i < x.second; i++) {
				ECS::Entity temp_boat = Boat::createBoat({ (x.first + 1) * 100, (i + 1) * 100 }, x.first,
					fleet.player_number, false);
				fleet.boats.push_back(temp_boat);
				//std::cout << "Fleet Number: " << ECS::registry<Status>.get(temp_boat).player_number << "\n";
			}
		}
		ECS::Entity ai_player1 = ECS::registry<AIPlayer>.entities[0];
		auto& ai_fleet1 = ECS::registry<Fleet>.get(ai_player1);

		if (mode == story) {

			if (level_selected == "1s") {
				ai_fleet1.boat_selections[0] = 1;
			}
			else if (level_selected == "2s") {
				ai_fleet1.boat_selections[3] = 1;
			}
			else if (level_selected == "3s") {
				ai_fleet1.boat_selections[0] = 1;
				ai_fleet1.boat_selections[1] = 1;
				ai_fleet1.boat_selections[2] = 1;
			}
			else if (level_selected == "4s") {
				ai_fleet1.boat_selections[0] = 1;
				ai_fleet1.boat_selections[1] = 1;
				ai_fleet1.boat_selections[2] = 1;
				ai_fleet1.boat_selections[3] = 1;
			}
		}
		else {
			ai_fleet1.boat_selections[0] = 1;
			ai_fleet1.boat_selections[1] = 1;
			ai_fleet1.boat_selections[2] = 1;
			ai_fleet1.boat_selections[3] = 1;
		}


		for (auto& x : ai_fleet1.boat_selections) {
			for (int i = 0; i < x.second; i++) {
				vec2 position = { window_size.x - (x.first + 1) * 100, (i + 1) * 200 };
				ECS::Entity temp_boat = Boat::createBoat(position, x.first, ai_fleet1.player_number, true);
				ai_fleet1.boats.push_back(temp_boat);
				// add the boat to debugComponent to show the movement spline in debug mode

				//std::cout << "Fleet Number: " << ECS::registry<Status>.get(temp_boat).player_number << "\n";
			}
		}
		//load level
		LevelSystem::load_level(level_selected);
		subj_level_done.notify_all(); // tell the map graph to add islands as obstacles
		// Make the regular boat selected
		ECS::registry<Selectable>.get(fleet.boats[0]).selected = true;
		WorldSystem::boats = { fleet.boats };

		//set initial timer to 10 seconds
		curr_phase = PLANNING;
		lastTime = (int)(action_phase_time / 1000.f);

		curr_num_enemies = ai_fleet1.boats.size();
		getTimer();

		if (mode == freeplay) {
			ECS::Entity e;
			Scene s;
			s.scene = current_scene;
			ECS::registry<Scene>.emplace(e, s);
			subj_help_listener.notify_all_entity(e);

			subj_help_listener.notify_all();
			clearHelptool();
		}

		// Notify AI System
		subj_game_restart.notify_all();
	}

	if (current_scene == 0) {

		float padding = 200.f;
		float w_padding = 50.f;
		DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 }, "background", "title_screen_picture.png",
			3.0);
		DisplayText::createDisplayText({ window_size.x / 2, window_size.y / 2 - (padding * 1.5) }, "title", "title.png",
			2.5);
		WorldSystem::buttons = { Button::createButton({window_size.x / 2 + padding, window_size.y / 2}, "story"),
								Button::createButton({window_size.x / 2 + padding * 2.25f, window_size.y / 2}, "PVE"),
								Button::createButton({window_size.x / 2 - padding * 0.25f, window_size.y / 2}, "load"),

								Button::createButton({window_size.x / 2 + padding * 0.50f, window_size.y / 2 + padding},
													 "Options"),
								Button::createButton({window_size.x / 2 + padding * 1.75f, window_size.y / 2 + padding},
													 "Exit") };

		ECS::registry<Text>.insert(buttons[2], Text("Continue", font, { window_size.x / 2 - padding * 0.25f - w_padding , window_size.y / 2 }));
		ECS::registry<Text>.get(buttons[2]).scale = 0.5;

		ECS::registry<Text>.insert(buttons[0], Text("Freeplay", font, { window_size.x / 2 + padding * 2.25f - w_padding - 10.f, window_size.y / 2 }));
		ECS::registry<Text>.get(buttons[0]).scale = 0.6;

		ECS::registry<Text>.insert(buttons[1], Text("New Story", font, { window_size.x / 2 + padding - w_padding - 5.f, window_size.y / 2 }));
		ECS::registry<Text>.get(buttons[1]).scale = 0.5;

		ECS::registry<Text>.insert(buttons[3], Text("Options", font, { window_size.x / 2 + padding * 0.50f - w_padding,window_size.y / 2 + padding }));
		ECS::registry<Text>.get(buttons[3]).scale = 0.6;

		ECS::registry<Text>.insert(buttons[4], Text("Exit", font, { window_size.x / 2 + padding * 1.75f - w_padding + 10.f , window_size.y / 2 + padding }));
		ECS::registry<Text>.get(buttons[4]).scale = 0.6;

		ECS::Entity e;
		Scene s;
		s.scene = current_scene;
		ECS::registry<Scene>.emplace(e, s);
		subj_help_listener.notify_all_entity(e);
		clearHelptool();
	}
}

void WorldSystem::set_mouse_pos() {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	this->mouse_pos = vec2(xpos, ypos);
}

vec2 WorldSystem::get_game_pos() const {
	float u = mouse_pos.x / window_dimensions.x;
	float t = mouse_pos.y / window_dimensions.y;
	//vec2 game_pos = vec2(0, 0) + vec2(u * window_dimensions.x, t * window_dimensions.y);
	vec2 camera_pos = (cam.upper_left_corner + vec2(u * cam.window_size.x, -t * cam.window_size.y));
	vec2 game_pos = vec2(camera_pos.x + (window_dimensions.x / 2), (window_dimensions.y / 2) - camera_pos.y);
	return game_pos;
}

void WorldSystem::shift_at_boundry() {
	//If mouse hits screen border, attempt to pan camera
	vec2 shift(0, 0);
	int shift_amount = 15;
	int boundry_amount = 80;
	//TODO: smooth and improve
	//Checking each border, NOT ensuring within window boundry (just checking if at a border, camera checks window bounds)
	if (!in_window_bounds())
		return;

	if (mouse_pos.x > window_dimensions.x - boundry_amount) {
		//shift camera right
		shift.x += shift_amount;
	}
	if (mouse_pos.x < boundry_amount) {
		//shift camera left
		shift.x -= shift_amount;
	}
	if (mouse_pos.y > window_dimensions.y - boundry_amount) {
		//shift camera down
		shift.y -= shift_amount;
	}
	if (mouse_pos.y < boundry_amount) {
		//shift camera up
		shift.y += shift_amount;
	}
	cam.pan(shift);
}

bool WorldSystem::in_window_bounds() {
	bool in_x = (mouse_pos.x <= window_dimensions.x) && (mouse_pos.x >= 0);
	bool in_y = (mouse_pos.y <= window_dimensions.y) && (mouse_pos.y >= 0);
	return in_x && in_y;
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	auto& registry = ECS::registry<PhysicsSystem::Collision>;
	for (unsigned int i = 0; i < registry.components.size(); i++) {
		// The entity and its collider
		auto entity = registry.entities[i];
		auto entity_other = registry.components[i].other;

		// TODO: Instead of instant death, collision damage?
		//if (ECS::registry<Boat>.has(entity)) {
		//    if (ECS::registry<Shark>.has(entity_other)) {
		//        // Destroy the boat TODO: boat sinking FX
		//        ECS::ContainerInterface::remove_all_components_of(entity);
		//    }
		//}
	}
}


// Should the game be over ?
bool WorldSystem::is_over() const {
	if (exit == false) {
		return glfwWindowShouldClose(window) > 0;
	}
	else {
		Mix_PlayChannel(-1, button_sound, 0);
		return true;
	}
}

void WorldSystem::scene_update(int scene_number) {

	switch (scene_number) {
	case 0:
		current_scene = 0;
		std::cout << "This is main menu" << std::endl;
		break;
	case 1:
		current_scene = 1;
		std::cout << "This is boat selection menu" << std::endl;
		break;
	case 2:
		current_scene = 2;
		std::cout << "This is game menu" << std::endl;
		// notify
		subj_game_start.notify_all();
		break;
	case 3:
		current_scene = 3;
		std::cout << "This is level selection menu" << std::endl;
		break;
	case 4:
		current_scene = 4;
		std::cout << "This is cutscene story scene" << std::endl;
		break;
	case 5:
		current_scene = 5;
		std::cout << "This is story scene" << std::endl;
		break;
	}
}

void WorldSystem::on_mouse_scroll(vec2 scroll_input) {
	// (-) when scrolling back, (+) when scrolling in.
	double scroll = scroll_input.y;
	cam.zoom(scroll);
}

// On key callback
void WorldSystem::on_key(int key, int, int key_action, int mod) {
	if (current_scene == 0) {
		if (key_action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
			//this go to level selection screen
			scene_stack.push(current_scene);
			scene_update(3);
			key_action = false;
			//int w, h;
			//glfwGetWindowSize(window, &w, &h);
			//restart();
			ECS::Entity startbutton = buttons[0];
			ECS::registry<TransitionTimer>.emplace(startbutton);
		}
	}

	if (current_scene == 2) {
		if (key_action == GLFW_PRESS && key == GLFW_KEY_B) {
			//this go to boat selection screen
			scene_update(1);
			key_action = false;
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			restart();
			return;
		}

		if (key_action == GLFW_PRESS && key == GLFW_KEY_T) {
			phase = action;
		}

	}

	if (current_scene == 1) {
		if (key_action == GLFW_PRESS && key == GLFW_KEY_B) {
			//this go to level selection screen
			scene_update(3);
			key_action = false;
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			restart();
			return;
		}

		if (key_action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
			auto& player1 = ECS::registry<Player>.entities[0];
			// Get fleet
			auto& fleet = ECS::registry<Fleet>.get(player1);
			if (fleet.tokens < 20) {
				scene_stack.push(current_scene);
				scene_update(2);
				key_action = false;
				ECS::Entity startbutton = buttons[0];
				ECS::registry<TransitionTimer>.emplace(startbutton);
			}
			else {
				if (ECS::registry<Text>.has(warning_text)) {
					ECS::registry<Text>.remove(warning_text);
				}
				ECS::registry<Text>.insert(warning_text, Text("Please allocate tokens before moving on to game", font,
					{ 300.f, 900.f }));
				//setting text to be smaller
				//ECS::registry<Text>.get(warning_text).scale = 0.7;
				//set to yellow
				ECS::registry<Text>.get(warning_text).colour = { 1.0f, 1.0f, 0.0f };
				if (!ECS::registry<WarningTimer>.has(warning_text)) {
					ECS::registry<WarningTimer>.emplace(warning_text);
				}
			}
		}
	}


	if (current_scene == 3) {
		if (key_action == GLFW_PRESS && key == GLFW_KEY_B) {
			//this go to title screen
			scene_update(0);
			key_action = false;
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			restart();
			return;
		}

		if (key_action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
			//this go to boat selection screen
			scene_stack.push(current_scene);
			scene_update(1);
			key_action = false;
			ECS::Entity startbutton = buttons[0];
			ECS::registry<TransitionTimer>.emplace(startbutton);
		}
	}


	// Resetting the camera
	if (key_action == GLFW_PRESS && key == GLFW_KEY_C) {
		cam.reset_camera();
	}


	if (key_action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, 1);

	// Resetting game
	if (key_action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart();
		return;
	}

	if (key == GLFW_KEY_LEFT_SHIFT) {
		if (key_action == GLFW_PRESS) {
			shift_mod = true;
		}
		if (key_action == GLFW_RELEASE) {
			shift_mod = false;
		}
	}

	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (key_action == GLFW_PRESS) {
			ctrl_mod = true;
		}
		if (key_action == GLFW_RELEASE) {
			ctrl_mod = false;
		}
	}
	if (current_scene == 2) {
		//this is for the pause
		//if (key == GLFW_KEY_BACKSPACE) {
		//	PauseModeSystem::in_pause_mode_menu = true;
		//}

		if (key == GLFW_KEY_P) {
			PauseModeSystem::in_pause_mode_menu = true;
		}
	}

	//if (key_action == GLFW_PRESS && key == GLFW_KEY_O) {
	//	OptionModeSystem::in_option_mode_menu = true;
	//}
	//if (key_action == GLFW_PRESS && key == GLFW_KEY_P) {
	//	OptionModeSystem::in_option_mode_menu = false;
	//}

	// Debugging
	if (key == GLFW_KEY_D) {
		DebugSystem::in_debug_mode = (key_action != GLFW_RELEASE);
	}

	if (key == GLFW_KEY_LEFT_SHIFT) {
		DebugModeSystem::in_debug_mode_menu = (key_action != GLFW_RELEASE);
	}
	//set all tutorial text to read
	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_S) {
		ECS::Entity e;
		Scene s;
		s.scene = current_scene;
		s.debug = 1;
		ECS::registry<Scene>.emplace(e, s);
		subj_help_listener.notify_all_entity(e);
		ECS::ContainerInterface::remove_all_components_of(e);

	}

	//set all tutorial text to unread
	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_A) {
		ECS::Entity e;
		Scene s;
		s.scene = current_scene;
		s.debug = 2;
		ECS::registry<Scene>.emplace(e, s);
		subj_help_listener.notify_all_entity(e);
		ECS::ContainerInterface::remove_all_components_of(e);

	}
	//this is for the auto win
	if (current_scene == 2) {
		if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_W) {
			result = true;
			if (mode == freeplay) {
				EndModeSystem::in_end_mode_menu = true;
			}
			else if (mode == story) {
				scene_stack.push(current_scene);
				//go to after battle cutscenes
				if (level_selected == "1s") {
					chapters_completed[0] = true;

				}
				else if (level_selected == "2s") {
					chapters_completed[1] = true;
				}
				else {
					chapters_completed[2] = true;
				}
				int next_scene = 5;
				scene_update(next_scene);
				if ((ECS::registry<TransitionTimer>.components.size() < 1)) {
					ECS::Entity entity;
					subj_restart_story_listener.notify_all();
					if (!ECS::registry<TransitionTimer>.has(entity)) {
						ECS::registry<TransitionTimer>.emplace(entity);
						auto& t_time = ECS::registry<TransitionTimer>.get(entity);
						t_time.counter_ms = 500;
					}
				}
			}
		}
		if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_L) {
			result = false;
			if (mode == freeplay) {
				EndModeSystem::in_end_mode_menu = true;
			}
			else if (mode == story) {
				EndModeSystem::in_end_mode_menu = true;
			}
		}

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_0) {
		clear_scene_stack();
		scene_update(0);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_1) {
		clear_scene_stack();
		scene_stack.push(0);
		scene_stack.push(3);
		scene_update(1);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_2) {
		clear_scene_stack();
		scene_stack.push(0);
		scene_stack.push(3);
		scene_stack.push(1);
		scene_update(2);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_3) {
		clear_scene_stack();
		scene_stack.push(0);
		scene_update(3);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_4) {
		scene_update(4);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;

	}

	if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_5) {
		scene_update(5);
		key_action = false;
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		restart();
		return;
	}
	if (current_scene == 3 && mode == story) {
		if (key_action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_T) {
			scene_update(3);
			key_action = false;
			chapters_completed = { true,true,true };
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			restart();
			return;
		}
	}
}


vec2 WorldSystem::window_offset(const vec2& v) const
{
	return vec2(v.x - (window_dimensions.x / 2), (window_dimensions.y / 2) - v.y);
}

void WorldSystem::handle_movement_arrow()
{
	unsigned int add_step = 5;
	for (auto& paths : ECS::registry<MovementPath>.entities)
	{
		auto& path = ECS::registry<MovementPath>.get(paths);
		if (path.drawing)
		{
			vec2 world_pos = window_offset(get_game_pos());

			path.render_coords.push_back(world_pos.x);
			path.render_coords.push_back(world_pos.y);
			path.position_coords.push_back(get_game_pos());

		}

	}
}

void WorldSystem::start_movement_arrow()
{
	for (auto& entity : ECS::registry<Boat>.entities)
	{
		auto& selectable = ECS::registry<Selectable>.get(entity);
		auto& path = ECS::registry<MovementPath>.get(entity);
		if (selectable.selected)
		{
			auto& motion = ECS::registry<Motion>.get(entity);
			path.render_coords.clear();
			path.position_coords.clear();
			path.drawing = true;
			path.ready = false;
			vec2 world_pos = window_offset(motion.position);
			path.render_coords.push_back(world_pos.x);
			path.render_coords.push_back(world_pos.y);
			path.position_coords.push_back(get_game_pos());
		}

	}
}
void WorldSystem::end_movement_arrow()
{
	for (auto& paths : ECS::registry<MovementPath>.entities)
	{
		auto& path = ECS::registry<MovementPath>.get(paths);
		if (path.drawing)
		{
			path.drawing = false;
			path.ready = true;
			vec2 pos = window_offset(get_game_pos());
			path.render_coords.push_back(pos.x);
			path.render_coords.push_back(pos.y);
			path.position_coords.push_back(get_game_pos());
			//TODO Call handle motion spline movement
			handle_movement(true);
		}
	}
}

void WorldSystem::on_mouse_move(vec2 new_mouse_pos) {

	set_mouse_pos();

	bool hovering = false;
	if (!PauseModeSystem::in_pause_mode_menu && !OptionModeSystem::in_option_mode_menu
		&& !EndModeSystem::in_end_mode_menu) {
		handle_movement_arrow();
		if (!(phase == action)) {
			for (auto entity : ECS::registry<Button>.entities) {
				auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
				if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position)) {
					Scene s;
					s.scene = current_scene;
					ECS::registry<Scene>.emplace(entity, s);
					subj_helptool_listener.notify_all_entity(entity);
					ECS::registry<Scene>.remove(entity);
					hovering = true;

				}
			}
			for (auto entity : ECS::registry<DisplayTextComponent>.entities) {
				auto& dtComponent = ECS::registry<DisplayTextComponent>.get(entity);
				if (check_button_click(mouse_pos, dtComponent.scale, dtComponent.position)) {
					std::string text = ECS::registry<TextComponent>.get(entity).text;
					if (text.find("background") == std::string::npos) {
						Scene s;
						s.scene = current_scene;
						ECS::registry<Scene>.emplace(entity, s);
						subj_helptool_listener.notify_all_entity(entity);
						ECS::registry<Scene>.remove(entity);
						hovering = true;
					}
				}
			}


			for (auto entity : ECS::registry<Motion>.entities) {
				auto& motion = ECS::registry<Motion>.get(entity);
				if (ECS::registry<Boat>.has(entity) || ECS::registry<Shark>.has(entity) ||
					ECS::registry<Crate>.has(entity) || ECS::registry<Island>.has(entity)) {
					if (check_button_click(get_game_pos(), motion.scale, motion.position)) {
						Scene s;
						s.scene = current_scene;
						ECS::registry<Scene>.emplace(entity, s);
						subj_helptool_listener.notify_all_entity(entity);
						ECS::registry<Scene>.remove(entity);
						hovering = true;

					}
				}
			}

			//for (auto entity : ECS::registry<Shark>.entities) {
			//	auto& motion = ECS::registry<Motion>.get(entity);
			//	if (check_button_click(get_game_pos(), motion.scale, motion.position)) {
			//		Scene s;
			//		s.scene = current_scene;
			//		ECS::registry<Scene>.emplace(entity, s);
			//		subj_helptool_listener.notify_all_entity(entity);
			//		ECS::registry<Scene>.remove(entity);
			//		hovering = true;

			//	}
			//}

		}
	}
	//if it not hovering over a button then don't show any help tip
	if (ECS::registry<Helptool>.components.size() > 0 && hovering == false) {
		for (auto entity : ECS::registry<Helptool>.entities) {
			ECS::ContainerInterface::remove_all_components_of(entity);
		}
		for (auto entity : ECS::registry<Text>.entities) {
			if (ECS::registry<TextComponent>.has(entity)) {
				std::string text = ECS::registry<TextComponent>.get(entity).text;
				if (text == "help_tool") {
					ECS::ContainerInterface::remove_all_components_of(entity);
				}
			}
		}
	}
}

// TODO: this should be reworked to use the BoundingBox selection code
bool WorldSystem::check_button_click(vec2 mouse_pos, vec2 scale, vec2 button_pos) {
	float x_scale = abs(scale.x / 2.f);
	float y_scale = abs(scale.y / 2.f);
	return mouse_pos.x >= (button_pos.x - x_scale) && mouse_pos.x <= (button_pos.x + x_scale)
		&& mouse_pos.y >= (button_pos.y - y_scale) && mouse_pos.y <= (button_pos.y + y_scale);
}

void WorldSystem::on_mouse_skill_helper(ECS::Entity boat, int skill_num, Mix_Chunk* act, Mix_Chunk* dea)
{
	auto& status = boat.get<Status>();
	float padding = 100.f;
	float offset = 50.f;
	vec2 s1_pos = vec2{ padding * 6.f + offset, window_size.y - padding + offset };
	vec2 s2_pos = vec2{ window_size.x - padding * 6.f + offset, window_size.y - padding + offset };
	if (skill_num == 1) {
		switch (status.s1_cd) {
		case 0:
			if (status.s1_selected) {
				status.s1_selected = false;
				for (auto removeable : ECS::registry<SkillUIRemoveComp>.entities) {
					if (removeable.get<TextComponent>().text == "checkmark_1") {
						ECS::ContainerInterface::remove_all_components_of(removeable);
					}
				}
				Mix_PlayChannel(-1, dea, 0);
			}
			else {
				status.s1_selected = true;
				auto checkmark_1 = DisplayText::createDisplayText(s1_pos, "checkmark_1", "checkmark.png", 1);
				ECS::registry<SkillUIRemoveComp>.emplace(checkmark_1);
				Mix_PlayChannel(-1, act, 0);
			}
			break;
		case 1:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_pos, "sk_one_1", "sk_one.png", 1));
			Mix_PlayChannel(-1, sk_cd_1, 0);
			break;
		case 2:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_pos, "sk_two_1", "sk_two.png", 1));
			Mix_PlayChannel(-1, sk_cd_2, 0);
			break;
		case 3:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_pos, "sk_three_1", "sk_three.png", 1));
			Mix_PlayChannel(-1, sk_cd_3, 0);
			break;
		default:
			std::cout << "Debug: skill system error!" << std::endl;
		}
	}
	else if (skill_num == 2) {
		switch (status.s2_cd) {
		case 0:
			if (status.s2_selected) {
				status.s2_selected = false;
				for (auto removeable : ECS::registry<SkillUIRemoveComp>.entities) {
					if (removeable.get<TextComponent>().text == "checkmark_2") {
						ECS::ContainerInterface::remove_all_components_of(removeable);
					}
				}
				Mix_PlayChannel(-1, dea, 0);
			}
			else {
				status.s2_selected = true;
				auto checkmark_2 = DisplayText::createDisplayText(s2_pos, "checkmark_2", "checkmark.png", 1);
				ECS::registry<SkillUIRemoveComp>.emplace(checkmark_2);
				Mix_PlayChannel(-1, act, 0);
			}
			break;
		case 1:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_pos, "sk_one_2", "sk_one.png", 1));
			Mix_PlayChannel(-1, sk_cd_1, 0);
			break;
		case 2:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_pos, "sk_two_2", "sk_two.png", 1));
			Mix_PlayChannel(-1, sk_cd_2, 0);
			break;
		case 3:
			ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_pos, "sk_three_2", "sk_three.png", 1));
			Mix_PlayChannel(-1, sk_cd_3, 0);
			break;
		default:
			std::cout << "Debug: skill system error!" << std::endl;
		}
	}
}

void WorldSystem::on_mouse_button(int button, int mouse_action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	std::string subString("allocation_");
	if (!PauseModeSystem::in_pause_mode_menu && !OptionModeSystem::in_option_mode_menu && !EndModeSystem::in_end_mode_menu)
	{
		if (ECS::registry<Helpbox>.components.size() <= 0) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && mouse_action == GLFW_PRESS) {
				for (auto entity : ECS::registry<Button>.entities) {
					auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
					auto& text = ECS::registry<TextComponent>.get(entity);
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "Start" || text.text == "PVE")) {
						Mix_PlayChannel(-1, button_sound, 0);
						if (current_scene == 1) {
							auto& player1 = ECS::registry<Player>.entities[0];
							// Get fleet
							auto& fleet = ECS::registry<Fleet>.get(player1);
							if (fleet.tokens < 20) {
								scene_stack.push(current_scene);
								int next_scene = 2;
								scene_update(next_scene);
								if (!ECS::registry<TransitionTimer>.has(entity)) {
									ECS::registry<TransitionTimer>.emplace(entity);
								}
							}
							else {
								if (ECS::registry<Text>.has(warning_text)) {
									ECS::registry<Text>.remove(warning_text);
								}
								ECS::registry<Text>.insert(warning_text, Text("Please allocate tokens before moving on to game", font, { 350.0f, 900.0f }));
								//setting text to be smaller
								ECS::registry<Text>.get(warning_text).scale = 0.7;
								//set to yellow
								ECS::registry<Text>.get(warning_text).colour = { 1.0f, 1.0f, 0.0f };
								if (!ECS::registry<WarningTimer>.has(warning_text)) {
									ECS::registry<WarningTimer>.emplace(warning_text);
								}
							}
						}
						else if (current_scene == 0) {
							mode = freeplay;
							scene_stack.push(current_scene);
							//go to map selection
							int next_scene = 3;
							scene_update(next_scene);
							if (!ECS::registry<TransitionTimer>.has(entity)) {
								ECS::registry<TransitionTimer>.emplace(entity);
							}
						}
						else if (current_scene == 3) {
							if (mode == freeplay) {
								scene_stack.push(current_scene);
								//go to boat selection
								int next_scene = 1;
								scene_update(next_scene);
								if (!ECS::registry<TransitionTimer>.has(entity)) {
									ECS::registry<TransitionTimer>.emplace(entity);
								}
							}
							if (mode == story) {
								scene_stack.push(current_scene);
								//go to go to cutscene
								int next_scene = 4;
								scene_update(next_scene);
								if (!ECS::registry<TransitionTimer>.has(entity)) {
									ECS::registry<TransitionTimer>.emplace(entity);
								}
							}
						}

					}
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "story")) {
						started_story = true;
						chapters_completed = { false,false,false };
						Mix_PlayChannel(-1, button_sound, 0);
						mode = story;
						scene_stack.push(current_scene);
						//go to map selection
						int next_scene = 3;
						scene_update(next_scene);
						if (!ECS::registry<TransitionTimer>.has(entity)) {
							ECS::registry<TransitionTimer>.emplace(entity);
						}

					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "load")) {
						if (started_story == true) {
							Mix_PlayChannel(-1, button_sound, 0);
							mode = story;
							scene_stack.push(current_scene);
							//go to map selection
							int next_scene = 3;
							scene_update(next_scene);
							if (!ECS::registry<TransitionTimer>.has(entity)) {
								ECS::registry<TransitionTimer>.emplace(entity);
							}
						}
						else {
							if (ECS::registry<Text>.has(warning_text)) {
								ECS::registry<Text>.remove(warning_text);
							}
							ECS::registry<Text>.insert(warning_text, Text("No story saved...", font, { 550.0f, 900.0f }));
							//setting text to be smaller
							ECS::registry<Text>.get(warning_text).scale = 0.7;
							//set to yellow
							//ECS::registry<Text>.get(warning_text).colour = { 1.0f, 1.0f, 0.0f };
							if (!ECS::registry<WarningTimer>.has(warning_text)) {
								ECS::registry<WarningTimer>.emplace(warning_text);
							}
						}

					}


					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "skip_cutscene") {
						Mix_PlayChannel(-1, button_sound, 0);
						int next_scene = 0;
						scene_update(next_scene);
						if (!ECS::registry<TransitionTimer>.has(entity)) {
							ECS::registry<TransitionTimer>.emplace(entity);
						}
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "game_pause") {
						PauseModeSystem::in_pause_mode_menu = true;
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "skip_story" || text.text == "Next")) {
						Mix_PlayChannel(-1, button_sound, 0);
						if (result == false) {
							scene_stack.push(current_scene);
							int next_scene = 2;
							scene_update(next_scene);
							if (!ECS::registry<TransitionTimer>.has(entity)) {
								ECS::registry<TransitionTimer>.emplace(entity);
							}
						}
						else if (result == true) {
							result = false;
							clear_scene_stack();
							scene_stack.push(0);
							int next_scene = 3;//to chapter selection
							scene_update(next_scene);
							if (!ECS::registry<TransitionTimer>.has(entity)) {
								ECS::registry<TransitionTimer>.emplace(entity);
							}
						}
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "Back") {
						Mix_PlayChannel(-1, button_sound, 0);
						int next_scene = scene_stack.top();
						scene_update(next_scene);
						scene_stack.pop();
						int w, h;
						glfwGetWindowSize(window, &w, &h);
						restart();
						return;
					}
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "Options")) {
						Mix_PlayChannel(-1, button_sound, 0);
						OptionModeSystem::in_option_mode_menu = true;
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position)
						&& (text.text == "map_1" || text.text == "map_2" || text.text == "map_3")) {
						Mix_PlayChannel(-1, button_sound, 0);
						std::string level_string = text.text.substr(text.text.size() - 1, text.text.size() - 1);
						level_selected = level_string;
						scene_stack.push(current_scene);
						//go to boat selection
						int next_scene = 1;
						scene_update(next_scene);
						if (!ECS::registry<TransitionTimer>.has(entity)) {
							ECS::registry<TransitionTimer>.emplace(entity);
						}
						return;
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) &&
						(text.text == "story_1s" || text.text == "story_2s" || text.text == "story_3s" || text.text == "story_4s")) {
						Mix_PlayChannel(-1, button_sound, 0);
						std::string level_string = text.text.substr(text.text.size() - 2, text.text.size() - 1);
						level_selected = level_string;
						scene_stack.push(current_scene);
						int next_scene = 5;
						scene_update(next_scene);
						subj_restart_story_listener.notify_all();
						if (!ECS::registry<TransitionTimer>.has(entity)) {
							ECS::registry<TransitionTimer>.emplace(entity);
						}
						return;
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "Exit") {
						Mix_PlayChannel(-1, button_sound, 0);
						exit = true;
					}

					for (auto entity : ECS::registry<StoryTextbox>.entities) {
						auto& scomponent = ECS::registry<StoryComponent>.get(entity);
						auto& text = ECS::registry<TextComponent>.get(entity);
						if (check_button_click(mouse_pos, scomponent.scale, scomponent.position) && (text.text == "story_text")) {
							Mix_PlayChannel(-1, button_sound, 0);
							ECS::Entity e;
							StoryLevel s;
							s.level = level_selected;
							s.after_battle = result;
							ECS::registry<StoryLevel>.emplace(e, s);
							subj_story_listener.notify_all_entity(e);
							ECS::ContainerInterface::remove_all_components_of(e);
						}

					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "recon_s1") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 1, sk_recon_a1, sk_recon_d1);
						// self speed * 2, this effect will be revert by reinit_stats, so no double effect
						auto& boat_status = boat.get<Status>();
						if (boat_status.s1_selected && boat_status.s1_cd == 0) {
							boat_status.speed *= 2;
						}
						else if (!boat_status.s1_selected && boat_status.s1_cd == 0) {
							boat_status.speed /= 2;
						}
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "recon_s2") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 2, sk_recon_a2, sk_recon_d2);
						// view range * 1.5, this effect will be revert by reinit_stats, so no double effect
						auto& boat_status = boat.get<Status>();
						if (boat_status.s2_selected && boat_status.s2_cd == 0) {
							boat_status.view_range *= 1.5;
						}
						else if (!boat_status.s2_selected && boat_status.s2_cd == 0) {
							boat_status.view_range /= 1.5;
						}
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "attack_s1") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 1, sk_attack_a1, sk_attack_d1);
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "attack_s2") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 2, sk_attack_a2, sk_attack_d2);
						// attack * 2, speed = 0.01f, this effect will be revert by reinit_stats, so no double effect
						auto& boat_status = boat.get<Status>();
						if (boat_status.s2_selected && boat_status.s2_cd == 0) {
							boat_status.weapon_damage *= 2;
							boat_status.speed *= 0.0001f;
						}
						else if (!boat_status.s2_selected && boat_status.s2_cd == 0) {
							boat_status.weapon_damage /= 2;
							boat_status.speed /= 0.0001f;
						}
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "reg_s1") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 1, sk_reg_a1, sk_reg_d1);
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "reg_s2") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 2, sk_reg_a2, sk_reg_d2);
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "big_s1") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 1, sk_big_a1, sk_big_d1);
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "big_s2") {
						// get this ship
						auto boat = entity.get<SkillButtonComponent>().selfboat; // will not crash since this button is skill button
						on_mouse_skill_helper(boat, 2, sk_big_a2, sk_big_d2);
					}

					// Character selection left and right buttons
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "chara_left") {
						// Vector of cat types
						std::vector<std::string> catTypes{ "story", "sailor", "robot", "business", "princess" };
						// Get Fleet details and cat type index
						auto& player1 = ECS::registry<Player>.entities[0];
						auto& fleet = ECS::registry<Fleet>.get(player1);
						int selectedCatIndex = 0;
						for (int i = 0; i < catTypes.size(); i++) {
							if (fleet.catType == catTypes[i]) {
								selectedCatIndex = i;
							}
						}
						// Shift index to the left
						if (selectedCatIndex == 0) {
							selectedCatIndex = catTypes.size() - 1;
						}
						else {
							selectedCatIndex--;
						}
						// Cycle through cat types to select cat
						std::string selectedCatType = catTypes[selectedCatIndex];
						auto& cat = ECS::registry<Cat>.entities[0];
						ECS::ContainerInterface::remove_all_components_of(cat);
						cats = { Cat::createCat({400, 325}, 6.0, 1.0, selectedCatType + "_cat_talking.png", selectedCatType) };
						fleet.catType = selectedCatType;
					}
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "chara_right") {
						// Vector of cat types
						std::vector<std::string> catTypes{ "story", "sailor", "robot", "business", "princess" };
						// Get Fleet details and cat type index
						auto& player1 = ECS::registry<Player>.entities[0];
						auto& fleet = ECS::registry<Fleet>.get(player1);
						int selectedCatIndex = 0;
						for (int i = 0; i < catTypes.size(); i++) {
							if (fleet.catType == catTypes[i]) {
								selectedCatIndex = i;
							}
						}
						// Shift index to the right
						selectedCatIndex = (selectedCatIndex + 1) % catTypes.size();
						// Cycle through cat types to select cat
						std::string selectedCatType = catTypes[selectedCatIndex];
						auto& cat = ECS::registry<Cat>.entities[0];
						ECS::ContainerInterface::remove_all_components_of(cat);
						cats = { Cat::createCat({400, 325}, 6.0, 1.0, selectedCatType + "_cat_talking.png", selectedCatType) };
						fleet.catType = selectedCatType;
					}

					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text == "End Turn") {

						Mix_PlayChannel(-1, button_sound, 0);

						//Clear drawn arrows
						for (auto& path : ECS::registry<MovementPath>.components)
						{
							path.render_coords.clear();
							path.position_coords.clear();
						}

						// deal with skills
						SkillSystem::reinit_stats();
						SkillSystem::init_skills();

						// change the phase
						phase = action;
						//TODO: show timer,std::cout << "Action" << std::endl;


					}
					// Token Allocation Buttons
					else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text.find(subString) != std::string::npos) {
						Mix_PlayChannel(-1, button_sound, 0);
						std::stringstream test(text.text);
						std::string segment;
						std::vector<std::string> seglist;

						while (std::getline(test, segment, '_')) {
							seglist.push_back(segment);
						}

						// (TEMP) safety check >.< very bad code... sad
						if (seglist.size() == 3) {
							int boatType = std::stoi(seglist[2]);
							if (seglist[1] == "left") {
								if (!ECS::registry<Player>.entities.empty()) {
									// Get player entity
									auto& player1 = ECS::registry<Player>.entities[0];
									// Get fleet
									auto& fleet = ECS::registry<Fleet>.get(player1);
									if (fleet.boat_selections[boatType] > 0) {
										fleet.boat_selections[boatType] -= 1;

										// Refund tokens, update text
										string boatTypeName;
										if (boatType == 0) {
											fleet.tokens += 4;
											boatTypeName = "regular boats";
										}
										if (boatType == 1) {
											fleet.tokens += 10;
											boatTypeName = "big boats";
										}
										if (boatType == 2) {
											fleet.tokens += 2;
											boatTypeName = "recon boats";
										}
										if (boatType == 3) {
											fleet.tokens += 6;
											boatTypeName = "attack boats";
										}
										for (auto entity2 : ECS::registry<Button>.entities) {
											auto& text2 = ECS::registry<TextComponent>.get(entity2);
											if (text2.text == "tokens") {
												auto& token_button_text = ECS::registry<Text>.get(entity2);
												token_button_text.content = std::to_string(fleet.tokens);
											}
											if (text2.text == boatTypeName) {
												auto& token_button_text = ECS::registry<Text>.get(entity2);
												token_button_text.content = std::to_string(fleet.boat_selections[boatType]);
											}
										}
									}
								}
							}
							if (seglist[1] == "right") {
								if (!ECS::registry<Player>.entities.empty()) {
									// Get player entity
									auto& player1 = ECS::registry<Player>.entities[0];
									// Get fleet
									auto& fleet = ECS::registry<Fleet>.get(player1);
									// subtract tokens, update text
									string boatTypeName;
									if (boatType == 0 && fleet.tokens >= 4) {
										fleet.boat_selections[boatType] += 1;
										fleet.tokens -= 4;
										boatTypeName = "regular boats";
									}
									if (boatType == 1 && fleet.tokens >= 10) {
										fleet.boat_selections[boatType] += 1;
										fleet.tokens -= 10;
										boatTypeName = "big boats";
									}
									if (boatType == 2 && fleet.tokens >= 2) {
										fleet.boat_selections[boatType] += 1;
										fleet.tokens -= 2;
										boatTypeName = "recon boats";
									}
									if (boatType == 3 && fleet.tokens >= 6) {
										fleet.boat_selections[boatType] += 1;
										fleet.tokens -= 6;
										boatTypeName = "attack boats";
									}
									for (auto entity2 : ECS::registry<Button>.entities) {
										auto& text2 = ECS::registry<TextComponent>.get(entity2);
										if (text2.text == "tokens") {
											auto& token_button_text = ECS::registry<Text>.get(entity2);
											token_button_text.content = std::to_string(fleet.tokens);
										}
										if (text2.text == boatTypeName) {
											auto& token_button_text = ECS::registry<Text>.get(entity2);
											token_button_text.content = std::to_string(fleet.boat_selections[boatType]);
										}
									}
								}
							}
						}

					}
				}

				// Disable interaction if we're in the Action phase
				if (phase == action) {
					return;
				}

				handle_selection();

				handle_movement(false);

			}
			/*
			if (button == GLFW_MOUSE_BUTTON_RIGHT && mouse_action == GLFW_PRESS) {
				handle_movement(true);
			}
			*/
			if (phase != action) {
				// Handle drawing a path
				if (button == GLFW_MOUSE_BUTTON_RIGHT && mouse_action == GLFW_PRESS)
					start_movement_arrow();
				if (button == GLFW_MOUSE_BUTTON_RIGHT && mouse_action == GLFW_RELEASE)
					end_movement_arrow();
			}
		}
		else {
			//when help text is on the screen
			if (button == GLFW_MOUSE_BUTTON_LEFT && mouse_action == GLFW_PRESS) {
				for (auto entity : ECS::registry<Helpbox>.entities) {
					auto& hcomponent = ECS::registry<HelpComponent>.get(entity);
					auto& text = ECS::registry<TextComponent>.get(entity);
					if (check_button_click(mouse_pos, hcomponent.scale, hcomponent.position) && (text.text == "help")) {
						Mix_PlayChannel(-1, button_sound, 0);
						ECS::Entity e;
						Scene s;
						s.scene = current_scene;
						ECS::registry<Scene>.emplace(e, s);
						subj_help_listener.notify_all_entity(e);
						ECS::ContainerInterface::remove_all_components_of(e);
					}

				}
				for (auto entity : ECS::registry<Button>.entities) {
					auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
					auto& text = ECS::registry<TextComponent>.get(entity);
					if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "skip")) {
						Mix_PlayChannel(-1, button_sound, 0);
						ECS::Entity e;
						Scene s;
						s.scene = current_scene;
						s.skip = true;
						ECS::registry<Scene>.emplace(e, s);
						subj_help_listener.notify_all_entity(e);
						ECS::ContainerInterface::remove_all_components_of(e);

					}
				}
			}
		}
	}
	else if (PauseModeSystem::in_pause_mode_menu && !OptionModeSystem::in_option_mode_menu && !EndModeSystem::in_end_mode_menu) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && mouse_action == GLFW_PRESS) {
			for (auto entity : ECS::registry<Button>.entities) {
				auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
				auto& text = ECS::registry<TextComponent>.get(entity);
				if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "pause_resume")) {
					Mix_PlayChannel(-1, button_sound, 0);
					PauseModeSystem::in_pause_mode_menu = false;
				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "pause_option")) {
					Mix_PlayChannel(-1, button_sound, 0);
					OptionModeSystem::in_option_mode_menu = true;

				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "pause_return")) {
					Mix_PlayChannel(-1, button_sound, 0);
					//go to menu
					clear_scene_stack();
					int next_scene = 0;
					scene_update(next_scene);
					ECS::Entity return_to_main;
					if (!ECS::registry<TransitionTimer>.has(return_to_main)) {
						ECS::registry<TransitionTimer>.emplace(return_to_main);
					}
					PauseModeSystem::in_pause_mode_menu = false;
					return;
				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "pause_selection")) {
					Mix_PlayChannel(-1, button_sound, 0);
					//go to menu
					clear_scene_stack();
					scene_stack.push(0);
					int next_scene = 3;
					scene_update(next_scene);
					ECS::Entity return_to_sel;
					if (!ECS::registry<TransitionTimer>.has(return_to_sel)) {
						ECS::registry<TransitionTimer>.emplace(return_to_sel);
					}
					PauseModeSystem::in_pause_mode_menu = false;
					return;
				}
			}
		}
	}
	else if (!PauseModeSystem::in_pause_mode_menu && !OptionModeSystem::in_option_mode_menu && EndModeSystem::in_end_mode_menu) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && mouse_action == GLFW_PRESS) {
			for (auto entity : ECS::registry<Button>.entities) {
				auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
				auto& text = ECS::registry<TextComponent>.get(entity);
				if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "end_again")) {
					Mix_PlayChannel(-1, button_sound, 0);
					clear_scene_stack();
					if (mode == freeplay) {
						scene_stack.push(0);
						scene_stack.push(3);
					}
					int next_scene = 1;
					scene_update(next_scene);
					ECS::Entity return_to_sel;
					if (!ECS::registry<TransitionTimer>.has(return_to_sel)) {
						ECS::registry<TransitionTimer>.emplace(return_to_sel);
					}
					EndModeSystem::in_end_mode_menu = false;
				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "end_map")) {
					Mix_PlayChannel(-1, button_sound, 0);
					clear_scene_stack();
					scene_stack.push(0);
					int next_scene = 3;
					scene_update(next_scene);
					ECS::Entity return_to_map;
					if (!ECS::registry<TransitionTimer>.has(return_to_map)) {
						ECS::registry<TransitionTimer>.emplace(return_to_map);
					}
					EndModeSystem::in_end_mode_menu = false;

				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "end_return")) {
					Mix_PlayChannel(-1, button_sound, 0);
					clear_scene_stack();
					//go to menu
					int next_scene = 0;
					scene_update(next_scene);
					ECS::Entity return_to_main;
					if (!ECS::registry<TransitionTimer>.has(return_to_main)) {
						ECS::registry<TransitionTimer>.emplace(return_to_main);
					}
					EndModeSystem::in_end_mode_menu = false;
					return;
				}
			}
		}


	}
	else if (OptionModeSystem::in_option_mode_menu) {

		if (button == GLFW_MOUSE_BUTTON_LEFT && mouse_action == GLFW_PRESS) {
			float max_volume = 100.f;
			for (auto entity : ECS::registry<Button>.entities) {
				auto& buttonComponent = ECS::registry<ButtonComponent>.get(entity);
				auto& text = ECS::registry<TextComponent>.get(entity);

				if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && text.text.find("volume_") != std::string::npos) {
					std::stringstream test(text.text);
					std::string segment;
					std::vector<std::string> seglist;

					while (std::getline(test, segment, '_')) {
						seglist.push_back(segment);
					}

					Mix_PlayChannel(-1, button_sound, 0);
					if (seglist.size() == 3) {
						if (seglist[1] == "left") {
							//do this
							if (seglist[2] == "0") {
								//bg sound decrease
								current_bg_volume -= 5.f;
								current_bg_volume = std::max(0.f, current_bg_volume);
							}
							else {
								//effect sound decrease
								current_effect_volume -= 5.f;
								current_effect_volume = std::max(0.f, current_effect_volume);
							}
						}
						else if (seglist[1] == "right") {
							//do this
							if (seglist[2] == "0") {
								//bg sound increase
								current_bg_volume += 5.f;
								current_bg_volume = std::min(100.f, current_bg_volume);
							}
							else {
								//effect sound increase
								current_effect_volume += 5.f;
								current_effect_volume = std::min(100.f, current_effect_volume);
							}
						}
					}
					//update the labels
					std::string bg_sound = std::to_string((int)(current_bg_volume));
					std::string effect_sound = std::to_string((int)(current_effect_volume));
					float padding = 100.f;
					for (auto entity2 : ECS::registry<Button>.entities) {
						auto& text2 = ECS::registry<TextComponent>.get(entity2);
						if (text2.text == "background_music") {
							auto& token_button_text = ECS::registry<Text>.get(entity2);
							token_button_text.content = bg_sound;

						}
						if (text2.text == "effect_music") {
							auto& token_button_text = ECS::registry<Text>.get(entity2);
							token_button_text.content = effect_sound;
						}
					}

					//reset the volume
					Mix_VolumeChunk(missile_sound, current_effect_volume / 8.f);
					Mix_VolumeMusic(current_bg_volume / 2.f);
				}
				else if (check_button_click(mouse_pos, buttonComponent.scale, buttonComponent.position) && (text.text == "exit_options")) {
					Mix_PlayChannel(-1, button_sound, 0);
					OptionModeSystem::in_option_mode_menu = false;
				}
			}
		}
	}
}

// Returns true if selection was changed
bool WorldSystem::handle_selection() {
	for (auto& fleet_e : ECS::registry<Fleet>.entities) {
		auto& fleet = ECS::registry<Fleet>.get(fleet_e);
		if (fleet.player_number > 0) {
			for (auto& boat : fleet.boats) {
				if (in_bounding_box(boat.get<BoundingBox>(), boat.get<Motion>().position, get_game_pos())) {

					// create skill buttons here
					float padding = 100.f;
					float offset = 50.f;
					vec2 s1_pos = vec2{ padding * 6.f, window_size.y - padding };
					vec2 s2_pos = vec2{ window_size.x - padding * 6.f, window_size.y - padding };
					vec2 s1_offset_pos = s1_pos + vec2{ offset, offset };
					vec2 s2_offset_pos = s2_pos + vec2{ offset, offset };
					vec2 skill_des_pos = vec2{ window_size.x - padding * 3.f, window_size.y * 0.5 };
					auto end_turn_pos = vec2{ window_size.x - padding * 2.f, window_size.y - padding };
					auto game_pause_pos = vec2{ padding , window_size.y - padding * 0.75f };
					auto pause_resume = vec2{ window_size.x / 2, window_size.y * 0.25f + 100.f };
					auto pause_option = vec2{ window_size.x / 2, window_size.y * 0.50f };
					auto pause_selection = vec2{ window_size.x / 2, window_size.y * 0.75f - 100.f };
					auto pause_return = vec2{ window_size.x / 2, window_size.y - 200.f };
					auto rank = boat.get<Status>().rank;
					// remove buttons
					for (auto entity : ECS::registry<Button>.entities) {
						if (entity.get<ButtonComponent>().position != end_turn_pos 
							&& entity.get<ButtonComponent>().position != game_pause_pos) {
							ECS::ContainerInterface::remove_all_components_of(entity);
						}
					}
					// remove displaytext
					for (auto removeable : ECS::registry<SkillUIRemoveComp>.entities) {
						ECS::ContainerInterface::remove_all_components_of(removeable);
					}
					for (auto removeable : ECS::registry<SkillUIRemoveTimerComp>.entities) {
						ECS::ContainerInterface::remove_all_components_of(removeable);
					}

					// readd selected displaytext
					if (boat.get<Status>().s1_selected) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_offset_pos, "checkmark_1", "checkmark.png", 1));
					}
					else if (boat.get<Status>().s1_cd == 1) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_offset_pos, "sk_one_1", "sk_one.png", 1));
					}
					else if (boat.get<Status>().s1_cd == 2) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_offset_pos, "sk_two_1", "sk_two.png", 1));
					}
					else if (boat.get<Status>().s1_cd == 3) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s1_offset_pos, "sk_three_1", "sk_three.png", 1));
					}

					if (boat.get<Status>().s2_selected) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_offset_pos, "checkmark_2", "checkmark.png", 1));
					}
					else if (boat.get<Status>().s2_cd == 1) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_offset_pos, "sk_one_2", "sk_one.png", 1));
					}
					else if (boat.get<Status>().s2_cd == 2) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_offset_pos, "sk_two_2", "sk_two.png", 1));
					}
					else if (boat.get<Status>().s2_cd == 3) {
						ECS::registry<SkillUIRemoveComp>.emplace(DisplayText::createDisplayText(s2_offset_pos, "sk_three_2", "sk_three.png", 1));
					}

					switch (rank) {
					case 10:
						// https://thenounproject.com/
						Button::createSkillButton(s1_pos, "recon_s1", "sk_recon_s1.png", boat);
						Button::createSkillButton(s2_pos, "recon_s2", "sk_recon_s2.png", boat);
						ECS::registry<SkillUIRemoveTimerComp>.emplace(DisplayText::createDisplayText(skill_des_pos, "sk_recon_des", "sk_recon_des.png", 1));
						Mix_PlayChannel(-1, name_recon, 0);
						break;
					case 20:
						Button::createSkillButton(s1_pos, "attack_s1", "sk_attack_s1.png", boat);
						Button::createSkillButton(s2_pos, "attack_s2", "sk_attack_s2.png", boat);
						ECS::registry<SkillUIRemoveTimerComp>.emplace(DisplayText::createDisplayText(skill_des_pos, "sk_attack_des", "sk_attack_des.png", 1));
						Mix_PlayChannel(-1, name_attack, 0);
						break;
					case 30:
						Button::createSkillButton(s1_pos, "reg_s1", "sk_reg_s1.png", boat);
						Button::createSkillButton(s2_pos, "reg_s2", "sk_reg_s2.png", boat);
						ECS::registry<SkillUIRemoveTimerComp>.emplace(DisplayText::createDisplayText(skill_des_pos, "sk_reg_des", "sk_reg_des.png", 1));
						Mix_PlayChannel(-1, name_reg, 0);
						break;
					case 40:
						Button::createSkillButton(s1_pos, "big_s1", "sk_big_s1.png", boat);
						Button::createSkillButton(s2_pos, "big_s2", "sk_big_s2.png", boat);
						ECS::registry<SkillUIRemoveTimerComp>.emplace(DisplayText::createDisplayText(skill_des_pos, "sk_big_des", "sk_big_des.png", 1));
						Mix_PlayChannel(-1, name_big, 0);
					}

					//cout << "Clicked boat\n";
					if (shift_mod) {
						boat.get<Selectable>().selected = true;
					}
					else if (ctrl_mod) {
						boat.get<Selectable>().selected = false;
					}
					else {
						for (auto& selectable : ECS::registry<Selectable>.components) {
							selectable.selected = false;
						}
						boat.get<Selectable>().selected = true;
					}
					return true;
				}
			}
		}
	}
	return false;
}

void WorldSystem::handle_movement(bool queue) {
	vec2 game_pos = get_game_pos();
	for (auto boat : ECS::registry<Boat>.entities) {
		if (boat.get<Selectable>().selected) {
			auto& motion = boat.get<Motion>();
			auto& spline = boat.get<MotionSpline>();
			auto& health = boat.get<Boat>().health;
			//auto &health_motion = health.get<Motion>();
			//auto &health_spline = health.get<MotionSpline>();
			auto& path = boat.get<MovementPath>();
			//TODO 
			spline.setPoints(path.position_coords, true);
			//health_spline.setPoints(path.position_coords, true);
			/*
			if (queue && spline.isMoving()) {
				spline.addPoint(game_pos);
				health_spline.addPoint(game_pos - vec2(0.f, health_bar_padding));
			} else {
				spline.setPoints({motion.position, game_pos}, true);
				health_spline.setPoints({health_motion.position, game_pos - vec2(0.f, health_bar_padding)}, true);
			}
			*/
			if (queue)
			{

			}
		}
	}
}

void WorldSystem::getTimer() {
	ECS::Entity c_phase;
	ECS::Entity timer;
	ECS::Entity round;
	ECS::Entity remaining;
	ECS::registry<Text>.insert(
		c_phase,
		Text("Phase: " + curr_phase, font, { window_size.x - 400.f, 50.f }));
	ECS::registry<Text>.insert(
		timer,
		Text("Timer: " + std::to_string(lastTime), font, { window_size.x - 300.f, 100.f }));
	float padding = 100.f;
	ECS::registry<Text>.insert(
		remaining,
		Text("Remaining Enemies: " + std::to_string(curr_num_enemies), font, { padding - 50.f, window_size.y - padding - 20.f }));
	ECS::registry<Text>.get(remaining).scale = 0.7;
	ECS::registry<Text>.insert(
		round,
		Text("Round: " + std::to_string(curr_round), font, { window_size.x / 2.f - 200.f, 50.f }));
}

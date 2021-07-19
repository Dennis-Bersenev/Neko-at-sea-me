#pragma once

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <tiny_ecs.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
static const float PI = 3.14159265359f;

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

// The 'Transform' component handles transformations passed to the Vertex shader
struct Transform {
	mat4 mat = glm::mat4(1.0f);
	void scale(vec2 scale);
	void rotate(float degrees);
	void translate(vec2 offset);
};

struct Transform2d {
	mat3 mat = glm::mat3(1.0f);
	void scale2d(vec2 scale);
	void rotate2d(float degrees);
	void translate2d(vec2 offset);
};

// Component to keep track of pos, angle, scale
struct Motion {
    vec2 position = { 0, 0 };
    float angle = 0;
    vec2 scale = { 10, 10 };
	bool isColliding = false;
};

// Component store queue of points to draw
struct MovementPath {
	std::vector<float> render_coords;
	std::vector<vec2> position_coords;
	bool drawing = false;
	bool ready = false;
};

//Component for button 
struct ButtonComponent {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 scale = { 10, 10 };
};

//Component for displaytext
struct DisplayTextComponent {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 scale = { 10, 10 };
};

//story component
struct StoryComponent {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 scale = { 10, 10 };
};

//Component for cats
struct CatComponent {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 scale = { 10, 10 };
	float frame = 0;
	float frames = 0;
	float row = 0;
	float rows = 0;
	float animation_frame_width = 0;
	float animation_frame_height = 0;
	float animationSpeed = 0;
	float elapsedTime = 0;
	float texture_x_Position_UV = 0;
	float texture_y_Position_UV = 0;
};

//Component for help 
struct HelpComponent {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 scale = { 10, 10 };
};

// Movement handled by the physics system
// repurpose to handle boids by yichen 04/06/2021
struct DynamicMovement {
	vec2 velocity = { 0, 0 };
	vec2 momentum = { 0.0f, 0.0f }; //For advanced
};

// Movement handled by the world update loop
struct FixedMovement {
    float speed = 0;
};

struct TextComponent {
	std::string text = "";
};

struct Scene {
	int scene = 0;
	bool skip = false;
    int debug = 0;
};

struct StoryLevel {
	bool after_battle = false;
	std::string level = "";
};

struct Fleet {
	int player_number = 0;
	int tokens = 20;
	std::string catType = "";
	std::vector<ECS::Entity> boats;
	std::map<int, int> boat_selections;
};

struct Obstacles {
    std::vector<ECS::Entity> obstacles;
};

struct AIComponent {
};

struct PlayerComponent {
};

// shark or boat
struct Status {
	std::string name = "";
	int cost = 0;
	int rank = 0;
	int player_number = 0;
	int s1_cd = 0;
	int s2_cd = 0;
	float weapon_damage = 0.f;
	float collision_damage = 1.f;
	float attack_range = 0.f;
	float view_range = 0.f;
	float health = 0.f;
	float maxHealth = 0.f;
	float speed = 0.f;
	float maxSpeed = 0.f;
	bool s1_selected = false;
	bool s2_selected = false;
};

struct MissileComponent {
	float damage = 0.f;
	float remaining_ms = 10000; // 10 seconds * 100 dist/seconds = 1000 dist in the life time.
	ECS::Entity target;
};

struct CrateComponent {
	bool isUsed = false;
};

struct SkillButtonComponent {
	ECS::Entity selfboat; // this self is boat
};

struct SkillUIRemoveComp {
};

struct SkillUIRemoveTimerComp {
	float timer = 5000.f;
};

// Entities with a Collider component will be processed by the Physics collision system
struct Collider {};

// Added it as a component
struct BoundingBox {
	float x_dim = 1.0f;
	float y_dim = 1.0f;
};

inline bool in_bounding_box(const BoundingBox & bb, const vec2 & boat_pos, const vec2 & mouse_pos)
{
	// boat_pos.x + bb.x_dim <= mouse_pos.x <= boat_pos.x + bb.x_dim
	//							and
	// boat_pos.y + bb.y_dim <= mouse_pos.y <= boat_pos.y + bb.y_dim
	return (((boat_pos.x - bb.x_dim <= mouse_pos.x) && (mouse_pos.x <= boat_pos.x + bb.x_dim)) 
		&& ((boat_pos.y - bb.y_dim <= mouse_pos.y) && (mouse_pos.y <= boat_pos.y + bb.y_dim)));
}
#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_components.hpp"
#include "motion_spline.hpp"
#include "shader.hpp"
#include "program.hpp"
#include "island.hpp"

struct InstancedMesh;
struct ShadedMesh;

// OpenGL utilities
void gl_has_errors();

// System responsible for setting up OpenGL and for rendering all the 
// visual entities in the game
class RenderSystem
{
public:
	// Initialize the window
	RenderSystem(GLFWwindow& window);

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw(vec2 window_size_in_game_units, const mat4& view, const mat4& p_proj, const mat4& o_proj, float elapsed_ms, int current_scene);

	// Expose the creating of visual representations to other systems
	static void createSprite(ShadedMesh& mesh_container, std::string texture_path, std::string shader_name);
	static void createSpriteSheet(ShadedMesh& mesh_container, std::string texture_path, std::string shader_name, float frames, float rows);
	static void createColoredMesh(ShadedMesh& mesh_container, std::string shader_name);

private:
	// Initialize the screeen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water shader
	void initScreenTexture();

	// Internal drawing functions for each entity type
	void drawMesh(ECS::Entity entity, const mat4& view, const mat4& perspective_proj, const mat4& orth_proj, const vec2& window_dims);
    void drawTexturedMesh(ECS::Entity entity, const mat3 &projection, float elapsed_ms);
	void drawTexturedMesh(ShadedMesh& texmesh, vec2 position, float angle, vec2 scale, const mat3& projection, float elapsed_ms);
	void drawToScreen();
	void drawUIScreen(ECS::Entity entity, glm::mat3 projection_2D, float elapsed_ms);
	void drawSpline(ECS::Entity entity, const mat4& view, const mat4& perspective_proj);
	void drawHealth(ECS::Entity entity, const mat4 &view, const mat4 &perspective_proj, const mat4 &boat_matrix,
                    float max_health, float health);
	void drawWater(const mat3& projection, float elapsed_ms);
	// Window handle
	GLFWwindow& window;

	// Screen texture handles
	GLuint frame_buffer;
	ShadedMesh screen_sprite;
	GLResource<RENDER_BUFFER> depth_render_buffer_id;
	ECS::Entity screen_state_entity;
	
	// Game texture related
	unsigned int game_textures[120];
	int cycle_count = 0;
	bool debug = true;

	float vertices[20] = {
		// positions          // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[6] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
};

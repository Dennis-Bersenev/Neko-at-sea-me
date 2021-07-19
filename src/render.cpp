// internal
#include "render.hpp"
#include "render_components.hpp"
#include "tiny_ecs.hpp"
#include "text.hpp"
#include "motion_spline.hpp"
#include "boat.hpp"
#include "selectable.hpp"
#include "ai_player.hpp"
#include "debug.hpp"
#include "shark.hpp"

#include <iostream>
#include <cat.hpp>


void RenderSystem::drawWater(const mat3& projection, float elapsed_ms)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	gl_has_errors();

	/* Setting up the shaders */
	// vertex shader
	shader* vs = new shader("data/shaders/test.vs.glsl", GL_VERTEX_SHADER);
	unsigned int vertexShader = vs->getID();
	
	// fragment shader
	shader* fs = new shader("data/shaders/test.fs.glsl", GL_FRAGMENT_SHADER);
	unsigned int fragmentShader = fs->getID();

	// link shaders
	program* p1 = new program(vertexShader, fragmentShader);
	int shaderProgram = p1->getID();
	glUseProgram(shaderProgram);

	//Querying for external var locs and uniforms
	GLint in_position_loc = glGetAttribLocation(shaderProgram, "in_pos");
	GLint texture_coord_loc = glGetAttribLocation(shaderProgram, "in_uv");
	
	GLint light_colour_uloc = glGetUniformLocation(shaderProgram, "light_colour");
	GLint light_dir_uloc = glGetUniformLocation(shaderProgram, "light_dir");
	glUniform3f(light_colour_uloc, 0.9f,0.9f,0.9f); //rgb components of light_colour; white atm
	glUniform3f(light_dir_uloc, 0.5, 0.5f, -0.7f); //direction vector of sunlight

	//set the time
	GLuint time_uloc = glGetUniformLocation(shaderProgram, "time");
	glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 10.0f));
	//std::cout << glfwGetTime() << std::endl;
	gl_has_errors();
	
	// Setting up the buffers and pointers
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	gl_has_errors();

	// Pos
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(in_position_loc);
	gl_has_errors();
	// UV
	glVertexAttribPointer(texture_coord_loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texture_coord_loc);
	gl_has_errors();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_textures[cycle_count]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	gl_has_errors();

	//Update cycle counter
	if (!debug)
	{
		cycle_count++;
		if (cycle_count >= 120)
			cycle_count = 0;
	}
	
	// Clean-up
	delete vs;
	vs = nullptr;
	delete fs;
	fs = nullptr;
	delete p1;
	p1 = nullptr;
	return;

	return;
}

void RenderSystem::drawSpline(ECS::Entity entity, const mat4& view, const mat4& perspective_proj)
{
	auto& path = ECS::registry<MovementPath>.get(entity);

	std::vector<float> vec_arr = path.render_coords;
	if (vec_arr.size() <= 0)
		return;
	float* a = vec_arr.data();

	// TODO: figure this code out! Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glLineWidth(1.0);
	gl_has_errors();

	/* Setting up the shaders */
	// vertex shader
	shader* vs = new shader("data/shaders/basic_mesh.vs.glsl", GL_VERTEX_SHADER);
	unsigned int vertexShader = vs->getID();
	// fragment shader
	shader* fs = new shader("data/shaders/basic_mesh.fs.glsl", GL_FRAGMENT_SHADER);
	unsigned int fragmentShader = fs->getID();

	// link shaders
	program* p1 = new program(vertexShader, fragmentShader);
	int shaderProgram = p1->getID();
	glUseProgram(shaderProgram);

	// Attribute and Uniform setup
	GLint in_position_loc = glGetAttribLocation(shaderProgram, "in_pos");
	GLint projection_uloc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projection_uloc, 1, GL_FALSE, (float*)&perspective_proj);
	gl_has_errors();

	Transform transform;
	transform.translate(vec2(0, 0));
	transform.rotate(0);
	transform.scale(vec2(1, 1));
	GLint model_uloc = glGetUniformLocation(shaderProgram, "model");
	GLint view_uloc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(model_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix4fv(view_uloc, 1, GL_FALSE, (float*)&view);



	// Buffer setup
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vec_arr.size() * sizeof(*a), a, GL_STATIC_DRAW);

	//Pointer for positions 
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(a[0]), (void*)0);

	glDrawArrays(GL_LINE_STRIP, 0, vec_arr.size() / 2);
	glBindVertexArray(0);

	// Clean-up
	delete vs;
	vs = nullptr;
	delete fs;
	fs = nullptr;
	delete p1;
	p1 = nullptr;
	return;
}

void
RenderSystem::drawHealth(ECS::Entity entity, const mat4 &view, const mat4 &perspective_proj, const mat4 &boat_matrix,
                         float max_health, float health)
{
	// Getting info from the entity
	auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;

	// Setting up coord matrix for health bar
	glm::mat4 health_matrix = glm::mat4(1.0f);
	health_matrix = glm::scale(health_matrix, vec3(1.f, 0.1f, 1.0f));
	health_matrix = glm::translate(health_matrix, glm::vec3(0.f, 7.5f, 0.0f));
	Transform transform;
	transform.mat = boat_matrix * health_matrix;
	transform.scale(vec2(health/max_health, 1.f));

	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();


	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");

	
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(sizeof(vec3)));
	gl_has_errors();

	// Uniform setup
	GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
	glUniformMatrix4fv(projection_uloc, 1, GL_FALSE, (float*)&perspective_proj);
	gl_has_errors();
	

	// Common uniform setup
	GLint model_uloc = glGetUniformLocation(texmesh.effect.program, "model");
	GLint view_uloc = glGetUniformLocation(texmesh.effect.program, "view");
	glUniformMatrix4fv(model_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix4fv(view_uloc, 1, GL_FALSE, (float*)&view);


	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);


	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

void RenderSystem::drawMesh(ECS::Entity entity, const mat4& view, const mat4& perspective_proj, const mat4& orth_proj, const vec2& window_dims)
{

	// Getting info from the entity
	auto& motion = ECS::registry<Motion>.get(entity);
	auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
	
	Transform transform;
	transform.translate(vec2(motion.position.x - window_dims.x, window_dims.y - motion.position.y));
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();


	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(texmesh.effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");

	if (in_texcoord_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(sizeof(vec3))); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
		gl_has_errors();

		// Uniform setup
		GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
		glUniformMatrix4fv(projection_uloc, 1, GL_FALSE, (float*)&orth_proj);
		gl_has_errors();
	}
	else if (in_color_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(sizeof(vec3)));
		gl_has_errors();

		// Uniform setup
		GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
		glUniformMatrix4fv(projection_uloc, 1, GL_FALSE, (float*)&perspective_proj);
		gl_has_errors();
	}
	else
	{
		throw std::runtime_error("This type of entity is not yet supported");
	}
	gl_has_errors();

	// Common uniform setup
	GLint model_uloc = glGetUniformLocation(texmesh.effect.program, "model");
	GLint view_uloc = glGetUniformLocation(texmesh.effect.program, "view");
	glUniformMatrix4fv(model_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix4fv(view_uloc, 1, GL_FALSE, (float*)&view);


	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);


	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}




void RenderSystem::drawTexturedMesh(ECS::Entity entity, const mat3 &projection, float elapsed_ms) {
    auto& motion = ECS::registry<Motion>.get(entity);
    auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
    drawTexturedMesh(texmesh, motion.position, motion.angle, motion.scale, projection, elapsed_ms);
}

// Overload to allow calling directly without an entity
void RenderSystem::drawTexturedMesh(ShadedMesh& texmesh, vec2 position, float angle, vec2 scale, const mat3& projection, float elapsed_ms)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	Transform2d transform2d;
	transform2d.translate2d(position);
	transform2d.rotate2d(angle);
	transform2d.scale2d(scale);

	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();

	GLint transform2d_uloc = glGetUniformLocation(texmesh.effect.program, "transform2d");
	GLint projection2d_uloc = glGetUniformLocation(texmesh.effect.program, "projection2d");
	gl_has_errors();

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(texmesh.effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");
	if (in_texcoord_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(sizeof(vec3))); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
	}
	else if (in_color_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(sizeof(vec3)));

		if (false)
		{
			GLint light_up_uloc = glGetUniformLocation(texmesh.effect.program, "light_up");

			// !!! TODO A1: set the light_up shader variable using glUniform1i
			(void)light_up_uloc; // placeholder to silence unused warning until implemented
		}
	}
	else
	{
		throw std::runtime_error("This type of entity is not yet supported");
	}
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(texmesh.effect.program, "fcolor");
	glUniform3fv(color_uloc, 1, (float*)&texmesh.texture.color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);
	//GLsizei num_triangles = num_indices / 3;

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform2d_uloc, 1, GL_FALSE, (float*)&transform2d.mat);
	glUniformMatrix3fv(projection2d_uloc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

// Draw the intermediate texture to the screen, with some distortion to simulate water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	glUseProgram(screen_sprite.effect.program);
	glBindVertexArray(screen_sprite.mesh.vao);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(&window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Disable alpha channel for mapping the screen texture onto the real screen
	glDisable(GL_BLEND); // we have a single texture without transparency. Areas with alpha <1 cab arise around the texture transparency boundary, enabling blending would make them visible.
	glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, screen_sprite.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_sprite.mesh.ibo); // Note, GL_ELEMENT_ARRAY_BUFFER associates indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// Draw the screen texture on the quad geometry
	gl_has_errors();

	// Set clock
	GLuint time_uloc = glGetUniformLocation(screen_sprite.effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(screen_sprite.effect.program, "darken_screen_factor");
	glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 10.0f));
	auto& screen = ECS::registry<ScreenState>.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();

	// Set the vertex position and vertex texture coordinates (both stored in the same VBO)
	GLint in_position_loc = glGetAttribLocation(screen_sprite.effect.program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	GLint in_texcoord_loc = glGetAttribLocation(screen_sprite.effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // note the stride to skip the preceeding vertex position
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_sprite.texture.texture_id);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr); // two triangles = 6 vertices; nullptr indicates that there is no offset from the bound index buffer
	glBindVertexArray(0);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw(vec2 window_size_in_game_units, const mat4& view, const mat4& p_proj, const mat4& o_proj, float elapsed_ms, int current_scene)
{
	// Getting size of window
	ivec2 frame_buffer_size; // in pixels
	glfwGetFramebufferSize(&window, &frame_buffer_size.x, &frame_buffer_size.y);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Clearing backbuffer
	glViewport(0, 0, frame_buffer_size.x, frame_buffer_size.y);
	glDepthRange(0, 10);
	glClearColor(0.f, 0.5f, 1.f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;
	float right = window_size_in_game_units.x;
	float bottom = window_size_in_game_units.y;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	// Getting all the player boats
	std::vector<ECS::Entity> player_boats;
	for (auto& boat : ECS::registry<Boat>.entities)
	{
		if (!ECS::registry<AIComponent>.has(boat))
		{
			player_boats.push_back(boat);
		}

	}

	if (current_scene == 2)
	{
		//Add water rendering 
		vec2 window_dims = vec2(window_size_in_game_units.x / 2, window_size_in_game_units.y / 2);
		drawWater(projection_2D, elapsed_ms);
	}

	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		//Restructuring to render boats properly.
		if (ECS::registry<Boat>.has(entity))
		{
			vec2 window_dims = vec2(window_size_in_game_units.x / 2, window_size_in_game_units.y / 2);
			auto& motion = ECS::registry<Motion>.get(entity);

			if (!ECS::registry<AIComponent>.has(entity))
			{
				drawMesh(entity, view, p_proj, o_proj, window_dims);
				Transform transform;
				transform.translate(vec2(motion.position.x - window_dims.x, window_dims.y - motion.position.y));
				//transform.rotate(motion.angle);
				transform.scale(motion.scale);
				ECS::Entity& hp = entity.get<Boat>().health;
				auto& status = ECS::registry<Status>.get(entity);
				auto& max_health = status.maxHealth;
				auto& health = status.health;
				drawHealth(hp, view, p_proj, transform.mat, max_health, health);
			}
			else
			{
				// Checks if AI is in sight range of any player boats
				for (auto& player : player_boats)
				{
					vec2 player_pos = ECS::registry<Motion>.get(player).position;
					vec2 enemy_pos = motion.position;
					float player_sight = ECS::registry<Status>.get(player).view_range;
					if (length(player_pos - enemy_pos) < player_sight)
					{
						//in-range, render
						drawMesh(entity, view, p_proj, o_proj, window_dims);
						Transform transform;
						transform.translate(vec2(motion.position.x - window_dims.x, window_dims.y - motion.position.y));
						//transform.rotate(motion.angle);
						transform.scale(motion.scale);
						ECS::Entity& hp = entity.get<Boat>().health;
						auto& status = ECS::registry<Status>.get(entity);
						auto& max_health = status.maxHealth;
						auto& health = status.health;
						drawHealth(hp, view, p_proj, transform.mat, max_health, health);
					}
				}
			}
		}
		else if (ECS::registry<Island>.has(entity))
			drawMesh(entity, view, p_proj, o_proj, vec2(window_size_in_game_units.x / 2, window_size_in_game_units.y / 2));
		else if (ECS::registry<Motion>.has(entity))
		{
			for (auto& player : player_boats)
			{
				vec2 player_pos = ECS::registry<Motion>.get(player).position;
				vec2 object_pos = ECS::registry<Motion>.get(entity).position;
				float player_sight = ECS::registry<Status>.get(player).view_range;
				if (length(player_pos - object_pos) < player_sight)
					drawMesh(entity, view, p_proj, o_proj, vec2(window_size_in_game_units.x / 2, window_size_in_game_units.y / 2));
			}
		}
		else 
			continue;
		gl_has_errors();
	}
	//render everything not in debug

	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (!ECS::registry<DebugModeComponent>.has(entity) && !ECS::registry<PauseModeComponent>.has(entity)
			&& !ECS::registry<OptionModeComponent>.has(entity)) {
            drawUIScreen(entity, projection_2D,  elapsed_ms);
		}
	}
	
	// Draw movement path
	for (ECS::Entity entity : ECS::registry<MovementPath>.entities)
	{
		auto& path = ECS::registry<MovementPath>.get(entity);
//		if (path.ready)
//		{
			drawSpline(entity, view, p_proj);
//		}
		gl_has_errors();
	}

	for (ECS::Entity entity : ECS::registry<Text>.entities)
	{
		Text& text = ECS::registry<Text>.get(entity);
		if (!ECS::registry<DebugModeComponent>.has(entity) && !ECS::registry<PauseModeComponent>.has(entity) 
			&& !ECS::registry<OptionModeComponent>.has(entity)) {
			drawText(text, frame_buffer_size);
		}
	}

	//this is for the pause
	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (ECS::registry<PauseModeComponent>.has(entity)) {
            drawUIScreen(entity, projection_2D,   elapsed_ms);
		}
	}

	for (ECS::Entity entity : ECS::registry<Text>.entities)
	{
		Text& text = ECS::registry<Text>.get(entity);
		if (ECS::registry<PauseModeComponent>.has(entity)) {
			drawText(text, window_size_in_game_units);
		}
	}

	//this is for the options
	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (ECS::registry<OptionModeComponent>.has(entity)) {
			drawUIScreen(entity, projection_2D, elapsed_ms);
		}
	}

	for (ECS::Entity entity : ECS::registry<Text>.entities)
	{
		Text& text = ECS::registry<Text>.get(entity);
		if (ECS::registry<OptionModeComponent>.has(entity)) {
			drawText(text, frame_buffer_size);
		}
	}

	//this is for the ending
	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (ECS::registry<EndModeComponent>.has(entity)) {
			drawUIScreen(entity, projection_2D, elapsed_ms);
		}
	}

	for (ECS::Entity entity : ECS::registry<Text>.entities)
	{
		Text& text = ECS::registry<Text>.get(entity);
		if (ECS::registry<EndModeComponent>.has(entity)) {
			drawText(text, frame_buffer_size);
		}
	}

	//render everything with debug
	for (ECS::Entity entity : ECS::registry<ShadedMeshRef>.entities)
	{
		if (ECS::registry<DebugModeComponent>.has(entity)) {
            drawUIScreen(entity, projection_2D, elapsed_ms);
		}
	}

	for (ECS::Entity entity : ECS::registry<Text>.entities)
	{
		Text& text = ECS::registry<Text>.get(entity);
		if (ECS::registry<DebugModeComponent>.has(entity)) {
			drawText(text, window_size_in_game_units);
		}
	}

	if (DebugSystem::in_debug_mode) {
        for (auto& ai_player : ECS::registry<AIPlayer>.entities) {
            auto &ai_fleet = ECS::registry<Fleet>.get(ai_player);
            for (auto boat : ai_fleet.boats) {
                MotionSpline &spline = ECS::registry<MotionSpline>.get(boat);
                if (!spline.isMoving()) continue;
                Motion &motion = ECS::registry<Motion>.get(boat);
                ShadedMesh &resource = spline.getShadedMesh(motion.position);
                drawTexturedMesh(resource, {0, 0}, 0.f, {1, 1}, projection_2D, elapsed_ms);
                gl_has_errors();
            }
        }

		for (auto& shark : ECS::registry<Shark>.entities) {
			MotionSpline& spline = shark.get<MotionSpline>();
			if (!spline.isMoving()) continue;
			Motion& motion = ECS::registry<Motion>.get(shark);
			ShadedMesh& resource = spline.getShadedMesh(motion.position);
			drawTexturedMesh(resource, { 0, 0 }, 0.f, { 1, 1 }, projection_2D, elapsed_ms);
			gl_has_errors();
		}
	}

	// Truly render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(&window);
}

void gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR)
		return;

	const char* error_str = "";
	while (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		std::cerr << "OpenGL:" << error_str << std::endl;
		error = glGetError();
	}
	throw std::runtime_error("last OpenGL error:" + std::string(error_str));
}
void RenderSystem::drawUIScreen(ECS::Entity entity, glm::mat3 projection_2D, float elapsed_ms) {
	if (ECS::registry<ButtonComponent>.has(entity)) {
		auto& buttoncomponent = ECS::registry<ButtonComponent>.get(entity);
		auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
		drawTexturedMesh(texmesh, buttoncomponent.position, buttoncomponent.angle, buttoncomponent.scale, projection_2D, elapsed_ms);
	}
	gl_has_errors();
	if (ECS::registry<DisplayTextComponent>.has(entity)) {
		auto& dtcomponent = ECS::registry<DisplayTextComponent>.get(entity);
		auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
		drawTexturedMesh(texmesh, dtcomponent.position, dtcomponent.angle, dtcomponent.scale, projection_2D, elapsed_ms);
	}
	if (ECS::registry<HelpComponent>.has(entity)) {
		auto& hcomponent = ECS::registry<HelpComponent>.get(entity);
		auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
		drawTexturedMesh(texmesh, hcomponent.position, hcomponent.angle, hcomponent.scale, projection_2D, elapsed_ms);
	}

    if (ECS::registry<CatComponent>.has(entity)) {
        auto& hcomponent = ECS::registry<CatComponent>.get(entity);
        auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
        drawTexturedMesh(texmesh, hcomponent.position, hcomponent.angle, hcomponent.scale, projection_2D, elapsed_ms);
    }

	if (ECS::registry<StoryComponent>.has(entity)) {
		auto& hcomponent = ECS::registry<StoryComponent>.get(entity);
		auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
		drawTexturedMesh(texmesh, hcomponent.position, hcomponent.angle, hcomponent.scale, projection_2D, elapsed_ms);
	}
	gl_has_errors();
}

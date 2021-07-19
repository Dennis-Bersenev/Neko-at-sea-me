#version 330

// Input attributes
in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
out vec2 vpos;

// Application data
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
void main()
{
	vpos = in_position.xy; // local coordinates before transform

	vcolor = in_color;

	gl_Position = projection * view * model * vec4(in_position.xy, 0.0f, 1.0f);
}
#version 330

// Input attributes
in vec2 in_pos;

// Uniforms
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	vec4 vpos = vec4(in_pos.xy, 0.0, 1.0);
	mat4 model2 = mat4(1.0);
	gl_PointSize = 10;
	gl_Position = projection * view * model * vpos;
}
#version 330

// Input attributes
in vec3 in_pos;
in vec2 in_uv;

out vec2 v_uv;

void main()
{
	v_uv = in_uv;
	gl_Position = vec4(in_pos.xyz, 1.0);
}
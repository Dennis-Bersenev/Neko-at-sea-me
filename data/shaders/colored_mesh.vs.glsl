#version 330 

// Input attributes
in vec3 in_color;
in vec3 in_position;

out vec3 vcolor;

// Application data
uniform mat3 transform2d;
uniform mat3 projection2d;

void main()
{
	vcolor = in_color;
	vec3 pos = projection2d * transform2d * vec3(in_position.xy, 1.0); // why not simply *in_position.xyz ?
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
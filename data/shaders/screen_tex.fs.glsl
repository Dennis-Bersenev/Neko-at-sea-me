#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;


layout(location = 0) out vec4 color;

vec4 fade_color(vec4 in_color) 
{
	vec4 color = in_color;
	if (darken_screen_factor > 0)
		color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);

	return color;
}

void main()
{
 color = texture(screen_texture, texcoord);
 color = fade_color(color);

}
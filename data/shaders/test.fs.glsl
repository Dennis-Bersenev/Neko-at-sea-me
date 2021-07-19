#version 330

in vec2 v_uv;

out vec4 colour;

uniform vec3 light_colour;
uniform vec3 light_dir;
uniform sampler2D ourTexture;

uniform float time;
//citation: leva lamp shader  - https://thebookofshaders.com/edit.php#11/lava-lamp.frag
//citation: Noise - https://thebookofshaders.com/11/

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
void main(){

	vec4 normal = 0.5 * texture(ourTexture, v_uv) + 0.5;
	vec3 v_norm = normal.xyz;

	// Constants setup (should do in render.cpp)
	float ka = 0.8f;
	float kd = 0.5f;
	float ks = 0.8f;
	float shininessVal = 20.0f;
	vec3 ambientColour = (1.0f/255) * vec3(22, 189, 233);

	// Get n and l 
	vec3 l = normalize(light_dir);
	vec3 n = normalize(v_norm);

	// Diffuse calculation
	vec3 diffuseColour = light_colour * max(0.0, dot(n, l));

	// Specular calculation
	vec3 v = normalize(-(vec3(0.0f, 0.0f, 1.0f)));					//view vector
	vec3 h = (v + l) / (length(v + l));								//heuristic vector

	float spec = pow(max(0.0, dot(h, n)), shininessVal);

	vec3 specularColour = spec * light_colour;

	vec3 i_colour = ka * ambientColour + kd * diffuseColour + ks * specularColour;

	vec4 curr = texture(ourTexture, v_uv);
	vec2 curr_text = vec2(curr.xy);

	curr_text.x = curr_text.x *3.f;

	vec3 blue_color = vec3(0.9,0.3,0.1);
    vec2 pos = vec2(curr_text);
    float DF = 0.0;

	float a = 0.0;
    vec2 vel = vec2(time * 0.000000001f);
    DF += snoise(pos+vel)*0.25f + 0.25f;

	a = snoise(pos*vec2(cos(time*0.75f),sin(time*0.1f))*0.1)*3.1415;
    vel = vec2(cos(a),sin(a));
    DF += snoise(pos+vel)*.15+.15;

	blue_color = blue_color + vec3( smoothstep(.1,.75,0.4*fract(DF)));
	blue_color = vec3(0.9,0.9,0.9) - 0.7 * blue_color;


	colour = vec4(blue_color, 1.0);

}

/*
void main()
{
	vec4 normal = 0.5 * texture(ourTexture, v_uv) + 0.5;
	vec3 v_norm = normal.xyz;

	// Constants setup (should do in render.cpp)
	float ka = 0.8f;
	float kd = 0.5f;
	float ks = 0.8f;
	float shininessVal = 20.0f;
	vec3 ambientColour = (1.0f/255) * vec3(22, 189, 233);

	// Get n and l 
	vec3 l = normalize(light_dir);
	vec3 n = normalize(v_norm);

	// Diffuse calculation
	vec3 diffuseColour = light_colour * max(0.0, dot(n, l));

	// Specular calculation
	vec3 v = normalize(-(vec3(0.0f, 0.0f, 1.0f)));					//view vector
	vec3 h = (v + l) / (length(v + l));								//heuristic vector

	float spec = pow(max(0.0, dot(h, n)), shininessVal);

	vec3 specularColour = spec * light_colour;

	colour = vec4(ka * ambientColour + kd * diffuseColour + ks * specularColour, 1.0);
}
*/
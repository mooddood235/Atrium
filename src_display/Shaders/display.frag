#version 460 core

uniform sampler2D film;

in vec2 fragCoord;
out vec4 fragColor;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main(){
	vec2 uv = (fragCoord + 1.0) / 2.0;
    vec3 color = textureLod(film, uv, 0.0).rgb;
	fragColor = vec4(color / (color + 0.5), 0.0);
}
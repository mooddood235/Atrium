#version 460 core

uniform sampler2D film;

in vec2 fragCoord;
out vec4 fragColor;

void main(){
	vec2 uv = (fragCoord + 1.0) / 2.0;
	fragColor = textureLod(film, uv, 0.0);
}
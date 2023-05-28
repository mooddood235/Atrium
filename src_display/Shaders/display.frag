#version 460 core

uniform sampler2D film;

in vec2 fragCoord;
out vec4 fragColor;

void main(){
	fragColor = textureLod(film, fragCoord, 0.0);
}
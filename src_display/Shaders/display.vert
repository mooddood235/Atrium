#version 460 core

layout (location = 0) in vec3 pos;
out vec2 fragCoord;

void main(){
	gl_Position = vec4(pos, 1.0);
	fragCoord = pos.xy;
}
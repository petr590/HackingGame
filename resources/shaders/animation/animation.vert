#version 330 core
#include "common.glsl"

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;

out vec3 fragPos;

void main() {
	vec4 pos = model * vec4(position, 1.0);
	fragPos = vec3(pos);
	gl_Position = projection * view * pos;
}

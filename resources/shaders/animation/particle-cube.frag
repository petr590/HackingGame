#version 330 core
#include "common.glsl"

uniform float alpha;
uniform uint mode;

out vec4 result;

const uint MODE_FADING = 0u;
const uint MODE_SOLID  = 1u;

void main() {
	switch (mode) {
		case MODE_FADING:
			result = GRAY(0.0, alpha);
			break;
		
		case MODE_SOLID:
			result = GRAY(0.0, 1.0);
			break;
		
		default:
			result = GRAY(0.0, 0.0);
			break;
	}
}

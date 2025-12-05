#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform float progress;
uniform int seed;

in vec3 fragPos;

out vec4 result;

const float TILE_SIZE = 0.05;
const float END_TIME = 0.4;

const float SQUARE_ANGLE = radians(30.0);
const float SQUARE_ANGLE_SIN = sin(SQUARE_ANGLE);
const float SQUARE_ANGLE_COS = cos(SQUARE_ANGLE);

const int NOISE_OUTER_RADIUS = 25;
const int NOISE_INNER_RADIUS = 22;
const float NOISE_START = 0.05;
const float NOISE_CHANCE = 0.25;


vec4 getNoise(vec2 pos) {
	pos *= zoom(progress, NOISE_START, END_TIME, 175.0, 150.0);
	ivec2 ipos = ivec2(pos);
	
	if (dot(ipos, ipos) > NOISE_OUTER_RADIUS * NOISE_OUTER_RADIUS) {
		return GRAY(0.0, 0.0);
	}
	
	float noise = simpleNoise(seed, ipos);
	
	float alpha = noise > NOISE_CHANCE ? 0.0 :
			noise * (1.0 / NOISE_CHANCE) * zoom(progress, NOISE_START, END_TIME, 0.5, 0.0);
	
	return GRAY(0.0, alpha * smoothstep(NOISE_OUTER_RADIUS, NOISE_INNER_RADIUS, length(pos)));
}


void main() {
	
	if (progress >= END_TIME) {
		discard;
	}
	
	float alpha = max(0.0, mod(0.025 - progress, 0.1) - 0.05) * (5.5 - 20.0 * progress);
	
	vec2 pos = rotate(centerPos.xz - fragPos.xz, SQUARE_ANGLE_SIN, SQUARE_ANGLE_COS);
	vec2 dist = abs(pos);
	float size = 1.0 * progress + 0.1;
	
	if (dist.x <= size && dist.y <= size) {
		result = GRAY(0.0, alpha);
		
		if (progress > NOISE_START) {
			result = blend(result, getNoise(pos));
		}
		
		return;
	}
	
	float rectSize = size * 2.0;
	
	if (dist.x <= size && dist.y <= rectSize ||
		dist.x <= rectSize && dist.y <= size) {
		
		result = GRAY(0.0, alpha * 0.5);
		return;
	}
	
	discard;
}

#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform float progress;
uniform int seed;

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 result;

const float SMOOTH_WIDTH = 0.0015;

const float RING1_INIT_RADIUS = 0.04;
const float RING1_INIT_WIDTH = 0.01;

const float RING1_GROW_SPEED = 1.56;
const float RING1_COLOR_CHANGE_SPEED = 13.0;

const float RING1_FADE_START_TIME = 0.08;
const float RING1_FADE_END_TIME = 0.18;

const float NOISE_ANGLE = radians(30.0);
const float NOISE_ANGLE_SIN = sin(NOISE_ANGLE);
const float NOISE_ANGLE_COS = cos(NOISE_ANGLE);

const float NOISE_CHANCE = 0.25;
const float NOISE_COLOR_CHANGE_START = 0.0;
const float NOISE_COLOR_CHANGE_END = 0.08;
const float NOISE_FADE_START = 0.08;
const float NOISE_FADE_END = 0.15;

const float NOISE_INIT_RADIUS = 0.2;
const float NOISE_GROW_SPEED = 0.1;

const vec3 NOISE_COLOR1 = vec3(0.98, 0.27, 0.27);
const vec3 NOISE_COLOR2 = vec3(0.3);

const float WAIT_TIME = 0.128;

void drawRing1(float delayedProgress, float dist) {
	float radius = RING1_INIT_RADIUS + RING1_GROW_SPEED * delayedProgress;
	float width = RING1_INIT_WIDTH;
	
	float outer = smoothstep(radius + SMOOTH_WIDTH, radius, dist);
	float inner = smoothstep(radius - width, radius, dist);
	
	float color = max(0.0, zoom(delayedProgress, 0.0, 1.0, 1.0, 1.0 - RING1_COLOR_CHANGE_SPEED));
	float alpha = clamp(zoom(delayedProgress, RING1_FADE_START_TIME, RING1_FADE_END_TIME, 1.0, 0.0), 0.0, 1.0);
	
	result = blend(result, GRAY(color, alpha * inner * outer));
}

void drawNoise(float delayedProgress, float dist) {
	vec2 pos = rotate(centerPos.xz - fragPos.xz, NOISE_ANGLE_SIN, NOISE_ANGLE_COS);
	float radius = NOISE_INIT_RADIUS + NOISE_GROW_SPEED * delayedProgress;
	
	bool filled = simpleNoise(seed, ivec2(pos * 80.0)) < NOISE_CHANCE;
	
	if (filled) {
		vec3 color = mix(NOISE_COLOR1, NOISE_COLOR2, smoothstep(NOISE_COLOR_CHANGE_START, NOISE_COLOR_CHANGE_END, delayedProgress));
		float alpha = clamp(zoom(delayedProgress, NOISE_FADE_START, NOISE_FADE_END, 1.0, 0.0), 0.0, 1.0);
		float outer = smoothstep(radius + SMOOTH_WIDTH, radius, dist);
		
		result = blend(result, vec4(color, alpha * outer));
	}
}

void main() {
	result = vec4(0.0);
	
	float delayedProgress = max(0.0, progress - WAIT_TIME);
	float dist = distance(fragPos, centerPos);
	drawRing1(delayedProgress, dist);
	drawNoise(delayedProgress, dist);
	
//	result = GRAY(0.0, 1.0);
}

#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform float progress;
uniform int seed;

in vec3 fragPos;

out vec4 result;

const float TILE_SIZE = 0.05;
const float EPSILON   = 0.003;

const float CIRCLE_MAX_RADIUS = 2.0 * TILE_SIZE;

const float RING_START      = 0.05;
const float RING_END        = 0.55;
const float RING_MAX_RADIUS = 3.0 * TILE_SIZE;
const float RING_MAX_WIDTH  = 0.5 * TILE_SIZE;

const float RING1_FADE_START  = 0.75;
const float RING1_FADE_END    = 1.0;
const float RING1_NOISE_START = 0.75;
const float RING1_NOISE_END   = 1.0;

const float RING1_NOISE_ROT = radians(30.0);
const float RING1_NOISE_ROT_SIN = sin(RING1_NOISE_ROT);
const float RING1_NOISE_ROT_COS = cos(RING1_NOISE_ROT);

const float RING2_RADIUS = 1.15 * TILE_SIZE;
const float RING2_FADE   = 0.25;

const float RING3_MAX_RADIUS  = 1.75 * TILE_SIZE;


float invMix(float x, float y, float a) {
	return (a - x) / (y - x);
}


float getNoise(vec2 pos, float radius, float chance) {
	ivec2 ipos = ivec2(rotate(pos, RING1_NOISE_ROT_SIN, RING1_NOISE_ROT_COS) * (vec2(2.0, 1.0) * 20.0) / radius);
	return step(chance, simpleNoise(seed, ipos));
}


void drawCircle(float dist, float radius) {
	if (dist > radius) {
		return;
	}
	
	float inner = smoothstep(CIRCLE_MAX_RADIUS, 0.0, dist);
	float outer = smoothstep(radius, radius - EPSILON, dist);
	float alpha = zoom(progress, RING_START, RING_END, 1.0, 0.0);
	
	result = blend(result, vec4(1.0, 0.0, 0.0, inner * outer * alpha));
}


void drawRing1(float dist, float radius) {
	float width = zoom(progress, RING_START, RING_END, RING_MAX_WIDTH * 0.25, RING_MAX_WIDTH);
	
	float inner = smoothstep(radius - width,   radius, dist);
	float outer = smoothstep(radius + EPSILON, radius, dist);
	float alpha = clamp(sqrt(invMix(RING1_FADE_END, RING1_FADE_START, progress)), 0.0, 1.0);
	
	if (progress >= RING1_NOISE_START && progress <= RING1_NOISE_END) {
		float chance = mix(0.75, 1.0, sqrt(invMix(RING1_NOISE_START, RING1_NOISE_END, progress)));
		alpha *= getNoise(fragPos.xz - centerPos.xz, radius, chance);
	}
	
	result = blend(result, GRAY(1.0, inner * outer * alpha));
}

void drawRing2(float dist, float radius, float ring3Outer) {
	float inner = smoothstep(RING2_RADIUS - EPSILON * 0.5, RING2_RADIUS, dist);
	float outer = smoothstep(RING2_RADIUS + EPSILON * 0.5, RING2_RADIUS, dist) * ring3Outer;
	float alpha = min(1.0, zoom(progress, 1.0 - RING2_FADE, 1.0, 1.0, 0.0));
	
	result = blend(result, GRAY(0.0, inner * outer * alpha));
}

void drawRing3(float dist, float radius, float outer) {
	float inner = smoothstep(radius - EPSILON * 0.5, radius, dist);
	
	result = blend(result, GRAY(1.0, inner * outer));
}


void main() {
	result = vec4(0.0);
	
	float dist = distance(fragPos.xz, centerPos.xz);
	
	float radius1 = zoom(progress, RING_START, RING_END, 0.0, RING_MAX_RADIUS);
	float radius2 = zoom(progress, RING_START, RING_END, 0.0, RING3_MAX_RADIUS);
	
	drawCircle(dist, radius1);
	drawRing1(dist, radius1);
	
	float ring3Outer = smoothstep(radius2 + EPSILON * 0.5, radius2, dist);
	drawRing2(dist, radius2, ring3Outer);
	
	if (progress >= RING_START && progress <= RING_END) {
		drawRing3(dist, radius2, ring3Outer);
	}
}

#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform float progress;

in vec3 fragPos;

out vec4 result;

const float EPSILON          = 0.0015;

const float RING1_END        = 0.8;
const float RING1_MAX_RADIUS = 0.25;
const float RING1_WIDTH      = 0.04;

const float RING2_START      = 0.5;
const float RING2_MIN_RADIUS = 0.08;
const float RING2_MAX_RADIUS = 0.25;


void drawRing1(float dist) {
	if (progress > RING1_END) return;
	
	float time = progress * (1.0 / RING1_END);
	float radius = time * RING1_MAX_RADIUS;
	
	float outer = smoothstep(radius + EPSILON, radius, dist);
	float inner = smoothstep(radius - RING1_WIDTH, radius, dist);
	
	float a = pow(2.0 * pow(time - 1.0, 2.0) - 1.0, 2.0);
	float alpha = 1.0 - a;
	float gray = 0.5 * a;
	
	result = blend(result, GRAY(gray, outer * inner * alpha));
}

void drawRing2(float dist) {
	if (progress < RING2_START) return;
	
	float radius = zoom(progress, RING2_START, 1.0, RING2_MIN_RADIUS, RING2_MAX_RADIUS);
	
	float outer = smoothstep(radius + EPSILON, radius, dist);
	float inner = smoothstep(radius - EPSILON, radius, dist);
	float alpha = 1.0 - pow(4.0 * progress - 3.0, 2.0);
	
	result = blend(result, GRAY(0.0, outer * inner * alpha));
}


void main() {
	result = vec4(0.0);
	
	float dist = distance(fragPos, centerPos);
	drawRing1(dist);
	drawRing2(dist);
}

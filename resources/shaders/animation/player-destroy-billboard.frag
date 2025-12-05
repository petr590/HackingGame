#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform vec3 angleNormal;
uniform float progress;
uniform int seed;

in vec3 fragPos;

out vec4 result;

const float PI = radians(180.0);
const float SMOOTH_WIDTH = 0.0015;

const float RING_START_TIME  = 0.01;
const float RING_WIDTH       = 0.001;
const float RING_INIT_RADIUS = 0.25;
const float RING_GROW_SPEED  = 15.0;

const float LINE_MIN_LENGHT = 0.05;
const float LINE_MAX_LENGHT = 0.5;

const float LINE_MIN_START = 0.0;
const float LINE_MAX_START = 0.25;
const float LINE_WIDTH     = 0.0001;

const vec3 LINE_COLOR = vec3(0.6, 0.1, 0.1);

const float HALO_WIDTH = 0.0005;
const float HALO_SMOOTH_WIDTH = 0.01;
const vec4 HALO_COLOR = vec4(0.98, 0.27, 0.27, 0.75);

const float WAIT_TIME = 0.128;

const float CENTER_HALO_SMOOTH_WIDTH = 0.2;
const float CENTER_HALO_FADE_START = 0.2;
const float CENTER_HALO_FADE_END   = 0.7;

const float HORIZ_HALO_GROW_SPEED   = 4.0;
const float HORIZ_HALO_HEIGHT       = 0.003;
const float HORIZ_HALO_SMOOTH_WIDTH = 0.05;

const int LINES_COUNT = 48; // Приблизительное количество линий (так как отрисовывать линию или нет, решает вероятность)
const int SECTORS = 128; // Общее количество секторов. Чем больше, тем рандомнее распределены линии по окружности

float randomBetween(float low, float high, int localSeed) {
	return mix(low, high, simpleNoise(seed, localSeed));
}

void drawRing(float delayedProgress, float distToCenter) {
	if (delayedProgress < RING_START_TIME) return;
	
	float radius = RING_INIT_RADIUS + RING_GROW_SPEED * (delayedProgress - RING_START_TIME);
	float outer = smoothstep(radius + RING_WIDTH + SMOOTH_WIDTH, radius + RING_WIDTH, distToCenter);
	float inner = smoothstep(radius - SMOOTH_WIDTH,              radius,              distToCenter);
	
	result = blend(result, GRAY(0.15, outer * inner));
	
}

void drawHolo(float delayedProgress, float distToCenter) {
	float alpha = smoothstep(CENTER_HALO_SMOOTH_WIDTH, 0.0, distToCenter);
	float fading = clamp(zoom(delayedProgress, CENTER_HALO_FADE_START, CENTER_HALO_FADE_END, 1.0, 0.0), 0.0, 1.0);
	
	result = blend(result, vec4(HALO_COLOR.rgb, alpha * fading));
	
	
	vec3 pos = fragPos - centerPos;
	
	float width = HORIZ_HALO_GROW_SPEED * delayedProgress;
	
	float left  = smoothstep(-(width + HORIZ_HALO_SMOOTH_WIDTH), -width, pos.x);
	float right = smoothstep(  width + HORIZ_HALO_SMOOTH_WIDTH,   width, pos.x);
	float up    = smoothstep(-(HORIZ_HALO_HEIGHT + SMOOTH_WIDTH), -HORIZ_HALO_HEIGHT, pos.z);
	float down  = smoothstep(  HORIZ_HALO_HEIGHT + SMOOTH_WIDTH,   HORIZ_HALO_HEIGHT, pos.z);
	
	result = blend(result, vec4(HALO_COLOR.rgb, HALO_COLOR.a  * 0.5 * up * down * left * right * fading));
}

void drawLine(float delayedProgress, float distToCenter, float distToMiddle, int localSeed) {
	float lineStart = 3.0 * delayedProgress - 0.1 + randomBetween(LINE_MIN_START, LINE_MAX_START, localSeed);
	float lineEnd   = lineStart + delayedProgress + randomBetween(LINE_MIN_LENGHT, LINE_MAX_LENGHT, localSeed + 1);
	
	if (distToCenter < lineStart || distToCenter > lineEnd) return;
	
	float up   = smoothstep(lineEnd, lineEnd - SMOOTH_WIDTH, distToCenter);
	float down = smoothstep(lineStart, lineStart + SMOOTH_WIDTH, distToCenter);
	
	float haloMultiplier = sqrt(max(0.0, min(lineEnd - distToCenter, distToCenter - lineStart)) * (2.0 / LINE_MAX_LENGHT));
	
	float haloSide = smoothstep(haloMultiplier * (HALO_WIDTH + HALO_SMOOTH_WIDTH), haloMultiplier * HALO_WIDTH, distToMiddle);
	float lineSide = smoothstep(LINE_WIDTH + SMOOTH_WIDTH, LINE_WIDTH, distToMiddle);
	
	result = blend(result, vec4(HALO_COLOR.rgb, HALO_COLOR.a * up * down * haloSide));
	result = blend(result, vec4(LINE_COLOR, up * down * lineSide));
}


void drawLines(float delayedProgress, float distToCenter) {
	vec3 direction = normalize(fragPos - centerPos);
	float angle = acos(dot(angleNormal, direction)) * sign(angleNormal.x - direction.x); // [-PI; PI]
	
	float zoomedAngle = zoom(angle, -PI, PI, 0.0, SECTORS);
	float distToMiddle = abs(fract(zoomedAngle) - 0.5) * distToCenter * (2.0 * PI / SECTORS);
	
	if (distToMiddle <= HALO_WIDTH + HALO_SMOOTH_WIDTH) {
		int localSeed = int(floor(zoomedAngle));
		
		if (simpleNoise(seed, localSeed) < float(LINES_COUNT) / float(SECTORS)) {
			drawLine(delayedProgress, distToCenter, distToMiddle, localSeed + 1);
		}
	}
}

void main() {
	result = vec4(0.0);
	
	float delayedProgress = max(0.0, progress - WAIT_TIME);	
	float distToCenter = distance(fragPos, centerPos);
	
	drawRing(delayedProgress, distToCenter);
	drawHolo(delayedProgress, distToCenter);
	drawLines(delayedProgress, distToCenter);
}

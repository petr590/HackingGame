#version 330 core
#include "common.glsl"

uniform vec3 centerPos;
uniform vec3 angleNormal;
uniform float progress;
uniform int seed;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

in vec3 fragPos;
in vec2 fragTexCoord;

out vec4 result;

const float SMOOTH_WIDTH      = 0.0015;

const float RING_END        = 0.5;
const float RING_MAX_RADIUS = 0.16;

const float LINE_MIN_LENGHT = 0.1;
const float LINE_MAX_LENGHT = 0.25;
const float LINE_SHARPNESS  = 0.2;
const float LINE_WIDTH             = 0.00002;
const float LINE_ENDS_SMOOTH_WIDTH = 0.0015;

const int LINES_COUNT = 6;
const int SECTORS = 256;

const float FIG0_START = 0.7;
const float FIG0_END   = 0.8;

const float FIG1_START = 0.8;
const float FIG1_END   = 0.9;

const float FIG2_START = 0.9;
const float FIG2_END   = 1.0;

const float PI = radians(180.0);

const float SIN_0 = sin(radians(0.0));
const float COS_0 = cos(radians(0.0));

const float SIN_45 = sin(radians(45.0));
const float COS_45 = cos(radians(45.0));


float randomBetween(float low, float high, int localSeed) {
	return mix(low, high, simpleNoise(seed, localSeed));
}


void drawRing(float distToCenter) {
	if (progress > RING_END) return;
	
	float time = progress * (1.0 / RING_END);
	float radius = time * RING_MAX_RADIUS;
	
	float outer = smoothstep(radius + SMOOTH_WIDTH, radius, distToCenter);
	float inner = smoothstep(radius - SMOOTH_WIDTH, radius, distToCenter);
	float alpha = smoothstep(0.5, 1.0, time);
	
	result = blend(result, GRAY(0.0, outer * inner * alpha));
}

void drawLine(float distToCenter, float distToMiddle, int localSeed) {
	float lineStart = progress * randomBetween(0.5, 1.0, localSeed);
	float lineEnd   = lineStart + randomBetween(LINE_MIN_LENGHT, LINE_MAX_LENGHT, localSeed + 1);
	
	if (distToCenter < lineStart || distToCenter > lineEnd) return;
	
	float up   = smoothstep(lineEnd,   lineEnd   - LINE_ENDS_SMOOTH_WIDTH, distToCenter);
	float down = smoothstep(lineStart, lineStart + LINE_ENDS_SMOOTH_WIDTH, distToCenter);
	float side = smoothstep(LINE_WIDTH + SMOOTH_WIDTH, LINE_WIDTH, distToMiddle);
	
	result = blend(result, GRAY(0.0, up * down * side));
}


void drawLines(float distToCenter) {
	vec3 direction = normalize(fragPos - centerPos);
	float angle = acos(dot(angleNormal, direction)) * sign(angleNormal.x - direction.x);
	
	float zoomedAngle = zoom(angle, -PI, PI, 0.0, SECTORS);
	float distToMiddle = abs(fract(zoomedAngle) - 0.5) * distToCenter * (2.0 * PI / SECTORS);
	
	if (distToMiddle <= LINE_WIDTH + SMOOTH_WIDTH) {
		int localSeed = int(floor(zoomedAngle));
		
		if (simpleNoise(seed, localSeed) < float(LINES_COUNT) / float(SECTORS)) {
			drawLine(distToCenter, distToMiddle, localSeed);
		}
	}
}

void drawFigure(sampler2D tex, float angleSin, float angleCos) {
	vec2 scaledTexCoord = rotate(fragTexCoord - 0.5, angleSin, angleCos) * 25.0 + 0.5;
	result = blend(result, texture(tex, scaledTexCoord));
}


void main() {
	result = vec4(0.0);
	
	float distToCenter = distance(fragPos, centerPos);
	drawRing(distToCenter);
	drawLines(distToCenter);
	
	if (progress >= FIG0_START && progress <= FIG0_END) {
		drawFigure(texture0, SIN_45, COS_45);
		
	} else if (progress >= FIG1_START && progress <= FIG1_END) {
		drawFigure(texture1, SIN_0, COS_0);
		
	} else if (progress >= FIG2_START && progress <= FIG2_END) {
		drawFigure(texture2, SIN_45, COS_45);
	}
}

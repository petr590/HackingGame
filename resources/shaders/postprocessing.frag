#version 330 core
#include "common.glsl"

uniform sampler2D sceneTexture;
uniform sampler2D guiTexture;
uniform float guiFadeProgress;
uniform float winScreenTime;
uniform vec2 pixelSize;
uniform int seed;

in vec2 fragTexCoord;

out vec4 color;

const float MAX_SHADING        = 0.6;
const float MARGIN_MAX_SHADING = 0.3;
const float MARGIN_HEIGHT      = 0.1;
const float BG_APPEAR_TIME     = 0.25;
const float BG_FADE_TIME       = 0.1;
const float STRIPE_MULTIPLIER  = 0.975;

const float EPSILON            = 1e-3;

const float SIN_45 = sin(radians(45.0));
const float COS_45 = cos(radians(45.0));
const vec2 TRIANGLE_SIZE = vec2(120.0, 120.0);
const vec2 TRIANGLE_SIZE_ROTATED = TRIANGLE_SIZE / vec2(SIN_45, COS_45);


vec4 getSceneColor() {
	float upperMaxShading = smoothstep(      MARGIN_HEIGHT,       MARGIN_HEIGHT + EPSILON, fragTexCoord.y);
	float lowerMaxShading = smoothstep(1.0 - MARGIN_HEIGHT, 1.0 - MARGIN_HEIGHT - EPSILON, fragTexCoord.y);
	float maxShading = mix(MARGIN_MAX_SHADING, MAX_SHADING, upperMaxShading * lowerMaxShading);
	
	vec4 color1 = texture(sceneTexture, fragTexCoord);
	vec4 color2 = texture(sceneTexture, fragTexCoord /*+ pixelSize*/);
	float progress = min(1.0, winScreenTime * (1.0 / BG_APPEAR_TIME)) * clamp((1.0 - winScreenTime) * (1.0 / BG_FADE_TIME), 0.0, 1.0);
	vec3 rgb = vec3(color1.r, color2.g, color1.b) * mix(1.0, maxShading, progress);
	return vec4(rgb, color1.a);
}

vec4 getGuiColor() {
	vec4 guiColor = texture(guiTexture, fragTexCoord);
	ivec2 pixCoordRem = ivec2(fragTexCoord / pixelSize * 0.5) % 6;
	
	if (pixCoordRem.x == 3 || pixCoordRem.x == 5 ||
		pixCoordRem.y == 3 || pixCoordRem.y == 5) {
		
		guiColor.rgb *= STRIPE_MULTIPLIER;
	}
	
	return guiColor;
}


float getAlpha() {
	if (winScreenTime > 0.0) return (1.0 - winScreenTime) * (1.0 / BG_FADE_TIME);
	if (guiFadeProgress >= 1.0) return 1.0;
	if (guiFadeProgress <= 0.0) return 0.0;
	
	int y = int(fragTexCoord.y / (pixelSize.y * TRIANGLE_SIZE.y));
	vec2 rotPos = rotate(fragTexCoord / (pixelSize * TRIANGLE_SIZE_ROTATED), SIN_45, COS_45);
	
	float alpha1 = simpleNoise(seed, ivec3(y, rotPos + pixelSize));
	float alpha2 = simpleNoise(seed, ivec3(y, rotPos.x - pixelSize.x, rotPos.y + pixelSize.y));
	float alpha3 = simpleNoise(seed, ivec3(y, rotPos.x + pixelSize.x, rotPos.y - pixelSize.y));
	float alpha4 = simpleNoise(seed, ivec3(y, rotPos - pixelSize));
	float alpha = 0.25 * (alpha1 + alpha2 + alpha3 + alpha4);
	return alpha - 1.0 + guiFadeProgress * 2.0;
}


void main() {
	vec4 sceneColor = getSceneColor();
	vec4 guiColor = getGuiColor();
	guiColor.a *= clamp(getAlpha(), 0.0, 1.0);
	color = blend(sceneColor, guiColor);
}

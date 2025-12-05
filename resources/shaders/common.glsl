#define GRAY(rgb, alp) vec4(rgb, rgb, rgb, alp)

vec4 blend(vec4 color1, vec4 color2) {
	float alpha = color2.a + color1.a * (1.0 - color2.a);
	vec3 rgb = vec3(0.0);
	
	if (alpha > 0.0) {
		rgb = (color2.rgb * color2.a + color1.rgb * color1.a * (1.0 - color2.a)) / alpha;
	}
	
	return vec4(rgb, alpha);
}


float zoom(float value, float srcStart, float srcEnd, float dstStart, float dstEnd) {
	return mix(dstStart, dstEnd, (value - srcStart) / (srcEnd - srcStart));
}

vec2 rotate(vec2 vec, float angleSin, float angleCos) {
	return vec2(
		vec.x * angleCos - vec.y * angleSin,
		vec.x * angleSin + vec.y * angleCos
	);
}


float simpleNoise(int seed, int x) {
	int h = seed * 9871 + x * 668265263;
	h = (h ^ (h >> 13)) * 1274126177;
	h = h ^ (h >> 16);
	return h * (1.0 / 0x80000000u);
}


float simpleNoise(int seed, ivec2 vec) {
	int h = seed * 9871 + vec.x * 668265263 + vec.y * 374761393;
	h = (h ^ (h >> 13)) * 1274126177;
	h = h ^ (h >> 16);
	return h * (1.0 / 0x80000000u);
}


float simpleNoise(int seed, ivec3 vec) {
	int h = seed * 9871 + vec.x * 668265263 + vec.y * 374761393 + vec.z * 310248241;
	h = (h ^ (h >> 13)) * 1274126177;
	h = h ^ (h >> 16);
	return h * (1.0 / 0x80000000u);
}

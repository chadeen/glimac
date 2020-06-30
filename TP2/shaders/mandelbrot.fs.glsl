#version 300 es

precision mediump float;

in vec3 vFragColor;
in vec3 vFragPosition;

out vec3 fFragColor;

vec2 complexSqr(vec2 z) {
	return vec2(z.x * z.x - z.y * z.y, z.x * z.y);
}

void main() {
	int nMax = 100, i;
	vec2 c = vFragPosition.xy, z = c;
	for(i = 0; i < nMax; i++) {
		z = complexSqr(z) + c;
		if(length(z) > 2.) {
			break;
		}
	}
	fFragColor = vec3(float(i) / float(nMax));
};

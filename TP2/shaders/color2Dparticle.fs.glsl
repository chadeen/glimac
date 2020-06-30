#version 300 es

precision mediump float;

in vec3 vFragColor;
in vec3 vFragPosition;

out vec3 fFragColor;

float transform0(vec2 xy) {
	return 1.;
}

float transform1(vec2 xy) {
	float alpha = 3.5;
	float beta = 30.;
	float d = distance(vFragPosition.xy, vec2(0, (-1/6)));
	return alpha * exp(-beta * (d * d));
}

float transform2(vec2 xy) {
	return length(fract(5. * xy));
}

float transform3(vec2 xy) {
	return length(abs(fract(5. * xy) * 2. - 1.));
}

float transform4(vec2 xy) {
	return mod(floor(10. * xy.x) + floor(10. * xy.y), 2.);
}

float transform5(vec2 xy) {
	return smoothstep(0.3, 0.32, length(fract(5. * xy) - 0.5));
}

float transform6(vec2 xy) {
	return smoothstep(0.4, 0.5, max(abs(fract(8.0 * xy.x - 0.5 * mod(floor(8.0 * xy.y), 2.0)) - 0.5), abs(fract(8.0 * xy.y) - 0.5)));
}


void main() {
    fFragColor = vFragColor * transform4(vFragPosition.xy);
};

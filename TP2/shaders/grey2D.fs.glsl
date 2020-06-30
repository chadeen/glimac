#version 300 es

precision mediump float;

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
	float M = (vFragColor.x + vFragColor.y + vFragColor.z) / 3.;
    fFragColor = vec3(M, M, M);
};

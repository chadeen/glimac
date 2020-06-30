#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;

mat3 translate(float tx, float ty) {
	return mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(tx, ty, 1));
}

mat3 scale(float sx, float sy) {
	return mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
}

mat3 rotate(float a) {
	float rad = radians(a);
	float cosa = cos(rad);
	float sina = sin(rad);
	return mat3(vec3(cosa, sina, 0), vec3(-sina, cosa, 0), vec3(0, 0, 1));
}

void main() {
    vFragColor = aVertexColor;
    gl_Position = vec4((
    	scale(0.5, 0.5) * rotate(45.) * translate(0.5, 0.) * vec3(aVertexPosition, 1)
    ).xy, 0, 1);
};

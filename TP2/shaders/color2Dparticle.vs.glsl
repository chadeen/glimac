#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;
out vec3 vFragPosition;

mat3 scale(float sx, float sy) {
	return mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
}

void main() {
    vFragColor = aVertexColor;
    gl_Position = vec4((scale(2., 2.) * vec3(aVertexPosition, 1)).xy, 0, 1);
    vFragPosition = gl_Position.xyz;
};

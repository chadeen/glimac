#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;

out vec3 vFragColor;
out vec3 vFragPosition;

void main() {
    vFragColor = vec3(255, 0, 0);
    gl_Position = vec4(aVertexPosition, 0, 1);
	vFragPosition = gl_Position.xyz;
};

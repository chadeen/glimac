#version 300 es

precision mediump float;

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

out vec3 fFragColor;

void main() {
	fFragColor = vec3(texture(uTexture0, vTexCoords)) + vec3(texture(uTexture1, vTexCoords));
}

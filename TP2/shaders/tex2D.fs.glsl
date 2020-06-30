#version 300 es

precision mediump float;

in vec2 vFragTexture;

uniform sampler2D uTexture;

out vec3 fFragColor;

void main() {
	fFragColor = (texture(uTexture, vFragTexture)).xyz;
};

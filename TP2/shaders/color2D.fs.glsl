#version 300 es

precision mediump float;

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
    fFragColor = vFragColor;
};

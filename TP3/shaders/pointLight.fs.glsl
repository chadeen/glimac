#version 300 es

precision mediump float;

in vec3 vPosition_vs;
in vec3 vNormal_vs;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;

out vec3 fFragColor;

vec3 blinnPhong() {
	vec3 
		w0 = normalize(-vPosition_vs),
		N = normalize(vNormal_vs),
		wi = normalize(uLightPos_vs),
		Li = normalize(uLightIntensity),
		kd = normalize(uKd),
		ks = normalize(uKs);
	vec3 halfVector = 0.5 * (w0 + wi);
	return Li * (kd * dot(wi, N) + ks * pow(dot(halfVector, N) , uShininess));
}

void main() {
	fFragColor = blinnPhong();
}

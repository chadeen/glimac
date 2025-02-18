#version 300 es

precision mediump float;

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;

uniform sampler2D uTexture;

out vec3 fFragColor;

vec3 blinnPhong() {
	vec3 
		w0 = normalize(-vPosition_vs),
		N = normalize(vNormal_vs),
		wi = normalize(uLightPos_vs - vPosition_vs);
	float d = distance(uLightPos_vs, vPosition_vs);

	vec3
		Li = normalize(uLightIntensity / (d * d)),
		kd = normalize(uKd) + vec3(texture(uTexture, vTexCoords)),
		ks = normalize(uKs);
	vec3 halfVector = 0.5 * (w0 + wi);
	return Li * (kd * dot(wi, N) + ks * pow(dot(halfVector, N) , uShininess));
}

void main() {
	fFragColor = blinnPhong();
}

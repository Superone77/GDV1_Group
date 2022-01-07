#version 330 core

/*
This fragment shader calculates the Lambertian light intensity (diffuse reflection) of a fragment (see lecture 8 Light).
*/

//Note that these per-fragment inputs have been calculated by interpolation of the per-vertex outputs.
in vec3 vColor;     //Color of the fragment
in vec3 vNormal;    //Normal in view space
in vec3 vPos;       //Position of the fragment in camera coordinates
in vec2 vTexCoord;  //Texture coordinate of the fragment
in vec3 vTangent;   //Tangent in view space

uniform vec3 lightPosition;         //Position of the light in camera coordinates

uniform bool useDiffuse;
uniform bool useNormal;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

out vec4 color; // output color

void main() {
	vec3 normal = normalize(vNormal); // re-normalize normal, because it has been interpolated

	if (useNormal) {
		vec3 n = normal;
		vec3 t = normalize(vTangent - n * dot(vTangent, n));
		vec3 b = cross(n, t);

		// TODO(3.4): Implement normal mapping.
	}

	// The lighting is performed in view-space, so the camera is located in the origin.
	vec3 viewDir = normalize(vec3(0, 0, 0) - vPos);
	vec3 lightDir = normalize(lightPosition - vPos);
	vec3 halfView = normalize(lightDir + viewDir);

	float ambientIntensity = 0.1;
	float diffuseIntensity = 0.9 * max(dot(lightDir, normal), 0.0);
	float specularIntesity = 0.2 * pow(max(dot(halfView, normal), 0.0), 30.0);
	float intensity = ambientIntensity + diffuseIntensity + specularIntesity;

	vec3 baseColor = useDiffuse ? texture(diffuseTexture, vTexCoord).rgb : vColor;
	color = vec4(baseColor * intensity, 1.0);
}

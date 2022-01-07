#version 330 core

layout(location = 0) in vec3 position; //Vertex position in object coordinates
layout(location = 1) in vec3 normal;   //Vertex normal
layout(location = 2) in vec3 color;    //Per-vertex color (for coloring using color array). Note that the vertex array gets disabled when STATIC_COLOR is used. This means that a standard value is inserted here.
layout(location = 3) in vec2 texCoord; //Texture coordinate (for using textures)
layout(location = 4) in vec3 tangent;

uniform mat4 modelView;     //ModelView matrix
uniform mat4 projection;    //Projection matrix
uniform mat3 normalMatrix;  //The transpose inverse of the ModelView matrix, used for transformation of normals.

uniform bool useDisplacement;

uniform sampler2D displacementTexture;

out vec3 vColor;    //Per-vertex color
out vec3 vNormal;   //Per-vertex normal, transformed
out vec3 vPos;      //Position in camera coordinates
out vec2 vTexCoord; //Texture coordinate of current vertex
out vec3 vTangent;  //Per-vertex tangent, in view space

void main() {
	vec3 pos = position;

	// TODO(3.4): Implement displacement mapping.

	vec4 viewPos = modelView * vec4(pos, 1.0);
	gl_Position = projection * viewPos;
	vPos = viewPos.xyz / viewPos.w; //inhomogenous coordinates
	vColor = color;
	vNormal = normalMatrix * normal;
	vTexCoord = texCoord;
	vTangent = normalize(vec3(modelView * vec4(tangent, 0.0)));
}

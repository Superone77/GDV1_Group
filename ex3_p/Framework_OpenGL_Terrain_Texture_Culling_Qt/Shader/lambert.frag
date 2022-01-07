#version 330 core

/*
This fragment shader calculates the Lambertian light intensity (diffuse reflection) of a fragment (see lecture 8 Light).
*/

//Note that these per-fragment inputs have been calculated by interpolation of the per-vertex outputs.
in vec3 vColor;     //Color of the fragment
in vec3 vNormal;    //Normal of the fragment
in vec3 vPos;       //Position of the fragment in camera coordinates
in vec2 vTexCoord;  //Texture coordinate of the fragment

uniform vec3 lightPosition;         //Position of the light in camera coordinates
uniform bool useTexture;            //Flag whether to use a texture instead of per-vertex colors
uniform sampler2D diffuseTexture;   //Texture to use

//Output color
out vec4 color;

void main() {
    //Calculate the direction of the light.
    vec3 lightDir = normalize(lightPosition - vPos);
    //Calculate Lambertian intensity. We clamp at 0.1 in order to simulate some kind of ambient light
    //Please note that both vectors are normalized, so the dot is the cosine of the encapsulated angle.
    float intensity = max(dot(lightDir, vNormal), 0.05);
    //Set color, depending on set color source
    if (useTexture) {
        color = vec4(texture(diffuseTexture, vTexCoord).xyz * intensity, 1.0);
    }
    else {
        color = vec4(vColor * intensity, 1.0);
    }
    
    //Note that a fragment shader implicitly calls following line:
    //gl_FragDepth = gl_FragCoord.z;
    //for the z-buffer test. Note that gl_FragCoord is not the same as vPos because gl_FragCoord is the
    //screen space coordinate of the fragment. 
}

varying vec3 normal;
varying vec4 vertexColor;

void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
	vertexColor = gl_Color;

}


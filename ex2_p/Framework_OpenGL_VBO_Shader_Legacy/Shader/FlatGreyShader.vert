#version 120

varying vec4 vertexColor;

void main(void) {
	gl_Position = ftransform();
	vertexColor = gl_Color;
}

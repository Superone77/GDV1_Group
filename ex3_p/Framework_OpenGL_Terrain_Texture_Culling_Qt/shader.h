// ========================================================================= //
// Authors: Daniel Ströter, Roman Getto, Matthias Bein                       //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: shader functions                                                 //
// ========================================================================= //

#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions_3_3_Core>

//Constants for shader locations
const GLuint POSITION_LOCATION = 0;
const GLuint NORMAL_LOCATION = 1;
const GLuint COLOR_LOCATION = 2;
const GLuint TEXCOORD_LOCATION = 3;
const GLuint TANGENT_LOCATION = 4;

void printProgramInfoLog(QOpenGLFunctions_3_3_Core* f, GLuint obj);
void printShaderInfoLog(QOpenGLFunctions_3_3_Core* f, GLuint obj);
GLuint readShaders(const char *vertexShaderFilename, const char *fragmentShaderFilename);

#endif // SHADER_H
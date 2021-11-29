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

#include <cstdio>        // cout
#include <iostream>      // cout
#include <fstream>       // read file
#include <string>

#include <QOpenGLFunctions_2_1>

void printProgramInfoLog(QOpenGLFunctions_2_1* f, GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
    f->glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
	    std::vector<GLchar> infoLog(infologLength);
		f->glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog.data());
		std::cout << infoLog.data() << std::endl;
	}
}

void printShaderInfoLog(QOpenGLFunctions_2_1* f, GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	f->glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 0)
	{
        std::vector<GLchar> infoLog(infologLength);
		f->glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog.data());
		std::cout << infoLog.data() << std::endl;
	}
}

GLuint readShaders(QOpenGLFunctions_2_1* f, const char* vertexShaderFilename, const char* fragmentShaderFilename) {
  // read vertex shader if possible
  std::ifstream vin(vertexShaderFilename, std::ifstream::in | std::ios::binary);
  std::noskipws(vin);
  if (!vin) {
      std::cout << "readShaders(): " << vertexShaderFilename << " not found!" << std::endl;
      return 0;
  }
  std::string vsource(std::istream_iterator<char>(vin), {});
  vin.close();

  // read fragment shader if possible
  std::ifstream fin(fragmentShaderFilename, std::ifstream::in | std::ios::binary);
  std::noskipws(fin);
  if (!fin) {
      std::cout << "readShaders(): " << fragmentShaderFilename << " not found!" << std::endl;
      return 0;
  }
  std::string fsource(std::istream_iterator<char>(fin), {});
  fin.close();

  const char* vShaderSourcePtr = vsource.c_str();
  const char* fShaderSourcePtr = fsource.c_str();

  // create shaders, set source and compile
  GLuint vertexShader = f->glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
  f->glShaderSource(vertexShader, 1, &vShaderSourcePtr, nullptr);
  f->glShaderSource(fragmentShader, 1, &fShaderSourcePtr, nullptr);
  f->glCompileShader(vertexShader);
  f->glCompileShader(fragmentShader);

  // create a program, attach both shaders and link the program (shaders can be deleted now)
  GLuint program = f->glCreateProgram();
  f->glAttachShader(program, vertexShader);
  f->glAttachShader(program, fragmentShader);
  f->glLinkProgram(program);
  f->glDeleteShader(vertexShader);
  f->glDeleteShader(fragmentShader);

  // check of all was successful and return the programID
  GLint success = 0;
  f->glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    std::cout << "===== Vertex Shader =====" << std::endl << vsource << std::endl;
    std::cout << std::endl;
    std::cout << "===== Vertex Shader Info Log =====" << std::endl;
    printShaderInfoLog(f, vertexShader);
    std::cout << std::endl;
    std::cout << "===== Fragment Shader =====" << std::endl << fsource << std::endl;
    std::cout << std::endl;
    std::cout << "===== Fragment Shader Info Log =====" << std::endl;
    printShaderInfoLog(f, fragmentShader);
    std::cout << std::endl;
    std::cout << "===== Program Info Log =====" << std::endl;
    printProgramInfoLog(f, program);
    std::cout << std::endl;
    f->glDeleteProgram(program);
    program = 0;
  }
  return program;
}
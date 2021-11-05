// ========================================================================= //
// Authors: Daniel Rutz, Daniel Ströter, Roman Getto, Matthias Bein          //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes, SOLUTION //
// ========================================================================= //

#include <iostream>
#include <fstream>
#include <cfloat>

#include <QOpenGLContext>
#include <QOpenGLFunctions_2_1>

#include "TriangleMesh.h"

void TriangleMesh::calculateNormals() {
  normals.clear();
  normals.resize(vertices.size());
  // TODO: calculate normals for each vertex

  for (auto& normal : normals) {
      //the normalize() function returns a boolean which can be used if you want to check for erroneous normals
      normal.normalize();
  }
}

// ================
// === RAW DATA ===
// ================

vector<TriangleMesh::Vertex>& TriangleMesh::getPoints() {
  return vertices;
}
vector<TriangleMesh::Triangle>& TriangleMesh::getTriangles() {
  return triangles;
}

vector<TriangleMesh::Normal>& TriangleMesh::getNormals() {
  return normals;
}

void TriangleMesh::flipNormals() {
  for (auto& normal : normals) {
    normal *= -1.0;
  }
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadLSA(const char* filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadLSA: can not open " << filename << endl;
    return;
  }
  char s[256];
  in >> s;
  // first word: LSA
  if (!(s[0] == 'L' && s[1] == 'S' && s[2] == 'A')) return;
  // get number of vertices nv, faces nf, edges ne and baseline distance
  int nv, nf, ne;
  float baseline;
  in >> nv;
  in >> nf;
  in >> ne;
  in >> baseline;
  if (nv <= 0 || nf <= 0) return;

  // read vertices
  vertices.resize(nv);
  // TODO: read alpha, beta, gamma for each vertex and calculate verticex coordinates


  // read triangles
  triangles.resize(nf);
  // TODO: read all triangles from the file

  // calculate normals
  calculateNormals();
}

void TriangleMesh::loadOFF(const char* filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadOFF: can not find " << filename << endl;
    return;
  }
  char s[256];
  in >> s;
  // first word: OFF
  if (!(s[0] == 'O' && s[1] == 'F' && s[2] == 'F')) return;
  // get number of vertices nv, faces nf, edges ne and baseline distance
  unsigned int nv, nf, ne;
  in >> nv;
  in >> nf;
  in >> ne;
  if (nv <= 0 || nf <= 0) return;

  // read vertices
  vertices.resize(nv);
  // TODO: read all vertices from the file
  for(auto &vert:vertices){
      float x,y,z;
      in>>x;
      in>>y;
      in>>y;

      vert[0] = x;
      vert[1] = y;
      vert[2] = z;
  }

  // read triangles
  triangles.resize(nf);
  // TODO: read triangles from the file
  for (auto &tri : triangles) {
      int numVerts, a, b, c;

      in >> numVerts; // ignored
      in >> a;
      in >> b;
      in >> c;

      tri[0] = a;
      tri[1] = b;
      tri[2] = c;
  }

  // calculate normals
  calculateNormals();
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::draw() {
  if (triangles.size() == 0) return;
  auto* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_1>();

  // TODO: draw triangles with immediate mode

}

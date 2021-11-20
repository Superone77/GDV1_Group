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
#include <math.h>

#include <QOpenGLContext>
#include <QOpenGLFunctions_2_1>

#include "TriangleMesh.h"

void TriangleMesh::calculateNormals() {
  normals.clear();
  normals.resize(vertices.size());
  // TODO: calculate normals for each vertex

  for(auto& tri : triangles){
      //get vertices from each triangle
      Vec3f vert1 = vertices[tri[0]];
      Vec3f vert2 = vertices[tri[1]];
      Vec3f vert3 = vertices[tri[2]];
      //calculate edge vectors
      Vec3f kVector12 = vert1 - vert2;
      Vec3f kVector13 = vert1 - vert3;
      //cross product->normal vector
      Vec3f normal = cross(kVector12,kVector13);
      //normal vector in each vertex add up
      normals[tri[0]] += normal;
      normals[tri[1]] += normal;
      normals[tri[2]] += normal;
  }

  for (auto& normal : normals) {
      //the normalize() function returns a boolean which can be used if you want to check for erroneous normals
      normal.normalize();
  }
}
/*
 * 4a) Answer: Because the magnitude of the cross product equals the area of a parallelogram with the vectors for sides.
 * which means. double area of the triangles.
 */
void TriangleMesh::calculateNormals_1() {
    normals.clear();
    normals.resize(vertices.size());
    // TODO: calculate normals for each vertex

    for(auto& tri : triangles){
        //get vertices from each triangle
        Vec3f vert1 = vertices[tri[0]];
        Vec3f vert2 = vertices[tri[1]];
        Vec3f vert3 = vertices[tri[2]];
        //calculate edge vectors
        Vec3f kVector12 = vert1 - vert2;
        Vec3f kVector13 = vert1 - vert3;
        //cross product->normal vector
        Vec3f normal = cross(kVector12,kVector13);
        //norm of 2 edge vectors
        float module12 = sqrt(pow(kVector12.x(),2)+ pow(kVector12.y(),2)+pow(kVector12.z(),2));
        float module13 = sqrt(pow(kVector13.x(),2)+ pow(kVector13.y(),2)+pow(kVector13.z(),2));

        //acos scalar product of edge vector
        float scalarProduct = kVector13.x()*kVector12.x()+kVector13.y()*kVector12.y()+kVector13.z()*kVector12.z();
        //calculate the weight:
        float weight = acos(scalarProduct/(module12*module13));
        //normal vector in each vertex add up
        normals[tri[0]] += normal * weight;
        normals[tri[1]] += normal * weight;
        normals[tri[2]] += normal * weight;
    }

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

void TriangleMesh::loadLSA(const char* filename, float offset_x, float offset_y, float offset_z) {
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
  for (auto &vert: vertices){
  	float alpha, beta, gamma;
  	in >> alpha;
  	in >> beta;
  	in >> gamma;
  	
  	float x, y, z;
  	z = -1 * baseline / ( tan(alpha*PI/180) + tan(beta*PI/180) );
  	x = -1 * z * tan(beta*PI/180);
  	y = sqrt(x * x + z * z) * tan(gamma*PI/180);
  	x = x + offset_x;
  	y = y + offset_y;
  	z = z + offset_z;
  	
  	vert[0] = x;
	vert[1] = y;
	vert[2] = z;
	
  }


  // read triangles
  triangles.resize(nf);
  // TODO: read all triangles from the file
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
  //calculateNormals_1();
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
  for (auto &vert: vertices){
      float x,y,z;
      in >> x;
      in >> y;
      in >> z;

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
  if (triangles.size() == 0){
   return;
  }
  auto* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_1>();

  // TODO: draw triangles with immediate mode 
  for (auto &tri : triangles) {

	f->glBegin(GL_TRIANGLES);
        f->glNormal3f(normals[tri[0]][0], normals[tri[0]][1], normals[tri[0]][2]);
		f->glVertex3f( vertices[tri[0]][0], vertices[tri[0]][1], vertices[tri[0]][2]);

        f->glNormal3f(normals[tri[1]][0], normals[tri[1]][1], normals[tri[1]][2]);
		f->glVertex3f(vertices[tri[1]][0], vertices[tri[1]][1], vertices[tri[1]][2]);

        f->glNormal3f(normals[tri[2]][0], normals[tri[2]][1], normals[tri[2]][2]);
		f->glVertex3f( vertices[tri[2]][0], vertices[tri[2]][1], vertices[tri[2]][2]);
	f->glEnd();
 }


}





















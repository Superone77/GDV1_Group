// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes            //
//   * readOFF                                                                //
//   * draw                                                                   //
//   * transformations                                                        //
// ========================================================================== //

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <cstddef>
#include <vector>

#include "Vec3.h"

#define M_PI 3.14159265358979f

//Forward declaration
class QOpenGLFunctions_2_1;

class TriangleMesh  {

private:

  // typedefs for data
  typedef Vec3ui Triangle;
  typedef Vec3f Normal;
  typedef Vec3f Vertex;
  typedef std::vector<Triangle> Triangles;
  typedef std::vector<Normal> Normals;
  typedef std::vector<Vertex> Vertices;

  // data of TriangleMesh
  Vertices vertices;
  Normals normals;
  Triangles triangles;

  // VBO ids for vertices, normals, faces
  GLuint VBOv, VBOn, VBOf;

  //OpenGL functions
  mutable QOpenGLFunctions_2_1* f;

  // bounding box data
  Vec3f boundingBoxMin;
  Vec3f boundingBoxMax;
  Vec3f boundingBoxMid;
  Vec3f boundingBoxSize;

  // =========================
  // === PRIVATE FUNCTIONS ===
  // =========================

  // calculate normals
  void calculateNormals();

  // create VBOs for vertices, faces and normals
  void createAllVBOs();

  // clean up VBO data (delete from gpu memory)
  void cleanupVBO();  

  // clean up all data (including VBO data)
  void cleanup();

public:

  // ===============================
  // === CONSTRUCTOR, DESTRUCTOR ===
  // ===============================

  TriangleMesh();

  ~TriangleMesh();

  // disable copy and copy assignment as these require to copy VBOs using unique ids
  TriangleMesh(const TriangleMesh& o) = delete;
  TriangleMesh& operator=(TriangleMesh o) = delete;

  // ================
  // === RAW DATA ===
  // ================

  // get raw data references
  std::vector<Vec3f>& getVertices() { return vertices; }
  std::vector<Vec3ui>& getTriangles() { return triangles; }
  std::vector<Vec3f>& getNormals() { return normals; }

  // get size of all elements
  std::size_t getNumVertices() const { return vertices.size(); }
  std::size_t getNumNormals() const { return normals.size(); }
  std::size_t getNumTriangles() const { return triangles.size(); }

  // flip all normals
  void flipNormals();

  // translates vertices so that the bounding box center is at newBBmid
  void translateToCenter(const Vec3f& newBBmid);

  // scales vertices so that the largest bounding box size has length newLength
  void scaleToLength(float newLength);

  // =================
  // === LOAD MESH ===
  // =================

  // read from an OFF file. also calculates normals if not given in the file.
  void loadOFF(QOpenGLFunctions_2_1* f, const char* filename);

  // read from an OFF file. also calculates normals if not given in the file.
  // translates and scales vertices with bounding box center at BBmid and largest side BBlength
  void loadOFF(QOpenGLFunctions_2_1* f, const char* filename, const Vec3f& BBmid, float BBlength);

  // ==============
  // === RENDER ===
  // ==============
  
  // draw mesh with immediate mode
  void drawImmediate();

  // draw mesh with vertex array
  void drawArray(); 

  // draw VBO
  void drawVBO();

};


#endif


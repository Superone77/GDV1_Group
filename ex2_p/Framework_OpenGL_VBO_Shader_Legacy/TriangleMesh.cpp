// ========================================================================= //
// Authors: Daniel Str�ter, Roman Getto, Matthias Bein                       //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes           //
//   * readOFF                                                               //
//   * draw                                                                  //
//   * transformations                                                       //
// ========================================================================= //

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cfloat>

#include <QOpenGLFunctions_2_1>

#include "TriangleMesh.h"

// =========================
// === PRIVATE FUNCTIONS ===
// =========================

void TriangleMesh::calculateNormals() {
    normals.clear();
    normals.resize(vertices.size());
    for (const auto& face : triangles) {
        const GLuint iX = face[0];
        const GLuint iY = face[1];
        const GLuint iZ = face[2];

        const Vertex& vecX = vertices[iX];
        const Vertex& vecY = vertices[iY];
        const Vertex& vecZ = vertices[iZ];

        const Vertex vec1 = vecY - vecX;
        const Vertex vec2 = vecZ - vecX;
        const Vertex normal = cross(vec1, vec2);

        normals[iX] += normal;
        normals[iY] += normal;
        normals[iZ] += normal;
    }

    for (auto& normal : normals) {
        normal.normalize();
    }
}

void TriangleMesh::createAllVBOs() {
	// TODO: create VBOs
}

TriangleMesh::~TriangleMesh()
{
	// delete VBO
	if (VBOv != 0) f->glDeleteBuffers(1, &VBOv);
	if (VBOn != 0) f->glDeleteBuffers(1, &VBOn);
	if (VBOf != 0) f->glDeleteBuffers(1, &VBOf);
}

void TriangleMesh::cleanupVBO() {
	// delete VBO
    if (VBOv != 0) f->glDeleteBuffers(1, &VBOv);
	if (VBOn != 0) f->glDeleteBuffers(1, &VBOn);
	if (VBOf != 0) f->glDeleteBuffers(1, &VBOf);
	VBOv = 0;
	VBOn = 0;
	VBOf = 0;
}

// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh() :
	VBOv(0), VBOn(0), VBOf(0), f(nullptr)
{ }

void TriangleMesh::cleanup() {
	// clear mesh data
	vertices.clear();
	triangles.clear();
	normals.clear();
	// clear bounding box data
	boundingBoxMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	boundingBoxMax = Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// clear VBO data
	cleanupVBO();
}

// ================
// === RAW DATA ===
// ================

void TriangleMesh::flipNormals() {
	for (auto& normal : normals) normal *= -1.0;
}

void TriangleMesh::translateToCenter(const Vec3f& newBBmid) {
	Vec3f trans = newBBmid - boundingBoxMid;
	for (auto& vertex : vertices) vertex += trans;
	boundingBoxMin += trans;
	boundingBoxMax += trans;
	boundingBoxMid += trans;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

void TriangleMesh::scaleToLength(const float newLength) {
	float length = std::max(std::max(boundingBoxSize.x(), boundingBoxSize.y()), boundingBoxSize.z());
	float scale = newLength / length;
	for (auto& vertex : vertices) vertex *= scale;
	boundingBoxMin *= scale;
	boundingBoxMax *= scale;
	boundingBoxMid *= scale;
	boundingBoxSize *= scale;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadOFF(QOpenGLFunctions_2_1* f, const char* filename) {
    this->f = f;
	std::ifstream in(filename);
	if (!in.is_open()) {
		std::cout << "loadOFF: can not find " << filename << std::endl;
		return;
	}
	const int MAX = 256;
	char s[MAX];
	in >> std::setw(MAX) >> s;
	// differentiate between OFF (vertices only) and NOFF (vertices and normals)
	bool noff = false;
	if (s[0] == 'O' && s[1] == 'F' && s[2] == 'F')
		;
	else if (s[0] == 'N' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F')
		noff = true;
	else
		return;
	// get number of vertices nv, faces nf and edges ne
	int nv,nf,ne;
	in >> std::setw(MAX) >> nv;
	in >> std::setw(MAX) >> nf;
	in >> std::setw(MAX) >> ne;
	if (nv <= 0 || nf <= 0) return;
	// clear any existing mesh
	cleanup();
	// read vertices
	vertices.resize(nv);
	for (int i = 0; i < nv; ++i) {
		in >> std::setw(MAX) >> vertices[i][0];
		in >> std::setw(MAX) >> vertices[i][1];
		in >> std::setw(MAX) >> vertices[i][2];
		boundingBoxMin[0] = std::min(vertices[i][0], boundingBoxMin[0]);
		boundingBoxMin[1] = std::min(vertices[i][1], boundingBoxMin[1]);
		boundingBoxMin[2] = std::min(vertices[i][2], boundingBoxMin[2]);
		boundingBoxMax[0] = std::max(vertices[i][0], boundingBoxMax[0]);
		boundingBoxMax[1] = std::max(vertices[i][1], boundingBoxMax[1]);
		boundingBoxMax[2] = std::max(vertices[i][2], boundingBoxMax[2]);
		if (noff) {
			in >> std::setw(MAX) >> normals[i][0];
			in >> std::setw(MAX) >> normals[i][1];
			in >> std::setw(MAX) >> normals[i][2];
		}
	}
	boundingBoxMid = 0.5f*boundingBoxMin + 0.5f*boundingBoxMax;
	boundingBoxSize = boundingBoxMax - boundingBoxMin;
	// read triangles
	triangles.resize(nf);
	for (int i = 0; i < nf; ++i) {
		int three;
		in >> std::setw(MAX) >> three;
		in >> std::setw(MAX) >> triangles[i][0];
		in >> std::setw(MAX) >> triangles[i][1];
		in >> std::setw(MAX) >> triangles[i][2];
	}
	// close ifstream
	in.close();
	// calculate normals if not given
	if (!noff) calculateNormals();
	// createVBO
	createAllVBOs();
}

void TriangleMesh::loadOFF(QOpenGLFunctions_2_1* f, const char* filename, const Vec3f& BBmid, const float BBlength) {
	loadOFF(f, filename);
	translateToCenter(BBmid);
	scaleToLength(BBlength);
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::drawImmediate() {
    if (triangles.empty()) return;

    f->glBegin(GL_TRIANGLES);
    for (const auto& face : triangles) {
        const auto& vertex1 = vertices[face[0]];
        const auto& vertex2 = vertices[face[1]];
        const auto& vertex3 = vertices[face[2]];

        const auto& normal1 = normals[face[0]];
        const auto& normal2 = normals[face[1]];
        const auto& normal3 = normals[face[2]];

        f->glNormal3f(normal1.x(), normal1.y(), normal1.z());
        f->glVertex3f(vertex1.x(), vertex1.y(), vertex1.z());

        f->glNormal3f(normal2.x(), normal2.y(), normal2.z());
        f->glVertex3f(vertex2.x(), vertex2.y(), vertex2.z());

        f->glNormal3f(normal3.x(), normal3.y(), normal3.z());
        f->glVertex3f(vertex3.x(), vertex3.y(), vertex3.z());
    }
    f->glEnd();
}

void TriangleMesh::drawArray() {
	if (triangles.empty()) return;
	// TODO: draw in array mode
}

void TriangleMesh::drawVBO() {
	if (triangles.empty()) return;
	if (VBOv == 0 || VBOn == 0 || VBOf == 0) return;
	// TODO: draw in VBO mode
}

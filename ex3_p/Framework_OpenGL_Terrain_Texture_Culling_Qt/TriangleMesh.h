//
// Created by danielr on 28.11.20.
//

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <QOpenGLContext>

#include <vector>

#include "Vec3.h"
#include "Utilities.h"

//Forward declaration, avoids being forced to include header
class QOpenGLFunctions_3_3_Core;
class RenderState;

class TriangleMesh {
public:
    enum class ColoringType {
        STATIC_COLOR,
        COLOR_ARRAY,
        TEXTURE,
        BUMP_MAPPING,
    };
private:
    // typedefs for data
    typedef Vec3ui Triangle;
    typedef Vec3f Vertex;
    typedef Vec3f Normal;
    typedef Vec3f Color;
    struct TexCoord { float u, v; };
    typedef Vec3f Tangent;

    typedef std::vector<Triangle> Triangles;
    typedef std::vector<Vertex> Vertices;
    typedef std::vector<Normal> Normals;
    typedef std::vector<Color> Colors;
    typedef std::vector<TexCoord> TexCoords;
    typedef std::vector<Tangent> Tangents;


    // data of TriangleMesh
    Vertices vertices;    // vertex positions
    Normals normals;      // normals per vertex
    Triangles triangles;  // indices of vertices that form a triangle
    Colors colors;        // r,g,b in [0,1]
    TexCoords texCoords;  // u,v in [0,1]
    Tangents tangents;    // tangent per vertex
    Vec3f staticColor;
    ColoringType coloringType{ColoringType::STATIC_COLOR};

    // VAO and VBO ids for vertices, normals, faces, colors, texCoords, tangents
    autoMoved<GLuint> VAO{}, VBOv{}, VBOn{}, VBOf{}, VBOc{}, VBOt{}, VBOtan{};
    // VBO for bounding box
    autoMoved<GLuint> VAObb{}, VBOvbb{}, VBOfbb{};
    //VBO for normal lines
    autoMoved<GLuint> VAOn{}, VBOvn{};
    // texture
    autoMoved<GLuint> textureID{};
    autoMoved<GLuint> normalMapID{};
    autoMoved<GLuint> displacementMapID{};

    // draw mode data
    bool withBB{false};
    bool withNormals{false};

    // bump mapping data
    bool enableDiffuseTexture = false;
    bool enableNormalMapping = false;
    bool enableDisplacementMapping = false;

    // bounding box data
    Vec3f boundingBoxMin;
    Vec3f boundingBoxMax;
    Vec3f boundingBoxMid;
    Vec3f boundingBoxSize;

public:
    TriangleMesh();
    ~TriangleMesh();
    //Make TriangleMesh non-copyable in order to avoid problems with VBO copying
    TriangleMesh(const TriangleMesh& other) = delete;
    TriangleMesh& operator= (const TriangleMesh& other) = delete;
    TriangleMesh(TriangleMesh&& other) noexcept = default;
    TriangleMesh& operator= (TriangleMesh&& other) noexcept = default;

    // clears all data, sets defaults
    void clear();

    // prints some information
    void coutData();

    // get raw data references
    std::vector<Vec3f>& getVertices() { return vertices; }
    std::vector<Vec3ui>& getTriangles() { return triangles; }
    std::vector<Vec3f>& getNormals() { return normals; }
    std::vector<Vec3f>& getColors() { return colors; }
    std::vector<TexCoord>& getTexCoords() { return texCoords; }

    // get size of all elements
    unsigned int getNumVertices() { return vertices.size(); }
    unsigned int getNumNormals() { return normals.size(); }
    unsigned int getNumTriangles() { return triangles.size(); }
    unsigned int getNumColors() { return colors.size(); }
    unsigned int getNumTexCoords() { return texCoords.size(); }

    // get boundingBox data
    Vec3f getBoundingBoxMin() { return boundingBoxMin; }
    Vec3f getBoundingBoxMax() { return boundingBoxMax; }
    Vec3f getBoundingBoxMid() { return boundingBoxMid; }
    Vec3f getBoundingBoxSize() { return boundingBoxSize; }

    // flip all normals
    void flipNormals(bool createVBOs = true);

    //set texture ID
    void setTexture(GLuint texID) { textureID.val = texID; };
    void setNormalTexture(GLuint texID) { normalMapID.val = texID; };
    void setDisplacementTexture(GLuint texID) { displacementMapID.val = texID; };
    //set default color
    void setStaticColor(Vec3f color);
    // translates vertices so that the bounding box center is at newBBmid
    void translateToCenter(const Vec3f& newBBmid, bool createVBOs = true);
    //enable or disable BB and normal drawing
    void toggleBB() { withBB = !withBB; }
    void toggleNormals() { withNormals = !withNormals; }
    void toggleDiffuse() { enableDiffuseTexture = !enableDiffuseTexture; }
    void toggleNormalMapping() { enableNormalMapping = !enableNormalMapping; }
    void toggleDisplacementMapping() { enableDisplacementMapping = !enableDisplacementMapping; }

    // scales vertices so that the largest bounding box size has length newLength
    void scaleToLength(float newLength, bool createVBOs = true);

    // =================
    // === LOAD MESH ===
    // =================

    // read from an OFF file. also calculates normals if not given in the file.
    void loadOFF(const char* filename, bool createVBOs = true);

    // read from an OFF file. also calculates normals if not given in the file.
    // translates and scales vertices with bounding box center at BBmid and largest side BBlength
    void loadOFF(const char* filename, const Vec3f& BBmid, float BBlength);

    void generateSphere();

    void generateTerrain();

private:
    // calculate normals, weighted by area
    void calculateNormalsByArea();

    // calculate texture coordinates by central projection
    void calculateTexCoordsSphereMapping();

    // calculates axis aligned bounding box data
    void calculateBB();

    // create VBOs for vertices, faces, normals, colors, textureCoords
    void createAllVBOs();
    // create VBOs for normals
    void createNormalVAO(QOpenGLFunctions_3_3_Core* f);
    void createBBVAO(QOpenGLFunctions_3_3_Core* f);

    // create VBO
    GLuint createVBO(QOpenGLFunctions_3_3_Core* f, const void* data, int dataSize, GLenum target, GLenum usage);

    // clean up VBO data (delete from gpu memory)
    void cleanupVBO();
    void cleanupVBO(QOpenGLFunctions_3_3_Core* f);

    template<typename T>
    T* getOpenGLFunctions() {
        auto* context = QOpenGLContext::currentContext();
        if (!context) return nullptr;
        else return context->versionFunctions<T>();
    }

    // ==============
    // === RENDER ===
    // ==============

public:

    // set coloring type
    void setColoringMode(ColoringType type) { coloringType = type; };

    // draw mesh with current drawing mode settings. returns the number of triangles drawn.
    unsigned int draw(RenderState& state);

private:

    // draw VBO
    void drawVBO(RenderState& state);

    // draw the bounding box (wired, immediate mode) (withBB)
    void drawBB(RenderState& state);

    // draw object normals (lines, immediate mode) (withNormals)
    void drawNormals(RenderState& state);

    // ===========
    // === VFC ===
    // ===========

    // check if bounding box is visible in view frustum
    bool boundingBoxIsVisible(const RenderState& state);
};




#endif //TRIANGLEMESH_H

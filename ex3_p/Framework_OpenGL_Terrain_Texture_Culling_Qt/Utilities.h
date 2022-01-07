//
// Created by danielr on 29.11.20.
//

#ifndef UTILITES_H
#define UTILITES_H

#include <utility>

template<typename T>
struct autoMoved {
    T val;

    autoMoved() : val() {}
    explicit autoMoved(T val) : val(val) {}

    autoMoved(autoMoved&& other) noexcept : val(other.val) {
        other.val = T();
    }

    autoMoved& operator=(autoMoved&& other) noexcept {
        using std::swap;
        swap(other.val, val);
        return *this;
    }

    autoMoved(const autoMoved& other) = delete;
    autoMoved& operator=(const autoMoved& other) = delete;
    T operator()() { return val; }
};

extern const GLfloat BoxVertices[];
extern const size_t BoxVerticesSize;
extern const GLuint BoxLineIndices[];
extern const size_t BoxLineIndicesSize;
extern const GLuint BoxTriangleIndices[];
extern const size_t BoxTriangleIndicesSize;

GLuint loadImageIntoTexture(const char* fileName, bool wrap = false);
GLuint loadCubeMap(const char* fileName[6]);

#endif //UTILITES_H

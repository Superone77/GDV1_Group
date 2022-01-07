//
// Created by danielr on 26.11.20.
//

#ifndef UEBUNG_03_RENDERSTATE_H
#define UEBUNG_03_RENDERSTATE_H

#include <stack>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>

#include "Vec3.h"

class RenderState {
    Vec3f lightPos;
    GLuint activeProgram{}, standardProgram{};
    std::stack<QMatrix4x4> modelViewMatrixStack;
    std::stack<QMatrix4x4> projectionMatrixStack;
    QOpenGLFunctions_3_3_Core* f;
    GLint modelViewMatrixUniformStandard{-1}, projectionMatrixUniformStandard{-1}, normalMatrixUniformStandard{-1}, lightPositionUniformStandard{-1},
            cameraPositionUniformStandard{-1}, textureUniformStandard{-1}, normalMapUniformStandard{-1}, useTextureUniformStandard{-1};
    GLint modelViewMatrixUniform{-1}, projectionMatrixUniform{-1}, normalMatrixUniform{-1}, lightPositionUniform{-1},
        cameraPositionUniform{-1}, textureUniform{-1}, normalMapUniform{-1}, useTextureUniform{-1};

    static void loadIdentity(std::stack<QMatrix4x4>& stack) {
        if (!stack.empty()) {
            stack.top().setToIdentity();
        }
    }

public:
    explicit RenderState(QOpenGLFunctions_3_3_Core* f = nullptr) : f(f) {
        //Put the identity matrix on all stacks
        modelViewMatrixStack.emplace();
        projectionMatrixStack.emplace();
    }

    void setOpenGLFunctions(QOpenGLFunctions_3_3_Core* f) {
        this->f = f;
    }

    QOpenGLFunctions_3_3_Core* getOpenGLFunctions() {
        return f;
    }

    void loadIdentityModelViewMatrix() {
        loadIdentity(modelViewMatrixStack);
    }

    void loadIdentityProjectionMatrix() {
        loadIdentity(projectionMatrixStack);
    }

    void pushModelViewMatrix() {
        modelViewMatrixStack.push(modelViewMatrixStack.top());
    }
    void popModelViewMatrix() {
        if (modelViewMatrixStack.size() > 1)
            modelViewMatrixStack.pop();
        else loadIdentityModelViewMatrix();
    }

    void pushProjectionMatrix() {
        projectionMatrixStack.push(projectionMatrixStack.top());
    }

    void popProjectionMatrix() {
        if (projectionMatrixStack.size() > 1)
            projectionMatrixStack.pop();
        else loadIdentityProjectionMatrix();
    }

    QMatrix4x4& getCurrentModelViewMatrix() { return modelViewMatrixStack.top(); }
    QMatrix4x4& getCurrentProjectionMatrix() { return projectionMatrixStack.top(); }
    const QMatrix4x4& getCurrentModelViewMatrix() const { return modelViewMatrixStack.top(); }
    const QMatrix4x4& getCurrentProjectionMatrix() const { return projectionMatrixStack.top(); }
    QMatrix3x3 calculateNormalMatrix() const { return modelViewMatrixStack.top().normalMatrix(); }
    GLuint getCurrentProgram() const { return activeProgram; }
    GLuint getStandardProgram() const { return standardProgram; }

    void setCurrentProgram(GLuint nextProgram) {
        f->glUseProgram(nextProgram);
        activeProgram = nextProgram;
        modelViewMatrixUniform = f->glGetUniformLocation(activeProgram, "modelView");
        projectionMatrixUniform = f->glGetUniformLocation(activeProgram, "projection");
        normalMatrixUniform = f->glGetUniformLocation(activeProgram, "normalMatrix");
        lightPositionUniform = f->glGetUniformLocation(activeProgram, "lightPosition");
        cameraPositionUniform = f->glGetUniformLocation(activeProgram, "cameraPosition");
        textureUniform = f->glGetUniformLocation(activeProgram, "diffuseTexture");
        normalMapUniform = f->glGetUniformLocation(activeProgram, "normalMap");
        useTextureUniform = f->glGetUniformLocation(activeProgram, "useTexture");
    }

    void setStandardProgram(GLuint standardProgram) {
        f->glUseProgram(standardProgram);
        activeProgram = standardProgram;
        this->standardProgram = standardProgram;

        modelViewMatrixUniformStandard = f->glGetUniformLocation(activeProgram, "modelView");
        projectionMatrixUniformStandard = f->glGetUniformLocation(activeProgram, "projection");
        normalMatrixUniformStandard = f->glGetUniformLocation(activeProgram, "normalMatrix");
        lightPositionUniformStandard = f->glGetUniformLocation(activeProgram, "lightPosition");
        cameraPositionUniformStandard = f->glGetUniformLocation(activeProgram, "cameraPosition");
        textureUniformStandard = f->glGetUniformLocation(activeProgram, "diffuseTexture");
        normalMapUniformStandard = f->glGetUniformLocation(activeProgram, "normalMap");
        useTextureUniformStandard = f->glGetUniformLocation(activeProgram, "useTexture");
    }

    void switchToStandardProgram() {
        f->glUseProgram(standardProgram);
        activeProgram = standardProgram;

        modelViewMatrixUniform = modelViewMatrixUniformStandard;
        projectionMatrixUniform = projectionMatrixUniformStandard;
        normalMatrixUniform = normalMatrixUniformStandard;
        lightPositionUniform = lightPositionUniformStandard;
        cameraPositionUniform = cameraPositionUniformStandard;
        textureUniform = textureUniformStandard;
        normalMapUniform = normalMapUniformStandard;
        useTextureUniform = useTextureUniformStandard;
    }

    GLint getModelViewUniform() const { return modelViewMatrixUniform; }
    GLint getProjectionUniform() const { return projectionMatrixUniform; }
    GLint getNormalMatrixUniform() const { return normalMatrixUniform; }
    GLint getLightPositionUniform() const { return lightPositionUniform; }
    GLint getCameraPositionUniform() const { return cameraPositionUniform; }
    GLint getTextureUniform() const { return textureUniform; }
    GLint getNormalMapUniform() const { return normalMapUniform; }
    GLint getUseTextureUniform() const { return useTextureUniform; }

    Vec3f& getLightPos() {
        return lightPos;
    }

    void setLightUniform() {
        QVector4D Qlp4d(lightPos.x(), lightPos.y(), lightPos.z(), 1.0f);
        Qlp4d = getCurrentModelViewMatrix().map(Qlp4d);
        const QVector3D Qlp = Qlp4d.toVector3DAffine();
        f->glUniform3f(getLightPositionUniform(), Qlp.x(), Qlp.y(), Qlp.z());
    }
};


#endif //UEBUNG_03_RENDERSTATE_H

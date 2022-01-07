//
// Created by danielr on 28.11.20.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QBasicTimer>
#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>

#include "Vec3.h"
#include "TriangleMesh.h"
#include "RenderState.h"

class MainWindow : public QOpenGLWindow {
    Q_OBJECT
    QOpenGLFunctions_3_3_Core* f;

    // camera Information
    QVector3D cameraPos;
    QVector3D cameraDir;
    float angleX, angleY, movementSpeed;

    //light information
    float lightMotionSpeed;

    // mouse information
    QPoint mousePos;
    float mouseSensitivy;

    //rendered objects
    unsigned int objectsLastRun, trianglesLastRun;
    std::vector<TriangleMesh> meshes;
    TriangleMesh sphereMesh; // sun
    TriangleMesh bumpSphereMesh;

    static GLuint csVAO, csVBOs[2];
    int gridSize;

    //timer for moving light
    QBasicTimer lightMoveTimer;
    //FPS counter
    QBasicTimer fpsCounterTimer;
    unsigned int frameCounter;
    bool outputFPS;

    //shaders
    GLuint currentProgramID;
    std::vector<GLuint> programIDs;
    GLuint bumpProgramID;

    //RenderState with matrix stack
    RenderState state;

    GLuint genCSVAO();

    void drawSkybox();
    void drawCS();
    void drawLight();
    void setDefaults();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    //void mouseReleaseEvent(QMouseEvent* ev) override;
    void timerEvent(QTimerEvent* ev) override;

public:
    MainWindow(QWindow *parent = nullptr);
    ~MainWindow() override;
};


#endif //MAINWINDOW_H

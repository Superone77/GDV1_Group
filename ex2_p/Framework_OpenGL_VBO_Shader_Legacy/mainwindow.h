// ========================================================================= //
// Authors: Daniel Rutz, Daniel Ströter, Roman Getto, Matthias Bein          //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Class for display of a simple OpenGL scene                       //
// ========================================================================= //

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QBasicTimer>
#include <QOpenGLWindow>
#include <QOpenGLFunctions_2_1>

#include "Vec3.h"

#include "TriangleMesh.h"

class MainWindow : public QOpenGLWindow
{
    Q_OBJECT
    QOpenGLFunctions_2_1* f;

    // scene Information
    Vec3f centerPos;
    float angleX, angleY;

    //light information
    Vec3f lightPos;
    float lightMotionSpeed;

    // mouse information
    QPoint mousePos;
    float mouseSensitivy;

    //rendered objects
    TriangleMesh triMesh;
    TriangleMesh triMesh_1;
    TriangleMesh triMesh_2;
    TriangleMesh triMesh_3;
    TriangleMesh triMesh_4;
    TriangleMesh sphereMesh;
    int gridSize;

    //current render mode
    enum {
        RENDER_MODE_IMMEDIATE = 0,
        RENDER_MODE_ARRAY = 1,
        RENDER_MODE_VBO = 2,
    } currentRenderMode;

    //timer for moving light
    QBasicTimer lightMoveTimer;
    //FPS counter
    QBasicTimer fpsCounterTimer;
    unsigned int frameCounter;
    bool outputFPS;

    //shaders
    std::vector<GLuint> programIDs;

    void drawCS();
    void drawLight();
    void setDefaults();
    void initialize();

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
    ~MainWindow() override = default;
};
#endif // MAINWINDOW_H

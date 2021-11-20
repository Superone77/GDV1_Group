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
    bool moveLight;

    // mouse information
    QPoint mousePos;
    float mouseSensitivy;

    //rendered objects
    TriangleMesh triMesh_1;
    TriangleMesh triMesh_2;
    TriangleMesh triMesh_3;
    TriangleMesh triMesh_4;
    TriangleMesh sphereMesh;

    //timer for moving light
    QBasicTimer lightMoveTimer;

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
    ~MainWindow();
};
#endif // MAINWINDOW_H

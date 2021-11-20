// ========================================================================= //
// Authors: Daniel Rutz, Daniel Ströter, Roman Getto, Matthias Bein          //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Implementation of a class for a simple OpenGL scene              //
// ========================================================================= //

#include "mainwindow.h"

#include <iostream>
#include <cmath>

#include <Qt>
#include <QInputEvent>
#include <QMatrix4x4>

void coutHelp()
{
    std::cout << std::endl;
    std::cout << "====== KEY BINDINGS ======" << std::endl;
    std::cout << "ESC: exit" << std::endl;
    std::cout << "H: show this (H)elp file" << std::endl;
    std::cout << "R: (R)eset view" << std::endl;
    std::cout << "L: toggle (L)ight movement" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << std::endl;
}

MainWindow::MainWindow(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
{
    setDefaults();

    // Load multi-mesh, after trying, we found we can display all the meshes smoothly
    triMesh_1.loadLSA("../Modelle/delphin.lsa", 0, 0, 0);   
    triMesh_2.loadLSA("../Modelle/ballon.lsa", 5, 0, 0);
    triMesh_3.loadLSA("../Modelle/83ford-gt90.lsa", 0, 5, 0);
    triMesh_4.loadLSA("../Modelle/brach.lsa", 0, 0, 8);

    // Load the sphere of the light
    sphereMesh.loadOFF("../Modelle/sphere.off");
}

MainWindow::~MainWindow(){}

void MainWindow::initializeGL()
{
    //load OpenGL functions
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_1>();

    initialize();
}

void MainWindow::paintGL()
{
    // clear and set camera
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glLoadIdentity();

    // translate to centerPos
    f->glTranslatef(centerPos.x(), centerPos.y(), centerPos.z());

    // disable lighting for coordinate system and light sphere
    f->glDisable(GL_LIGHTING);

    // rotate scene, then render cs
    f->glRotatef(angleX,0.0f,1.0f,0.0f);
    f->glRotatef(angleY,1.0f,0.0f,0.0f);
    drawCS();

    // draw sphere for light still without lighting
    drawLight();

    // draw object
    f->glEnable(GL_LIGHTING);
    f->glColor3f(1.f,0.1f,0.1f);
    triMesh_1.draw();
    triMesh_2.draw();
    triMesh_3.draw();
    triMesh_4.draw();
}

// =================
// === RENDERING ===
// =================

void MainWindow::drawCS() {
    f->glBegin(GL_LINES);
    // red X
    f->glColor3f(1.f,0.f,0.f);
    f->glVertex3f(0.f,0.f,0.f);
    f->glVertex3f(5.f,0.f,0.f);
    // green Y
    f->glColor3f(0.f,1.f,0.f);
    f->glVertex3f(0.f,0.f,0.f);
    f->glVertex3f(0.f,5.f,0.f);
    // blue Z
    f->glColor3f(0.f,0.f,1.f);
    f->glVertex3f(0.f,0.f,0.f);
    f->glVertex3f(0.f,0.f,5.f);
    f->glEnd();
}

void MainWindow::drawLight() {
    // set light position in within current coordinate system
    GLfloat lp[] = { lightPos.x(), lightPos.y(), lightPos.z(), 1.0f };
    f->glLightfv(GL_LIGHT0, GL_POSITION, lp);

    // draw yellow sphere for light source
    f->glPushMatrix();
    f->glTranslatef(lp[0], lp[1], lp[2]);
    f->glScalef(0.3f, 0.3f, 0.3f);
    f->glColor3f(1,1,0);
    sphereMesh.draw();
    f->glPopMatrix();
}

void MainWindow::resizeGL(int width, int height)
{
    //Calculate new projection matrix
    QMatrix4x4 projectionMatrix;
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    projectionMatrix.perspective(65.f, aspectRatio, 0.1f, 100.f);

    //Get OpenGL context (Qt does not guarantee the context to be current in resizeGL function)
    makeCurrent();

    //Resize viewport
    f->glViewport(0, 0, width, height);

    //Set projection matrix
    f->glMatrixMode(GL_PROJECTION);
    f->glLoadIdentity();
    f->glMultMatrixf(projectionMatrix.constData());
    f->glMatrixMode(GL_MODELVIEW);
    f->glLoadIdentity();

    //Release context
    doneCurrent();
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_H:
        coutHelp();
        break;
    case Qt::Key_R:
        centerPos = Vec3f(0.f, -2.f, -5.f);
        angleX = 0.f;
        angleY = 0.f;
        update();
        break;
    case Qt::Key_L:
        if (lightMoveTimer.isActive()) {
            lightMoveTimer.stop();
        } else {
            lightMoveTimer.start(20, this);
        }
        break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    mousePos = ev->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    //rotate
    if (ev->buttons() & Qt::LeftButton) {
        angleX = std::fmod(angleX + (ev->x() - mousePos.x()) * mouseSensitivy, 360.f);
        angleY += (ev->y() - mousePos.y()) * mouseSensitivy;
    }

    //zoom (here translation in z)
    if (ev->buttons() & Qt::RightButton) {
        centerPos[2] -= 0.2f * (ev->y() - mousePos.y()) * mouseSensitivy;
    }

    // translation in xy
    if (ev->buttons() & Qt::MiddleButton) {
        centerPos[0] += 0.2f * (ev->x() - mousePos.x()) * mouseSensitivy;
        centerPos[1] -= 0.2f * (ev->y() - mousePos.y()) * mouseSensitivy;
    }

    mousePos = ev->pos();
    update();
}

void MainWindow::timerEvent(QTimerEvent *ev)
{
    //rotate light
    if (ev->timerId() == lightMoveTimer.timerId()) {
        lightPos.rotY(lightMotionSpeed);
        update();
    };
}

void MainWindow::initialize() {
    // black screen
    f->glClearColor(0.f,0.f,0.f,0.f);
    // enable depth buffer
    f->glEnable(GL_DEPTH_TEST);
    // set shading model
    f->glShadeModel(GL_SMOOTH);
    // set lighting and material
    GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat ambientLight[]   = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuseLight[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess = 128.0f;
    f->glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    f->glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    f->glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    f->glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    f->glLightf(GL_LIGHT0, GL_SHININESS, shininess);
    f->glEnable(GL_LIGHT0);
    // enable use of glColor instead of glMaterial for ambient and diffuse property
    f->glEnable(GL_COLOR_MATERIAL);
    f->glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    // white shiny specular highlights
    GLfloat specularLightMaterial[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininessMaterial = 128.0f;
    f->glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininessMaterial);
    f->glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, specularLightMaterial);

    // keybindings => cout
    coutHelp();
}

void MainWindow::setDefaults() {
    // scene Information
    centerPos = Vec3f(1.0f, -2.0f, -5.0f);
    angleX = 0.0f;
    angleY = 0.0f;
    // light information
    lightPos = Vec3f(-10.0f, 0.0f, 0.0f);
    lightMotionSpeed = 2.0f;
    moveLight = false;
    // mouse information
    mouseSensitivy = 1.0f;
}

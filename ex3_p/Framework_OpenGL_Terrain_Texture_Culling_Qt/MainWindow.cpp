//
// Created by danielr on 28.11.20.
//

#include <cmath>

#include <Qt>
#include <QInputEvent>
#include <QMatrix4x4>

#include "shader.h"
#include "Utilities.h"
#include "MainWindow.h"

GLuint MainWindow::csVAO = 0;
GLuint MainWindow::csVBOs[2] = {0, 0};

void coutHelp()
{
    std::cout << std::endl;
    std::cout << "====== KEY BINDINGS ======" << std::endl;
    std::cout << "ESC: exit" << std::endl;
    std::cout << "H: show this (H)elp file" << std::endl;
    std::cout << "R: (R)eset to default values" << std::endl;
    std::cout << "L: toggle (L)ight movement" << std::endl;
    std::cout << "F: toggle FPS output" << std::endl << std::endl;
    std::cout << "W,A,S,D: first person movement" << std::endl;
    std::cout << "+,-: movement speed up and down" << std::endl;
    std::cout << "1+:  Custom Shader" << std::endl;
    std::cout <<  std::endl;
    std::cout << "M: Switch Draw (M)ode. 0: Array, 1: VBO" << std::endl;
    std::cout << "X: toggle use of Static Color" << std::endl;
    std::cout << "C: toggle use of (C)olor array" << std::endl;
    std::cout << "T: toggle use of (T)extures" << std::endl;
    std::cout << "B: toggle (B)ounding Box" << std::endl;
    std::cout << "N: toggle (N)ormal Rendering" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "BUMP MAPPING / DISPLACEMENT MAPPING" << std::endl;
    std::cout << "Z: toggle diffuse texture" << std::endl;
    std::cout << "U: toggle normal mapping" << std::endl;
    std::cout << "I: toggle displacement mapping" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << std::endl;
}

MainWindow::MainWindow(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent) {
    setDefaults();
    fpsCounterTimer.start(1000, this);
}

void MainWindow::initializeGL() {
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    const GLubyte* versionString = f->glGetString(GL_VERSION);
    std::cout << "The current OpenGL version is: " << versionString << std::endl;
    state.setOpenGLFunctions(f);

    //black screen
    f->glClearColor(0.f, 0.f, 0.f, 1.f);
    //enable depth buffer
    f->glEnable(GL_DEPTH_TEST);

    GLuint testTexture = loadImageIntoTexture("../Textures/TEST_GRID.bmp");

    GLuint diffuseTexture = loadImageIntoTexture("../Textures/rough_block_wall_diff_1k.jpg", true);
    GLuint normalTexture = loadImageIntoTexture("../Textures/rough_block_wall_nor_1k.jpg", true);
    GLuint displacementTexture = loadImageIntoTexture("../Textures/rough_block_wall_disp_1k.jpg", true);

    //Load the sphere of the light
    sphereMesh.loadOFF("../Models/sphere.off");
    sphereMesh.setStaticColor(Vec3f(1.0f, 1.0f, 0.0f));

    //load meshes
    meshes.emplace_back();
    meshes[0].loadOFF("../Models/doppeldecker.off");
    meshes[0].setStaticColor(Vec3f(0.0f, 1.0f, 0.0f));
    meshes[0].setTexture(testTexture);
    meshes[0].setColoringMode(TriangleMesh::ColoringType::TEXTURE);

    meshes.emplace_back();
    meshes[1].generateTerrain();
    meshes[1].setStaticColor(Vec3f(1.f, 1.f, 0.f));
    meshes[1].setColoringMode(TriangleMesh::ColoringType::COLOR_ARRAY);

    bumpSphereMesh.generateSphere();
    bumpSphereMesh.setStaticColor(Vec3f(0.8f, 0.8f, 0.8f));
    bumpSphereMesh.setColoringMode(TriangleMesh::ColoringType::BUMP_MAPPING);
    bumpSphereMesh.setTexture(diffuseTexture);
    bumpSphereMesh.setNormalTexture(normalTexture);
    bumpSphereMesh.setDisplacementTexture(displacementTexture);

    //load coordinate system
    csVAO = genCSVAO();

    //load shaders
    GLuint lightShaderID = readShaders("../Shader/only_mvp.vert", "../Shader/constant_color.frag");
    programIDs.push_back(lightShaderID);
    state.setStandardProgram(lightShaderID);
    GLuint shaderID = readShaders("../Shader/only_mvp.vert", "../Shader/lambert.frag");
    if (shaderID != 0) programIDs.push_back(shaderID);
    currentProgramID = lightShaderID;

    bumpProgramID = readShaders("../Shader/bump.vert", "../Shader/bump.frag");

    std::cout << programIDs.size() << " shaders loaded. Use keys 1 to " << programIDs.size() << "." << std::endl;

    //print key bindings
    coutHelp();
}

void MainWindow::paintGL() {
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    state.loadIdentityModelViewMatrix();

    //translate to center, rotate and render coordinate system and light sphere
    QVector3D cameraLookAt = cameraPos + cameraDir;
    static QVector3D upVector(0.0f, 1.0f, 0.0f);
    state.getCurrentModelViewMatrix().lookAt(cameraPos, cameraLookAt, upVector);
    drawSkybox();
    state.switchToStandardProgram();
    drawCS();
    drawLight();

    // draw bump mapping sphere
    state.setCurrentProgram(bumpProgramID);
    state.pushModelViewMatrix();
    state.setLightUniform();
    state.getCurrentModelViewMatrix().translate(0, 5, 0);
    bumpSphereMesh.draw(state);
    state.popModelViewMatrix();
    
    state.setCurrentProgram(currentProgramID);
    state.setLightUniform();

    // draw objects. count triangles and objects drawn.
    unsigned int triangles, trianglesDrawn = 0, objectsDrawn = 0;
    for (auto& mesh : meshes) {
        triangles = mesh.draw(state);
        if (triangles > 0) {
            trianglesDrawn += triangles;
            objectsDrawn++;
        }
    }
    // cout number of objects and triangles if different from last run
    if (objectsDrawn != objectsLastRun || trianglesDrawn != trianglesLastRun) {
        objectsLastRun = objectsDrawn;
        trianglesLastRun = trianglesDrawn;
        std::cout << "renderScene: " << objectsDrawn << " objects and " << trianglesDrawn << " triangles." << std::endl;
    }

    frameCounter++;
    update();
}

void MainWindow::drawSkybox() {
    // TODO(3.2): Draw a skybox
    // prepare (no depth test)
    // draw skybox
    // ...
    // restore matrix and attributes
}

// This creates a VAO that represents the coordinate system
GLuint MainWindow::genCSVAO() {
    GLuint VAOresult;
    f->glGenVertexArrays(1, &VAOresult);
    f->glGenBuffers(2, csVBOs);

    f->glBindVertexArray(VAOresult);
    f->glBindBuffer(GL_ARRAY_BUFFER, csVBOs[0]);
    const static float vertices[] = {
            0.f, 0.f, 0.f,
            5.f, 0.f, 0.f,
            0.f, 0.f, 0.f,
            0.f, 5.f, 0.f,
            0.f, 0.f, 0.f,
            0.f, 0.f, 5.f,
    };
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    f->glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    f->glEnableVertexAttribArray(POSITION_LOCATION);
    f->glBindBuffer(GL_ARRAY_BUFFER, csVBOs[1]);
    const static float colors[] = {
            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f,
    };
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    f->glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    f->glEnableVertexAttribArray(COLOR_LOCATION);
    f->glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    f->glBindVertexArray(GL_NONE);
    return VAOresult;
}

void MainWindow::drawCS() {
    f->glUniformMatrix4fv(state.getModelViewUniform(), 1, GL_FALSE, state.getCurrentModelViewMatrix().constData());
    f->glBindVertexArray(csVAO);
    f->glDrawArrays(GL_LINES, 0, 6);
    f->glBindVertexArray(GL_NONE);
}

void MainWindow::drawLight() {
    // draw yellow sphere for light source
    state.pushModelViewMatrix();
    Vec3f& lp = state.getLightPos();
    state.getCurrentModelViewMatrix().translate(lp.x(), lp.y(), lp.z());
    sphereMesh.draw(state);
    state.popModelViewMatrix();
}

void MainWindow::resizeGL(int width, int height) {
    //Calculate new projection matrix
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    state.loadIdentityProjectionMatrix();
    state.getCurrentProjectionMatrix().perspective(65.f, aspectRatio, 0.5f, 10000.f);
    //Get OpenGL context (Qt does not guarantee the context to be current in resizeGL function)
    makeCurrent();

    //set projection matrix in OpenGL shader
    state.switchToStandardProgram();
    f->glUniformMatrix4fv(state.getProjectionUniform(), 1, GL_FALSE, state.getCurrentProjectionMatrix().constData());
    state.setCurrentProgram(bumpProgramID);
    f->glUniformMatrix4fv(state.getProjectionUniform(), 1, GL_FALSE, state.getCurrentProjectionMatrix().constData());
    for (GLuint progID : programIDs) {
        state.setCurrentProgram(progID);
        f->glUniformMatrix4fv(state.getProjectionUniform(), 1, GL_FALSE, state.getCurrentProjectionMatrix().constData());
    }

    //Resize viewport
    f->glViewport(0, 0, width, height);

    //Release context
    doneCurrent();
}

void MainWindow::keyPressEvent(QKeyEvent *ev) {
    unsigned int gridLength;
    QVector3D ortho(-cameraDir.z(),0.0f,cameraDir.x());
    switch(ev->key()) {
        case Qt::Key_Escape:
            close();
            break;
        case Qt::Key_H:
            coutHelp();
            break;
        case Qt::Key_R:
            setDefaults();
            break;
        case Qt::Key_L:
            if (lightMoveTimer.isActive()) {
                lightMoveTimer.stop();
            } else {
                lightMoveTimer.start(20, this);
            }
            break;
        case Qt::Key_F:
            outputFPS = !outputFPS;
            break;
        case Qt::Key_W:
            cameraPos += movementSpeed * cameraDir;
            break;
        case Qt::Key_S:
            cameraPos -= movementSpeed * cameraDir;
            break;
        case Qt::Key_C:
            for (auto& mesh : meshes) mesh.setColoringMode(TriangleMesh::ColoringType::COLOR_ARRAY);
            break;
        case Qt::Key_T:
            for (auto& mesh : meshes) mesh.setColoringMode(TriangleMesh::ColoringType::TEXTURE);
            break;
        case Qt::Key_X:
            for (auto& mesh : meshes) mesh.setColoringMode(TriangleMesh::ColoringType::STATIC_COLOR);
            break;
        case Qt::Key_B:
            for (auto& mesh : meshes) mesh.toggleBB();
                bumpSphereMesh.toggleBB();
            break;
        case Qt::Key_N:
            for (auto& mesh : meshes) mesh.toggleNormals();
                bumpSphereMesh.toggleNormals();
            break;
        case Qt::Key_Z:
            bumpSphereMesh.toggleDiffuse();
            break;
        case Qt::Key_U:
            bumpSphereMesh.toggleNormalMapping();
            break;
        case Qt::Key_I:
            bumpSphereMesh.toggleDisplacementMapping();
            break;
        case Qt::Key_A:
            ortho.normalize();
            cameraPos -= movementSpeed * ortho;
            break;
        case Qt::Key_D:
            ortho.normalize();
            cameraPos += movementSpeed * ortho;
            break;
        case Qt::Key_Plus:
            movementSpeed *= 2.0f;
            break;
        case Qt::Key_Minus:
            movementSpeed /= 2.0f;
            break;
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            int progID = ev->key() - Qt::Key_1;
            if (programIDs.size() > progID) {
                currentProgramID = programIDs[progID];
            }
            break;
    }
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    mousePos = ev->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev) {
    angleX = std::fmod(angleX + (ev->x() - mousePos.x()) * mouseSensitivy, 360.f);
    angleY -= (ev->y() - mousePos.y()) * mouseSensitivy;
    angleY = std::max(-70.f, std::min(angleY, 70.f));


    cameraDir.setX(std::sin(angleX * M_RadToDeg) * std::cos(angleY * M_RadToDeg));
    cameraDir.setZ(-std::cos(angleX * M_RadToDeg) * std::cos(angleY * M_RadToDeg));
    cameraDir.setY(std::max(0.0f, std::min(std::sqrt(1.0f - cameraDir.x() * cameraDir.x() - cameraDir.z() * cameraDir.z()), 1.0f)));

    if (angleY < 0.f) cameraDir.setY(-cameraDir.y());

    // update mouse for next relative movement
    mousePos = ev->pos();
    update();
}

void MainWindow::timerEvent(QTimerEvent *ev) {
    if (ev->timerId() == lightMoveTimer.timerId()) {
        //rotate light
        state.getLightPos().rotY(lightMotionSpeed);
        update();
    } else if (outputFPS && ev->timerId() == fpsCounterTimer.timerId()) {
        //print current FPS
        std::cout << "Current FPS: " << frameCounter << std::endl;
        frameCounter = 0;
    }
}

void MainWindow::setDefaults() {
    // scene Information
    cameraPos = QVector3D(0.0f, 0.0f, -3.0f);
    cameraDir = QVector3D(0.f, 0.f, -1.f);
    movementSpeed = 0.02f;
    angleX = 0.0f;
    angleY = 0.0f;
    // light information
    state.getLightPos() = Vec3f(0.0f, 5.0f, 20.0f);
    lightMotionSpeed = 0.2f;
    lightMoveTimer.stop();
    // mouse information
    mouseSensitivy = 1.0f;

    outputFPS = true;
    gridSize = 3;
    // last run: 0 objects and 0 triangles
    objectsLastRun = 0;
    trianglesLastRun = 0;
}

MainWindow::~MainWindow() {
    makeCurrent();
    sphereMesh.clear();
    for (auto& mesh : meshes) mesh.clear();
    // Clear coordinate system VBOs
    f->glDeleteBuffers(2, csVBOs);
    f->glDeleteVertexArrays(1, &csVAO);
    doneCurrent();
}

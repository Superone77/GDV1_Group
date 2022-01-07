// ========================================================================= //
// Authors: Daniel Rutz, Daniel Ströter, Roman Getto, Matthias Bein          //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Initialisation: Set basic parameters and create OpenGL window    //
// ========================================================================= //
#include <QGuiApplication>
#include <QSurfaceFormat>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    //A surface format specifies several parameters about the OpenGL context we want to create
    QSurfaceFormat format;
    //Set version to OpenGL 3.3 Core, this should work on MacOS as well
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    //Enable vsync
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    //Enable depth buffer
    format.setDepthBufferSize(24);

    MainWindow w;
    w.setWidth(1200);
    w.setHeight(800);
    w.setFormat(format);
    w.setTitle(QStringLiteral("OpenGL Terrain Texture Culling and Normal Mapping"));
    w.show();

    return a.exec();
}
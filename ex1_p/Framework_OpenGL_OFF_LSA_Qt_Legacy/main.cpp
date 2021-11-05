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

#include "mainwindow.h"

#include <string>

#include <QGuiApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    //A surface format specifies several parameters about the OpenGL context we want to create
    QSurfaceFormat format;
    //Set version to OpenGL 2.1, this should work on MacOS as well
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setMajorVersion(2);
    format.setMinorVersion(1);
    //Enable vsync
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //Enable depth buffer
    format.setDepthBufferSize(24);

    MainWindow w;
    w.setWidth(600);
    w.setHeight(400);
    w.setFormat(format);
    w.setTitle(QStringLiteral("TU Darmstadt, GDV1, OpenGL P1"));
    w.show();

    return a.exec();
}

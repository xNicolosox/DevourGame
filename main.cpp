#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>

#include "core/app.h"
#include "core/window.h"
#include "core/game.h"



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  
    int monitorW = glutGet(GLUT_SCREEN_WIDTH);
    int monitorH = glutGet(GLUT_SCREEN_HEIGHT);

    janelaW = monitorW;
    janelaH = monitorH;

    glutInitWindowSize(monitorW, monitorH);
    glutCreateWindow("Agora isso e Devour");

    glutFullScreen(); 

    GLenum err = glewInit();
    if (err != GLEW_OK) return 1;

    appInit();

    if (!gameInit("maps/map1.txt")) return 1;



    // Esconde o mouse para aumentar a imersão
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();
    return 0;
}
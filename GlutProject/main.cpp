#include <stdio.h>
#include <iostream>
#include </usr/local/include/GL/glew.h>
#include <GLUT/GLUT.h>
#include </usr/local/include/math_3d.h>


GLuint VBO;

void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnableVertexAttribArray(0);                   // ?
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_POINTS, 0, 1);
    
    glDisableVertexAttribArray(0);                  // ?
   
    glutSwapBuffers();
}

static void CreateVertexBuffer()
{
    Vector3f Vertices[1];
    Vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}


int main (int argc, char * argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 02");

    //glClearColor(0.5f, 0.0f, 0.0f, 0.3f);
    glutDisplayFunc(RenderSceneCB);


    //glewExperimental = GL_TRUE;  не знаю что это
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
           return -1;
    }

    glClearColor(0.0f,0.0f,0.0f,0.0f);
    CreateVertexBuffer();

    glutMainLoop();

    return 0;
}

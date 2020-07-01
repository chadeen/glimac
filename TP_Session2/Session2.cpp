#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Cylinder.hpp>
#include <glimac/TrackballCamera.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    // Load and compile shaders
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(applicationPath.dirPath() + "shaders/session2.vs.glsl", applicationPath.dirPath() + "shaders/session2.fs.glsl");
    program.use();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    Cylinder cylinder(2, .5, 32, 32);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, cylinder.getVertexCount() * sizeof(ShapeVertex), cylinder.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint uMVPMatrix, uMVMatrix, uNormalMatrix, uKd, uKs, uShininess, uLightPos, uLightIntensity;
    uMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    uMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    uKd = glGetUniformLocation(program.getGLId(), "uKd");
    uKs = glGetUniformLocation(program.getGLId(), "uKs");
    uShininess = glGetUniformLocation(program.getGLId(), "uShininess");
    uLightPos = glGetUniformLocation(program.getGLId(), "uLightPos_vs");
    uLightIntensity = glGetUniformLocation(program.getGLId(), "uLightIntensity");

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix, MVMatrix, ViewMatrix, ModelMatrix, NormalMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 100.f);
    unsigned int cameraRotateCounter = 0, lightRotateCounter = 0;
    TrackballCamera camera;
    glm::vec3
        red = glm::vec3(255., 0., 0.),
        yellow = glm::vec3(255., 255., 0.),
        blue = glm::vec3(0., 0., 255.),
        white = glm::vec3(255., 255., 255.);
    GLfloat shininess = 255.;

    glUniform3fv(uLightIntensity, 1, glm::value_ptr(white));

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
            else if(windowManager.isKeyPressed(SDLK_r)) {
                camera.reset();
            }
            else if(windowManager.isKeyPressed(SDLK_c) && cameraRotateCounter == 0) {
                cameraRotateCounter = 3600;
            }
            else if(windowManager.isKeyPressed(SDLK_l) && lightRotateCounter == 0) {
                lightRotateCounter = 3600;
            }
        }

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);

        if(cameraRotateCounter > 0) {
            camera.rotateLeft(-0.1);
            cameraRotateCounter--;
        }
        ViewMatrix = camera.getViewMatrix();

        if(lightRotateCounter > 0) {
            glUniform3fv(uLightPos, 1, glm::value_ptr(
                glm::vec3(ViewMatrix * glm::rotate(glm::mat4(1), (float)(2. * glm::pi<float>() * (float)lightRotateCounter/3600.), glm::vec3(0, 1, 0)) * glm::vec4(0, 1, 20, 0))
            ));
            lightRotateCounter--;
        } else {
            glUniform3fv(uLightPos, 1, glm::value_ptr(
                glm::vec3(ViewMatrix * glm::vec4(0, 1, 20, 0))
            ));
        }

        // Tronc de l'avatar
        ModelMatrix = glm::mat4(4);
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(red));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        // Membres de l'avatar

        // Bras 1
        ModelMatrix = glm::rotate(glm::mat4(4), (float)45., glm::vec3(0, 0, 1));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1, 1, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5, 0.5, 0.5));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(yellow));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        // Bras 2
        ModelMatrix = glm::rotate(glm::mat4(4), (float)-45., glm::vec3(0, 0, 1));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1, 1, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5, 0.5, 0.5));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(yellow));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        // Jambes
        ModelMatrix = glm::translate(glm::mat4(4), glm::vec3(-0.25, -1, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5, 0.5, 0.5));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(blue));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        ModelMatrix = glm::translate(glm::mat4(4), glm::vec3(0.25, -1, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5, 0.5, 0.5));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(blue));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        // Tête
        ModelMatrix = glm::scale(glm::mat4(4), glm::vec3(0.75, 0.75, 0.75));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 1.5, 0));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(uKd, 1, glm::value_ptr(yellow));
        glUniform3fv(uKs, 1, glm::value_ptr(white));
        glUniform1f(uShininess, shininess);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.getVertexCount());

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}

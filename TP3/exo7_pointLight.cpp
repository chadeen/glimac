#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/FreeflyCamera.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

struct MyProgram {
	Program m_Program;
	GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightPos;
    GLint uLightIntensity;

    MyProgram(const FilePath& applicationPath): 
    	m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
    						  applicationPath.dirPath() + "shaders/pointLight.fs.glsl")) {
    	GLint id = m_Program.getGLId();
    	uMVPMatrix = glGetUniformLocation(id, "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(id, "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(id, "uNormalMatrix");
        uKd = glGetUniformLocation(id, "uKd");
        uKs = glGetUniformLocation(id, "uKs");
        uShininess = glGetUniformLocation(id, "uShininess");
        uLightPos = glGetUniformLocation(id, "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(id, "uLightIntensity");
    }
};

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
    MyProgram myProgram(applicationPath);
    myProgram.m_Program.use();


    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    Sphere sphere(1, 32, 32);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
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

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix, MVMatrix, ViewMatrix, ModelMatrix, NormalMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 100.f);
    float timer;
    FreeflyCamera camera;
    glm::vec3
    	earthKd = glm::sphericalRand((GLfloat)255.),
    	earthKs = glm::sphericalRand((GLfloat)255.),
    	moonKd = glm::sphericalRand((GLfloat)255.),
    	moonKs = glm::sphericalRand((GLfloat)255.),
        lightPos = glm::vec3(1, 1, 1);
    GLfloat
    	earthShininess = glm::linearRand(1., 255.),
    	moonShininess = glm::linearRand(1., 255.);

    glUniform3fv(myProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(255, 255, 255)));

    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        glm::ivec2 oldPos = windowManager.getMousePosition(), newPos;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
            else if(windowManager.isKeyPressed(SDLK_z)) {
                camera.moveFront(0.1);
            }
            else if(windowManager.isKeyPressed(SDLK_s)) {
                camera.moveFront(-0.1);
            }
            else if(windowManager.isKeyPressed(SDLK_q)) {
                camera.moveLeft(0.1);
            }
            else if(windowManager.isKeyPressed(SDLK_d)) {
                camera.moveLeft(-0.1);
            }
            else if(windowManager.isKeyPressed(SDLK_r)) {
                camera.reset();
            }
            else if(windowManager.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
                glm::ivec2 diff = windowManager.getMousePosition() - oldPos;
                camera.rotateLeft(diff.x == 0 ? 0 : (diff.x < 0 ? 0.1 : -0.1));
                camera.rotateUp(diff.y == 0 ? 0 : (diff.y < 0 ? 0.1 : -0.1));
            }
        }

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);

        timer = windowManager.getTime();
        ViewMatrix = camera.getViewMatrix();
        glUniform3fv(myProgram.uLightPos, 1, glm::value_ptr(
        	glm::vec3(ViewMatrix * glm::vec4(lightPos, 0))
        ));

        ModelMatrix = glm::translate(glm::mat4(4), glm::vec3(0, 0, -5));
        ModelMatrix = glm::rotate(ModelMatrix, timer, glm::vec3(0, 1, 0));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(myProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(myProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(myProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(myProgram.uKd, 1, glm::value_ptr(earthKd));
        glUniform3fv(myProgram.uKs, 1, glm::value_ptr(earthKs));
        glUniform1f(myProgram.uShininess, earthShininess);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        ModelMatrix = glm::translate(glm::mat4(4), glm::vec3(0, 0, -5));
        ModelMatrix = glm::rotate(ModelMatrix, -timer, glm::vec3(0, 1, 0));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-2, 0, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2, 0.2, 0.2));
        ModelMatrix = glm::rotate(ModelMatrix, timer, glm::vec3(0, 1, 0));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(myProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(myProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(myProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform3fv(myProgram.uKd, 1, glm::value_ptr(moonKd));
        glUniform3fv(myProgram.uKs, 1, glm::value_ptr(moonKs));
        glUniform1f(myProgram.uShininess, moonShininess);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}

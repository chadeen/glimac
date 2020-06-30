#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/TrackballCamera.hpp>
#include <GL/glew.h>
#include <iostream>

using namespace glimac;

struct EarthProgram {
    Program m_Program;
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture0;
    GLint uTexture1;

    EarthProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/multiTex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture0 = glGetUniformLocation(m_Program.getGLId(), "uTexture0");
        uTexture1 = glGetUniformLocation(m_Program.getGLId(), "uTexture1");
    }
};

struct MoonProgram {
    Program m_Program;
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const FilePath& applicationPath):
        m_Program(loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "shaders/tex3D.fs.glsl")) {
        uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture = glGetUniformLocation(m_Program.getGLId(), "uTexture");
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
    std::vector<std::string> texturesPaths = {"EarthMap.jpg", "MoonMap.jpg", "CloudMap.jpg"};
    std::vector<std::unique_ptr<Image>> images(texturesPaths.size());

    for(size_t i = 0; i < texturesPaths.size(); i++) {
        images[i] = loadImage(applicationPath.dirPath() + "../../GLImac-Template/assets/textures/" + texturesPaths[i]);
        if(images[i] == 0) {
            std::cerr << "Loading texture " << texturesPaths[i] << " failed." << std::endl;
            return EXIT_FAILURE;
        }
    }

    EarthProgram earthProgram(applicationPath);
    MoonProgram moonProgram(applicationPath);


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

    std::vector<GLuint> textos(images.size());
    glGenTextures(textos.size(), textos.data());
    for(size_t i = 0; i < textos.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textos[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i]->getWidth(), images[i]->getHeight(), 0, GL_RGBA, GL_FLOAT, images[i]->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
	glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix, MVMatrix, ViewMatrix, ModelMatrix, NormalMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 100.f);
    float timer;
    TrackballCamera camera;

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
                camera.moveFront(1);
            }
            else if(windowManager.isKeyPressed(SDLK_s)) {
                camera.moveFront(-1);
            }
            else if(windowManager.isKeyPressed(SDLK_r)) {
                camera.reset();
            }
            else if(windowManager.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
                newPos = windowManager.getMousePosition() - oldPos;
                camera.rotateLeft(newPos.x);
                camera.rotateUp(newPos.y);
            }
        }

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);

        timer = windowManager.getTime();
        ViewMatrix = camera.getViewMatrix();

        earthProgram.m_Program.use();
        glUniform1i(earthProgram.uTexture0, 0);
        glUniform1i(earthProgram.uTexture1, 1);
        ModelMatrix = glm::rotate(glm::mat4(1), timer, glm::vec3(0, 1, 0));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textos[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textos[2]);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindTexture(GL_TEXTURE_2D, 0);

        moonProgram.m_Program.use();
        glUniform1i(moonProgram.uTexture, 0);
        ModelMatrix = glm::rotate(glm::mat4(4), -timer, glm::vec3(0, 1, 0));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-2, 0, 0));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2, 0.2, 0.2));
        ModelMatrix = glm::rotate(ModelMatrix, timer, glm::vec3(0, 1, 0));
        MVMatrix = ViewMatrix * ModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textos[1]);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteTextures(textos.size(), textos.data());
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}

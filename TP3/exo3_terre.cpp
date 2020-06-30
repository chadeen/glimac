#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
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

    std::unique_ptr<Image> image_earth = loadImage(applicationPath.dirPath() + "../../GLImac-Template/assets/textures/EarthMap.jpg");
    if(image_earth == 0) {
    	std::cerr << "Loading Earth texture failed." << std::endl;
    	return EXIT_FAILURE;
    }

    std::unique_ptr<Image> image_moon = loadImage(applicationPath.dirPath() + "../../GLImac-Template/assets/textures/MoonMap.jpg");
    if(image_moon == 0) {
    	std::cerr << "Loading Moon texture failed." << std::endl;
    	return EXIT_FAILURE;
    }

    Program program = loadProgram(applicationPath.dirPath() + "shaders/3D.vs.glsl", applicationPath.dirPath() + "shaders/tex3D.fs.glsl");
    program.use();

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

    GLint uMVPMatrix, uMVMatrix, uNormalMatrix, uTexture;
    uMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    uMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    uTexture = glGetUniformLocation(program.getGLId(), "uTexture");

    GLuint texto;
    glGenTextures(1, &texto);
    glBindTexture(GL_TEXTURE_2D, texto);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 ProjMatrix, MVMatrix, NormalMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f), 800.f/600.f, 0.1f, 100.f);


    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, texto);

        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
        MVMatrix = glm::rotate(MVMatrix, windowManager.getTime(), glm::vec3(0, 1, 0));
   	 	NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
   	 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_earth->getWidth(), image_earth->getHeight(), 0, GL_RGBA, GL_FLOAT, image_earth->getPixels());
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform1i(uTexture, 0);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        float moon_timer = windowManager.getTime();
        MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
		MVMatrix = glm::rotate(MVMatrix, moon_timer, glm::vec3(0, 1, 0));
		MVMatrix = glm::translate(MVMatrix, glm::vec3(-2, 0, 0));
		MVMatrix = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));
		MVMatrix = glm::rotate(MVMatrix, -moon_timer, glm::vec3(0, 1, 0));
		NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_moon->getWidth(), image_moon->getHeight(), 0, GL_RGBA, GL_FLOAT, image_moon->getPixels());
		glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glUniform1i(uTexture, 0);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteTextures(1, &texto);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}

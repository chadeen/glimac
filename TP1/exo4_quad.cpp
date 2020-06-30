#include <glimac/glm.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <cstddef>

using namespace glimac;
using namespace glm;

class Vertex2DColor {
public:
	vec2 position;
	vec3 color;
	Vertex2DColor() {
		position = vec2(0.f, 0.f);
		color = vec3(0.f, 0.f, 0.f);
	}

	Vertex2DColor(vec2 p, vec3 c) {
		position = p;
		color = c;
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
    Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl", applicationPath.dirPath() + "shaders/triangle.fs.glsl");
    program.use();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    GLuint vbos[2];
    glGenBuffers(2, vbos); // Create VBO

    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]); // Bind VBO
    /* Filling VBO with data */
    Vertex2DColor vertices0[] = { 
    	Vertex2DColor(glm::vec2(-0.5, 0), glm::vec3(1, 0, 0)),
    	Vertex2DColor(glm::vec2(0.5, 0), glm::vec3(0, 1, 0)),
    	Vertex2DColor(glm::vec2(0, 0.5), glm::vec3(0, 0, 1))
	};
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DColor), vertices0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]); // Bind VBO
    /* Filling VBO with data */
    Vertex2DColor vertices1[] = { 
    	Vertex2DColor(glm::vec2(-0.5, 0), glm::vec3(1, 0, 0)),
    	Vertex2DColor(glm::vec2(0.5, 0), glm::vec3(0, 1, 0)),
    	Vertex2DColor(glm::vec2(0, -0.5), glm::vec3(0, 0, 1))
	};
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DColor), vertices1, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

    GLuint vaos[2];
    glGenVertexArrays(2, vaos); // Create VAO
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR = 8;

    glBindVertexArray(vaos[0]); // Bind VAO
    /* Enable Vertex Attributes */
	glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
	/* Specify Vertex Attributes */
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]); // Bind VBO
	glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), offsetof(Vertex2DColor, position));
	glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, color)));
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindVertexArray(0); // Unbind VAO

	glBindVertexArray(vaos[1]); // Bind VAO
    /* Enable Vertex Attributes */
	glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
	/* Specify Vertex Attributes */
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]); // Bind VBO
	glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), offsetof(Vertex2DColor, position));
	glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, color)));
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
	glBindVertexArray(0); // Unbind VAO

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
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vaos[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glBindVertexArray(vaos[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(2, vbos);
    glDeleteVertexArrays(2, vaos);

    return EXIT_SUCCESS;
}

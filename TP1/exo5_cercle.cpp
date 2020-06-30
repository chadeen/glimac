#include <glimac/glm.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <cstddef>
#include <vector>

using namespace glimac;
using namespace glm;
using glm::cos;
using glm::sin;

class Vertex2DColor {
public:
	vec2 position;
	vec3 color;
	Vertex2DColor(): position(vec2(0.f, 0.f)), color(vec3(0.f, 0.f, 0.f)) {}
	Vertex2DColor(vec2 p, vec3 c): position(p), color(c) {}
};

int main(int argc, char** argv) {
    int N = 3;
    std::cout << "Entrer un nombre >= 3: ";
    std::cin >> N;
    std::cout << std::endl;
    
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
    N = max(3, N);
    float r = 1.f;
    float pi = glm::pi<float>();
    float _2pi = 2.f * pi;

    GLuint vbos[N];
    glGenBuffers(N, vbos); // Create VBO

    GLuint vaos[N];
    glGenVertexArrays(N, vaos); // Create VAO

    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR = 8;

    for(int i = 0; i < N; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]); // Bind VBO
        /* Filling VBO with data */
        std::vector<Vertex2DColor> vertices(3);
        vertices[0] = Vertex2DColor(glm::vec2(0, 0), glm::vec3(1, 1, 1));
        vertices[1] = Vertex2DColor(glm::vec2(r * cos(_2pi * (float)i/N), r * sin(_2pi * (float)i/N)), glm::vec3((float)i/N, 0, 0));
        vertices[2] = Vertex2DColor(glm::vec2(r * cos(_2pi * (float)(i + 1)/N), r * sin(_2pi * (float)(i + 1)/N)), glm::vec3((float)(i + 1)/N, 0, 0));
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

        glBindVertexArray(vaos[i]); // Bind VAO
        /* Enable Vertex Attributes */
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
        /* Specify Vertex Attributes */
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]); // Bind VBO
        glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), offsetof(Vertex2DColor, position));
        glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)(offsetof(Vertex2DColor, color)));
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        glBindVertexArray(0); // Unbind VAO
    }

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
        for(int i = 0; i < N; i++) {
            glBindVertexArray(vaos[i]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(N, vbos);
    glDeleteVertexArrays(N, vaos);

    return EXIT_SUCCESS;
}

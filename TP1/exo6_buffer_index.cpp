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
    SDLWindowManager windowManager(640, 640, "GLImac");

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

    GLuint vbo, vao, ibo; // Define structs for Buffer Objects
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COLOR = 8;

    glGenBuffers(1, &vbo); // Create VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind VBO
    /* Filling VBO with data */
    std::vector<Vertex2DColor> vertices(N + 1);
    vertices[0] = Vertex2DColor(glm::vec2(0, 0), glm::vec3(0, 0, 0));
    for(int i = 1; i < N + 1; i++) {
        vertices[i] = Vertex2DColor(glm::vec2(r * cos(_2pi * (float)(i - 1)/N), r * sin(_2pi * (float)(i - 1)/N)), glm::vec3(1. - (float)(i - 1)/N, 0.5, (float)(i - 1)/N));
    }
    for(int i = 0; i < N + 1; i++) {
        std::cout << i << ": " << vertices[i].position << " " << vertices[i].color << std::endl;
    }
    glBufferData(GL_ARRAY_BUFFER, (N + 1) * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

    glGenBuffers(1, &ibo); // Create IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind IBO
    // Indexes array: indexes of vertices to be drawn
    uint32_t indexes[3 * N]; // 3N indexes to draw N triangles
    for(int i = 0; i < N; i++) {
        indexes[3 * i] = 0;
        indexes[(3 * i) + 1] = (i + 1);
        indexes[(3 * i) + 2] = (i == N - 1) ? (1) : (i + 2); // This must work well, since we need at least 3 points to draw our triangles
        std::cout << indexes[3 * i] << " " << indexes[3 * i + 1] << " " << indexes[3 * i + 2] << std::endl;
    } // Each index is associated with its vertex in the VBO
    /* Filling IBO with indexes */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (3 * N) * sizeof(uint32_t), indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind IBO

    glGenVertexArrays(1, &vao); // Create VAO
    glBindVertexArray(vao); // Bind VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind IBO to store it into the VAO
    /* Enable Vertex Attributes */
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
    /* Specify Vertex Attributes */
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind VBO
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, position));
    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, color));
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
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3 * N, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    return EXIT_SUCCESS;
}

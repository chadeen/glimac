#include <glimac/glm.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glimac/Program.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace glimac;

class Vertex2DUV {
public:
	glm::vec2 position;
	glm::vec2 texture;
	Vertex2DUV() {
		position = glm::vec2(0.f, 0.f);
		texture = glm::vec2(0.f, 0.f);
	}

	Vertex2DUV(glm::vec2 p, glm::vec2 t) {
		position = p;
		texture = t;
	}
};

glm::mat3 rotate(float a) {
	float rad = glm::radians(a);
	float cosa = cos(rad);
	float sina = sin(rad);
	return glm::mat3(
		glm::vec3(cosa, sina, 0), 
		glm::vec3(-sina, cosa, 0), 
		glm::vec3(0, 0, 1)
	);
}

glm::mat3 translate(float tx, float ty) {
	return glm::mat3(
		glm::vec3(1, 0, 0), 
		glm::vec3(0, 1, 0), 
		glm::vec3(tx, ty, 1)
	);
}

glm::mat3 scale(float sx, float sy) {
	return glm::mat3(
		glm::vec3(sx, 0, 0), 
		glm::vec3(0, sy, 0), 
		glm::vec3(0, 0, 1)
	);
}

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

    FilePath applicationPath(argv[0]);

    std::unique_ptr<Image> image = loadImage(applicationPath.dirPath() + "../../GLImac-Template/assets/textures/triforce.png");
    if(image == 0) {
    	std::cerr << "Loading texture failed." << std::endl;
    	return EXIT_FAILURE;
    }

    Program program = loadProgram(applicationPath.dirPath() + "shaders/tex2D.vs.glsl", applicationPath.dirPath() + "shaders/tex2D.fs.glsl");
    program.use();

    GLint uModelMatrix = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    GLint uTexture = glGetUniformLocation(program.getGLId(), "uTexture");

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    Vertex2DUV vertices[] = {
    	Vertex2DUV(glm::vec2(-1, -1), glm::vec2(0, 1)),
    	Vertex2DUV(glm::vec2(1, -1), glm::vec2(1, 1)),
    	Vertex2DUV(glm::vec2(0, 1), glm::vec2(0.5, 0))
    };
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex2DUV), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_TEXTURE = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), offsetof(Vertex2DUV, position));
	glVertexAttribPointer(VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DUV), (const GLvoid*)(offsetof(Vertex2DUV, texture)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint texto;
    glGenTextures(1, &texto);
    glBindTexture(GL_TEXTURE_2D, texto);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getWidth(), image->getHeight(), 0, GL_RGBA, GL_FLOAT, image->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

    float speed = 0.1, timer;
    std::chrono::time_point<std::chrono::system_clock> start, current;

    // Application loop:
    bool done = false;
    start = std::chrono::system_clock::now();
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
        current = std::chrono::system_clock::now();
        timer = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count();

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, texto);
        glUniform1i(uTexture, 0);

        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, 
        	glm::value_ptr(
        		rotate(timer * speed) *
        		translate(0.5, 0.5) * 
        		scale(0.25, 0.25) * 
        		rotate(timer * speed)
        	)
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, 
        	glm::value_ptr(
        		rotate(timer * speed) *
        		translate(0.5, -0.5) * 
        		scale(0.25, 0.25) * 
        		rotate(timer * speed)
        	)
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, 
        	glm::value_ptr(
        		rotate(timer * speed) *
        		translate(-0.5, -0.5) * 
        		scale(0.25, 0.25) * 
        		rotate(-2 * timer * speed)
        	)
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUniformMatrix3fv(uModelMatrix, 1, GL_FALSE, 
        	glm::value_ptr(
        		rotate(timer * speed) *
        		translate(-0.5, 0.5) * 
        		scale(0.25, 0.25) * 
        		rotate(-2 * timer * speed)
        	)
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

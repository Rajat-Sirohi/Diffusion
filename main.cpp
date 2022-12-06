#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "particle.h"

#include <iostream>
#include <vector>

#define SCR_WIDTH  800
#define SCR_HEIGHT 800
#define N_VO 0

GLFWwindow* window;
glm::vec2 mouse_pos(0.0f);
GLuint VAO[N_VO], VBO[N_VO];
Shader passthroughShader = Shader();

std::vector<Particle*> particles;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        particles.push_back(new Particle());
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_R) {
                particles.push_back(new Particle());
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Map to (-1,-1) -- (1,1) coordinate space
    float x = 2 * (xpos / SCR_WIDTH) - 1;
    float y = 1 - 2 * (ypos / SCR_HEIGHT);
    mouse_pos = glm::vec2(x, y);
}

void init() {
    srand(time(NULL));

    // Setup window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Diffusion", NULL, NULL);
    if (window == NULL) {
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
        return;
	}
    glfwMakeContextCurrent(window);

    glfwSwapInterval(0); // disable VSync, which artificially limits FPS
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	    std::cout << "Failed to initialize GLAD" << std::endl;
        return;
	}

    // Setup shader programs
    passthroughShader = Shader("shaders/passthrough.vs", "shaders/passthrough.fs");

    for (int i = 0; i < 500; i++) {
        particles.push_back(new Particle());
    }
}

void update(float dt) {
    for (Particle *particle : particles)
        particle->move(dt);

    for (unsigned long i = 0; i < particles.size(); i++) {
        for (unsigned long j = i+1; j < particles.size(); j++) {
            resolveCollision(particles[i], particles[j]);
        }
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    passthroughShader.use();
    glm::vec3 colors[] = {
        glm::vec3(1.0f, 0.0f, 1.0f),
        // glm::vec3(1.0f, 0.0f, 0.0f),
        // glm::vec3(0.0f, 1.0f, 0.0f),
        // glm::vec3(0.0f, 0.0f, 1.0f),
        // glm::vec3(1.0f, 1.0f, 0.0f),
        // glm::vec3(0.0f, 1.0f, 1.0f),
        // glm::vec3(1.0f, 1.0f, 1.0f),
    };
    int numColors = sizeof(colors) / sizeof(glm::vec3);
    for (unsigned long i = 0; i < particles.size(); i++) {
        passthroughShader.setVec3("color", colors[i % numColors]);
        particles[i]->draw();
    }
}

void run() {
    unsigned int frameCount = 0;
    float dt = 0, prevTime = 0, frameTime = 0;
    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        dt = time - prevTime;
        prevTime = time;

        processInput(window);
        update(dt);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (time - frameTime > 0.3) {
            std::cout << "N: " << particles.size() << "\t   " << "FPS: " << std::round(frameCount/(time - frameTime)) << std::endl;
            frameCount = 0;
            frameTime = time;
        }
        frameCount++;
    }
}

int main() {
    init();
    run();

    glDeleteVertexArrays(N_VO, VAO);
    glDeleteBuffers(N_VO, VBO);

    glfwTerminate();
    return 0;
}

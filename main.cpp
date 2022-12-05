#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <thread>

#define SCR_WIDTH  800
#define SCR_HEIGHT 800
#define N_VO 1

GLFWwindow* window;
glm::vec2 mouse_pos(0.0f);
GLuint VAO[N_VO], VBO[N_VO];
Shader passthroughShader = Shader();

void processInput(GLFWwindow* window) {
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    // }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // if (key >= 0 && key < 1024) {
    //     if (action == GLFW_PRESS) {
    //         if (key == GLFW_KEY_R) {
    //         }
    //     }
    // }
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

    // Setup VAOs, VBOs
    glPointSize(10);
    float vertices[] = { 0.0f, 0.0f };

    glGenVertexArrays(N_VO, VAO);
    glGenBuffers(N_VO, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    passthroughShader.use();
    passthroughShader.setVec3("color", glm::vec3(1.0f, 0.0f, 1.0f));
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_POINTS, 0, 1);
}

void run() {
    unsigned int frameCount = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    float dt = 0, prevTime = 0;
    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        dt = time - prevTime;
        prevTime = time;

        processInput(window);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // std::cout << "FPS: " << (double)frameCount / (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()) * 1000000 << std::endl;
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

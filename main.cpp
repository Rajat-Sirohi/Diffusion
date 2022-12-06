#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <vector>

#define RAND_F (float(std::rand()) / float(RAND_MAX))
#define SCR_WIDTH  800
#define SCR_HEIGHT 800

GLFWwindow* window;
GLuint VAO, VBO, QuadVAO, QuadVBO, FBO;
GLuint positions[2], velocities[2];
Shader drawShader = Shader();
Shader updateShader = Shader();

bool paused = false;
unsigned int N = 5000;
float particleSize = 0.002;

void processInput(GLFWwindow* window) {
    glfwPollEvents();
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    // }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_TAB) {
                paused = !paused;
            }
        }
    }
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

    glfwSwapInterval(0); // disable VSync
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	    std::cout << "Failed to initialize GLAD" << std::endl;
        return;
	}

    // Setup shader programs
    drawShader = Shader("shaders/draw.vs", "shaders/draw.fs");
    updateShader = Shader("shaders/update.vs", "shaders/update.fs");

    // Particle point size
    glPointSize(SCR_HEIGHT * particleSize);
}

void setup() {
    // Initial data
    float initPositions[4*N], initVelocities[4*N];
    int indices[N];
    for (unsigned int i = 0; i < N; i++) {
        initPositions[4*i] = 2*RAND_F-1;
        initPositions[4*i+1] = 2*RAND_F-1;
        initPositions[4*i+2] = 0;
        initPositions[4*i+3] = 1;
        initVelocities[4*i] = RAND_F+1;
        initVelocities[4*i+1] = RAND_F+1;
        initVelocities[4*i+2] = 0;
        initVelocities[4*i+3] = 1;
        indices[i] = i;
    }

    // Indices VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 1, GL_INT, 0, (void*)0);
    glBindVertexArray(0);

    // Quad VAO
    const GLfloat vertices[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
    glGenVertexArrays(1, &QuadVAO);
    glGenBuffers(1, &QuadVBO);

    glBindVertexArray(QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    // Textures
    glGenTextures(2, positions);
    glGenTextures(2, velocities);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, positions[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, 1, 0, GL_RGBA, GL_FLOAT, nullptr);

        glBindTexture(GL_TEXTURE_2D, velocities[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, 1, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, positions[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, 1, 0, GL_RGBA, GL_FLOAT, initPositions);
    glBindTexture(GL_TEXTURE_2D, velocities[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, 1, 0, GL_RGBA, GL_FLOAT, initVelocities);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    for (int i = 0; i < 2; i++) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2*i, GL_TEXTURE_2D, positions[i], 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + 2*i, GL_TEXTURE_2D, velocities[i], 0);
    }
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    drawShader.use();
    drawShader.setInt("positions", 0);
    drawShader.setInt("velocities", 1);
    
    updateShader.use();
    updateShader.setInt("N", N);
    updateShader.setFloat("size", particleSize);
    updateShader.setInt("positions", 0);
    updateShader.setInt("velocities", 1);
}

void render(float dt, int k) {
    // Update positions, velocities
    updateShader.use();
    updateShader.setFloat("dt", dt);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    GLenum DrawBuffers[2];
    if (k == 0) {
        DrawBuffers[0] = GL_COLOR_ATTACHMENT2;
        DrawBuffers[1] = GL_COLOR_ATTACHMENT3;
    } else if (k == 1) {
        DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
        DrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    }
    glDrawBuffers(2, DrawBuffers);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positions[k]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocities[k]);

    glViewport(0, 0, N, 1);
    glBindVertexArray(QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Draw
    glClear(GL_COLOR_BUFFER_BIT);
    drawShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positions[k]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocities[k]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, N);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}

void run() {
    int k = 1;
    unsigned int frameCount = 0;
    float prevTime = 0, frameTime = 0;
    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        float dt = time - prevTime;
        prevTime = time;

        processInput(window);
        if (paused)
            continue;
        render(dt, k);
        k = (k + 1) % 2;

        if (time - frameTime > 0.3) {
            std::cout << "N: " << N << "\t   " << "FPS: " << std::round(frameCount/(time - frameTime)) << std::endl;
            frameCount = 0;
            frameTime = time;
        }
        frameCount++;
    }
}

int main() {
    init();
    setup();
    run();

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &QuadVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &QuadVBO);
    glDeleteFramebuffers(1, &FBO);

    glfwTerminate();
    return 0;
}

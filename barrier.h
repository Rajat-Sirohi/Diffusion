#ifndef BARRIER_H
#define BARRIER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

class Barrier {
public:
    glm::vec2 pos, vel, size;
    Barrier(glm::vec2 pos, glm::vec2 vel, float width, float height);
    Barrier(glm::vec2 pos, glm::vec2 vel) : Barrier(pos, vel, 0.5f, 0.5f) {}
    Barrier(glm::vec2 pos) : Barrier(pos, glm::vec2(0.0f)) {}
    Barrier() : Barrier(glm::vec2(0.0f)) {}
    ~Barrier();
    void draw();
    void updateVBO();
    void move(float dt);
private:
    GLuint VAO, VBO;
};

#endif

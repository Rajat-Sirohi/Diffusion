#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#define RAND_F (float(std::rand()) / float(RAND_MAX))

class Particle {
public:
    glm::vec2 pos, vel;
    float size;
    Particle(glm::vec2 pos, glm::vec2 vel, float size);
    Particle(glm::vec2 pos, glm::vec2 vel) : Particle(pos, vel, 0.005f) {}
    Particle(glm::vec2 pos) : Particle(pos, glm::vec2(2*RAND_F, 2*RAND_F)) {}
    Particle() : Particle(glm::vec2(0.0f)) {}
    ~Particle();
    void draw();
    void move(float dt);
private:
    GLuint VAO, VBO;
    void updateVBO() {
        float vertices[] = {
            pos.x - size, pos.y - size,
            pos.x - size, pos.y + size,
            pos.x + size, pos.y - size,
            pos.x + size, pos.y + size,
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    }
};

void resolveCollision(Particle *p1, Particle *p2);

#endif

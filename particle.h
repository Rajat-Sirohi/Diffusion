#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "barrier.h"

#include <iostream>
#include <vector>

#define RAND_F (float(std::rand()) / float(RAND_MAX))

class Particle {
public:
    glm::vec2 pos, vel;
    float size;
    Particle(glm::vec2 pos, glm::vec2 vel, float size);
    Particle(glm::vec2 pos, glm::vec2 vel) : Particle(pos, vel, 0.005f) {}
    Particle(glm::vec2 pos) : Particle(pos, glm::vec2(RAND_F+1, RAND_F+1)) {}
    Particle() : Particle(glm::vec2(-1.0f)) {}
    ~Particle();
    void draw();
    void updateVBO();
    void move(float dt);
    void resolveBarrierCollision(Barrier *barrier);
private:
    GLuint VAO, VBO;
};

void resolveParticleCollision(Particle *p1, Particle *p2);

#endif

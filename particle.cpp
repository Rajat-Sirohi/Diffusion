#include "particle.h"

Particle::Particle(glm::vec2 position, glm::vec2 velocity, float size) : pos(position), vel(velocity), size(size) {
    // Setup VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    updateVBO();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Particle::~Particle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Particle::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Particle::move(float dt) {
    pos += vel * dt;

    if (pos.x - size <= -1) {
        pos.x = -1 + size;
        vel.x *= -1;
    } else if (pos.x + size >= 1) {
        pos.x = 1 - size;
        vel.x *= -1;
    }

    if (pos.y - size <= -1) {
        pos.y = -1 + size;
        vel.y *= -1;
    } else if (pos.y + size >= 1) {
        pos.y = 1 - size;
        vel.y *= -1;
    }

    updateVBO();
}

void resolveCollision(Particle *o1, Particle *o2) {
    glm::vec2 p1 = o1->pos; glm::vec2 v1 = o1->vel;
    glm::vec2 p2 = o2->pos; glm::vec2 v2 = o2->vel;

    // Detect collision
    glm::vec2 dist = glm::abs(p1 - p2);
    float extent = o1->size + o2->size;
    bool collision = (dist.x < extent) && (dist.y < extent);
    if (!collision)
        return;

    // Separate particle squares
    glm::vec2 sep = glm::vec2(extent) - dist;
    if (sep.x < sep.y && sep.x > 0)
        sep.y = 0;
    else if (sep.y > 0)
        sep.x = 0;
    if (p1.x - p2.x < 0)
        sep.x = -sep.x;
    if (p1.y - p2.y < 0)
        sep.y = -sep.y;
    o1->pos += sep;

    // Bounce
    glm::vec2 dvel = glm::dot(v1 - v2, p1 - p2) / glm::dot(p1 - p2, p1 - p2) * (p1 - p2);
    o1->vel -= dvel;
    o2->vel += dvel;
}
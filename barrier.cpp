#include "barrier.h"

Barrier::Barrier(glm::vec2 position, glm::vec2 velocity, float width, float height) : pos(position), vel(velocity), size(glm::vec2(width/2, height/2)) {
    // Setup VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    updateVBO();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Barrier::~Barrier() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Barrier::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Barrier::updateVBO() {
    float vertices[] = {
        pos.x - size.x, pos.y - size.y,
        pos.x - size.x, pos.y + size.y,
        pos.x + size.x, pos.y - size.y,
        pos.x + size.x, pos.y + size.y,
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void Barrier::move(float dt) {
    pos += vel * dt;

    if (pos.x - size.x < -1) {
        pos.x = -1 + size.x;
        vel.x = -vel.x;
    } else if (pos.x + size.x > 1) {
        pos.x = 1 - size.x;
        vel.x = -vel.x;
    }

    if (pos.y - size.y < -1) {
        pos.y = -1 + size.y;
        vel.y = -vel.y;
    } else if (pos.y + size.y > 1) {
        pos.y = 1 - size.y;
        vel.y = -vel.y;
    }
}
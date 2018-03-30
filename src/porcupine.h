#include "main.h"

#ifndef PORCUPINE_H
#define PORCUPINE_H


class Porcupine {
public:
    Porcupine() {}
    Porcupine(float x, float y, color_t color, float radius, int sides);
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 acc;
    float rotation, omega;
    float radius;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    bounding_box_t bounding_box();
private:
    VAO *object;
    VAO *object1;
};

#endif // PORCUPINE_H

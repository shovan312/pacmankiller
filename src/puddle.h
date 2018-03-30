#include "main.h"

#ifndef PUDDLE_H
#define PUDDLE_H


class Puddle {
public:
    Puddle() {}
    //angle?
    Puddle(float centrex, float centrey, float length, float width, color_t color);
    glm::vec3 position;
    void draw(glm::mat4 VP);
    float rotation, length, width;
    void set_position(float centrex, float centrey);
    void tick();
    double speedx, speedy;
    bounding_box_t bounding_box();
private:
    VAO *object;
    VAO *object1;
};

#endif // PUDDLE_H

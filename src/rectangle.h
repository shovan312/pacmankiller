#include "main.h"

#ifndef RECTANGLE_H
#define RECTANGLE_H


class Rectangle {
public:
    Rectangle() {}
    //angle?
    Rectangle(float centrex, float centrey, float length, float width, color_t color);
    glm::vec3 position;
    void draw(glm::mat4 VP);
    float rotation, length, width;
    void set_position(float centrex, float centrey);
    void tick();
    double speedx, speedy;
    bounding_box_t bounding_box();
private:
    VAO *object;
};

#endif // RECTANGLE_H

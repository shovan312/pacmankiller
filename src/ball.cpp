#include "ball.h"
#include "main.h"

Ball::Ball(float x, float y, color_t color, float radius) {
    this->position = glm::vec3(x, y, 0);
    this->speed = glm::vec3(speed.x, speed.y, 0);
    this->acc = glm::vec3(acc.x, acc.y, 0);
    this->rotation = 0;
    this->radius = radius;
    this->type = color.r;
    
    int i, n=20;
    GLfloat vertex_buffer_data[9*n+10]={};

    for(i=1; i<=n; i++)
    {
        float pi=3.14159265;
        double syn=radius*sin((i*2*(pi/n)));
        double cas=radius*cos(i*2*(pi/n));
        float coord[3]={};
        coord[0]=cas;
        coord[1]=syn;
        coord[2]=0.0;
        int j=0;
        for(j=0; j<3; j++)
        {
            // printf("%f %f %f\n", coord[0], coord[1], coord[2]);
            vertex_buffer_data[(3*(i-1))+j]=coord[j];
        }
    }

    this->object = create3DObject(GL_TRIANGLE_FAN, 3*n, vertex_buffer_data, color, GL_FILL);
}

void Ball::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    this->position.x -= speed.x;
    this->position.y -= speed.y;
    this->speed.x += acc.x;
    // this->speed.y += acc.y;
}

bounding_box_t Ball::bounding_box() {
    bounding_box_t bb = {this->position.x, this->position.y, this->radius, this->radius};
    return bb;
}
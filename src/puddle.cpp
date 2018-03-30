#include "puddle.h"
#include "main.h"

//angle?
Puddle::Puddle(float centrex, float centrey, float length, float width, color_t color) {
    color_t color2;
    color2.r =  0;
    color2.g = 0;
    color2.b = 0;
    this->length = length;
    this->width = width;
    float radius = width/2 + ((length*length)/(8*width));
    this->position = glm::vec3(centrex, centrey + radius - width, 0);
    float pi=3.14159265;
    int n=20;
    GLfloat vertex_buffer_data[9*n+10] = {};
    float theta = 2*asin(length/(2.00*radius));
    this->rotation = -90-(theta/2.00)*(180.00/M_PI);

    printf("%f\n", theta);
    for(int i=1; i<n; i++)
    {
        double syn=radius*sin((i*(theta/n)));
        double cas=radius*cos(i*(theta/n));
        float coord[3]={};
        coord[0]=cas;
        coord[1]=syn;
        coord[2]=0.0;
        int j=0;
        for(j=0; j<3; j++)
        {
            vertex_buffer_data[3*i+j]=coord[j];
        }
    }
    GLfloat vertex_buffer_data1[9] = {0, 0, 0, 
        vertex_buffer_data[3], vertex_buffer_data[4], vertex_buffer_data[5], 
        vertex_buffer_data[3*(n-1)], vertex_buffer_data[3*(n-1)+1], 0};

    this->object = create3DObject(GL_TRIANGLE_FAN, n, vertex_buffer_data, color, GL_FILL);
    this->object1 = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data1, color2, GL_FILL);
}

void Puddle::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
    draw3DObject(this->object1);
}

void Puddle::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Puddle::tick() {
    this->position.x -= speedx;
    this->position.y -= speedy;
}

bounding_box_t Puddle::bounding_box() {
    bounding_box_t bb = {this->position.x, this->position.y, this->length, this->width};
    return bb;
}
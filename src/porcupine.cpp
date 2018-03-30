#include "porcupine.h"
#include "main.h"

Porcupine::Porcupine(float x, float y, color_t color, float radius, int sides) {
    this->position = glm::vec3(x, y, 0);
    this->speed = glm::vec3(speed.x, speed.y, 0);
    this->acc = glm::vec3(acc.x, acc.y, 0);
    this->rotation = 0;
    this->radius = radius;
    int m = sides;
    
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


    GLfloat vertex_buffer_data1[9*m+10]={};

    float triangle[9]={
        radius/2, -radius/2, 0,
        radius/2, radius/2, 0,
        radius/2*(2.732), 0, 0
    };

    for(i=0; i<m; i++)
    {
        float pi=3.14159265;
        float theta = i*(2*pi/m);
        float syn=sin(theta);
        float cas=cos(theta);
        float translated[3]={};
        for(int j=0; j<3; j++)
        {
            for(int k=0; k<2; k++)
            {
                //k = 0, 1
                if(k%2 == 0)
                {
                    translated[k] = triangle[3*j+k]*cas - triangle[3*j+k+1]*syn;
                }
                else
                {
                    translated[k] = triangle[3*j+k-1]*syn + triangle[3*j+k]*cas;
                }
            }
            translated[2]=0.00;
            for(int k=0; k<3; k++)
            {
                vertex_buffer_data1[9*i + 3*j + k] = translated[k];
            }
        } 
    }

    // for(i=0; i<m; i++)
    // {
    //     for(int j=0; j<3; j++)
    //     {
    //         for(int k =0; k<3; k++)
    //             printf("%f ", vertex_buffer_data1[9*m+3*j+k]);
    //         printf("\n");
    //     }
    //     printf("\n\n");
    // }

    this->object = create3DObject(GL_TRIANGLE_FAN, 3*n, vertex_buffer_data, color, GL_FILL);
    this->object1 = create3DObject(GL_TRIANGLES, 9*m, vertex_buffer_data1, color, GL_FILL);
}

void Porcupine::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate = rotate * glm::translate(glm::vec3(0, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    if(fabs(this->radius - 0.05) >= 0.1)
        draw3DObject(this->object);
    draw3DObject(this->object1);
}

void Porcupine::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Porcupine::tick() {
    this->position.x -= speed.x;
    this->position.y -= speed.y;
    this->speed.x += acc.x;
    this->speed.y += acc.y;
    this->rotation += omega;
}

bounding_box_t Porcupine::bounding_box() {
    bounding_box_t bb = {this->position.x, this->position.y, this->radius, this->radius};
    return bb;
}
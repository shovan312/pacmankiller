#include "main.h"
#include "timer.h"
#include "ball.h"
#include "rectangle.h"
#include "porcupine.h"
#include "puddle.h"
#include <string>

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

int score = 0;
int level = 1;

Rectangle ground;
Rectangle rec;
Rectangle trampoline[3];
Rectangle sticky[3];
Rectangle magnet;
Rectangle line;
Ball player;
Porcupine porc1;
Porcupine porc2;
const int max_enemies = 10;
const int max_stars = 20;
Porcupine stars[max_stars];
Puddle pond1;
Rectangle boards[max_enemies+10]={};
Ball enemies[max_enemies+10]={};
float enemies_rot[max_enemies+10]={};
float y_min = -2.00;
float e = 0.7;
float pi = 3.14159265;
int isStuck = 0;
int inWater = 0;

int ball_types[3] = {0, 1, 255}; 
int ball_scores[3] = {10, 20, 50};
char ball_names[3][100] = {"Green", "Blue", "Pink"};

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);

void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram (programID);

    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    glm::mat4 VP = Matrices.projection * Matrices.view;

    glm::mat4 MVP;  // MVP = Projection * View * Model

    for(int i=0; i<max_stars; i++)
        stars[i].draw(VP);
    ground.draw(VP);
    rec.draw(VP);
    pond1.draw(VP);
    player.draw(VP);

    for(int i=0; i<max_enemies; i++)
    {
        enemies[i].draw(VP);
        boards[i].draw(VP);
    }
    for(int i=0; i<3; i++)
    {
        trampoline[i].draw(VP);
        sticky[i].draw(VP);
    }
    magnet.draw(VP);
    porc1.draw(VP);
    porc2.draw(VP);

    // line.draw(VP);
}

void main_release(int key)
{
    if(key == GLFW_KEY_LEFT)
    {
        player.speed.x = 0;
    }
    if(key == GLFW_KEY_RIGHT)
    {
        player.speed.x = 0;
    }
}

void tick_input(GLFWwindow *window) {
    string text = "Pacman Killer, Your score is: ";
    text+=to_string(score);
    glfwSetWindowTitle(window, text.c_str());


    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    int i=0;
    if(isStuck)
    {
        int c=0;
        for(i=0; i<3; i++)
        {
            if(abs(player.position.x - sticky[i].position.x) >=1.4)
            {
                c++;
            }
        } 
        if(c==3)
            isStuck = 0;
    }

    if(!inWater)
    {
        if(!isStuck)
        {
            if(left)
            {
                player.speed.x = 0.032;
            }
            if(right)
            {
                player.speed.x = -0.032;
            }
        }
        if(up)
        {
            if(player.position.y <= y_min)
                player.speed.y = -0.12;
        }
    }
    else if(inWater)
    {
        if(!isStuck)
        {
            if(left)
            {
                player.speed.x = 0.012;
            }
            if(right)
            {
                player.speed.x = -0.012;
            }
        }
        if(up)
        {
            // printf("%f\n", player.speed.y);
            player.speed.y = -0.045;
        }
    }
    player.tick();
    for(i=0; i<max_enemies; i++)
    {
        enemies[i].tick();
        boards[i].tick();
    }
    for(int i=0; i<3; i++)
    {
        trampoline[i].tick();
        sticky[i].tick();
    }
    porc1.tick();
    porc2.tick();
    for(int i=0; i<max_stars; i++)
        stars[i].tick();

    for(i=0; i<max_enemies; i++)
    {
        //regular ball
        if(detect_collision_balls(player.position.x, player.position.y, enemies[i].position.x, enemies[i].position.y, player.radius, enemies[i].radius)) {
            if(player.position.y - enemies[i].position.y >= (enemies[i].radius + player.radius) * sin(pi/4) && player.speed.y >=0.0)
            {
                int type = enemies[i].type;
                int j;
                for(j=0; j<3; j++)
                {
                    if(type == ball_types[j])
                    {
                        break;
                    }
                }
                score+=ball_scores[j];
                printf("Hit a %s ball! Score is now: %d\n", ball_names[j], score);
                player.speed.x = 0;
                player.speed.y = -0.12;
                createNewEnemy(i);
            }
        }

        //ball with board
        if(detect_collision(player.bounding_box(), boards[i].bounding_box())) {
            score+=5;
            printf("Hit a board! Score is now: %d\n", score);
            float rot=enemies_rot[i]+90;
            glm::vec3 normal = glm::vec3(cos(rot), sin(rot), 0);
            glm::vec3 out = glm::reflect(player.speed, normal);
            player.speed.x = out.x;
            player.speed.y = out.y;
            createNewEnemy(i);
        }

        if(enemies[i].position.x >= 8*(level-1)+4)
        {
            createNewEnemy(i);
        }
    }

    for(i=0; i<max_stars; i++)
    {
        if(stars[i].position.x >= 8*(level-1)+4)
        {
            createNewStar(i);
        }
    }
    
    if (player.position.y <= y_min && !inWater)
    {
        player.speed.y *= -e;
        player.speed.x *= e;
        player.position.y = y_min;
    }
    else if (player.position.y <= y_min && inWater)
    {
        float radius_pond = pond1.width/2 + ((pond1.length*pond1.length)/(8*pond1.width));
        float pond_centerX = pond1.position.x;
        float pond_centerY = radius_pond - pond1.width;
        float dist_ball = find_dist(player.position.x, player.position.y, pond_centerX, pond_centerY);
        if(!up)
            player.position.y = -0.76-pond_centerY - sqrt((radius_pond - player.radius)*(radius_pond - player.radius) - (player.position.x - pond_centerX)*(player.position.x - pond_centerX));
    }
    if (player.position.y > y_min)
    {
        player.speed.y += player.acc.y;
    }

    for(i=0; i<3; i++)
    {
        if(trampoline[i].position.x >= 8*(i+1)+4 - trampoline[i].length/2 || trampoline[i].position.x - trampoline[i].length/2 <= 8*(i+1)-4)
        {
            trampoline[i].speedx = -trampoline[i].speedx;
            sticky[i].speedx = -sticky[i].speedx;
        }

        //sticky part under the trampoline[i]
        if(detect_collision(player.bounding_box(), trampoline[i].bounding_box())) {
            if(player.speed.y >= 0.124)
            {
                player.position.y = trampoline[i].position.y + trampoline[i].width/2;
                player.speed.y = -0.124;
            }
            else
            {
                player.position.y = trampoline[i].position.y + trampoline[i].width/2;
                player.speed.y *= -1.2;
            }
        }
        if(detect_collision(player.bounding_box(), sticky[i].bounding_box())) {
            player.speed.y = 0;
            player.speed.x = 0;
            isStuck = 1;
        }
    }
    if(porc1.position.x + porc1.radius/2 >= 20 || porc1.position.x <= 12 + porc1.radius/2)
    {
        porc1.speed.x = -porc1.speed.x;
        porc1.omega = -porc1.omega;
        porc2.speed.x = -porc2.speed.x;
        porc2.speed.y = -porc2.speed.y;
        porc2.omega = -porc2.omega;
    }

    //decrease score
    if(detect_collision_balls(player.position.x, player.position.y, porc1.position.x, porc1.position.y, player.radius, porc1.radius)) 
    {
        score-=50;
        printf("Hit a porcupine! Score is now: %d\n", score);
        player.position.x = 2;
        player.position.y = 4;
        player.speed.x = 0.0;
        player.speed.y = 0.0;
    }

    if(detect_collision_balls(player.position.x, player.position.y, porc2.position.x, porc2.position.y, player.radius, porc2.radius)) 
    {
        score-=50;
        printf("Hit a porcupine! Score is now: %d\n", score);
        player.position.x = 2;
        player.position.y = 4;
        player.speed.x = 0.0;
        player.speed.y = 0.0;
    }

    //magnet
    if(abs(player.position.x - magnet.position.x) <= 2.00)
    {
        player.acc.x = ((abs(player.position.x - magnet.position.x)/(player.position.x - magnet.position.x))*(0.004));
    }
    else
    {
        player.acc.x = 0.00;
    }

    if(score >=100)
    {
        score = 0;
        level++;
        printf("Level Up! Now you are in Level: %d\nYou may now proceed to the next world\n", level);
        for(i=0; i<max_stars; i++)
            stars[i].position.x += 8;
    }

    if(player.position.x >= pond1.position.x - pond1.length/2 && player.position.x <= pond1.position.x + pond1.length/2 && player.position.y <= y_min+0.1)
    {
        inWater = 1;
    }
    else if(player.position.x < pond1.position.x - pond1.length/2 || player.position.x > pond1.position.x + pond1.length/2 || player.position.y > y_min)
    {
        inWater = 0;
    }

    if(player.position.x <= 8*level - 4)
    {
        screen_center_x = 8*((int)((player.position.x+4)/8));
        reset_screen();
    }
    else
    {
        player.position.x = 8*level - 4 - player.radius;
    }

    // printf("%d\n", inWater);
}

float find_dist(float x1, float y1, float x2, float y2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void createNewEnemy(int i){
    color_t colors[3]={COLOR_GREEN, COLOR_BLUE, COLOR_PINK};
    float temp_ypos = (float)rand()/(float)(RAND_MAX/3);
    float temp_speedx = (float)rand()/(float)(RAND_MAX/3);
    //random color
    int col_rand = rand()/(RAND_MAX/2);
    enemies[i] = Ball((level-1)*8-4, temp_ypos-1.5, colors[col_rand], 0.15);
    enemies[i].speed.x = -(temp_speedx+3)/100;
    enemies[i].speed.y = 0.0;
    if(i%5 == 0)
        createNewBoard(i);

    if(level == 2 || level == 3 || level ==4)
    {
        int decider = rand()/(RAND_MAX/2);
        if(decider%2)
        {
            int col_rand = rand()/(RAND_MAX/2);
            enemies[i] = Ball((level-1)*8-4, temp_ypos-1.5, colors[col_rand], 0.15);
            enemies[i].speed.x = -(temp_speedx+3)/100;
            enemies[i].speed.y = 0.0;
            if(i%5 == 0)
                createNewBoard(i);
        }
        else
        {
            enemies[i] = Ball((level-1)*8-4, temp_ypos+2, colors[2], 0.15);
            enemies[i].speed.x = -(temp_speedx+3)/100;
            enemies[i].speed.y = 0.0;
            if(i%5 == 0)
                createNewBoard(i);
        }
        
    }       
}

void createNewStar(int i){
    float temp_ypos = (float)rand()/(float)(RAND_MAX/6);
    stars[i] = Porcupine((level-1)*8-4, temp_ypos-2, COLOR_WHITE, 0.05, 5);
    stars[i].speed.x = -0.008;
    stars[i].speed.y = 0.0;
    stars[i].omega = 0.0;
}

void createNewBoard(int i){
    float rot = 0;
    // float rot = (float)rand()/(float)(RAND_MAX/90);
    rot+=45;
    float rec_xpos=enemies[i].position.x+(enemies[i].radius*cos(rot*pi/180.0));
    float rec_ypos=enemies[i].position.y+(enemies[i].radius*sin(rot*pi/180.0));
    boards[i]=Rectangle(rec_xpos, rec_ypos, 0.5, 0.05, COLOR_BROWN);
    boards[i].speedx=enemies[i].speed.x;
    boards[i].speedy=enemies[i].speed.y;
    rot-=90;
    boards[i].rotation=rot;
    enemies_rot[i]=rot;
}

void initGL(GLFWwindow *window, int width, int height) {

    player = Ball(2, 0, COLOR_RED, 0.2);
    player.speed.x = 0.0;
    player.speed.y = 0.0;
    player.acc.y=0.004;

    ground = Rectangle(0, -3 - player.radius, 80, 2, COLOR_BROWN);
    rec = Rectangle(0, -2.1 - player.radius, 80, 0.2, COLOR_GREEN); //y = -2 - (width/2) - player.radius
    int i=0;
    for(i=0; i<max_enemies; i++)
    {
        createNewEnemy(i);
        float temp_xpos = (float)rand()/(float)(RAND_MAX/3);
        enemies[i].position.x = temp_xpos+8*(level-1)-4;

        boards[i]=Rectangle(0, 0, 0, 0, COLOR_BROWN);
    }

    for(i=0; i<3; i++)
    {
        trampoline[i] = Rectangle(1.5+8*(i+1), 1, 3, 0.2, COLOR_BLUE);
        trampoline[i].speedx = 0.01;
        sticky[i] = Rectangle(1.5+8*(i+1), 0.9, 3, 0.1, COLOR_PINK);
        sticky[i].speedx = 0.01;
    }


    magnet = Rectangle(27.75, 2, 0.5, 3, COLOR_RED);

    porc1  = Porcupine(19, -0.5, COLOR_BROWN, 0.3, 8);
    porc1.speed.x = 0.01;
    porc1.omega = (porc1.speed.x/porc1.radius)*(180/pi);

    porc2  = Porcupine(19, 3.5, COLOR_BROWN, 0.3, 8);
    porc2.speed.x = 0.01;
    porc2.speed.y = 0.006;
    porc2.omega = (porc1.speed.x/porc1.radius)*(180/pi);

    pond1 = Puddle(22, y_min, 3, 1, COLOR_BLUE);

    line = Rectangle(0, -0.05, 8, 0.1, COLOR_BLUE);

    for(i=0; i<max_stars; i++)
    {
        createNewStar(i);
        float temp_xpos = (float)rand()/(float)(RAND_MAX/8);
        stars[i].position.x = temp_xpos+8*(level-1)-4;
    }

    for(int i=0; i<max_enemies+10; i++)
        enemies_rot[i]=0.00;

    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    while (!glfwWindowShouldClose(window)) {

        if (t60.processTick()) {
            draw();
            glfwSwapBuffers(window);
            tick_input(window);
        }
        glfwPollEvents();
    }
    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

bool detect_collision_balls(float posx1, float posy1, float posx2, float posy2, float radius1, float radius2) {
    return ( ((posx2-posx1)*(posx2-posx1)) + ((posy2-posy1)*(posy2-posy1)) <= ((radius1 + radius2)*(radius1 + radius2)) );
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;

    // float ratio = 1/3;

    // if(player.position.y >= bottom*(1-ratio)+ratio*top)
    // {
    //     float offset = player.position.y - bottom+ratio*top;
    //     top += offset;
    //     bottom += offset;
    //     screen_center_y += offset;
    // }

    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}

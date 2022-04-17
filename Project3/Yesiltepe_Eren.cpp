/*********
   CTIS164 - Template Source Program
----------
STUDENT :Eren Yeþiltepe
SECTION :2
HOMEWORK:2
----------
PROBLEMS:
1)In auto mode it sometimes chooses the long path
----------
ADDITIONAL FEATURES:
1)Manual mode added:
    -fishes wing position changes
    -level and score system added
    -while levels increase fish numbers on the screen increases and the game becomes harder
2)Auto mode
3)opening and try again screens added
4)In opening the button changes its size when you put the cursor on it. When you press the game starts in auto mode
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

#define TIMER_PERIOD  10// Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define PI 3.1415

#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 16 // after 8 frames you can throw another one.

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false,mclick;
int  winWidth = WINDOW_WIDTH, winHeight = WINDOW_HEIGHT; // current Window width and height

double mainx = 0, mainy = 0;//positions of the canon
//int mx, my;
double mainangle = 0;//angle of the canon

typedef struct {
    int enx, eny;
    bool envisible;
    float enangle;
    int wingpos;
}enemy_t;

typedef struct {
    int bulx, buly;
    bool set;
    float angle;
}bullet_t;

typedef struct {
    double x, y;

}vertex_t;

bool keeprot = true;// prevent double checking in auto mode
float timev = 60;
int hit = 0,hitminus=0;//hit: number of hits, hitminus: number of missed in manual

enemy_t chararr[10];
bullet_t bularr[MAX_FIRE];

int size = 1;
int level = 1;

double cloudPos[16];

int gamemode = 0; //1:automathic, -1:manual/game mode
int fire_rate = 0;

float buttonsize = 1;//changes button size on opening screen


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}
void drawBG() {
    //clouds
    glColor3f(1, 1, 1);
    int cx, cy;
    for (int i = 0; i < 16; i += 2)
    {
        cx = cloudPos[i];
        cy = cloudPos[i + 1];

        circle(cx, cy, 30);
        circle(cx - 35, cy + 5, 15);
        circle(cx + 25, cy - 2, 15);
        circle(cx + 20, cy + 25, 10);
        circle(cx + 4, cy - 26, 8);

    }

    //ground
    glColor3f(235 / 255., 183 / 255., 52 / 255.);
    glRectf(-winWidth / 2, 150 - winHeight / 2, winWidth / 2, -winHeight / 2);


    //grass
    glColor3f(83 / 255., 235 / 255., 52 / 255.);
    glRectf(-winWidth / 2, 150 - winHeight / 2, winWidth / 2, 100 - winHeight / 2);

    //background statistics
    glColor3f(0, 0, 1);
    glRectf(-winWidth / 2, winHeight / 2, 150 - winWidth / 2, winHeight / 2 - 80);
    glRectf(winWidth / 2 - 80, winHeight / 2, winWidth / 2, winHeight / 2 - 60);
    glColor3f(0, 0, 0);
    vprint2(10 - winWidth / 2, winHeight / 2 - 20, 0.1, "Eren Yesiltepe");
    vprint2(10 - winWidth / 2, winHeight / 2 - 40, 0.1, "Score:%d", hit-hitminus);
    vprint2(10 - winWidth / 2, winHeight / 2 - 60,0.1, "level:%d", level);
    if(timev>0)
    vprint2(winWidth / 2 - 80, winHeight / 2 - 20, 0.1, "Time:%.2f", timev);
    else
        vprint2(winWidth / 2 - 80, winHeight / 2 - 20, 0.1, "Time:%.2f", 0);
}

void drawOpening() {

    glColor3f(0, 0, 0);
    vprint2(-350, 100, 0.5, "Hit The Fish Challange");
    vprint2(-220, 0, 0.2, "Please press the button to play");
    vprint2(-500, -200, 0.2, "You can press <space> button to switch between manual and automatic modes");
    vprint2(-300, -250, 0.2, "The game ends in 60 seconds after you start");

    glColor3ub(245, 227, 66);
    circle(0, -100, 40*buttonsize);

    glColor3ub(66, 173, 245);
    circle(0, -100, 36*buttonsize);

    glColor3ub(245, 227, 66);
    glBegin(GL_TRIANGLES);
    glVertex2f(-16 * buttonsize, -100+20 * buttonsize);
    glVertex2f(-16 * buttonsize, -100-20 * buttonsize);
    glVertex2f(20 * buttonsize, -100);
    glEnd();
}

void drawEnd() {

    glColor3f(0, 0, 0);
    vprint2(-250, 100, 0.6, "Game Over");
    vprint2(-250, 0, 0.3, "Thank You For Playing");
    vprint2(-330, -100, 0.2, "Your last level is:%d       Your hit count is:%d",level,hit-hitminus);
    vprint2(-350, -200, 0.2, "You can play again by restarting the program");
    

}


void drawCharacter(enemy_t en) {
    int size = 3;


    //tail
    glBegin(GL_POLYGON);
    glColor3f(235 / 255., 235 / 255., 52 / 255.);

    glVertex2f(en.enx - 6 * size, en.eny - 0 * size);
    glVertex2f(en.enx - 10 * size, en.eny - 1 * size);
    glVertex2f(en.enx - 18 * size, en.eny + 1 * size);
    glVertex2f(en.enx - 10 * size, en.eny - 6 * size);
    glVertex2f(en.enx - 18 * size, en.eny - 12 * size);
    glVertex2f(en.enx - 10 * size, en.eny - 11 * size);
    glVertex2f(en.enx - 6 * size, en.eny - 10 * size);

    glEnd();

    //fin
    glBegin(GL_POLYGON);
    float angle;

    for (int i = 7; i < 35; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(en.enx - 7 * size / 5 + 12 * cos(angle) * size, en.eny + 12 * sin(angle) * size - 26 * size / 5.);
    }
    glVertex2f(en.enx - 3 * size - 15, en.eny - 1 * size / 5.);

    glEnd();

    //body
    glBegin(GL_POLYGON);
    glColor3f(0, 0, 1);

    glVertex2f(en.enx - 6 * size, en.eny + 10 * size - 10 * size);

    for (int i = 70; i < 85; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(en.enx + 20 * cos(angle) * size, en.eny + 9 * size + 20 * sin(angle) * size);
    }

    glVertex2f(en.enx + 11 * size, en.eny + 10 * size - 10 * size);

    glEnd();

    //eye
    glColor3f(0, 0, 0);
    circle(en.enx + 6 * size, en.eny + 8 * size - 10 * size, 5);

    //wing
    glColor3f(205 / 255., 205 / 255., 32 / 255.);
    
    if (en.wingpos == -1) {
        glBegin(GL_POLYGON);
        glVertex2f(en.enx + 4 * size, en.eny - 5 * size);
        glVertex2f(en.enx - 2 * size, en.eny - 5 * size);
        glVertex2f(en.enx - 6 * size, en.eny - 10 * size);
        glVertex2f(en.enx + 8 * size, en.eny - 10 * size);
        glEnd();
    }
    else {
        glBegin(GL_POLYGON);
        glVertex2f(en.enx + 4 * size, en.eny - 5 * size);
        glVertex2f(en.enx - 2 * size, en.eny - 5 * size);
        glVertex2f(en.enx - 6 * size, en.eny - 0 * size);
        glVertex2f(en.enx + 8 * size, en.eny - 0 * size);
        glEnd();
    }
    

}

void vertex(vertex_t P, vertex_t Tr, double angle)
{
    double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2d(xp, yp);
}

void elipse(int x, int y, int r1, int r2, double angle)
{
#define PI 3.1415
    float angleel;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angleel = 2 * PI * i / 100;
        vertex({ x + r1 * cos(angleel) ,y + r2 * sin(angleel) }, { mainx,mainy }, mainangle * D2R);
    }
    glEnd();
}

void drawmain() {
    int size = 4;
    double k = atan2(6, 8);
    double rec = 90 * D2R;

    glColor3f(0, 0, 0);


    glBegin(GL_POLYGON);

    vertex({ +80,+20 }, { mainx,mainy }, mainangle * D2R);
    vertex({ +80,-20 }, { mainx,mainy }, mainangle * D2R);
    vertex({ 0 ,-20 }, { mainx,mainy }, mainangle * D2R);
    vertex({ 0,+20 }, { mainx,mainy }, mainangle * D2R);

    glEnd();

    glColor3f(0.1, 0.1, 0.1);
    elipse(80, 0, 10, 20, mainangle * D2R);
    glColor3ub(200, 140, 2);
    circle(mainx, mainy, 20);

    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(mainx, mainy);
    glVertex2f(40 * cos(mainangle * D2R), 40 * sin(mainangle * D2R));
    glEnd();
    glLineWidth(1);

}


int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (bularr[i].set == false) return i;
    }
    return -1;
}

void randchar(int i) {
    if(gamemode==1)
          do {
             chararr[i].enx = rand() % winWidth - winWidth / 2;
             chararr[i].eny = rand() % winHeight - winHeight / 2;
            } while ((chararr[i].enx<90 && chararr[i].enx > -90 && chararr[i].eny<90 && chararr[i].eny > -90) || chararr[i].eny < -150);

    else
        do {
            chararr[i].enx = rand() % winWidth - winWidth / 2;
            chararr[i].eny = rand() % winHeight - winHeight / 2;
        } while ((chararr[i].enx<90 && chararr[i].enx > -90 && chararr[i].eny<90 && chararr[i].eny > -90) || chararr[i].eny < -150 || chararr[i].eny > 200);
}
//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0.2, 160 / 255., 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

 
    if (gamemode == 0)
        drawOpening();
    else
    {
        if (timev > 0)
        {
            drawBG();

            //drawmain();
            drawmain();

            //blue circle
            for (int i = 0; i < size; i++)
                if (chararr[i].envisible)
                    drawCharacter(chararr[i]);

            glColor3f(0, 0, 0);
            for (int k = 0; k <= 20; k++)
                if (bularr[k].set)
                    circle(bularr[k].bulx, bularr[k].buly, 5);
        }
        else
            drawEnd();


    }



    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == ' ') 
        gamemode *= -1;

       



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{

    int x2 = x - winWidth / 2;
    int y2 = winHeight / 2 - y;
    // Write your codes here.
     //if fires
    if (gamemode == -1) {
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {

            mclick = true;
        }
        else
            mclick = false;

    }
    else if (gamemode == 0 && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
        if (y2<-55 && y2>-145 && x2<45 && x2>-45)
            gamemode = 1;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {

    int x2 = x - winWidth / 2;
    int y2 = winHeight / 2 - y;

    if (gamemode == -1) {

        mainangle = atan2(y2, x2) / D2R;

        if (mainangle < 0)
            mainangle += 360;
        else if (mainangle >= 360)
            mainangle = 0;

    }
    else if (gamemode == 0)
        if (y2<-55 && y2>-145 && x2<45 && x2>-45)
            buttonsize = 0.75;
        else
            buttonsize = 1;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    if (timev > 0 && gamemode!=0)
    {
        timev -= TIMER_PERIOD / 1000.;
       
        //auto mode controls
        if (gamemode == 1)
        {
            for (int i = 1; i < size; i++)
                chararr[i].envisible = 0;
            size = 1;
          
            float temp;
                temp = atan2((chararr[0].eny - mainy), (chararr[0].enx - mainx)) / D2R;

                if (temp < 0)
                    temp += 360;
                chararr[0].enangle = temp;
           
     
               if (keeprot) {

                if (chararr[0].enangle < mainangle)
                    mainangle -= 1;
                else
                    mainangle += 1;

                if (mainangle < 0)
                    mainangle += 360;
                else if (mainangle >= 360)
                    mainangle = 0;

            }

            if (keeprot)
                if ((int)mainangle == (int)chararr[0].enangle) {

                    bularr[0].set = true;
                    bularr[0].bulx = 0;
                    bularr[0].buly = 0;
                    bularr[0].angle = chararr[0].enangle;

                    keeprot = false;
                  //  printf("attým\n");
                }
        }
      
        //manual mode controls
        if (gamemode == -1)
        {
           
            size = level;
           
            for (int k = 0; k < size; k++)
            {
                chararr[k].wingpos *= -1;
                chararr[k].eny++;

                if (chararr[k].eny > winHeight / 2.)
                {
                    hitminus++;
                    randchar(k);

                    if (hitminus % 8 == 0 && hitminus != 0 && level < 8 && level>0)
                        level--;
                }
            }

            if (mclick && fire_rate == 0) 
            {
                int availFire = findAvailableFire();
                if (availFire != -1) {
                    bularr[availFire].bulx = 0;
                    bularr[availFire].buly = 0;
                    bularr[availFire].angle = mainangle;
                    bularr[availFire].set = true;
                    fire_rate = FIRE_RATE;
                }
            }

            if (fire_rate > 0) fire_rate--;
        }
        

       

        // Move all fires that are active.
        for (int i = 0; i < MAX_FIRE; i++) {
            if (bularr[i].set)
            {
                bularr[i].bulx += 10 * cos(bularr[i].angle * D2R);
                bularr[i].buly += 10 * sin(bularr[i].angle * D2R);

                if (bularr[i].bulx > winWidth/2. || bularr[i].bulx < -winWidth / 2. || bularr[i].buly > winHeight / 2. || bularr[i].buly < -winHeight / 2.) {
                    bularr[i].set = false;

                    if (gamemode == 1)
                        randchar(0);
                }

                //test collusion
                for(int c=0;c<size;c++)
                if (abs((bularr[i].buly) - (chararr[c].eny)) < 50 && abs((bularr[i].bulx) - (chararr[c].enx)) < 50 )
                {
                    bularr[i].set = false;
                    bularr[i].bulx = 0;
                    bularr[i].buly = 0;

                    randchar(c);

                    if (gamemode == 1)
                        keeprot = true;

                    //hit with level controls
                    hit++;
                    if (hit % 8 == 0 && hit != 0 && level < 8 && level>0)
                        level++;

                   
                }

                if (gamemode == -1) {
                    size = level;
                    for (int l = 1; l < size; l++)
                        chararr[l].envisible = true;
                }
            }
        }
        if (gamemode == -1)
            keeprot = true;


    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Eren Yeþiltepe-22002527-hit the enemy");



    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //random clouds
    srand(time(NULL));
    for (int i = 0; i < 16; i += 2)
    {
        cloudPos[i] = rand() % winWidth - 640;
        cloudPos[i + 1] = rand() % winHeight - 300;
    }

    //initialising
    for (int i = 0; i < 20; i++) {
        bularr[i].bulx = 0;
        bularr[i].buly = 0;
        bularr[i].set = false;
    }
    
    //setting positions of the fishes
        chararr[0].envisible = 1;
        for (int i = 0; i < 10; i++) {
            randchar(i);
            chararr[i].wingpos = -1;
        }

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}
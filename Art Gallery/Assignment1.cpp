//  ========================================================================
//  COSC363: Computer Graphics (2023);  University of Canterbury.
//  Assignment1 Zheng Chao 1/3/2023
//  NAME: Optical Illusions Art Gallery
//  FILE NAME:Assignment1.cpp
//
//  ========================================================================

#include <iostream>
#include <fstream>
#include <climits>
#include "loadBMP.h"
#include "loadTGA.h"
#include <math.h>
#include <GL/freeglut.h>
using namespace std;
int theta = -1;
GLuint txId[10];
GLuint texture;
GLUquadric *q;

//--Globals ---------------------------------------------------------------
//From Labs(DOWN)
float* x, * y, * z;					//vertex coordinates
int* nv, * t1, * t2, * t3, * t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces
float angleX = 0.0, angleY = -8;	//Rotation angles about x, y axes
float xmin, xmax, ymin, ymax;		//min, max values of  object coordinates
//From Labs(UP)
float cam_hgt = 0;
int cam_dis = -30;
int direction = 0;
int backX = 0;
float backY = 0;
int moireTimer = 0;
int i = 0;
float dirc = 0.5;
float dNumber = 0;
float dCount = 1;
float hMove = 0;
float hCount = 1;
float  xEye = 0,  yEye = 25,  zEye = -40;
float xMove = 0, yMove = 25, zMove = 100;
int moveCount = 0;
float strict = 1;

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int ne;

    fp_in.open(fname, ios::in);
    if (!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');				//ignore first line
    fp_in >> nvert >> nface >> ne;			    // read number of vertices, polygons, edges (not used)

    x = new float[nvert];                        //create arrays
    y = new float[nvert];
    z = new float[nvert];

    nv = new int[nface];
    t1 = new int[nface];
    t2 = new int[nface];
    t3 = new int[nface];
    t4 = new int[nface];

    for (int i = 0; i < nvert; i++)                         //read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for (int i = 0; i < nface; i++)                         //read polygon list
    {
        fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i] >> t4[i];

    }
    fp_in.close();
    cout << " File successfully read." << endl;
}


void loadTexture(void)
{
    glGenTextures(10, txId); 	// Create texture ids


    glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
    loadTGA("aao3.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadBMP("backGround.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
    loadBMP("try.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture
    loadBMP("ball.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[4]);  //Use this texture
    loadBMP("earth.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[5]);  //Use this texture
    loadBMP("jupiter.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[6]);  //Use this texture
    loadBMP("moon.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, txId[7]);  //Use this texture
    loadBMP("sun.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    float nx, ny, nz;
    nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    glNormal3f(nx, ny, nz);
}
void drawFront()
{
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.3);

    glPushMatrix();//front picture move
    glTranslatef(backY * 0.1, 0, 0);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0., 0.);
    glVertex3f(4, -4, 14);
    glTexCoord2f(1., 0.);
    glVertex3f(-4, -4, 14);
    glTexCoord2f(1., 1.);
    glVertex3f(-4, 4, 14);
    glTexCoord2f(0., 1.);
    glVertex3f(4, 4, 14);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);

    glPopMatrix();

}






void drawBack()        //front photo to make scanimation, op aplha = 0.5
{

    glPushMatrix();

    glColor4f(1, 1, 1, 0.5);    //alpha = 0.5
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.3);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);   //"enable"  allow to use aplha

    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);

    glTexCoord2f(0., 0.);
    glVertex3f(4, -4, 14);
    glTexCoord2f(1., 0.);
    glVertex3f(-4, -4, 14);
    glTexCoord2f(1., 1.);
    glVertex3f(-4, 4, 14);
    glTexCoord2f(0., 1.);
    glVertex3f(4, 4, 14);


    glEnd();
    glDisable(GL_BLEND);    //"Disable"     also need to use glBlendFunc()
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glPopMatrix();
}




void amsWindowTimer(int value)
{
    glutPostRedisplay();
    angleY++;
    backX+=5;
    //backY-=5;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(60, -10, 50); //-0.1 so that the shadow can be seen clearly
        glTexCoord2f(1.0, 0.0);
        glVertex3f(-60, -10, 50);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(-60, 10, 50);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(60, 10, 50);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    if (backY == 15) {
        dirc = -0.5;
    } else if(backY ==- 15) {
        dirc = 0.5;
    }

    backY+=dirc;

    if (dNumber == 10) {
        dCount = -0.5;
    } else if (dNumber == -10) {
        dCount = 0.5;
    }
    dNumber += dCount; glPopMatrix();
    glutTimerFunc(50, amsWindowTimer, 0);
}




void torusModel()     //create a 3D model
{
    glPushMatrix();
        glColor4f(0.8, 0.5, 0, 0.5);
        glTranslatef(-47.7, 0, 19.5);
        glRotatef(-35, 0, 1, 0);
        glRotatef(backX*1.2 ,1,0,0);
        glutSolidTorus(0.3, 1, 10, 20);
    glPopMatrix();

    glPushMatrix();             //left cycle
        glTranslatef(-47.7, 0, 19.5);
        glRotatef(-35, 0, 1, 0);
        glTranslatef(8, 0, 0);
        glRotatef(-backX*1.2 ,1,0,0);
        glutSolidTorus(0.3, 1, 10, 20);
    glPopMatrix();



    glPushMatrix();     //earth
    glTranslatef(-47.7, 0, 19.5);
    glRotatef(-35, 0, 1, 0);
        glColor4f(0, 1, 1, 0.5);
        glTranslatef(4, 0, 0);
        glRotatef(backX * 0.6,0,1,0);
        glRotatef(270,0,1,0);
        glTranslatef(-4, 0, 0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[4]);
        glColor3f(1., 0.7, 0.6);
        GLUquadric *q1;
        q1 = gluNewQuadric();
        gluQuadricTexture(q1, GL_TRUE);
        gluQuadricDrawStyle(q1, GLU_FILL);
        gluSphere(q1, 0.5, 16, 16);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);



    glPushMatrix();      //Jupiter
    glTranslatef(-47.7, 0, 19.5);
    glRotatef(-35, 0, 1, 0);
    glColor4f(0, 1, 1, 0.5);
    glTranslatef(4, 0, 0);
    glRotatef(backX* 0.6,0,1,0);
    glRotatef(180,0,1,0);
    glTranslatef(-4, 0, 0);
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[5]);
        glColor3f(1., 0.7, 0.6);
        GLUquadric *q2;
        q2 = gluNewQuadric();
        gluQuadricTexture(q2, GL_TRUE);
        gluQuadricDrawStyle(q2, GLU_FILL);
        gluSphere(q2, 0.5, 16, 16);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);



    glPushMatrix();    //Moon
    glTranslatef(-47.7, 0, 19.5);
    glRotatef(-35, 0, 1, 0);
    glColor4f(0, 1, 1, 0.5);
    glTranslatef(4, 0, 0);
    glRotatef(backX * 0.6,0,1,0);
    glRotatef(90,0,1,0);
    glTranslatef(-4, 0, 0);

    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[6]);
        glColor3f(1., 0.7, 0.6);
        GLUquadric *q3;
        q3 = gluNewQuadric();
        gluQuadricTexture(q3, GL_TRUE);
        gluQuadricDrawStyle(q3, GLU_FILL);
        gluSphere(q3, 0.5, 16, 16);
    glPopMatrix();



    glPushMatrix();   //Sun
    glTranslatef(-47.7, 0, 19.5);
    glRotatef(-35, 0, 1, 0);
    glColor4f(0, 1, 1, 0.5);
    glTranslatef(4, 0, 0);
    glRotatef(backX* 0.6,0,1,0);
    glTranslatef(-4, 0, 0);
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId[7]);
        glColor3f(1., 0.7, 0.6);
        GLUquadric *q4;
        q4 = gluNewQuadric();
        gluQuadricTexture(q4, GL_TRUE);
        gluQuadricDrawStyle(q4, GLU_FILL);
        gluSphere(q4, 0.5, 16, 16);
    glPopMatrix();


}


void drawBasketBall()      //extra 3d model
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(36, -9, 0);    //position

        glBindTexture(GL_TEXTURE_2D, txId[3]);
        glColor3f(1., 0.7, 0.6);
        GLUquadric *q0;
        q0 = gluNewQuadric();
        gluQuadricTexture(q0, GL_TRUE);
        gluQuadricDrawStyle(q0, GLU_FILL);
        gluSphere(q0, 1, 33, 10);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //Draw the shadow object



}

void drawHuman()
{
    glColor3f(1., 0.78, 0.06);		//Head
    glPushMatrix();
    glRotatef(-15, 0, 1, 0);
    glTranslatef(-47.7, 0, 19.5);

       glTranslatef(10, -1.5, 13);
       glutSolidSphere(0.9, 16, 16);
    glPopMatrix();

    glColor3f(1., 0., 0.);			//Torso
    glPushMatrix();
    glTranslatef(-50, 0, 3);
    glRotatef(-30, 0, 1, 0);

       glTranslatef(14, -3.8, 13);
       glScalef(3, 3, 2.4);
       glutSolidCube(1);
    glPopMatrix();

    glColor3f(0., 0., 1.);			//Left leg
    glPushMatrix();
    glTranslatef(-50, 0, 2.8);
    glRotatef(-30, 0, 1, 0);
    glTranslatef(14.8, -7.3, 13);
         glTranslatef(3, 6.5 ,0);
         glRotatef(-hMove, 1, 0, 0);
         glTranslatef(-3, -6.5, 0);


         glScalef(1, 4, 1);
         glutSolidCube(1);
    glPopMatrix();

    glColor3f(0., 0., 1.);			//Right leg
    glPushMatrix();
    glTranslatef(-50, 0, 2.8);
    glRotatef(-30, 0, 1, 0);
    glTranslatef(13.3, -7.3, 13);
         glTranslatef(-0.8,4,0);
         glRotatef(hMove, 1, 0, 0);
         glTranslatef(0.8, -4, 0);

         glScalef(1, 4, 1);
         glutSolidCube(1);
    glPopMatrix();


    glColor3f(0., 0., 1.);			//Left arm
    glPushMatrix();
    glTranslatef(-50, 0, 3);
    glRotatef(-30, 0, 1, 0);
         glTranslatef(16.5, -2.7, 13);
         glRotatef(-25, 1, 0, 0);
         glRotatef(145, 0, 0, 1);

         glScalef(1, 3, 1);
         glutSolidCube(1);

    glPopMatrix();

    glColor3f(0., 0., 1.);			//Left arm
    glPushMatrix();
    glTranslatef(-50, 0, 3);
    glRotatef(-30, 0, 1, 0);
         glTranslatef(11.5, -2.7, 13);
         glRotatef(-25, 1, 0, 0);
         glRotatef(-145, 0, 0, 1);
         glScalef(1, 3, 1);
         glutSolidCube(1);
    glPopMatrix();


    glColor3f(0.0, 0.0, 0.0);     //left eye
    glPushMatrix();
    glTranslatef(-49, 0, 2.7);
    glRotatef(-30, 0, 1, 0);
       glTranslatef(14, -1.3, 13.4);
       glutSolidSphere(0.1, 16, 16);
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);    //right eye
    glPushMatrix();
    glTranslatef(-49, 0, 2.7);
    glRotatef(-30, 0, 1, 0);
       glTranslatef(13, -1.3, 13.4);
       glutSolidSphere(0.1, 16, 16);
    glPopMatrix();

}





void modelTimer(int value)  //to control the model movement
{
    glutPostRedisplay();
    theta++;
    if (theta >= 5) direction = 1;
    else if (theta <= -5) direction = -1;
    theta += direction;

    if (hMove==4){
          hCount=-1;
    }
    if (hMove==-4){
          hCount=1;
    }
    hMove+=hCount;
    glutTimerFunc(50, modelTimer, 0);
}

//-- Computes the min, max values of coordinates  -----------------------
void computeMinMax()
{
    xmin = xmax = x[0];
    ymin = ymax = y[0];
    for (int i = 1; i < nvert; i++)
    {
        if (x[i] < xmin) xmin = x[i];
        else if (x[i] > xmax) xmax = x[i];
        if (y[i] < ymin) ymin = y[i];
        else if (y[i] > ymax) ymax = y[i];
    }
}


//draw the Gallery floor
void drawFloor()
{

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(60, -10, 80); //-0.1 so that the shadow can be seen clearly
    glTexCoord2f(2.0, 0.0);
    glVertex3f(60, -10, -80);
    glTexCoord2f(2.0, 1.0);
    glVertex3f(-60, -10, -80);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-60, -10, 80);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void drawBackGround()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(60, -10, 50);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-60, -10, 50);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-60, 10, 50);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(60, 10, 50);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//draw the Gallery's roof.
void drawRoof()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(60, 10, 80); //-0.1 so that the shadow can be seen clearly
        glTexCoord2f(2.0, 0.0);
        glVertex3f(60, 10, -80);
        glTexCoord2f(2.0, 1.0);
        glVertex3f(-60, 10, -80);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-60, 10, 80);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void drawLeftWall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(60, 10, 80); //-0.1 so that the shadow can be seen clearly
        glTexCoord2f(2.0, 0.0);
        glVertex3f(60, 10, -80);
        glTexCoord2f(2.0, 1.0);
        glVertex3f(60, -10, -80);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(60, -10, 80);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void drawRightWall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-60, 10, 80); //-0.1 so that the shadow can be seen clearly
        glTexCoord2f(2.0, 0.0);
        glVertex3f(-60, 10, -80);
        glTexCoord2f(2.0, 1.0);
        glVertex3f(-60, -10, -80);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-60, -10, 80);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawFrontWall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(60, 10, -80); //-0.1 so that the shadow can be seen clearly
        glTexCoord2f(2.0, 0.0);
        glVertex3f(-60, 10, -80);
        glTexCoord2f(2.0, 1.0);
        glVertex3f(-60, -10, -80);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(60, -10, -80);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}





//--Function to compute the normal vector of a triangle with index indx ----------
void normaloff(int indx)
{
    float x1 = x[t1[indx]], x2 = x[t2[indx]], x3 = x[t3[indx]];
    float y1 = y[t1[indx]], y2 = y[t2[indx]], y3 = y[t3[indx]];
    float z1 = z[t1[indx]], z2 = z[t2[indx]], z3 = z[t3[indx]];
    float nx, ny, nz;
    nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    glNormal3f(nx, ny, nz);
}



void amesWindow()
{
    glPushMatrix();
    glRotatef(45,0,1,0);
    glTranslatef(25,0,3);

    glRotatef(angleX, 1.0, 0.0, 0.0);			//rotate the object about the x-axis
    glRotatef(angleY, 0.0, 1.0, 0.0);			//rotate the object about the y-axis
    for (int indx = 0; indx < nface; indx++)		//draw each face
    {
        if (indx <= 6) glColor3f(1., 1., 1.);
        else glColor3f(0., 1., 1.);
        normaloff(indx);
        if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
        else				glBegin(GL_QUADS);
        glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
        glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
        glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
        if (nv[indx] == 4)
            glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
        glEnd();
    }
    glPopMatrix();
}

//--Display: all of functions
void display()
{

    glEnable(GL_LIGHTING);
    glClearColor(1, 1, 1, 1);
    //Front light's position
    float lpos[4] = {xEye , 0, zEye, 1.0 };
    float lpos1[4] = { 0, 0, -10, 1.0 };			//Back light's position
    float lpos2[4] = { -1., 1., 0, 1.0 };

    float white[4] = { 1, 1, 1, 1 };
    float gray[4] = { 0.2, 0.2, 0.2, 1 };
    float cyan[4] = { 0, 1, 1, 1 };
    float black[4] = { 0, 0, 0, 1 };
    float mat_diffuse[4] = { 1, 0, 0, 1 };
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float XaxisDir = sin(cam_hgt);     //x camera
    float ZaxisDir = cos(cam_hgt);     //z camera
        xEye += strict * moveCount * XaxisDir;
        zEye += strict * moveCount * ZaxisDir;
        moveCount = 0;
        xMove = xEye + XaxisDir * 2;
        yMove = 25;
        zMove = zEye + ZaxisDir * 2;
    gluLookAt(xEye, 0, zEye, xMove, 0, zMove, 0, 1, 0);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);	//set light position
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glLightfv(GL_LIGHT1, GL_POSITION, lpos1);	//set back light position
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glEnable(GL_LIGHT1);


    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
    drawFront();
    drawBack();
    drawFloor();
    drawBasketBall();
    torusModel();
    drawBackGround();
    drawRoof();
    drawHuman();
    drawLeftWall();
    drawRightWall();
    drawFrontWall();
    amesWindow();
    glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
    float widthMd, hightMd;
    float white[] = { 1, 1, 1, 1 };
    loadMeshFile("AmesWindow.off");			//Specify mesh file name here
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour
    q = gluNewQuadric();
    loadTexture();
    glEnable(GL_LIGHTING);					//Enable OpenGL states
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    //Opacity

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);   //light1
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10); //

    computeMinMax();						    //Compute min, max values of x, y coordinates for defining camera frustum
    widthMd = xmax - xmin;						//Model width
    hightMd = ymax - ymin;    //model hight
    xmin -= 0.2 * widthMd;						//make 20% of origin
    xmax += 0.2 * widthMd;
    ymin -= 0.2 * hightMd;
    ymax += 0.2 * hightMd;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(xmin, xmax, ymin, ymax, 10, 1000);  //The camera view volume
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene
void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) {
            cam_hgt += 0.02;
        }
        else if(key == GLUT_KEY_RIGHT) {
            cam_hgt -= 0.02;
        }
        else if (key == GLUT_KEY_UP) {
            moveCount = 1;
        }
        else if (key == GLUT_KEY_DOWN) {
            moveCount = -1;
        }
     glutPostRedisplay();
}


void keyBoard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case '1': cam_hgt = 0; xEye = 2; yEye = -5; zEye = -20; break; //turn left
    case '2': cam_hgt = 7.2; xEye = 2; zEye = -32; break; //turn right              0.02 * 36 = 7.2
    case '3': cam_hgt = -7; xEye = 2; zEye = -32;break; //move forward
    case '0': cam_hgt = 0; xEye = 0; zEye = -80;break; //move backward
    }
    glutPostRedisplay(); //update display


}



//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(1800, 1800);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Optical Illusions Art Gallery");
    initialize();

    glutTimerFunc(50, amsWindowTimer, 0);
    glutTimerFunc(50, modelTimer, 0);
    glutKeyboardFunc(keyBoard);
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}

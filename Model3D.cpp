//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: Model3D.cpp
//  See Lab01.pdf for details
//
//  Program to load a mesh file in OFF format.
//  Assumptions: 
//      1.  Each polygon has 3 or 4 vertices.
//      2.  The file does not contain any additional comment lines.
//  ========================================================================
 
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h> 
#include <GL/freeglut.h>
#include "loadBMP.h"
#include "loadTGA.h"
#include <math.h>
using namespace std;

//--Globals ---------------------------------------------------------------
float *x, *y, *z;					//vertex coordinates
int *nv, *t1, *t2, *t3, *t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces
float chack = 0.5;
float angleX = 0,  angleY = -20;	//Rotation angles about x, y axes
float xmin, xmax, ymin, ymax;		//min, max values of  object coordinates
float timer_r;
float mover_r = 0;
float s=1;         //camera motion factor
float d=2;
float cam_angle=0;
float  e_x = 0,  e_y = 25,  e_z = -30;
float l_x = 0, l_y = 25, l_z = 100;
int step = 0;
GLuint txId[4];

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)  
{
	ifstream fp_in;
	int ne;

	fp_in.open(fname, ios::in);
	if(!fp_in.is_open())
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

	for(int i=0; i < nvert; i++)                         //read vertex list 
		fp_in >> x[i] >> y[i] >> z[i];

	for(int i=0; i < nface; i++)                         //read polygon list 
	{
		fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i];
		if (nv[i] == 4)
			fp_in >> t4[i];
		else
			t4[i] = -1;
	}

	fp_in.close();
	cout << " File successfully read." << endl;
}

//-- Computes the min, max values of coordinates  -----------------------
void computeMinMax()
{
	xmin = xmax = x[0];
	ymin = ymax = y[0];
	for(int i = 1; i < nvert; i++)
	{
		if(x[i] < xmin) xmin = x[i];
		else if(x[i] > xmax) xmax = x[i];
		if(y[i] < ymin) ymin = y[i];
		else if(y[i] > ymax) ymax = y[i];
	}
}

//--Function to compute the normal vector of a triangle with index indx ----------
void normaloffs(int indx)
{
	float x1 = x[t1[indx]], x2 = x[t2[indx]], x3 = x[t3[indx]];
	float y1 = y[t1[indx]], y2 = y[t2[indx]], y3 = y[t3[indx]];
	float z1 = z[t1[indx]], z2 = z[t2[indx]], z3 = z[t3[indx]];
	float nx, ny, nz;
	nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
	ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
	nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
	glNormal3f(nx, ny, nz);
}

void timmer(int)
{
    glutPostRedisplay();
    timer_r ++;
    glutTimerFunc(50, timmer,0);
}

void reverse_y(int)
{
    glutPostRedisplay();
    angleY++;
    glutTimerFunc(50, reverse_y,0);
}

void mover(int)
{

    glutPostRedisplay();

    if (mover_r == 0)
    {
        chack = -0.5;
    }else if (mover_r == -10)
    {
        chack = 0.5;
    }
    mover_r += chack;

    glutTimerFunc(50, mover,0);
}



//---- Floor plane -----
void drawFloor()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);

    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-80, -20, 50);
       glTexCoord2f(5.,0.);
       glVertex3f(80, -20, 50);
       glTexCoord2f(5.,5.);
       glVertex3f(80, -20, -150);
       glTexCoord2f(0.,5.);
       glVertex3f(-80, -20, -150);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//---- BlackGround plane -----
void drawBlackg()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-80, -20, -150);
       glTexCoord2f(1.,0.);
       glVertex3f(-80, -20, 50);
       glTexCoord2f(1.,1.);
       glVertex3f(-80, 20, 50);
       glTexCoord2f(0.,1.);
       glVertex3f(-80, 20, -150);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(80, -20, -150);
       glTexCoord2f(1.,0.);
       glVertex3f(80, -20, 50);
       glTexCoord2f(1.,1.);
       glVertex3f(80, 20, 50);
       glTexCoord2f(0.,1.);
       glVertex3f(80, 20, -150);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-80, -20, 50);
       glTexCoord2f(1.,0.);
       glVertex3f(80, -20, 50);
       glTexCoord2f(1.,1.);
       glVertex3f(80, 20, 50);
       glTexCoord2f(0.,1.);
       glVertex3f(-80, 20, 50);
    glEnd();
    glDisable(GL_TEXTURE_2D);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-80, -20, -150);
       glTexCoord2f(1.,0.);
       glVertex3f(80, -20, -150);
       glTexCoord2f(1.,1.);
       glVertex3f(80, 20, -150);
       glTexCoord2f(0.,1.);
       glVertex3f(-80, 20, -150);
    glEnd();
    glDisable(GL_TEXTURE_2D);


}





//---- ceiling plane -----
void drawCeiling()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[2]);

    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-80, 20, 50);
       glTexCoord2f(1.,0.);
       glVertex3f(80, 20, 50);
       glTexCoord2f(1.,1.);
       glVertex3f(80, 20, -150);
       glTexCoord2f(0.,1.);
       glVertex3f(-80, 20, -150);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


//---- movie pic -----
void drawpic1()
{
    glPushMatrix();
    glTranslatef(-30, 0, 0);
    glRotatef(-45,0,1,0);
    glTranslatef(30, 0, 0);
    glColor4f(1,1,1,0.5);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.8);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-25, -5, 4.9);
       glTexCoord2f(1.,0.);
       glVertex3f(-35, -5, 4.9);
       glTexCoord2f(1.,1.);
       glVertex3f(-35, 5, 4.9);
       glTexCoord2f(0.,1.);
       glVertex3f(-25, 5, 4.9);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glPopMatrix();
}
void drawpic2()
{
    glPushMatrix();
    glTranslatef(-30, 0, 0);
    glRotatef(-45,0,1,0);
    glRotatef(timer_r, 0, 0, 1);
    glTranslatef(mover_r * 0.3,0, 0);

    glTranslatef(30, 0, 0);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.8);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);

    glBegin(GL_QUADS);			//A single quad
       glTexCoord2f(0.,0.);
       glVertex3f(-25, -5, 5);
       glTexCoord2f(1.,0.);
       glVertex3f(-35, -5, 5);
       glTexCoord2f(1.,1.);
       glVertex3f(-35, 5, 5);
       glTexCoord2f(0.,1.);
       glVertex3f(-25, 5, 5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glPopMatrix();
}

//---- 3D model -----
void play3d()
{

    glPushMatrix();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,0,0); //atom
    glRotatef(-timer_r * 20,0,1,1);
    glRotatef(45,1,1,1);
    glTranslatef(-0.5,0,0);
    glColor3f(1, 0.2, 0.2);
    glutSolidSphere(1,33,10);
    glTranslatef(1,0,0);
    glColor3f(0.2, 0.2, 1);
    glutSolidSphere(1,33,10);
    glPopMatrix();


    glPushMatrix();//each electron
    glTranslatef(30,0,0);
    glRotatef(20,0,0,1);
    glRotatef(-timer_r * 5,0,1,0);
    glRotatef(-10,0,0,1);
    glTranslatef(-6,0,0);
    glColor3f(0, 0.6, 1);
    glutSolidSphere(0.4,33,10);
    glPopMatrix();

    glPushMatrix(); //each electron
    glTranslatef(30,0,0);
    glRotatef(50,0,0,1);
    glRotatef(timer_r * 8,0,1,0);
    glRotatef(-10,0,0,1);
    glTranslatef(-5,0,0);
    glColor3f(0, 0.6, 1);
    glutSolidSphere(0.4,33,10);
    glPopMatrix();

    glPushMatrix(); //each electron
    glTranslatef(30,0,0);
    glRotatef(-52,0,0,1);
    glRotatef(timer_r * 4,0,1,0);
    glRotatef(-10,0,0,1);
    glTranslatef(-3,0,0);
    glColor3f(0.2, 0.2, 0.2);
    glutSolidSphere(0.2,33,10);
    glPopMatrix();
}


//---- window off -----
void playwindow()
{
    glPushMatrix();
    glRotatef(angleX, 1.0, 0.0, 0.0);			//rotate the object about the x-axis
    glRotatef(angleY, 0.0, 1.0, 0.0);
    for(int indx = 0; indx < nface; indx++)		//draw each face
    {
        if (indx <= 6) glColor3f(0., 0., 1.);
        else glColor3f(0., 1., 1.);
        normaloffs(indx);
        if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
        else				glBegin(GL_QUADS);
            glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
            glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
            glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
            if(nv[indx]==4)
              glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
        glEnd();
    }
    glPopMatrix();
}


void loadTextures()
{
    glGenTextures(4, txId); 	// Create 4 texture ids

    glBindTexture(GL_TEXTURE_2D, txId[0]);  //First texture
    loadBMP("floor.bmp");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, txId[1]);  //second texture
    loadTGA("123.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, txId[2]);  //second texture
    loadBMP("star.bmp");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, txId[3]);  //second texture
    loadBMP("back.bmp");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);


    \
}

//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.

void display()  
{
    float lpos1[4] = {0, 0., -100, 1};
    float lpos2[4] = {0, 0., +100, 1};			//light's position
    float white[3] = {1, 1, 1};
    float black[3] = {0, 0, 0};
    float orange[3] = {0.8, 0.2, 0};
    float gray[3] = {0.2, 0.2, 0.2};
    float cyan[3] = {0, 1, 1};
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    float dir_x = sin(cam_angle), dir_z = cos(cam_angle);
    e_x += step * s * dir_x;
    e_z += step * s * dir_z;
    step = 0;
    if (e_x >= 25){
        e_x = 25;
    } else if (e_x <= -25){
        e_x = -25;
    }
    if (e_z >= -22){
        e_z = -22;
    } else if (e_z <= -100){
        e_z =-100;
    }
    l_x = e_x + d * dir_x;
    l_z = e_z + d * dir_z;
    gluLookAt(e_x, 3, e_z, l_x, 3, l_z, 0, 1, 0);








    glLightfv(GL_LIGHT1, GL_POSITION, lpos2);// light 1
    glLightfv(GL_LIGHT1, GL_AMBIENT, gray);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, orange);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos1);// light 2
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cyan);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cyan);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);	//set light position
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    drawpic1();
    drawpic2();

    drawFloor();//drow the floor
    drawBlackg();
    drawCeiling();



    playwindow(); //play the windows pic

    play3d(); // play the 3d model

	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
	float model_wid, model_hgt;
    loadMeshFile("squer.off");			//Specify mesh file name here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour
    loadTextures();
	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	computeMinMax();						    //Compute min, max values of x, y coordinates for defining camera frustum
	model_wid = xmax-xmin;						//Model width and height
	model_hgt = ymax-ymin;
	xmin -= 0.2*model_wid;						//Extend minmax window further by 20% of its size.
	xmax += 0.2*model_wid;
	ymin -= 0.2*model_hgt;
	ymax += 0.2*model_hgt; 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(xmin, xmax, ymin, ymax, 10, 1000);  //The camera view volume  
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene

void NormalKey(unsigned char key, int x, int y)
{
    switch(key){
        case'0':
            cam_angle = 0;
            e_x = 0;
            e_z = -100;
            break;
        case'1':
            cam_angle = 0;
            e_x = 0;
            e_z = -30;
            break;
        case'2':
            cam_angle = 0.8;
            e_x = 0;
            e_z = -30;
            break;
        case'3':

            cam_angle = -0.8;
            e_x = 0;
            e_z = -30;
            break;


    }
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) {
        cam_angle += 0.1;
    }
    else if(key == GLUT_KEY_RIGHT) {
        cam_angle -= 0.1;
    }
    else if (key == GLUT_KEY_UP) {
        step = -2;
    }
    else if (key == GLUT_KEY_DOWN) {
        step = 2;
    }
    glutPostRedisplay();
}

//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH);
   glutInitWindowSize (600, 600); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("Model3D");
   initialize();
   glutTimerFunc(100, timmer,0);
   glutTimerFunc(100, reverse_y,0);
   glutTimerFunc(100, mover,0);
   glutDisplayFunc(display);
   glutKeyboardFunc(NormalKey);
   glutSpecialFunc(special); 
   glutMainLoop();
   return 0;
}

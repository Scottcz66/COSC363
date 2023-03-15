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
#include "loadBMP.h"
#include <math.h> 
#include <GL/freeglut.h>
using namespace std;
//int theta = 0;
GLuint txId[1];

//--Globals ---------------------------------------------------------------
float* x, * y, * z;					//vertex coordinates
int* nv, * t1, * t2, * t3, * t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces
float angleX = 0.0, angleY = -8;	//Rotation angles about x, y axes
float xmin, xmax, ymin, ymax;		//min, max values of  object coordinates
int cam_hgt = 0;



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
		//fp_in >> t4[i];

	}

	fp_in.close();
	cout << " File successfully read." << endl;
}


void amsWindowTimer(int value)
{
	glutPostRedisplay();
	angleY++;
	glutTimerFunc(50, amsWindowTimer, 0);
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
//void drawModel()
//{
	//glColor3f(1.0, 1.0, 0.0);           //改颜色
	//glPushMatrix();
	//glTranslatef(-10, 5, 17);
	//glScalef(80, 10, 6);
	//glutSolidCube(1);
	//glPopMatrix();
//}

//void drawFloor()
//{
	//glColor3f(0., 0., 0.); // Floor colour

	//for (int i = -50; i <= 50; i++)
	//{
		//glBegin(GL_LINES); // A set of grid lines on the xz-plane
		//glVertex3f(-50, -3, i);
		//glVertex3f(50, -3, i);
		//glVertex3f(i, -3, -50);
		//glVertex3f(i, -3, 50);
		//glEnd();
	//}
//}

void drawFloor()
{
	float white[4] = { 1., 1., 1., 1. };
	float black[4] = { 0 };

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);

	//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBegin(GL_QUADS);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	for (int i = -256; i < 256; i += 16)
	{
		for (int j = -256; j < 256; j += 16)
		{
			//glTexCoord2f(0.0, 0.0);
			glVertex3f(i, -18, j); //-0.1 so that the shadow can be seen clearly
			//glTexCoord2f(0.0, 1.0);
			glVertex3f(i, -18, j + 16);
			//glTexCoord2f(1.0, 1.0);
			glVertex3f(i + 16, -18, j + 16);
			//glTexCoord2f(1.0, 0.0);
			glVertex3f(i + 16, -18, j);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, black);
}


void loadTexture(void)
{
	glGenTextures(1, txId); 	// Create texture ids

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
	loadBMP("ground.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
}




//--Function to compute the normal vector of a triangle with index indx ----------
void normal(int indx)
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


//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display()
{

	glEnable(GL_LIGHTING);

	float lpos[4] = { 1., 1., 10, 1.0 };			//Front light's position
	float lpos1[4] = { 1., 1., -10, 1.0 };			//Back light's position
	float lpos2[4] = { -1., 1., 0, 1.0 };

	float white[4] = { 1, 1, 1, 1 };
	float gray[4] = { 0.2, 0.2, 0.2, 1 };
	float cyan[4] = { 0, 1, 1, 1 };
	float black[4] = { 0, 0, 0, 1 };
	float mat_diffuse[4] = { 1, 0, 0, 1 };
	glClearColor(1, 1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(cam_hgt, 0, 20, 0, 0, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);	//set light position 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glLightfv(GL_LIGHT1, GL_POSITION, lpos1);	//set back light position 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glEnable(GL_LIGHT1);


	

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	drawFloor();
	//drawModel();

	glRotatef(angleX, 1.0, 0.0, 0.0);			//rotate the object about the x-axis
	glRotatef(angleY, 0.0, 1.0, 0.0);			//rotate the object about the y-axis

	

	for (int indx = 0; indx < nface; indx++)		//draw each face
	{
		if (indx <= 6) glColor3f(1., 1., 1.);
		else glColor3f(0., 1., 1.);
		normal(indx);
		if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
		else				glBegin(GL_QUADS);
		glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
		glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
		glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
		if (nv[indx] == 4)
			glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
		glEnd();
	}

	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
	float model_wid, model_hgt;
	loadMeshFile("Octahedron.off");			//Specify mesh file name here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	loadTexture();
	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	computeMinMax();						    //Compute min, max values of x, y coordinates for defining camera frustum
	model_wid = xmax - xmin;						//Model width and height
	model_hgt = ymax - ymin;
	xmin -= 0.2 * model_wid;						//Extend minmax window further by 20% of its size.
	xmax += 0.2 * model_wid;
	ymin -= 0.2 * model_hgt;
	ymax += 0.2 * model_hgt;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(xmin, xmax, ymin, ymax, 10, 1000);  //The camera view volume  
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene
void special(int key, int x, int y)
{
	//int theta = 0;
	//if (theta >= 0) theta++; angleX++;
	if (key == GLUT_KEY_LEFT) angleY--;
	else if (key == GLUT_KEY_RIGHT) angleY++;
	else if (key == GLUT_KEY_UP) angleX--;
	else if (key == GLUT_KEY_DOWN) angleX++;

	 //if (theta >= 0) {
		//angleX++;
	//}
	glutPostRedisplay();
}

//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Model3D");
	initialize();
	glutTimerFunc(50, amsWindowTimer, 0);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;
}

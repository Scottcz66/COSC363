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
int theta = -1;
GLuint txId[4];

//--Globals ---------------------------------------------------------------
float* x, * y, * z;					//vertex coordinates
int* nv, * t1, * t2, * t3, * t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces
float angleX = 0.0, angleY = -8;	//Rotation angles about x, y axes
float xmin, xmax, ymin, ymax;		//min, max values of  object coordinates
int cam_hgt = 1;
int cam_dis = -30;
int direction = 0;


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


void loadTexture(void)
{
	glGenTextures(3, txId); 	// Create texture ids

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
	loadBMP("ground.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
	loadBMP("cat.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
	loadBMP("back_photo.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture
	loadBMP("VaseTexture.bmp");
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
void drawBird()
{
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(-15, -4, 0);
	glTexCoord2f(1., 0.);
	glVertex3f(-35, -4, 0); //-0.1 so that the shadow can be seen clearly
	glTexCoord2f(1., 1.);
	glVertex3f(-35, 4, 0);
	glTexCoord2f(0., 1.);
	glVertex3f(-15, 4, 0);
	
	

	glEnd();
	glDisable(GL_TEXTURE_2D);
	
}

void drawPhoto()
{
	glColor4f(1, 1, 1, 0.5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	glBegin(GL_QUADS);

	glNormal3f(0, 1, 1);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 0); //-0.1 so that the shadow can be seen clearly
	glTexCoord2f(1, 0.0);
	glVertex3f(5, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(5, 5, 0);
	glTexCoord2f(0.0, 1);
	glVertex3f(0, 5, 0);


	glEnd();
	glDisable(GL_TEXTURE_2D);

}




void amsWindowTimer(int value)
{
	glutPostRedisplay();
	angleY++;
	glutTimerFunc(50, amsWindowTimer, 0);
}



void D_model()     //create a 3D model
{
	glPushMatrix();
	glTranslatef(6, -3, 0);
	glRotatef(theta, 0, 1, 0);
	glTranslatef(-6, 3, 0);
	glRotatef(theta, 0, 0, 1);
	glTranslatef(13, 0, 0);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(10, 4, 0);
	glRotatef(theta, 1, 0, 0);
	glTranslatef(-10, -4, 0);

	glTranslatef(13, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

}

void modelTimer(int value)
{
	glutPostRedisplay();
	theta++;
	if (theta >= 5) direction = 1;
	else if (theta <= -5) direction = -1;
	theta += direction;
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


void drawFloor()
{
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0, 0.0);
	glVertex3f(500, -10, 100); //-0.1 so that the shadow can be seen clearly
	glTexCoord2f(4.0, 0.0);
	glVertex3f(500, -10, -100);
	glTexCoord2f(4.0, 4.0);
	glVertex3f(-500, -10, -100);
	glTexCoord2f(0.0, 4.0);
	glVertex3f(-500, -10, 100);
		
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



void playxx()
{
	glPushMatrix();

	//float light1_pos[4] = { 21.21, 10, 21.21, 1 };
	//float light1_dir[3] = { 0.5, -0.7071, 0.5 };
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);


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

//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display()
{

	glEnable(GL_LIGHTING);
	glClearColor(0, 0, 0, 0);
	float lpos[4] = { 0., 5., 10, 1.0 };			//Front light's position
	float lpos1[4] = { 0., 5., -10, 1.0 };			//Back light's position
	float lpos2[4] = { -1., 1., 0, 1.0 };

	float white[4] = { 1, 1, 1, 1 };
	float gray[4] = { 0.2, 0.2, 0.2, 1 };
	float cyan[4] = { 0, 1, 1, 1 };
	float black[4] = { 0, 0, 0, 1 };
	float mat_diffuse[4] = { 1, 0, 0, 1 };
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(cam_hgt, 0, cam_dis, cam_hgt, 0, 0, 0, 1, 0);
	//gluPerspective(30, 10, 10, 20);
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
	drawBird();
	drawFloor();
	
	//drawModel();
	D_model();




	//glRotatef(cam_hgt, 0.0, 1.0, 0.0);		//rotate the whole scene


	playxx();


	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
	float model_wid, model_hgt;
	//float white[] = { 1, 1, 1, 1 };
	loadMeshFile("Octahedron.off");			//Specify mesh file name here
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	loadTexture();
	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//glEnable(GL_LIGHT1);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, white);   //light1
	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10); //See slides [03]:24-26

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
	if (key == GLUT_KEY_LEFT) cam_hgt++;
	else if (key == GLUT_KEY_RIGHT) cam_hgt--;
	else if (key == GLUT_KEY_UP) cam_dis++;
	else if (key == GLUT_KEY_DOWN) cam_dis--;

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
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Model3D");
	initialize();

	glutTimerFunc(50, amsWindowTimer, 0);
	glutTimerFunc(50, modelTimer, 0);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;
}

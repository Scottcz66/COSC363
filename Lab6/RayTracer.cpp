/*==================================================================================
* COSC 363  Computer Graphics (2023)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab06.pdf   for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Plane.h"
#include "SceneObject.h"
#include "TextureBMP.h"
#include "Ray.h"
#include "Cylinder.h"
#include "Cone.h"
#include <GL/freeglut.h>
using namespace std;

TextureBMP texture;

const float EDIST = 25.0;
const int NUMDIV = 1500;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;
//set Fog

//const float Z1 = -100;
//const float Z2 = -300;

const float Z1 = 0;
const float Z2 = 0;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtainehorizontald by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
    glm::vec3 lightPos(15, 40, -3);					//Light's position
    glm::vec3 lightPos1(-15, 40, -3);
	glm::vec3 color(0);
    SceneObject* obj;
    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
    obj = sceneObjects[ray.index];	 //object on which the closest point of intersection is found

    const float refractivityETA = 1 / 1.5;;

    if(ray.index == 5) {
        int stripeWidth = 3;
        int ix = ((ray.hit.x) / stripeWidth) + 50;
        int iz = ((ray.hit.z) / stripeWidth);
        int k = (iz + ix) % 2;
        if (k != 0) color = glm::vec3(0.953, 0.6, 0.227);
        else color = glm::vec3(1, 1, 1);
        obj->setColor(color);
    }
    if(ray.index == 2) {
        glm::vec3 d = obj->normal(ray.hit);
        float u = 0.5 + asin(d.x) / M_PI;
        float v = 0.5 + asin(d.y) / M_PI;
        color = texture.getColorAt(u, v);
        obj->setColor(color);;
    }




    color = obj->lighting(lightPos, lightPos1 ,-ray.dir, ray.hit);						//Object's colour
    //add shadows
    glm::vec3 lightVec = lightPos - ray.hit;
    glm::vec3 lightVec1 = lightPos1 - ray.hit;


    Ray shadowRay(ray.hit, lightVec);
    shadowRay.closestPt(sceneObjects);

    Ray shadowRay1(ray.hit, lightVec1);
    shadowRay1.closestPt(sceneObjects);

    float lightDist = glm::length(lightVec);
    if (shadowRay.index > -1 && shadowRay.dist < lightDist) {
        SceneObject* shadowobject = sceneObjects[shadowRay.index];
        if (shadowobject->isRefractive() || shadowobject->isTransparent()) {
            color = 0.4f * obj->getColor();
        } else  {
            color = 0.2f * obj->getColor();
        }
    }

    float lightDist1 = glm::length(lightVec1);
    if (shadowRay1.index > -1 && shadowRay1.dist < lightDist1) {
        SceneObject* shadowobject = sceneObjects[shadowRay1.index];
        if (shadowobject->isRefractive() || shadowobject->isTransparent()) {
            color = 0.4f * obj->getColor();
        } else  {
            color = 0.2f * obj->getColor();
        }
    }


    //Reflections

    if (obj->isReflective() && step < MAX_STEPS) {
        float rho = obj->getReflectionCoeff();

        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color = color + (rho * reflectedColor);
    }



    if (obj->isTransparent() && step < MAX_STEPS)
        {
            float rho = obj->getTransparencyCoeff();
            Ray transpRay1(ray.hit, ray.dir);
            transpRay1.closestPt(sceneObjects);
            Ray transpRay2(transpRay1.hit, transpRay1.dir);
            glm::vec3 transpColor = trace(transpRay2, step + 1);
            color = color + ((1 - rho) * transpColor);
        }



    if (obj->isRefractive() && step < MAX_STEPS) {
        float cof = obj->getRefractionCoeff();
        float rint = obj->getRefractiveIndex();

        glm::vec3 normalVec = obj->normal(ray.hit);
        glm::vec3 refract = glm::refract(ray.dir, normalVec, 1/rint);
        Ray refrRay(ray.hit, refract);
        refrRay.closestPt(sceneObjects);
        glm::vec3 m = obj->normal(refrRay.hit);
        glm::vec3 h = glm::refract(refract, -m, 1.0f/(1/rint));
        Ray refrRay1(refrRay.hit, h);
        refrRay1.closestPt(sceneObjects);

         //Ray transparencyRay(ray.hit, ray.dir);
         glm::vec3 transparentColour = trace(refrRay1, 1);
         color = color * cof + transparentColour * (1-cof);
    }








    //fog function
    glm::vec3 fogColor(1, 1, 1);
    float lamda = (ray.hit.z - Z1) / (Z2 - Z1);
    if (ray.hit.z < Z2 || ray.hit.z > Z1) {
        lamda = 0;
    }
    color = (1 - lamda) * color + lamda * fogColor;


	return color;
}


//open anti-Aliasing
glm::vec3 antiAliasing(Ray ray, glm::vec3 view, float xp, float yp) {
    float pixel = (XMAX - XMIN) / NUMDIV;
    //make 1 pixel to 4
    Ray quarter1 = Ray(view, glm::vec3(xp + pixel * 0.25, yp + pixel * 0.25, -EDIST));
    quarter1.normalize();
    Ray quarter2 = Ray(view, glm::vec3(xp + pixel * 0.25, yp + pixel * 0.75, -EDIST));
    quarter2.normalize();
    Ray quarter3 = Ray(view, glm::vec3(xp + pixel * 0.75, yp + pixel * 0.25, -EDIST));
    quarter3.normalize();
    Ray quarter4 = Ray(view, glm::vec3(xp + pixel * 0.75, yp + pixel * 0.75, -EDIST));
    quarter4.normalize();
    //set the average number
    glm::vec3 average(0.25);
    return (trace(quarter1, 1) + trace(quarter2, 1) + trace(quarter3, 1) + trace(quarter4, 1)) * average;
}


//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray trtaced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid pointx
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
    glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

			Ray ray = Ray(eye, dir);

            //ray.normalize();                                   //Open the anti-aliasing
            //glm::vec3 col = antiAliasing(ray, eye, xp, yp);    //Open the anti-aliasing


            glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}



    glEnd();
    glFlush();
}


//create a mirror
void bigMirror1() {
    Plane* mirror1 = new Plane(glm::vec3(-30., 20, -110), //Point A
        glm::vec3(-15, 20, -110), //Point B
        glm::vec3(-15, 30, -95), //Point C
        glm::vec3(-30., 30, -95));   //Point D
        mirror1->setColor(glm::vec3(0, 0, 0));
        mirror1->setReflectivity(true, 1.05);
        sceneObjects.push_back(mirror1);
}

void bigMirror() {
    Plane* mirror1 = new Plane(glm::vec3(-50., -15, -150), //Point A
        glm::vec3(50, -15, -150), //Point B
        glm::vec3(50., 10, -150), //Point C
        glm::vec3(-50., 10, -150));   //Point D
        mirror1->setColor(glm::vec3(0, 0, 0));
        mirror1->setReflectivity(true, 1.1);
        sceneObjects.push_back(mirror1);
}

//create the second mirror
void secondMirror() {
    Plane* mirror2 = new Plane(glm::vec3(-50., -15, 30), //Point A
        glm::vec3(50, -15, 30), //Point B
        glm::vec3(50., 10, 30), //Point C
        glm::vec3(-50., 10, 30));   //Point D
        mirror2->setColor(glm::vec3(0, 0, 0));
        mirror2->setReflectivity(true, 1.1);
        sceneObjects.push_back(mirror2);
}

void secondMirror1() {
    Plane* mirror2 = new Plane(glm::vec3(49., -10, -200), //Point A
                               glm::vec3(49, -10, 40), //Point B
                               glm::vec3(49., 40, 40), //Point C
                               glm::vec3(49., 40, -200));   //Point D
                               mirror2->setColor(glm::vec3(0, 0, 0));
                               mirror2->setReflectivity(true, 1.05);
                               //mirror1->setSpecularity(true);
                               sceneObjects.push_back(mirror2);
}

void secondMirror2() {
    Plane* mirror2 = new Plane(glm::vec3(-20., -15, -60), //Point A
                               glm::vec3(10, -15, -60), //Point B
                               glm::vec3(10., -15, -90), //Point C
                               glm::vec3(-20., -15, -90));   //Point D
                               mirror2->setColor(glm::vec3(0, 0, 0));
                               mirror2->setReflectivity(true, 1.05);
                               //mirror1->setSpecularity(true);
                               sceneObjects.push_back(mirror2);
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    texture = TextureBMP("earth.bmp");

//Add the diff objects

    //Object 0 (ray.index == 0)0.953, 0.6, 0.227
    Sphere *sphere1 = new Sphere(glm::vec3(0.0, 9, -90.0), 7.0);    //15.0 means radius 15 units.....
    sphere1->setColor(glm::vec3(0.004, 0.188, 0.329));   //Set colour to blue
    //sphere1->setSpecularity(false);    //open/close secula.
    //sphere1->setShininess(500);    //change the light point size.
    sphere1->setReflectivity(true, 0.5);  //Lab7
    sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

    //Object 10, 0., 0.
    Sphere *sphere2 = new Sphere(glm::vec3(-10, -4, -75.0), 4.0);
    //sphere2->setColor(glm::vec3(1, 1, 1));
    sphere2->setRefractivity(true, 0.2, 1.005);
    //sphere2->setReflectivity(false);  //Lab7
    sceneObjects.push_back(sphere2);


    //Object 2  earth
    Sphere *sphere3 = new Sphere(glm::vec3(3.0, -12.0, -50.0), 2.0);
    //sphere3->setColor(glm::vec3(0, 1, 0));   //Set colour to blue
    sphere1->setReflectivity(true, 0.8);

    sceneObjects.push_back(sphere3);		 //Add sphere to scene objects

    //Object 3
    Sphere *sphere4 = new Sphere(glm::vec3(10.0, -4.0, -75.0), 4.0);
    //sphere4->setColor(glm::vec3(0, 1, 1));   //Set colour to green
    sphere4->setRefractivity(true, 0.2, 1.5);
    sceneObjects.push_back(sphere4);		 //Add sphere to scene ob5jects

    //transparency
    Sphere *sphere5 = new Sphere(glm::vec3(-3.0, -12.0, -50.0), 2.0);
    sphere5->setColor(glm::vec3(0.556, 0.643, 0.792));   //Set colour to green
    sphere5->setTransparency(true, 0.7);
    sceneObjects.push_back(sphere5);		 //Add sphere to scene objects



    //Object 4 (ray.index == 5)
    Plane *plane = new Plane (glm::vec3(-50., -15, -40), //Point A     -----add the plane
                              glm::vec3(50., -15, -40), //Point B
                              glm::vec3(50., -15, -300), //Point C
                              glm::vec3(-50., -15, -300)); //Point D
    //plane->setColor(glm::vec3(0.8, 0.8, 0));
    plane->setSpecularity(false);
    sceneObjects.push_back(plane);




    Plane *roof = new Plane(glm::vec3(-50, 50, -40),   //A
                                 glm::vec3(50, 50, -40),  //B
                                 glm::vec3(50, 50, -300),  //C
                                 glm::vec3(-50, 50, -300));  //D
    roof->setColor(glm::vec3(1, 1, 1));
    roof->setSpecularity(false);
    sceneObjects.push_back(roof);


    Plane *backWall = new Plane(glm::vec3(-50, -15, -300),
                                 glm::vec3(50, -15, -300),
                                 glm::vec3(50, 50, -300),
                                 glm::vec3(-50, 50, -300));
    backWall->setColor(glm::vec3(0.839, 0.764, 0.698));
    backWall->setSpecularity(false);
    sceneObjects.push_back(backWall);


    Plane *leftWall = new Plane(glm::vec3(-50, -15, -40),
                                 glm::vec3(-50, -15, -300),
                                 glm::vec3(-50, 50, -300),
                                 glm::vec3(-50, 50, -40));
    leftWall->setColor(glm::vec3(0.973, 0.808, 0.843));
    leftWall->setSpecularity(false);
    sceneObjects.push_back(leftWall);


    Plane *rightWall = new Plane(glm::vec3(50, -15, -300),
                                 glm::vec3(50, -15, -40),
                                 glm::vec3(50, 50, -40),
                                 glm::vec3(50, 50, -300));
    rightWall->setColor(glm::vec3(0.635, 0.811, 0.835));
    rightWall->setSpecularity(false);
    sceneObjects.push_back(rightWall);




    Cone *cone = new Cone(glm::vec3(10, 5, -60), 2, 5);
    cone->setColor(glm::vec3(0, 1, 1));   //Set colour
    //sceneObjects.push_back(cone);



    Cylinder *cylinder1 = new Cylinder(glm::vec3(-10, -15, -75), 2, 7);
    cylinder1->setColor(glm::vec3(0.647, 0.8, 0.863));   //Set colour
    //cylinder1->setReflectivity(true, 0.8);
    sceneObjects.push_back(cylinder1);

    Cylinder *cylinder2 = new Cylinder(glm::vec3(0, -15, -90), 0.5, 17);
    cylinder2->setColor(glm::vec3(0.89, 0.854, 0.839));   //Set colour
    sceneObjects.push_back(cylinder2);

    Cylinder *cylinder3 = new Cylinder(glm::vec3(10, -15, -75), 2, 7);
    cylinder3->setColor(glm::vec3(0.518, 0.733, 0.624));   //Set colour
    sceneObjects.push_back(cylinder3);


    Cylinder *cylinder4 = new Cylinder(glm::vec3(-30, -15, -105), 1, 3);
    cylinder4->setColor(glm::vec3(0, 1, 1));   //Set colour
    sceneObjects.push_back(cylinder4);

    bigMirror();
    secondMirror();
    bigMirror1();



}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}

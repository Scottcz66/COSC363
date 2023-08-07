
#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    //glm::vec3 d = p0 - center;
    float h = height;
    float r = radius;
    float a = (dir.x * dir.x) + (dir.z * dir.z);
    float b = 2 * (dir.x * (p0.x - center.x) + dir.z * (p0.z - center.z));
    float c = ((p0.x - center.x) * (p0.x - center.x) + (p0.z - center.z) * (p0.z - center.z) - r * r);
    float delta = b * b - 4 * (a * c);

    if(delta < 0.001){
        return -1;
    }
    if(delta < 0.0) return -1;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float bT;
    float sT;

    if(t1 < 0.01) t1 = -1;
    if(t2 < 0.01) t2 = -1;

    float ypos = p0.y + dir.y * t1;
    float ypos1 = p0.y + dir.y * t2;

    if (ypos > center.y + h  && ypos1 > center.y + h) {
        return -1;
    } else if (ypos < center.y && ypos1 < center.y) {
        return -1;
    }
    if (ypos > center.y + h && ypos1 < center.y + h){
        return (center.y + h - p0.y) / dir.y;  //with cap;
    }else if (ypos1 > center.y + h && ypos < center.y + h){
        return (center.y + h - p0.y) / dir.y; //with cap;
    }else if (t1 < 0) {
        return (t2 > 0) ? t2 : -1;
    }
        else return t1;

}


glm::vec3 Cylinder::normal(glm::vec3 p)
{

    glm::vec3 n = glm::vec3 (p.x - center.x,0,p.z -center.z);
    n = glm::normalize(n);
    return n;

}

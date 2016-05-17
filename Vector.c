#include <math.h>

#include "Vector.h"

Vector V_new(float x, float y, float z){
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

void V_print(Vector v, char *message){
    fprintf(stderr,"%s : %f %f %f\n",message, v.x,v.y,v.z);
}

Vector V_add(Vector v1, Vector v2){
    return  V_new(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector V_substract(Vector v1, Vector v2){
    return V_new(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector V_multiply(double lambda, Vector v){
	return V_new(lambda*v.x, lambda*v.y, lambda*v.z);
}

Vector V_cross(Vector v1, Vector v2){
    return V_new(
        v1.y*v2.z - v1.z - v2.y,
        v1.z*v2.x - v1.x - v2.z,
        v1.x*v2.y - v1.y - v2.x
    );
}

float V_dot(Vector v1, Vector v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float V_length(Vector v){
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}   

Vector V_unit(Vector v){
    //first compute the length
    float l = V_length(v);
    return V_new(v.x/l, v.y/l, v.z/l);
}

int V_isOnTheRight(Vector M, Vector A, Vector B){
    return (det(M,A,B)<=0);
}

int V_segmentsIntersect(Vector p1, Vector p2, Vector q1, Vector q2){
    /*
    si q1 et q2 de par et d'autre de [p1p2]
    &&
    si p1 et p2 de par et d'autre de [q1q2]
    */
    if( V_isOnTheRight(q1, p1, p2) + V_isOnTheRight(q2, p1, p2) == 1 &&
            V_isOnTheRight(p1, q1, q2) + V_isOnTheRight(p2, q1, q2) == 1)
        return 1;
    return 0;
}

int V_rayIntersectsSegment(Vector M, Vector u_ray, Vector p1, Vector p2);

Vector V_turnAroundY(Vector p, double angle){
    return V_new(
        cos(angle)*p.x + sin(angle)*p.z,
        p.y,
        cos(angle)*p.z - sin(angle)*p.x
    );
}

Vector V_turnAroundZ(Vector p, double angle){
    return V_new(
        cos(angle)*p.x - sin(angle)*p.y,
        sin(angle)*p.x + cos(angle)*p.y,
        p.z
    );
}

Vector V_projectOnPlane(Vector v, Vector normal){
    //scalar distance from point to plane along the normal
    double dist = V_dot(v, normal);
    //projection
    return V_substract(v, V_multiply(dist/V_length(normal) * V_length(normal), normal)); 
}

double V_decompose(Vector p, Vector u){
    return V_length(u)*V_dot(p, u);
}

Vector V_recompose(double x, double y, double z, Vector u, Vector v, Vector w){
    return V_add(V_multiply(x,u), V_add(V_multiply(y,v), V_multiply(z,w)));
}

void V_uxUyFromUz(Vector u_z, Vector *u_x, Vector *u_y){
    *u_x = V_multiply(V_length(u_z), V_unit(V_cross(V_new(0,1,0), u_z)));
    *u_y = V_multiply(V_length(u_z), V_unit(V_cross(*u_x, u_z)));
}

Vector V_rotate(Vector p, Vector centre, Vector v1, Vector v2){
    Vector ux, uy, vx, vy;

    //repere ux uy v1
    V_uxUyFromUz(V_unit(v1), &ux, &uy);
    //repere vx vy v2
    V_uxUyFromUz(V_unit(v2), &vx, &vy);
    
    //on decompose le premier repere
    float x = V_decompose(p, ux);
    float y = V_decompose(p, uy);
    float z = V_decompose(p, V_unit(v1));

    //image de p
    return V_add(centre, V_recompose(x, y, z, vx, vy, V_unit(v2)));
}

//fonctions supplémentaires

//calcul du déterminant de 3 vecteurs
float det(Vector v1, Vector v2, Vector v3){
    return (v1.x*v2.y*v3.z + v1.y*v2.z*v3.x + v1.z*v2.x*v3.y
        - v3.x*v2.y*v1.z - v3.y*v2.z*v1.x - v3.z*v2.x*v1.y);
}
#include "Polygon.h"

Polygon* P_new(){
	Polygon* p = malloc(sizeof(Polygon));
	P_init(p);
	return p;
}

void P_init(Polygon *p){
	//0 arêtes
	p->_nb_vertices = 0;
	//set properties
	p->_is_closed = 0;
	p->_is_filled = 0;
	p->_is_convex = 0;
}

void P_copy(Polygon *original, Polygon *copy){
	//recopier les propriétés
	copy->_is_closed = original->_is_closed;
	copy->_is_filled = original->_is_filled;
	copy->_is_convex = original->_is_convex;

	//recopier le tableau
	copy->_nb_vertices = original->_nb_vertices;
	int i;
	for(i=0; i < original->_nb_vertices; i++){
		copy->_vertices[i] = original->_vertices[i];
	}
}

void P_addVertex(Polygon *P, Vector p){
	P->_vertices[P->_nb_vertices] = p;
	P->_nb_vertices++;
    P->_is_convex = P_isConvex(P);
    if (P->_nb_vertices > 2) {
        P->_is_closed = 1;
    }
}

void P_removeLastVertex(Polygon *P){
	if(P->_nb_vertices>0) {
        P->_nb_vertices--;
        P->_is_convex = P_isConvex(P);
        if (P->_nb_vertices < 3) {
            P->_is_closed = 0;
        }
    }
}

void P_print(Polygon *P, char *message){
	int i;
	fprintf(stderr, "%s\n",message);
	fprintf(stderr, "%d\n", P->_is_closed);
	fprintf(stderr, "%d\n", P->_is_filled);
	fprintf(stderr, "%d\n", P->_is_convex);
	fprintf(stderr, "%d\n", P->_nb_vertices);
	for(i=0; i < P->_nb_vertices; i++){
		V_print(P->_vertices[i], "point : ");
	}
} 

void P_draw(Polygon *P, int width, int height){
	switch(P->_nb_vertices) {
        case 1:
            glBegin(GL_POINTS);
            Vector v = P->_vertices[0];
            glVertex3f(v.x,v.y,v.z);
            break;
        case 2:
            glBegin(GL_LINES);
            Vector v1 = P->_vertices[0];
            Vector v2 = P->_vertices[1];
            glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z);
            break;
        default:
        	if(P->_is_filled)
            	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
            	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_POLYGON);
            int i = 0;
            for (i = 0; i<P->_nb_vertices; i++) {
                Vector vi = P->_vertices[i];
                glVertex3f(vi.x, vi.y, vi.z);
            }
    }
    glEnd();
}

int P_isConvex(Polygon *P){
	//si ce n'ai pas au moins un triangle retourne faux
	if(P->_nb_vertices < 2)
		return 0;
	
	int is_convex= 0;
	int i;
	//tester si, lors d'un tour du polygone, on va toujours du meme cote
	for(i=0; i<P->_nb_vertices; i++)
		//cas général
		if(i+2<P->_nb_vertices-1)
			is_convex += V_isOnTheRight(P->_vertices[i+2], P->_vertices[i], P->_vertices[i+1]);
		//2e sommet / dernier sommet / premier sommet
		else if(i+1>P->_nb_vertices-1)
			is_convex += V_isOnTheRight(P->_vertices[1], P->_vertices[i], P->_vertices[0]);
		//1er sommet / avant-dernier / dernier
		else
			is_convex += V_isOnTheRight(P->_vertices[0], P->_vertices[i], P->_vertices[i+1]);
	//si tous a gauche ou tous a droite retourne vrai
	if(is_convex == 0 || is_convex == P->_nb_vertices)
		return 1;
	return 0;
}

int P_isOnTheLeftOfAllEdges(Polygon *P, Vector M){
	int i;
	if(P->_nb_vertices == 0)
		return 0;
	//si M est a droite une fois retourne faux
	for(i=0; i<P->_nb_vertices; i++){
		if(i+1 < P->_nb_vertices){
			if(V_isOnTheRight(M, P->_vertices[i], P->_vertices[i+1]))
				return 0;
		}
		else{
			if(V_isOnTheRight(M, P->_vertices[i], P->_vertices[0]))
				return 0;
		}
	}
	return 1;
}

int P_nbEdgesIntersectedByRay(Polygon *P, Vector M, Vector u_ray);
int P_isInside(Polygon *P, Vector M);

void P_turnAroundY(Polygon *P, double radians){
	int i;
    for (i=0; i<P->_nb_vertices; i++) {
        P->_vertices[i] = V_turnAroundY(P->_vertices[i], radians);
    }
}

Vector P_center(Polygon *P){
	//on part de l'origine
	Vector v = V_new(0,0,0);
    if(P->_nb_vertices > 0){
	    int i;
	    //on ajoute a iterativement chaque sommets
	    for (i=0; i<P->_nb_vertices; i++) {
	        v = V_add(v, P->_vertices[i]);
	    }
		//on fait la moyenne (on multiplie par 1/n)
	    v = V_multiply((float)1.0/P->_nb_vertices, v);
	}
    return v;
}

Vector P_normal(Polygon *P){
	//arete n°1
	Vector v1 = V_substract(P->_vertices[1], P->_vertices[0]);
	//arete n°2
	Vector v2 = V_substract(P->_vertices[2], P->_vertices[0]);
	//on recupere la normale (produit vectoriel) de v1 et v2)
	return V_unit(V_cross(v1, v2));
}

void P_scale(Polygon *P, double factor); 
void P_translate(Polygon *P, Vector trans){
	int i;
    for (i=0; i<P->_nb_vertices; i++) {
        P->_vertices[i] = V_add(P->_vertices[i], trans);
    }
}

void P_rotate(Polygon *P, Vector normal){
	Vector normal_p = P_normal(P);
	Vector center_p = P_center(P);
	int i;
	for (i = 0; i < P->_nb_vertices; i++)
		V_rotate(P->_vertices[i], center_p, normal_p, normal);
}
#include "Mesh.h"

Mesh* M_new(){
	Mesh* m = malloc(sizeof(Mesh));
	m->_nb_quads = 0;
	m->_is_filled = 0;
	return m;
}

void M_print(Mesh *P, char *message){
	fprintf(stderr, "%s\n", message);
	fprintf(stderr, "nombre de quads : %d\n", P->_nb_quads );
	fprintf(stderr, "est rempli : %d\n", P->_is_filled);
	int i;
	for(i=0; i<P->_nb_quads; i++){
		V_print(P->_quads[i]._vertices[0], "arete 1 ");
		V_print(P->_quads[i]._vertices[1], "arete 2 ");
		V_print(P->_quads[i]._vertices[2], "arete 3 ");
		V_print(P->_quads[i]._vertices[3], "arete 4 ");
	}
}

void M_addQuad(Mesh *P, Quad q){
	P->_quads[P->_nb_quads] = q;
	P->_nb_quads++;
}

void M_addSlice(Mesh *P, Polygon *p1, Polygon *p2){
	if (p1->_nb_vertices == p2->_nb_vertices) {
		int i;
		for (i=0; i<p1->_nb_vertices; i++) {
			Quad q;
			q._vertices[0] = p1->_vertices[i];
			q._vertices[1] = p1->_vertices[(i+1)%p1->_nb_vertices];
			q._vertices[2] = p2->_vertices[(i+1)%p1->_nb_vertices];
			q._vertices[3] = p2->_vertices[i];
			M_addQuad(P, q);
		}
	}
}

void M_revolution(Mesh *P, Polygon *p1, int nb_slices){
	Polygon poly;
	float angle = (float)(2.0)*M_PI/nb_slices;
	int i;
	for (i=0; i<nb_slices; i++) {
		P_copy(p1, &poly);
		P_turnAroundY(p1, angle);
		M_addSlice(P, &poly, p1);
	}
}

void M_perlinExtrude(Mesh *QM, Polygon *p, int nb_slices){
	Polygon copy;
	Vector v;
	int i;
	for (i=0; i<nb_slices; i++) {
		v = PRLN_vectorNoise(P_center(p));
		P_copy(p, &copy);
		P_translate(p, v);
		P_rotate(p, v);
		M_addSlice(QM, &copy, p);
	}
}

void drawQuad(Quad q){
	
	glBegin(GL_QUADS);
	//premier vecteur
	Vector v = q._vertices[0];
	glVertex3f(v.x, v.y, v.z);
	//deuxieme
	v = q._vertices[1];
	glVertex3f(v.x, v.y, v.z);
	//troisieme vecteur
	v = q._vertices[2];
	glVertex3f(v.x, v.y, v.z);
	//dernier vecteur
	v = q._vertices[3];
	glVertex3f(v.x, v.y, v.z);
	
	glEnd();
}

void M_draw(Mesh *P){
	int i;
	//parametres
	//mode non filaire
	if (P->_is_filled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//mode filaire
	} else {
    	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	for (i=0; i<P->_nb_quads; i++) {
		glColor3f(drand48(), drand48(), drand48());
		drawQuad(P->_quads[i]);		
	}
}
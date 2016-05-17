
/*======================================================*\
  Wednesday September the 25th 2013
  Arash HABIBI
  Question1.c
\*======================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glx.h>

#include "Vector.h"
#include "Polygon.h"
#include "Mesh.h"

#define DIM2 0
#define DIM3 1
int dim=DIM2;

/* dimensions de la fenetre */
int width = 650;
int height = 650;

int theta = 0;
int phi = 0;

int affichage = 0;

GLfloat p_light[4];
Vector p_aim;
Polygon* P;
Mesh* M;
int stop = 0;

//------------------------------------------------------------

void chooseColor(double r, double g, double b)
{
  	glColor3d(r,g,b);
}

void drawLine(Vector p1, Vector p2)
{
	glBegin(GL_LINES);
	glVertex3d(p1.x,p1.y,p1.z);
	glVertex3d(p2.x,p2.y,p2.z);
	glEnd();
}
void drawRepere(){
	Vector origine = V_new(0,0,0);
	Vector x = V_new(1,0,0);
	Vector y = V_new(0,1,0);
	Vector z = V_new(0,0,1);

	chooseColor(1, 0, 0);
	drawLine(origine, x);
	chooseColor(0, 1, 0);
	drawLine(origine, y);
	chooseColor(0, 0, 1);
	drawLine(origine, z);
}

//------------------------------------------------------------

void initShade()
{
	GLfloat mat_diffuse[] = {1,1,1,1.0};
	GLfloat mat_ambient[] = {0.1,0.1,0.1,0.0};

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, p_light);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}


/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(dim==DIM2)	glOrtho(-1,1,-1,1,-1,1);
	else{
		gluPerspective( 40, (float)width/height, 1, 100);
		glTranslatef(p_aim.x, p_aim.y, p_aim.z);
		glRotatef(theta, 1, 0, 0);
		glRotatef(phi, 0, 1, 0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Dessiner ici
	// ...
	if (P_isConvex(P)) {
		//si p est convex couleur rouge
		glColor3d(1,0,0);
	} 
	else {
		//sinon de couleur bleue
		glColor3d(0,0,1);
	}

	if(dim==DIM2) {
		P_draw(P, width, height);
	}
	//en dimension 3
	else {
		M_draw(M);
	}


	// Repere du monde

	drawRepere();

	glutSwapBuffers();
}

//------------------------------------------------------------

void keyboard(unsigned char keycode, int x, int y)
{
	// printf("Touche frapee : %c (code ascii %d)\n",keycode, keycode);

	if (keycode==27) // ECHAP
		exit(0);
	if (keycode=='e') {
		dim = DIM3;
		M_perlinExtrude(M, P, 64);
		stop = 1;
	}
	if (keycode=='r') {
		dim = DIM3;
		M_revolution(M, P, 128);
		stop = 1;
	}
	//initialisation
	if (keycode=='i') {
		//reset things
		P = P_new();
		M = M_new();
		p_aim = V_new(0,0,-2.75);
		theta = 0;
		phi = 0;
		stop = 0;
		dim = DIM2;
	}
	if(keycode=='a'){
		if(affichage == 0){
			M->_is_filled = 1;
			affichage++;
		}
		else if(affichage == 1){
			affichage++;
		}
		else{
			M->_is_filled = 0;
			affichage = 0;
		}
	}

	//rotations
	switch(keycode){
		case 'z':
			theta +=10;
			break;
		case 's':
			theta -=10;
			break;
		case 'q':
			phi += 1;
			break;
		case 'd':
			phi -=1;
			break;
	}

	glutPostRedisplay();
}

//------------------------------------------------------------

void special(int keycode, int x, int y)
{
	int mod = glutGetModifiers();
	switch(keycode)
    {
    case GLUT_KEY_UP        : printf("Flèche haut\n"); break;
    case GLUT_KEY_DOWN      : printf("Flèche bas\n"); break;
    case GLUT_KEY_LEFT      : printf("Flèche gauche\n"); break;
    case GLUT_KEY_RIGHT     : printf("Flèche droite\n"); break;
    case GLUT_KEY_PAGE_UP   : printf("Flèche avant\n"); break;
    case GLUT_KEY_PAGE_DOWN : printf("Flèche arriere\n"); break;
    default : fprintf(stderr,"function special : unknown keycode %d\n",keycode); break;
    }
	if(mod==GLUT_ACTIVE_CTRL)
		glLightfv(GL_LIGHT0, GL_POSITION, p_light);
}

//------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
	float xf = (double)(x-width/2.0)/(width/2.0);
    float yf = (double)(-y+height/2)/(height/2.0);

	switch(button){	
    	case GLUT_LEFT_BUTTON :
		if(state==GLUT_DOWN) {
			if (!stop) {
				P_addVertex(P, V_new(xf, yf, 0));
			}
		}
		break;
    	
    	case GLUT_MIDDLE_BUTTON :
			if(state==GLUT_DOWN) {
				fprintf(stderr,"Clic milieu\n");
			}
			break;

    	case GLUT_RIGHT_BUTTON :
			if(state==GLUT_DOWN) {
				if (!stop) {
					P_removeLastVertex(P);
				}
			}
			break;
	}
	glutPostRedisplay();
}

//------------------------------------------------------------

void idle()
{
	// animation du personnage ici

	glutPostRedisplay();
}

/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
    glutCreateWindow("Transformations matricielles");
    glViewport(0, 0, width, height);
	glClearColor(0,0,0,0);

	glutDisplayFunc(display);
//	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	p_light[0]=-10.0;
	p_light[1]=20.0;
	p_light[2]=0.0;
	p_light[3]=1.0;

	p_aim = V_new(0,0,-2.75);
	P = P_new();
	M = M_new();

	glutMainLoop();

    return 0;
}

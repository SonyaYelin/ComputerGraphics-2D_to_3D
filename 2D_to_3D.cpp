#include "glut.h"
#include <math.h>
#include <time.h>


typedef struct 
{
	double x, y;

} POINT2D;


typedef struct 
{
	double x, y, z;

} POINT3D;


const int		POINTS_MAX_SIZE = 100;
const int		DRAW_POINT_SIZE = 5;
const int		SLICES = 360;
const double	PI = 3.14;
const double	WIN_SIZE = 600.0;

int				m_currSize = 0;
POINT2D			m_points[POINTS_MAX_SIZE];


void init();
void drawCylinder( int n, double topr, double bottomr );
void drawSphere( int n, int slices );
void drawLines();
void drawBackground();
void display();
void idle();
void drawPoints();
void mouse( int button, int state, int x, int y );
void main( int argc, char* argv[] );


//--------------------------------------------------------------------------//
void init()
{
	glClearColor( 0, 0, 0.3, 0 );	// color of window background
	glOrtho( -1, 1, -1, 1, -1, 1 );	// set the coordinates system
	glEnable( GL_DEPTH_TEST );		// use Z-buffer
}
//--------------------------------------------------------------------------//
void drawCylinder( int n, double topr, double bottomr )
{
	double alpha;
	double teta = 2 * PI / n;

	for( alpha = 0; alpha <= 2 * PI; alpha += teta )
	{
		glBegin( GL_POLYGON );
		glColor3d( 1 - 0.7 * fabs( sin( alpha ) ), 1 - fabs( sin( alpha ) ), ( 1 + cos( alpha ) ) / 2 );
		glVertex3d( topr * sin( alpha ), 1, topr * cos( alpha ) );						//2
		glVertex3d( topr * sin( alpha + teta ), 1, topr * cos( alpha + teta ) );		//3
		glVertex3d( bottomr * sin( alpha + teta ), 0, bottomr * cos( alpha + teta ) );	//4
		glVertex3d( bottomr * sin( alpha ), 0, bottomr * cos( alpha ) );				//1
		glEnd();
	}
}
//--------------------------------------------------------------------------//
void drawSphere( int n, int slices )
{
	double diff = 0;

	glPushMatrix();
	{
		glTranslated( 0, 1, 0 );

		for ( int i = 0; i < m_currSize - 1; ++i )
		{
			diff += m_points[i].y - m_points[i + 1].y;
			glPushMatrix();
			glTranslated( 0, -diff, 0 );
			glScaled( 1, m_points[i].y - m_points[i + 1].y, 1 );
			drawCylinder( n, m_points[i].x - 0.5, m_points[i + 1].x - 0.5 );
			glPopMatrix();
		}
	}
	glPopMatrix();
}
//--------------------------------------------------------------------------//
void drawLines()
{
	glBegin( GL_LINES );
	glColor3d( 0.5, 0.5, 0.1 );
	for ( int i = 0; i < m_currSize - 1; ++i )
	{
		glVertex3d( m_points[i].x, m_points[i].y, 0 );
		glVertex3d( m_points[i + 1].x, m_points[i + 1].y, 0 );

		glVertex3d( m_points[i].x - 2 * ( m_points[i].x - 0.5 ), m_points[i].y, 0 );
		glVertex3d( m_points[i + 1].x - 2 * ( m_points[i + 1].x - 0.5 ), m_points[i + 1].y, 0 );
	}
	glEnd();
}
//--------------------------------------------------------------------------//
void drawBackground()
{
	//1. draw right side 
	glPushMatrix();
	{
		glTranslated(0, 0, -0.001); //backwords

		glBegin(GL_POLYGON);
		glColor3d(0.5, 0.5, 0.5);	//gray
		glVertex3d(0, 1, 0);
		glVertex3d(1, 1, 0);
		glVertex3d(1, -1, 0);
		glVertex3d(0, -1, 0);
		glEnd();
	}
	glPopMatrix();

	//2. draw aux y
	glBegin(GL_LINES);
	glColor3d(1, 1, 1);
	glVertex3d(0.5, 1, 0);		
	glVertex3d(0.5, -1, 0);		
	glEnd();

	//3. draw left side 
	glBegin(GL_POLYGON);
	glColor3d(0.5, 0.5, 1);
	glVertex3d(-1, 1, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(0, -1, 0);
	glVertex3d(-1, -1, 0);
	glEnd();
}
//--------------------------------------------------------------------------//
void display()
{	// clean frame buffer and set Z-buffer to infinity
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//set transformations from the start
	glLoadIdentity(); 

	glMatrixMode( GL_PROJECTION );
	glFrustum( -1, 1, -1, 1, 1, 300 );
	gluLookAt(	0, 0, 2,	// eye coordinates
				0, 0, 0,	// point of interest
				0, 1, 0	);	// up

	glPushMatrix();
	{
		glScaled( 2, 2, 1 );
		drawBackground();
		drawPoints();
		drawLines();
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslated( -1, 0, 0 );
		drawSphere( SLICES, SLICES );
	}
	glPopMatrix();

	glutSwapBuffers(); // show all
}
//--------------------------------------------------------------------------//
void idle()
{
	glutPostRedisplay();
}
//--------------------------------------------------------------------------//
void drawPoints()
{
	glPointSize( DRAW_POINT_SIZE );
	glBegin( GL_POINTS );
	glColor3d( 1, 0, 0 ); //red

	for ( int i = 0; i < m_currSize; ++i )
	{
		//right point
		glVertex3d( m_points[i].x, m_points[i].y, 0 );

		//left Point
		glVertex3d( m_points[i].x - 2 * (m_points[i].x - 0.5 ), m_points[i].y, 0 );
	}

	glEnd();
}
//--------------------------------------------------------------------------//
void mouse( int button, int state, int x, int y )
{
	if(	state == GLUT_DOWN &&
		button == GLUT_LEFT_BUTTON &&
		m_currSize < POINTS_MAX_SIZE 
		&& x >= 450 )
	{
		m_points[m_currSize].x = ( x / ( WIN_SIZE / 2 ) ) - 1.0;
		m_points[m_currSize].y = ( -y / ( WIN_SIZE / 2 ) ) + 1.0;
		++m_currSize;
	}
}
//--------------------------------------------------------------------------//
void main( int argc, char* argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WIN_SIZE, WIN_SIZE );
	glutInitWindowPosition( 300, 50 );
	glutCreateWindow( "Ex4" );

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMouseFunc(mouse);

	init();

	glutMainLoop();
}
//--------------------------------------------------------------------------//
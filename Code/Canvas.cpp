// Canvas.cpp

#include "Canvas.h"
#include <gl/GLU.h>

int Canvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

Canvas::Canvas( wxWindow* parent ) : wxGLCanvas( parent, wxID_ANY, attributeList )
{
	rotatingBoxWithMouse = false;

	context = nullptr;
	flowerBox = new FlowerBox();

	orientMatrix[0] = 1.0;
	orientMatrix[1] = 0.0;
	orientMatrix[2] = 0.0;
	orientMatrix[3] = 0.0;

	orientMatrix[4] = 0.0;
	orientMatrix[5] = 1.0;
	orientMatrix[6] = 0.0;
	orientMatrix[7] = 0.0;

	orientMatrix[8] = 0.0;
	orientMatrix[9] = 0.0;
	orientMatrix[10] = 1.0;
	orientMatrix[11] = 0.0;

	orientMatrix[12] = 0.0;
	orientMatrix[13] = 0.0;
	orientMatrix[14] = 0.0;
	orientMatrix[15] = 1.0;

	Bind( wxEVT_PAINT, &Canvas::OnPaint, this );
	Bind( wxEVT_SIZE, &Canvas::OnSize, this );
	Bind( wxEVT_LEFT_DOWN, &Canvas::OnMouseLeftDown, this );
	Bind( wxEVT_LEFT_UP, &Canvas::OnMouseLeftUp, this );
	Bind( wxEVT_MOTION, &Canvas::OnMouseMotion, this );
	Bind( wxEVT_MOUSE_CAPTURE_LOST, &Canvas::OnMouseCaptureLost, this );
}

/*virtual*/ Canvas::~Canvas( void )
{
	delete context;
	delete flowerBox;
}

void Canvas::OnPaint( wxPaintEvent& event )
{
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// We may or may not want back-face culling too.  Perhaps add a toggle for it.
	glEnable( GL_DEPTH_TEST );

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	GLdouble aspectRatio = GLdouble( viewport[2] ) / GLdouble( viewport[3] );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, aspectRatio, 0.01, 1000.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	glPushMatrix();
	glMultMatrixf( orientMatrix );

	DrawAxes();

	flowerBox->Draw();

	glPopMatrix();

	glFlush();

	SwapBuffers();
}

void Canvas::DrawAxes( void )
{
	glLineWidth( 2.f );
	glBegin( GL_LINES );

	glColor3f( 1.f, 0.f, 0.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( 2.f, 0.f, 0.f );

	glColor3f( 0.f, 1.f, 0.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( 0.f, 2.f, 0.f );

	glColor3f( 0.f, 0.f, 1.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( 0.f, 0.f, 2.f );

	glEnd();
}

void Canvas::GetOrientAxes( c3ga::vectorE3GA& xAxis, c3ga::vectorE3GA& yAxis, c3ga::vectorE3GA& zAxis )
{
	xAxis.m_e1 = orientMatrix[0];
	xAxis.m_e2 = orientMatrix[1];
	xAxis.m_e3 = orientMatrix[2];

	yAxis.m_e1 = orientMatrix[4];
	yAxis.m_e2 = orientMatrix[5];
	yAxis.m_e3 = orientMatrix[6];

	zAxis.m_e1 = orientMatrix[8];
	zAxis.m_e2 = orientMatrix[9];
	zAxis.m_e3 = orientMatrix[10];
}

void Canvas::SetOrientAxes( const c3ga::vectorE3GA& xAxis, const c3ga::vectorE3GA& yAxis, const c3ga::vectorE3GA& zAxis )
{
	orientMatrix[0] = xAxis.m_e1;
	orientMatrix[1] = xAxis.m_e2;
	orientMatrix[2] = xAxis.m_e3;

	orientMatrix[4] = yAxis.m_e1;
	orientMatrix[5] = yAxis.m_e2;
	orientMatrix[6] = yAxis.m_e3;

	orientMatrix[8] = zAxis.m_e1;
	orientMatrix[9] = zAxis.m_e2;
	orientMatrix[10] = zAxis.m_e3;
}

void Canvas::OnSize( wxSizeEvent& event )
{
	BindContext();

	wxSize size = event.GetSize();
	glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

	Refresh();
}

void Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();
	rotatingBoxWithMouse = true;
	CaptureMouse();
}

void Canvas::OnMouseLeftUp( wxMouseEvent& event )
{
	ReleaseMouse();
	rotatingBoxWithMouse = false;
}

void Canvas::OnMouseMotion( wxMouseEvent& event )
{
	if( rotatingBoxWithMouse )
	{
		wxPoint mouseDelta = event.GetPosition() - mousePos;
		mousePos = event.GetPosition();

		double mouseSensativityFactor = 0.3;

		double xAxisMouseRotAngleDegs = double( mouseDelta.y ) * mouseSensativityFactor;
		double yAxisMouseRotAngleDegs = double( mouseDelta.x ) * mouseSensativityFactor;

		c3ga::vectorE3GA xAxisMouse( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
		c3ga::vectorE3GA yAxisMouse( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );

		c3ga::rotorE3GA xAxisRotorMouse = FlowerBox::AxisAngleRotor( xAxisMouse, xAxisMouseRotAngleDegs );
		c3ga::rotorE3GA yAxisRotorMouse = FlowerBox::AxisAngleRotor( yAxisMouse, yAxisMouseRotAngleDegs );

		c3ga::vectorE3GA xAxis, yAxis, zAxis;
		
		GetOrientAxes( xAxis, yAxis, zAxis );

		xAxis = c3ga::applyUnitVersor( xAxisRotorMouse, xAxis );
		yAxis = c3ga::applyUnitVersor( xAxisRotorMouse, yAxis );
		zAxis = c3ga::applyUnitVersor( xAxisRotorMouse, zAxis );

		xAxis = c3ga::applyUnitVersor( yAxisRotorMouse, xAxis );
		yAxis = c3ga::applyUnitVersor( yAxisRotorMouse, yAxis );
		zAxis = c3ga::applyUnitVersor( yAxisRotorMouse, zAxis );

		SetOrientAxes( xAxis, yAxis, zAxis );

		Refresh();
	}
}

void Canvas::OnMouseCaptureLost( wxMouseCaptureLostEvent& event )
{
	rotatingBoxWithMouse = false;
}

void Canvas::BindContext( void )
{
	if( !context )
		context = new wxGLContext( this );

	SetCurrent( *context );
}

// Canvas.cpp
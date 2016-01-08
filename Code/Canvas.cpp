// Canvas.cpp

#include "Canvas.h"
#include <gl/GLU.h>

int Canvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

Canvas::Canvas( wxWindow* parent ) : wxGLCanvas( parent, wxID_ANY, attributeList )
{
	context = nullptr;
	flowerBox = new FlowerBox();

	Bind( wxEVT_PAINT, &Canvas::OnPaint, this );
	Bind( wxEVT_SIZE, &Canvas::OnSize, this );
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

	flowerBox->Draw();

	glFlush();

	SwapBuffers();
}

void Canvas::OnSize( wxSizeEvent& event )
{
	BindContext();

	wxSize size = event.GetSize();
	glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

	Refresh();
}

void Canvas::BindContext( void )
{
	if( !context )
		context = new wxGLContext( this );

	SetCurrent( *context );
}

// Canvas.cpp
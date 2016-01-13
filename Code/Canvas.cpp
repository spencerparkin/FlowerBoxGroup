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
	Bind( wxEVT_MIDDLE_DOWN, &Canvas::OnMouseMiddleDown, this );
	Bind( wxEVT_LEFT_DOWN, &Canvas::OnMouseLeftDown, this );
	Bind( wxEVT_LEFT_UP, &Canvas::OnMouseLeftUp, this );
	Bind( wxEVT_MOTION, &Canvas::OnMouseMotion, this );
	Bind( wxEVT_MOUSE_CAPTURE_LOST, &Canvas::OnMouseCaptureLost, this );
	Bind( wxEVT_RIGHT_DOWN, &Canvas::OnMouseRightDown, this );
}

/*virtual*/ Canvas::~Canvas( void )
{
	delete context;
	delete flowerBox;
}

void Canvas::OnPaint( wxPaintEvent& event )
{
	Render( GL_RENDER );
}

void Canvas::Render( GLenum renderMode )
{
	BindContext();

	GLuint* hitBuffer = nullptr;
	GLuint hitBufferSize = 0;

	if( renderMode == GL_SELECT )
	{
		hitBufferSize = 512;
		hitBuffer = new GLuint[ hitBufferSize ];
		glSelectBuffer( hitBufferSize, hitBuffer );
		glRenderMode( GL_SELECT );
		glInitNames();
		glPushName(-1);
	}

	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glEnable( GL_DEPTH_TEST );
	glShadeModel( GL_SMOOTH );

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	GLdouble aspectRatio = GLdouble( viewport[2] ) / GLdouble( viewport[3] );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( renderMode == GL_SELECT )
		gluPickMatrix( GLdouble( mousePos.x ), GLdouble( viewport[3] - mousePos.y - 1 ), 2.0, 2.0, viewport );

	gluPerspective( 60.0, aspectRatio, 0.01, 1000.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	glPushMatrix();
	glMultMatrixf( orientMatrix );

	DrawAxes();

	flowerBox->Draw( renderMode );

	glPopMatrix();

	glFlush();

	if( renderMode == GL_RENDER )
		SwapBuffers();
	else if( renderMode == GL_SELECT )
	{
		GLint hitCount = glRenderMode( GL_RENDER );

		flowerBox->ProcessHitBuffer( hitBuffer, hitBufferSize, hitCount );

		delete[] hitBuffer;
		hitBuffer = nullptr;
		hitBufferSize = 0;
	}
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

void Canvas::Animate( void )
{
	if( flowerBox->StillAnimating() )
	{
		if( flowerBox->Animate( 0.9f ) )
			Refresh();
	}
	else if( moveSequence.size() > 0 )
	{
		MoveSequence::iterator iter = moveSequence.begin();
		Move move = *iter;
		moveSequence.erase( iter );

		flowerBox->PermuteCorner( move.corner, move.rotate, true );
	}
}

void Canvas::OnSize( wxSizeEvent& event )
{
	BindContext();

	wxSize size = event.GetSize();
	glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

	Refresh();
}

void Canvas::OnMouseMiddleDown( wxMouseEvent& event )
{
	// Center face tri-cycle.
#if 0
	for( int i = 0; i < 2; i++ )
	{
		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CCW ) );

		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CCW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CCW ) );

		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CCW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );
		moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CW ) );
	}

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CCW ) );
#endif

	// Swap adjacent spike-pairs.
#if 0

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CW ) );

#endif

	// Parity fix!
#if 1

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_PZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CCW ) );

	moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );
	moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );

#endif

	wxString seqString = PrintSequence( moveSequence );
	seqString = "";
}

/*static*/ wxString Canvas::PrintSequence( const MoveSequence& moveSequence )
{
	wxString seqString;

	MoveSequence::const_iterator iter = moveSequence.begin();
	while( iter != moveSequence.end() )
	{
		const Move& move = *iter;

		wxString cornerStr = "(?)";

		switch( move.corner )
		{
			case FlowerBox::CORNER_NX_PY_PZ:	cornerStr = "X";	break;
			case FlowerBox::CORNER_PX_PY_PZ:	cornerStr = "Y";	break;
			case FlowerBox::CORNER_PX_PY_NZ:	cornerStr = "Z";	break;
			case FlowerBox::CORNER_NX_PY_NZ:	cornerStr = "W";	break;

			case FlowerBox::CORNER_NX_NY_NZ:	cornerStr = "A";	break;
			case FlowerBox::CORNER_PX_NY_NZ:	cornerStr = "B";	break;
			case FlowerBox::CORNER_PX_NY_PZ:	cornerStr = "C";	break;
			case FlowerBox::CORNER_NX_NY_PZ:	cornerStr = "D";	break;
		}

		if( move.rotate == FlowerBox::ROTATE_CCW )
			cornerStr += "i";

		seqString += cornerStr + ",";

		iter++;
	}

	return seqString;
}

void Canvas::OnMouseRightDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();

	Render( GL_SELECT );

	if( flowerBox->selectedFaceId != -1 )
	{
		FlowerBox::Corner corner = flowerBox->ClosestCornerOfFace( flowerBox->selectedFaceId );
		if( corner != FlowerBox::CORNER_COUNT )
		{
			flowerBox->Snap();

			Move move;
			move.corner = corner;
			
			if( event.AltDown() )
				move.rotate = FlowerBox::ROTATE_CCW;
			else
				move.rotate = FlowerBox::ROTATE_CW;

			moveSequence.push_back( move );
		}
	}
}

void Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();
	rotatingBoxWithMouse = true;
	CaptureMouse();
}

void Canvas::OnMouseLeftUp( wxMouseEvent& event )
{
	if( HasCapture() )
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

		// Account for accumulated round-off error.
		xAxis = c3ga::unit( xAxis );
		yAxis = c3ga::unit( yAxis );
		zAxis = c3ga::unit( zAxis );

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

/*static*/ int Canvas::RandomInt( int min, int max )
{
	float r = ( float )rand();
	float r_max = ( float )RAND_MAX;
	float t = r / r_max;
	float a = ( float )min;
	float b = ( float )max;
	float c = a + ( b - a ) * t;
	c = round( c );
	int i = ( int )c;
	wxASSERT( min <= c && c <= max );
	return c;
}

void Canvas::DesperateSearch( void )
{
	int moveCount = 20;

	srand( 1701 );

	MoveSequence historySequence;

	while( true )
	{
		historySequence.clear();

		delete flowerBox;
		flowerBox = new FlowerBox();

		int x, y, z;

		x = 2;
		y = 0;
		z = 0;
		int redFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		x = 0;
		y = 2;
		z = 0;
		int whiteFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		x = 0;
		y = 0;
		z = 2;
		int blueFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		x = -2;
		y = 0;
		z = 0;
		int orangeFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		x = 0;
		y = -2;
		z = 0;
		int yellowFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		x = 0;
		y = 0;
		z = -2;
		int greenFaceId = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ].id;

		moveSequence.clear();

		for( int i = 0; i < moveCount; i++ )
		{
			Move move;
			move.Random();
			moveSequence.push_back( move );
		}

		FlushSequence( historySequence );

		// Get red in position.
		int i;
		for( i = 0; i < 10; i++ )
		{
			flowerBox->FindFace( redFaceId, x, y, z );
			if( x == 2 && y == 0 && z == 0 )
				break;

			if( x == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CCW ) );
			else if( y == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CCW ) );
			else if( y == 2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_PX_PY_NZ, FlowerBox::ROTATE_CCW ) );
			else if( z == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_NZ, FlowerBox::ROTATE_CCW ) );
			else if( z == 2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_PX_NY_PZ, FlowerBox::ROTATE_CW ) );
			else
				wxASSERT( false );

			FlushSequence( historySequence );
		}
		wxASSERT( i < 10 );

		// Get white in position without disturbing red.
		for( i = 0; i < 10; i++ )
		{
			flowerBox->FindFace( whiteFaceId, x, y, z );
			if( x == 0 && y == 2 && z == 0 )
				break;

			if( x == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CCW ) );
			else if( y == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_PZ, FlowerBox::ROTATE_CCW ) );
			else if( z == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_NZ, FlowerBox::ROTATE_CW ) );
			else if( z == 2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_PY_PZ, FlowerBox::ROTATE_CCW ) );
			else
				wxASSERT( false );

			FlushSequence( historySequence );
		}
		wxASSERT( i < 10 );

		// Get blue in position without disturbing red or white.
		for( int i = 0; i < 10; i++ )
		{
			flowerBox->FindFace( blueFaceId, x, y, z );
			if( x == 0 && y == 0 && z == 2 )
				break;

			if( x == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_PZ, FlowerBox::ROTATE_CW ) );
			else if( y == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_PZ, FlowerBox::ROTATE_CCW ) );
			else if( z == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
			else
				wxASSERT( false );

			FlushSequence( historySequence );
		}
		wxASSERT( i < 10 );

		// Get orange into position without disturbing red, white or blue.
		for( int i = 0; i < 10; i++ )
		{
			flowerBox->FindFace( orangeFaceId, x, y, z );
			if( x == -2 && y == 0 && z == 0 )
				break;

			if( y == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CCW ) );
			else if( z == -2 )
				moveSequence.push_back( Move( FlowerBox::CORNER_NX_NY_NZ, FlowerBox::ROTATE_CW ) );
			else
				wxASSERT( false );

			FlushSequence( historySequence );
		}
		wxASSERT( i < 10 );

		// Now here's the parity test.  Where are yellow and green?
		flowerBox->FindFace( yellowFaceId, x, y, z );
		if( !( x == 0 && y == -2 && z == 0 ) )
		{
			flowerBox->FindFace( greenFaceId, x, y, z );
			x = y = z = 0;
			break;
		}
	}

	// Here our history sequence should contain the secret sauce!!!!!!
	wxString seqString = PrintSequence( historySequence );
	seqString = "";
}

void Canvas::FlushSequence( MoveSequence& historySequence )
{
	while( moveSequence.size() > 0 )
	{
		MoveSequence::iterator iter = moveSequence.begin();
		Move move = *iter;
		flowerBox->PermuteCorner( move.corner, move.rotate, false );
		moveSequence.erase( iter );
		historySequence.push_back( move );
	}
}

// Canvas.cpp
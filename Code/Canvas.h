// Canvas.h

#pragma once

#include "FlowerBox.h"
#include "c3ga/c3ga.h"
#include <wx/glcanvas.h>

class Canvas : public wxGLCanvas
{
public:

	Canvas( wxWindow* parent );
	virtual ~Canvas( void );

	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnMouseLeftDown( wxMouseEvent& event );
	void OnMouseLeftUp( wxMouseEvent& event );
	void OnMouseMotion( wxMouseEvent& event );
	void OnMouseRightDown( wxMouseEvent& event );
	void OnMouseMiddleDown( wxMouseEvent& event );
	void OnMouseCaptureLost( wxMouseCaptureLostEvent& event );

	void Animate( void );
	void BindContext( void );
	void DrawAxes( void );
	void Render( GLenum renderMode );

	void GetOrientAxes( c3ga::vectorE3GA& xAxis, c3ga::vectorE3GA& yAxis, c3ga::vectorE3GA& zAxis );
	void SetOrientAxes( const c3ga::vectorE3GA& xAxis, const c3ga::vectorE3GA& yAxis, const c3ga::vectorE3GA& zAxis );

	static int RandomInt( int min, int max );

	struct Move
	{
		Move( void ) {}
		Move( FlowerBox::Corner corner, FlowerBox::Rotate rotate )
		{
			this->corner = corner;
			this->rotate = rotate;
		}

		void Random( void )
		{
			corner = FlowerBox::Corner( RandomInt( 0, 100 ) % FlowerBox::CORNER_COUNT );
			rotate = ( RandomInt( 0, 100 ) >= 50 ) ? FlowerBox::ROTATE_CW : FlowerBox::ROTATE_CCW;
		}

		FlowerBox::Corner corner;
		FlowerBox::Rotate rotate;
	};

	typedef std::list< Move > MoveSequence;
	MoveSequence moveSequence;

	static void TriCycleCenterFaces( MoveSequence& moveSequence, bool inverse );
	static void ConcateSequenceInv( const MoveSequence& moveSequence, MoveSequence& moveSequenceConcat );
	static void ConcateSequence( const MoveSequence& moveSequence, MoveSequence& moveSequenceConcat );
	static wxString PrintSequence( const MoveSequence& moveSequence );

	static int attributeList[];
	wxGLContext* context;
	GLfloat orientMatrix[16];
	FlowerBox* flowerBox;
	bool rotatingBoxWithMouse;
	wxPoint mousePos;

	void DesperateSearch( void );
	void FlushSequence( MoveSequence& historySequence );
};

// Canvas.h

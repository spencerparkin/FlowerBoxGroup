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

	struct Move
	{
		Move( void ) {}
		Move( FlowerBox::Corner corner, FlowerBox::Rotate rotate )
		{
			this->corner = corner;
			this->rotate = rotate;
		}

		FlowerBox::Corner corner;
		FlowerBox::Rotate rotate;
	};

	typedef std::list< Move > MoveSequence;
	MoveSequence moveSequence;

	static wxString PrintSequence( const MoveSequence& moveSequence );

	static int attributeList[];
	wxGLContext* context;
	GLfloat orientMatrix[16];
	FlowerBox* flowerBox;
	bool rotatingBoxWithMouse;
	wxPoint mousePos;
};

// Canvas.h

// Canvas.h

#pragma once

#include "FlowerBox.h"
#include <wx/glcanvas.h>

class Canvas : public wxGLCanvas
{
public:

	Canvas( wxWindow* parent );
	virtual ~Canvas( void );

	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );

	void BindContext( void );

	static int attributeList[];
	wxGLContext* context;
	FlowerBox* flowerBox;
};

// Canvas.h

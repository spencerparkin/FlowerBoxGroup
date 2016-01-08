// Frame.h

#pragma once

#include <wx/frame.h>

class Canvas;

class Frame : public wxFrame
{
public:

	Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size );
	virtual ~Frame( void );

	Canvas* canvas;
};

// Frame.h
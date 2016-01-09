// Frame.h

#pragma once

#include <wx/frame.h>
#include <wx/timer.h>

class Canvas;

class Frame : public wxFrame
{
public:

	Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size );
	virtual ~Frame( void );

	enum
	{
		ID_Timer = wxID_HIGHEST,
	};

	void OnTimer( wxTimerEvent& event );

	Canvas* canvas;
	wxTimer timer;
};

// Frame.h
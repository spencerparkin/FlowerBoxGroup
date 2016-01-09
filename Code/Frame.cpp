// Frame.cpp

#include "Frame.h"
#include "Canvas.h"
#include <wx/sizer.h>

Frame::Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size ) : wxFrame( parent, wxID_ANY, "Flower Box Group", pos, size ), timer( this, ID_Timer )
{
	canvas = new Canvas( this );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );

	Bind( wxEVT_TIMER, &Frame::OnTimer, this );

	timer.Start(1);
}

/*virtual*/ Frame::~Frame( void )
{
}

void Frame::OnTimer( wxTimerEvent& event )
{
	static bool inHere = false;

	if( inHere )
		return;

	inHere = true;

	canvas->Animate();

	inHere = false;
}

// Frame.cpp
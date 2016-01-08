// Frame.cpp

#include "Frame.h"
#include "Canvas.h"
#include <wx/sizer.h>

Frame::Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size ) : wxFrame( parent, wxID_ANY, "Flower Box Group", pos, size )
{
	canvas = new Canvas( this );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );
}

/*virtual*/ Frame::~Frame( void )
{
}

// Frame.cpp
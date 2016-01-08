// App.cpp

#include "App.h"
#include "Frame.h"

wxIMPLEMENT_APP( App );

App::App( void )
{
}

/*virtual*/ App::~App( void )
{
}

/*virtual*/ bool App::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	Frame* frame = new Frame( nullptr, wxDefaultPosition, wxSize( 500, 500 ) );
	frame->Show();

	return true;
}

// App.cpp
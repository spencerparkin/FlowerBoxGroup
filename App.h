// App.h

#pragma once

#include <wx/setup.h>
#include <wx/app.h>

class Frame;

class App : public wxApp
{
public:

	App( void );
	virtual ~App( void );

	virtual bool OnInit( void ) override;

	Frame* frame;
};

wxDECLARE_APP( App );

// App.h

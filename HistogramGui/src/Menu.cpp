#include "Menu.h"
#include "MyDrawMain.h"


MyMenu::MyMenu(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE) {

	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_SAVE, wxT("&Save"));
	file->Append(wxID_OPEN, wxT("&Load"));
	file->Append(wxID_REVERT, wxT("&Reset\tCtrl-R"));
	file->Append(INCREASE_SCALE_BUTTON_ID, wxT("&Zoom in\tCtrl-="));
	file->Append(DECREASE_SCALE_BUTTON_ID, wxT("&Zoom out\tCtrl--"));
	file->Append(wxID_EXIT, wxT("&Quit"));
	menubar->Append(file, wxT("&File"));
	SetMenuBar(menubar);

	// TODO inconsistent event handling, move it to MyDrawMain.cpp
	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HistoDrawPane::OnExit));
	Centre();
}

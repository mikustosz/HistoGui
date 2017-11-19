#include "Menu.h"
#include "MyDrawMain.h"


MyMenu::MyMenu(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE) {

	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_SAVE, wxT("&Save"));
	file->Append(wxID_OPEN, wxT("&Open"));
	file->Append(wxID_EXIT, wxT("&Quit"));
	menubar->Append(file, wxT("&File"));

	view = new wxMenu;
	view->Append(wxID_REVERT, wxT("&Reset\tCtrl-R"));
	view->Append(INCREASE_SCALE_BUTTON_ID, wxT("&Zoom in\tCtrl-="));
	view->Append(DECREASE_SCALE_BUTTON_ID, wxT("&Zoom out\tCtrl--"));
	view->Append(TOGGLE_BACKGROUND_ID, wxT("&Toggle background\tb"));
	menubar->Append(view, wxT("&View"));

	help = new wxMenu;
	help->Append(USAGE_ID, wxT("&Usage"));
	help->Append(ABOUT_ID, wxT("&About"));
	menubar->Append(help, wxT("&Help"));

	SetMenuBar(menubar);

	// TODO inconsistent event handling, move it to MyDrawMain.cpp
	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(HistoDrawPane::OnExit));
	Centre();
}

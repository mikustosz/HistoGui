#include "Menu.h"

MyMenu::MyMenu(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE) {

	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_EXIT, wxT("&Quit"));
	menubar->Append(file, wxT("&File"));
	SetMenuBar(menubar);

//	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyMenu::OnQuit));
	Centre();
}

//void MyMenu::OnQuit(wxCommandEvent& WXUNUSED(event)) {
////  Close(true);
//}
//void MyMenu::OnSave(wxCommandEvent& WXUNUSED(event)) {
//  Close(true);
//}

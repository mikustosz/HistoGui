#ifndef _MENUXXX_H
#define _MENUXXX_H
#include <wx/wx.h>
#include <wx/menu.h>

class MyMenu : public wxFrame
{
public:
	MyMenu(const wxString& title);

//    void OnQuit(wxCommandEvent& event);
//    void OnSave(wxCommandEvent& event);

    wxMenuBar *menubar;
    wxMenu *file;
//    HistoDrawPane *histoDrawPane;

};

#endif // _MENUXXX_H

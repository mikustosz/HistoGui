#ifndef _MENUXXX_H
#define _MENUXXX_H
#include <wx/wx.h>
#include <wx/menu.h>

class MyMenu : public wxFrame
{
public:
	MyMenu(const wxString& title);

    wxMenuBar *menubar;
    wxMenu *file;
};

#endif // _MENUXXX_H

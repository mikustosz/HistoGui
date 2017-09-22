/**
 @file      MyDrawApp.cpp
 @brief   Implementation for Application Class
 @author    kzawisto
 @created  2015-10-29
 * Copyright: kzawisto ()
 * License:
 **************************************************************/

#include "MyDrawApp.h"
#include "wx/wx.h"
#include "wx/sizer.h"

//(*AppHeaders
#include "MyDrawMain.h"
#include <wx/image.h>

#include <HistoCreator.h>
#include "Menu.h"
//*)

IMPLEMENT_APP(MyHistoApp)

bool MyHistoApp::OnInit() {
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	menu = new MyMenu( wxT("CMS lab"));
	drawPane = new HistoDrawPane((wxFrame*) menu);

	sizer->Add(drawPane, 1, wxEXPAND);
	menu->SetSizer(sizer);
	menu->SetAutoLayout(true);

	menu->Show();
	return true;
}
